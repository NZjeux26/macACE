"""
Hefentalf Opening Book Generator
==================================
Reads a CSV of game move-lists, finds the top N most common opening
sequences (first PLIES moves) for Attacker and Defender, and writes
a C header file  opening_book.h  ready to include in an Amiga 500 project.

CSV format (no header row):
    result, game_id, move_string

Move string format:
    1. <attacker_move> <defender_move>  2. <attacker_move> <defender_move> ...

C output uses a simple static array of structs — no heap, no pointers,
easy to ROM or BSS on a 68000.
"""

import csv
import re
from collections import Counter
from pathlib import Path
from datetime import datetime


# ── Configuration ────────────────────────────────────────────────────────────

CSV_PATH    = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/game_moves.csv"   # change to your file path
OUTPUT_H    = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/opening_book.h"   # output header filename
TOP_N       = 20                      # how many sequences per side
PLIES       = 10                      # how many moves deep to record
MOVE_MAXLEN = 8                       # max chars in one move token (inc. \0)

COLS    = list("abcdefghijk")
COL_IDX = {c: i for i, c in enumerate(COLS)}

# ── Board index converter ─────────────────────────────────────────────────────
 
def square_to_board_index(col, row):
    return 14 + (11 - row) * 13 + COL_IDX[col]
 
def parse_move_to_indices(token):
    m = re.match(r'^([a-k])(\d{1,2})-([a-k])(\d{1,2})', token)
    if not m:
        return None, None
    src_col, src_row = m.group(1), int(m.group(2))
    dst_col, dst_row = m.group(3), int(m.group(4))
    if src_row < 1 or src_row > 11 or dst_row < 1 or dst_row > 11:
        return None, None
    return square_to_board_index(src_col, src_row), square_to_board_index(dst_col, dst_row)
 
# ── Parsing ───────────────────────────────────────────────────────────────────
 
def parse_move_string(move_string):
    cleaned = re.sub(r'\d+\.', '', move_string)
    cleaned = re.sub(r'\bresigned\b', '', cleaned, flags=re.IGNORECASE)
    move_tokens = [t for t in cleaned.split() if '-' in t]
    return move_tokens[0::2], move_tokens[1::2]
 
def load_games(csv_path):
    games = []
    path = Path(csv_path)
    if not path.exists():
        raise FileNotFoundError(f"Cannot find CSV file: {csv_path!r}")
    with open(path, newline='', encoding='utf-8') as fh:
        for row in csv.reader(fh):
            if len(row) < 3:
                continue
            move_string = ','.join(row[2:]).strip()
            if not move_string:
                continue
            att, dfd = parse_move_string(move_string)
            games.append((att, dfd))
    return games
 
# ── Analysis ──────────────────────────────────────────────────────────────────
 
def build_sequence_counter(games, role, plies):
    counter = Counter()
    idx = 0 if role == 'attacker' else 1
    for att, dfd in games:
        moves = att if idx == 0 else dfd
        seq = tuple(moves[:plies])
        if seq:
            counter[seq] += 1
    return counter
 
# ── C Header Generation ───────────────────────────────────────────────────────
 
def seq_to_c_array(seq, plies):
    cells = []
    for i in range(plies):
        token = seq[i] if i < len(seq) else None
        src, dst = parse_move_to_indices(token) if token else (0, 0)
        src = src or 0
        dst = dst or 0
        cells.append(f'        {{ {src:>3}, {dst:>3} }}')
    return "{\n" + ",\n".join(cells) + "\n    }"
 
def write_header(attacker_counter, defender_counter, output_path, top_n, plies):
    att_top   = attacker_counter.most_common(top_n)
    dfd_top   = defender_counter.most_common(top_n)
    att_count = len(att_top)
    dfd_count = len(dfd_top)
    guard     = "OPENING_BOOK_H"
    now       = datetime.now().strftime("%Y-%m-%d %H:%M")
 
    lines = []
    a = lines.append
 
    a(f"/* opening_book.h  —  generated {now} */")
    a(f"/* Top {top_n} opening sequences, first {plies} moves each side. */")
    a(f"/* boardState[169]: 13x13 with outer ring. A11=14, K1=154.       */")
    a( "")
    a(f"#ifndef {guard}")
    a(f"#define {guard}")
    a( "")
    a(f"#define BOOK_PLIES      {plies}")
    a(f"#define ATTACKER_LINES  {att_count}")
    a(f"#define DEFENDER_LINES  {dfd_count}")
    a( "")
    a( "/* A single half-move as boardState indices. */")
    a( "typedef struct {")
    a( "    unsigned char src; /* boardState[] from-index */")
    a( "    unsigned char dst; /* boardState[] to-index   */")
    a( "} BookMove;")
    a( "")
    a( "typedef struct {")
    a( "    BookMove       moves[BOOK_PLIES];")
    a( "    unsigned char  length;    /* moves actually set     */")
    a( "    unsigned short frequency; /* times seen in data     */")
    a( "} OpeningLine;")
    a( "")
 
    for label, top, count_macro in [
        ("attacker", att_top, "ATTACKER_LINES"),
        ("defender", dfd_top, "DEFENDER_LINES"),
    ]:
        a(f"/* ---- {label.upper()} opening book ---- */")
        a(f"static const OpeningLine {label}_book[{count_macro}] = {{")
        for seq, freq in top:
            length = min(len(seq), plies)
            a( "    {")
            a(f"        /* moves */ {seq_to_c_array(seq, plies)},")
            a(f"        /* length */ {length},")
            a(f"        /* frequency */ {freq}")
            a( "    },")
        a("};")
        a( "")
 
    a( "/*")
    a( " * book_lookup(book, book_size, played, n_played)")
    a( " *   played[]  : array of BookMove structs already made this game.")
    a( " *   n_played  : how many moves have been played so far.")
    a( " *   Returns pointer to next BookMove, or NULL — fall back to minimax.")
    a( " */")
    a( "static const BookMove *book_lookup(")
    a( "    const OpeningLine *book, int book_size,")
    a( "    const BookMove played[], int n_played)")
    a( "{")
    a( "    int i, j, match;")
    a( "    for (i = 0; i < book_size; i++) {")
    a( "        if (book[i].length <= n_played) continue;")
    a( "        match = 1;")
    a( "        for (j = 0; j < n_played; j++) {")
    a( "            if (book[i].moves[j].src != played[j].src ||")
    a( "                book[i].moves[j].dst != played[j].dst) {")
    a( "                match = 0; break;")
    a( "            }")
    a( "        }")
    a( "        if (match) return &book[i].moves[n_played];")
    a( "    }")
    a( "    return NULL; /* fall back to minimax */")
    a( "}")
    a( "")
    a(f"#endif /* {guard} */")
 
    Path(output_path).write_text("\n".join(lines) + "\n", encoding='utf-8')
    print(f"Written: {output_path}  ({att_count} attacker lines, {dfd_count} defender lines)")
 
# ── Main ──────────────────────────────────────────────────────────────────────
 
def main():
    print(f"\nHefentalf Opening Book Generator")
    print(f"Input : {CSV_PATH}   Output: {OUTPUT_H}")
    print(f"Depth : first {PLIES} moves   |   Top {TOP_N} per side\n")
 
    games = load_games(CSV_PATH)
    print(f"Games loaded: {len(games)}")
 
    att_counter = build_sequence_counter(games, 'attacker', PLIES)
    dfd_counter = build_sequence_counter(games, 'defender', PLIES)
 
    write_header(att_counter, dfd_counter, OUTPUT_H, TOP_N, PLIES)
 
if __name__ == "__main__":
    main()