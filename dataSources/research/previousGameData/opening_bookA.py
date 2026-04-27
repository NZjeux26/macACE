"""
Hefentalf Opening Book Generator
==================================
Reads boardstate_games.csv (output of convert_games.py) and produces
opening_book.h — a static C header for use on Amiga 500.

Pipeline:
  1. Load & filter  — drop games shorter than MIN_GAME_LENGTH half-moves
  2. Build tiered sequences at depths 1, 2, 3
  3. Filter by MIN_FREQUENCY (no win requirement — all theory is valid)
  4. Rank by frequency descending, smoothed win rate as tiebreaker
  5. Score: (frequency / global_max_frequency) * 500, stored as WORD
  6. Output tiered static C structs using existing AIMove type

Scoring convention:
  - Score is purely frequency-based, range 0-500
  - Higher = more commonly played = higher book confidence
  - Win rate (Laplace smoothed, k=16) used only as tiebreaker
  - Score is positive for both sides — minimax takes over after book

Tiered lookup: try depth-3 first, then depth-2, then depth-1, then minimax.
Levenshtein deviation threshold: >30% of moves deviated -> fall back to minimax.

boardState[169]: 13x13 with outer ring. A11=14, K1=154.
"""

import csv
from collections import defaultdict
from pathlib import Path
from datetime import datetime

# ── Configuration ─────────────────────────────────────────────────────────────

INPUT_CSV        = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/boardstate_games.csv"
OUTPUT_H         = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/opening_book.h"
BOOK_DEPTHS      = [1, 2, 3]
TOP_N            = 20
MIN_FREQUENCY    = 4
MIN_GAME_LENGTH  = 4
SMOOTHING_K      = 16
SCORE_MAX        = 1000     # WORD score for the most frequent move
GLOBAL_MAX_FREQ  = 584     # highest frequency seen across all depths/roles

# ── Load & filter ─────────────────────────────────────────────────────────────

def load_games(csv_path):
    games = []
    with open(csv_path, newline='', encoding='utf-8') as fh:
        reader = csv.DictReader(fh)
        for row in reader:
            att_moves = []
            for token in row['attacker_moves'].strip().split():
                parts = token.split(':')
                if len(parts) == 2:
                    att_moves.append((int(parts[0]), int(parts[1])))
            def_moves = []
            for token in row['defender_moves'].strip().split():
                parts = token.split(':')
                if len(parts) == 2:
                    def_moves.append((int(parts[0]), int(parts[1])))
            if len(att_moves) + len(def_moves) < MIN_GAME_LENGTH:
                continue
            games.append({
                'att_wins': row['result'].strip().lower() == 'white won',
                'att':      att_moves,
                'def':      def_moves,
            })
    return games

# ── Sequence analysis ─────────────────────────────────────────────────────────

def smoothed_win_rate(wins, total, k=SMOOTHING_K):
    return (wins + k * 0.5) / (total + k)


def freq_to_word_score(frequency):
    """Scale frequency to WORD score 0-500."""
    return int((frequency / GLOBAL_MAX_FREQ) * SCORE_MAX)


def build_sequences(games, role, plies):
    """
    Build sequences of exactly `plies` depth.
    Sort by frequency desc, smoothed win rate as tiebreaker.
    Returns list of (seq, frequency, smoothed, word_score).
    """
    stats = defaultdict(lambda: {'total': 0, 'wins': 0})
    for game in games:
        moves = game['att'] if role == 'attacker' else game['def']
        if len(moves) < plies:
            continue
        seq = tuple(moves[:plies])
        won = game['att_wins'] if role == 'attacker' else not game['att_wins']
        stats[seq]['total'] += 1
        if won:
            stats[seq]['wins'] += 1

    scored = []
    for seq, s in stats.items():
        total = s['total']
        if total < MIN_FREQUENCY:
            continue
        smoothed   = smoothed_win_rate(s['wins'], total)
        word_score = freq_to_word_score(total)
        scored.append((seq, total, smoothed, word_score))

    # Primary: frequency desc, Secondary: smoothed win rate desc
    scored.sort(key=lambda x: (x[1], x[2]), reverse=True)
    return scored

# ── C Header Generation ───────────────────────────────────────────────────────

