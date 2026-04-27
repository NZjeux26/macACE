"""
Hnefatafl Opening Book Generator (Improved)
===========================================

Changes:
- Uses smoothed win-rate scoring (Bayesian prior)
- Introduces 'weight' for move selection (safe, bounded)
- Removes misuse of AIMove.score (always 0)
- Cleaner struct layout
"""

import csv
from collections import defaultdict
from pathlib import Path
from datetime import datetime

# ── Configuration ─────────────────────────────────────────────────────────────

INPUT_CSV       = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/boardstate_games.csv"
OUTPUT_H        = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/opening_book.h"

DEPTHS          = [1, 2, 3, 4, 5]
MIN_FREQUENCY   = 4
MIN_GAME_LENGTH = 4
MAX_SIZE_BYTES  = 100_000

SMOOTHING_K     = 16     # Bayesian prior strength
SCALE           = 128    # scales weights into useful integer range

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

def build_sequences(games, role, depth):
    stats = defaultdict(lambda: {'total': 0, 'wins': 0})

    for game in games:
        moves = game['att'] if role == 'attacker' else game['def']
        seq   = tuple(moves[:depth])

        if len(seq) < depth:
            continue

        won = game['att_wins'] if role == 'attacker' else not game['att_wins']

        stats[seq]['total'] += 1
        if won:
            stats[seq]['wins'] += 1

    results = []

    for seq, s in stats.items():
        total = s['total']

        if total < MIN_FREQUENCY:
            continue

        wins = s['wins']

        # ── Smoothed win rate (Bayesian) ──
        smoothed = (wins + 0.5 * SMOOTHING_K) / (total + SMOOTHING_K)

        # ── Weight combines quality + confidence ──
        weight = int(smoothed * total * SCALE)

        # Clamp to UWORD
        weight = max(1, min(weight, 65535))

        win_rate = int((wins / total) * 100)

        results.append((seq, total, win_rate, weight))

    # Sort by weight (descending)
    results.sort(key=lambda x: x[3], reverse=True)

    return results


# ── C formatting helpers ──────────────────────────────────────────────────────

def moves_to_c_array(seq, depth):
    cells = []

    for i in range(depth):
        src, dst = seq[i] if i < len(seq) else (0, 0)
        cells.append(f'        {{ {src:>3}, {dst:>3}, 0 }}')

    return "{\n" + ",\n".join(cells) + "\n    }"


def bytes_for_depth(depth, count):
    # AIMove = 4 bytes (UBYTE,UBYTE,WORD)
    return count * (depth * 4 + 1 + 2 + 1 + 2)
    # moves + length + weight + winrate + freq


# ── C Header Generation ───────────────────────────────────────────────────────

def write_header(all_results, output_path):
    guard = "OPENING_BOOK_H"
    now   = datetime.now().strftime("%Y-%m-%d %H:%M")

    total_bytes = sum(
        bytes_for_depth(depth, len(seqs))
        for (role, depth), seqs in all_results.items()
    )

    lines = []
    a = lines.append

    a(f"/* opening_book.h — generated {now} */")
    a(f"/* Smoothed-weight opening book (K={SMOOTHING_K}, SCALE={SCALE}) */")
    a(f"/* Size: ~{total_bytes} / {MAX_SIZE_BYTES} bytes */")
    a("")

    a(f"#ifndef {guard}")
    a(f"#define {guard}")
    a("")

    # ── Count macros ──
    for depth in DEPTHS:
        for role in ('attacker', 'defender'):
            count = len(all_results.get((role, depth), []))
            macro = f"BOOK_{role.upper()}_D{depth}_LINES"
            a(f"#define {macro:<36} {count}")

    a("")

    a("/* Opening line structure */")

    for depth in DEPTHS:
        a("")
        a(f"typedef struct {{")
        a(f"    AIMove moves[{depth}];")
        a(f"    UBYTE  length;")
        a(f"    UWORD  weight;     /* selection weight */")
        a(f"    UBYTE  win_rate;   /* 0–100 (debug) */")
        a(f"    UWORD  frequency;  /* raw count */")
        a(f"}} OpeningLine_D{depth};")

    # ── Data arrays ──
    for depth in DEPTHS:
        for role in ('attacker', 'defender'):
            seqs  = all_results.get((role, depth), [])
            macro = f"BOOK_{role.upper()}_D{depth}_LINES"

            a("")
            a(f"/* ---- {role.upper()} depth-{depth} ---- */")
            a(f"static const OpeningLine_D{depth} {role}_book_d{depth}[{macro}] = {{")

            for seq, freq, win_rate, weight in seqs:
                a("    {")
                a(f"        /* moves     */ {moves_to_c_array(seq, depth)},")
                a(f"        /* length    */ {depth},")
                a(f"        /* weight    */ {weight},")
                a(f"        /* win_rate  */ {win_rate},")
                a(f"        /* frequency */ {freq}")
                a("    },")

            a("};")

    a("")
    a(f"#endif /* {guard} */")

    Path(output_path).write_text("\n".join(lines) + "\n", encoding='utf-8')

    return total_bytes


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print("\nHnefatafl Opening Book Generator (Improved)\n")

    games = load_games(INPUT_CSV)

    att_wins = sum(1 for g in games if g['att_wins'])

    print(f"Games: {len(games)}")
    print(f"Attacker wins: {att_wins} ({att_wins/len(games)*100:.1f}%)")
    print(f"Defender wins: {len(games)-att_wins}\n")

    all_results = {}

    for depth in DEPTHS:
        print(f"Depth {depth}:")

        for role in ('attacker', 'defender'):
            seqs = build_sequences(games, role, depth)
            all_results[(role, depth)] = seqs

            top = seqs[0] if seqs else None

            print(f"  {role:<10}: {len(seqs):>4}"
                  + (f"  top weight={top[3]}" if top else ""))

    total_bytes = write_header(all_results, OUTPUT_H)

    print(f"\nWritten: {OUTPUT_H}")
    print(f"Size: {total_bytes} / {MAX_SIZE_BYTES} bytes")

    if total_bytes > MAX_SIZE_BYTES:
        print("WARNING: Over budget!")
    else:
        print("OK")

    print("\nDone.")


if __name__ == "__main__":
    main()