def moves_to_c_array(seq, plies, word_score):
    cells = []
    for i in range(plies):
        src, dst = seq[i] if i < len(seq) else (0, 0)
        cells.append(f'        {{ {src:>3}, {dst:>3}, {word_score:>5} }}')
    return "{\n" + ",\n".join(cells) + "\n    }"


def write_header(all_results, output_path, top_n):
    guard = "OPENING_BOOK_H"
    now   = datetime.now().strftime("%Y-%m-%d %H:%M")

    lines = []
    a = lines.append

    a(f"/* opening_book.h  —  generated {now}                                 */")
    a(f"/* Tiered opening book: depths {BOOK_DEPTHS}, top {top_n} lines per side per depth.      */")
    a(f"/* Score = (frequency / {GLOBAL_MAX_FREQ}) * {SCORE_MAX}, range 0-{SCORE_MAX}. Win rate (k={SMOOTHING_K}) tiebreaker. */")
    a(f"/* boardState[169]: 13x13 with outer ring. A11=14, K1=154.              */")
    a( "")
    a(f"#ifndef {guard}")
    a(f"#define {guard}")
    a( "")
    a(f"#define BOOK_TOP_N      {top_n}")
    a(f"#define BOOK_SMOOTH_K   {SMOOTHING_K}")
    a(f"#define BOOK_SCORE_MAX  {SCORE_MAX}")
    a(f"#define BOOK_GLOBAL_MAX {GLOBAL_MAX_FREQ}")
    a( "")

    for depth in BOOK_DEPTHS:
        att_top = all_results[depth]['attacker'][:top_n]
        def_top = all_results[depth]['defender'][:top_n]
        att_cnt = len(att_top)
        def_cnt = len(def_top)
        macro   = f"BOOK_D{depth}"

        a(f"/* ================================================================ */")
        a(f"/* DEPTH {depth} — sequences of {depth} move{'s' if depth > 1 else ''}                                      */")
        a(f"/* ================================================================ */")
        a(f"#define {macro}_PLIES      {depth}")
        a(f"#define {macro}_ATT_LINES  {att_cnt}")
        a(f"#define {macro}_DEF_LINES  {def_cnt}")
        a( "")
        a(f"typedef struct {{")
        a(f"    AIMove moves[{macro}_PLIES]; /* fromIndex, toIndex, score 0-{SCORE_MAX} */")
        a( "    UBYTE  length;               /* moves actually set              */")
        a( "    UWORD  frequency;            /* times seen in training data     */")
        a(f"}} OpeningLine_D{depth};")
        a( "")

        for label, top, cnt_macro in [
            ("attacker", att_top, f"{macro}_ATT_LINES"),
            ("defender", def_top, f"{macro}_DEF_LINES"),
        ]:
            a(f"/* ---- {label.upper()} depth-{depth} book ---- */")
            a(f"/* sorted: frequency desc, smoothed win rate (k={SMOOTHING_K}) tiebreaker */")
            a(f"static const OpeningLine_D{depth} {label}_book_d{depth}[{cnt_macro}] = {{")
            for seq, freq, smoothed, word_score in top:
                a( "    {")
                a(f"        /* moves    */ {moves_to_c_array(seq, depth, word_score)},")
                a(f"        /* length   */ {depth},")
                a(f"        /* freq     */ {freq}")
                a( "    },")
            a("};")
            a( "")

    # Per-depth lookup helpers
    for depth in BOOK_DEPTHS:
        macro = f"BOOK_D{depth}"
        a(f"/* book_lookup_d{depth} — match against depth-{depth} lines */")
        a(f"static const AIMove *book_lookup_d{depth}(")
        a(f"    const OpeningLine_D{depth} *book, int book_size,")
        a( "    const AIMove played[], int n_played)")
        a( "{")
        a( "    int i, j, match;")
        a(f"    if (n_played < {depth}) return NULL;")
        a( "    for (i = 0; i < book_size; i++) {")
        a( "        match = 1;")
        a(f"        for (j = 0; j < {depth}; j++) {{")
        a( "            if (book[i].moves[j].fromIndex != played[j].fromIndex ||")
        a( "                book[i].moves[j].toIndex   != played[j].toIndex) {")
        a( "                match = 0; break;")
        a( "            }")
        a( "        }")
        a( "        if (match) return &book[i].moves[n_played];")
        a( "    }")
        a( "    return NULL;")
        a( "}")
        a( "")

    # Master dispatcher
    a( "/*")
    a( " * book_lookup — tiered dispatcher")
    a( " *")
    a( " * Tries depth-3 lines first, then depth-2, then depth-1.")
    a( " * Returns NULL (fall back to minimax) if:")
    a( " *   - no line matches, or")
    a( " *   - deviations > (n_played * 3) / 10  (>30% Levenshtein threshold)")
    a( " *")
    a( " * role       : TEAM_ATTACKER or TEAM_DEFENDER")
    a( " * deviations : running count of opponent moves not matching book")
    a( " *")
    a( " * Returned AIMove->score is frequency-based, range 0-500.")
    a( " * Higher = more commonly played opening = higher confidence.")
    a( " */")
    a( "static const AIMove *book_lookup(")
    a( "    const AIMove played[], int n_played,")
    a( "    int deviations, int role)")
    a( "{")
    a( "    const AIMove *move = NULL;")
    a( "")
    a( "    if (n_played > 0 && deviations > (n_played * 3) / 10)")
    a( "        return NULL;")
    a( "")
    a( "    if (role == TEAM_ATTACKER) {")
    a( "        if (n_played >= 3)")
    a( "            move = book_lookup_d3(attacker_book_d3, BOOK_D3_ATT_LINES, played, n_played);")
    a( "        if (!move && n_played >= 2)")
    a( "            move = book_lookup_d2(attacker_book_d2, BOOK_D2_ATT_LINES, played, n_played);")
    a( "        if (!move)")
    a( "            move = book_lookup_d1(attacker_book_d1, BOOK_D1_ATT_LINES, played, n_played);")
    a( "    } else {")
    a( "        if (n_played >= 3)")
    a( "            move = book_lookup_d3(defender_book_d3, BOOK_D3_DEF_LINES, played, n_played);")
    a( "        if (!move && n_played >= 2)")
    a( "            move = book_lookup_d2(defender_book_d2, BOOK_D2_DEF_LINES, played, n_played);")
    a( "        if (!move)")
    a( "            move = book_lookup_d1(defender_book_d1, BOOK_D1_DEF_LINES, played, n_played);")
    a( "    }")
    a( "    return move;")
    a( "}")
    a( "")
    a(f"#endif /* {guard} */")

    Path(output_path).write_text("\n".join(lines) + "\n", encoding='utf-8')


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print(f"\nHefentalf Opening Book Generator")
    print(f"Input : {INPUT_CSV}")
    print(f"Output: {OUTPUT_H}")
    print(f"Depths: {BOOK_DEPTHS}  |  Top {TOP_N} per side per depth")
    print(f"Min freq: {MIN_FREQUENCY}  |  Smoothing k={SMOOTHING_K}  |  Score max: {SCORE_MAX}\n")

    games = load_games(INPUT_CSV)
    att_wins = sum(1 for g in games if g['att_wins'])
    print(f"Games loaded  : {len(games)}  (filtered < {MIN_GAME_LENGTH} half-moves)")
    print(f"Attacker wins : {att_wins} ({att_wins/len(games)*100:.1f}%)")
    print(f"Defender wins : {len(games)-att_wins} ({(len(games)-att_wins)/len(games)*100:.1f}%)\n")

    all_results = {}
    for depth in BOOK_DEPTHS:
        all_results[depth] = {}
        print(f"Depth {depth}:")
        for role in ('attacker', 'defender'):
            scored = build_sequences(games, role, depth)
            all_results[depth][role] = scored
            top = scored[:TOP_N]
            print(f"  {role.capitalize():10} — {len(scored):>4} sequences  "
                  f"(showing top {min(len(top), 5)})")
            for seq, freq, smoothed, word_score in top[:5]:
                moves_str = ' '.join(f"{s}:{d}" for s, d in seq)
                print(f"    score={word_score:>5}  freq={freq:>4}  "
                      f"wr={smoothed:.3f}  {moves_str}")
        print()

    write_header(all_results, OUTPUT_H, TOP_N)
    print(f"Written : {OUTPUT_H}\n\nDone.")


if __name__ == "__main__":
    main()
