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
OUTPUT_H    = "opening_book.h"        # output header filename
TOP_N       = 20                      # how many sequences per side
PLIES       = 10                      # how many moves deep to record
MOVE_MAXLEN = 8                       # max chars in one move token (inc. \0)


# ── Parsing ───────────────────────────────────────────────────────────────────

def parse_move_string(move_string: str) -> tuple[list[str], list[str]]:
    """
    Given the raw move string (e.g. '1. k8-i8 d6-d9 2. a4-c4 g7-i7 ...'),
    return (attacker_moves, defender_moves) as lists of individual move tokens.
    """
    cleaned = re.sub(r'\d+\.', '', move_string)
    cleaned = re.sub(r'\bresigned\b', '', cleaned, flags=re.IGNORECASE)
    move_tokens = [t for t in cleaned.split() if '-' in t]
    attacker = move_tokens[0::2]
    defender = move_tokens[1::2]
    return attacker, defender


def load_games(csv_path: str) -> list[tuple[list[str], list[str]]]:
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

def build_sequence_counter(games, role: str, plies: int) -> Counter:
    counter: Counter = Counter()
    idx = 0 if role == 'attacker' else 1
    for att, dfd in games:
        moves = att if idx == 0 else dfd
        seq = tuple(moves[:plies])
        if seq:
            counter[seq] += 1
    return counter


# ── C Header Generation ───────────────────────────────────────────────────────

def c_string(s: str) -> str:
    """Wrap a Python string as a C string literal."""
    return f'"{s}"'


def seq_to_c_array(seq: tuple, plies: int) -> str:
    """Render a move sequence as a C initialiser for moves[PLIES][MOVE_MAXLEN]."""
    cells = []
    for i in range(plies):
        move = seq[i] if i < len(seq) else ""
        cells.append(f'        {c_string(move)}')
    return "{\n" + ",\n".join(cells) + "\n    }"


def write_header(attacker_counter: Counter,
                 defender_counter: Counter,
                 output_path: str,
                 top_n: int,
                 plies: int,
                 move_maxlen: int) -> None:

    att_top = attacker_counter.most_common(top_n)
    dfd_top = defender_counter.most_common(top_n)
    att_count = len(att_top)
    dfd_count = len(dfd_top)

    guard = "OPENING_BOOK_H"
    now   = datetime.now().strftime("%Y-%m-%d %H:%M")

    lines = []
    a = lines.append

    a(f"/* opening_book.h  —  generated {now} */")
    a(f"/* Top {top_n} opening sequences, first {plies} moves each side. */")
    a(f"/* Move tokens are at most {move_maxlen - 1} chars + null terminator. */")
    a("")
    a(f"#ifndef {guard}")
    a(f"#define {guard}")
    a("")
    a(f"#define BOOK_PLIES      {plies}")
    a(f"#define BOOK_MOVE_LEN   {move_maxlen}")
    a(f"#define ATTACKER_LINES  {att_count}")
    a(f"#define DEFENDER_LINES  {dfd_count}")
    a("")
    a("typedef struct {")
    a(f"    char     moves[BOOK_PLIES][BOOK_MOVE_LEN]; /* move tokens, empty string = not played */")
    a(f"    unsigned char  length;                     /* how many moves are actually set        */")
    a(f"    unsigned short frequency;                  /* times seen in training data            */")
    a("} OpeningLine;")
    a("")

    for label, top, count_macro in [
        ("attacker", att_top, "ATTACKER_LINES"),
        ("defender", dfd_top, "DEFENDER_LINES"),
    ]:
        a(f"/* ---- {label.upper()} opening book ---- */")
        a(f"static const OpeningLine {label}_book[{count_macro}] = {{")
        for seq, freq in top:
            length = min(len(seq), plies)
            a(f"    {{")
            a(f"        /* moves */ {seq_to_c_array(seq, plies)},")
            a(f"        /* length */ {length},")
            a(f"        /* frequency */ {freq}")
            a(f"    }},")
        a("};")
        a("")

    # Tiny lookup helper — returns next book move or NULL
    a("/*")
    a(" * book_lookup(book, book_size, played, n_played)")
    a(" *   Returns the next move string for the given sequence of played moves,")
    a(" *   or NULL if no book entry matches.  Linear scan — fine for 20 entries.")
    a(" */")
    a('#include <string.h>')
    a("static const char *book_lookup(")
    a("    const OpeningLine *book, int book_size,")
    a("    const char played[][BOOK_MOVE_LEN], int n_played)")
    a("{")
    a("    int i, j, match;")
    a("    for (i = 0; i < book_size; i++) {")
    a("        if (book[i].length <= n_played) continue;")
    a("        match = 1;")
    a("        for (j = 0; j < n_played; j++) {")
    a("            if (strncmp(book[i].moves[j], played[j], BOOK_MOVE_LEN) != 0) {")
    a("                match = 0; break;")
    a("            }")
    a("        }")
    a("        if (match) return book[i].moves[n_played];")
    a("    }")
    a("    return NULL; /* fall back to minimax */")
    a("}")
    a("")
    a(f"#endif /* {guard} */")

    Path(output_path).write_text("\n".join(lines) + "\n", encoding='utf-8')
    print(f"Written: {output_path}  ({att_count} attacker lines, {dfd_count} defender lines)")


# ── Main ──────────────────────────────────────────────────────────────────────

def main() -> None:
    print(f"\nHefentalf Opening Book Generator")
    print(f"Input : {CSV_PATH}   Output: {OUTPUT_H}")
    print(f"Depth : first {PLIES} moves   |   Top {TOP_N} per side\n")

    games = load_games(CSV_PATH)
    print(f"Games loaded: {len(games)}")

    att_counter = build_sequence_counter(games, 'attacker', PLIES)
    dfd_counter = build_sequence_counter(games, 'defender', PLIES)

    write_header(att_counter, dfd_counter, OUTPUT_H, TOP_N, PLIES, MOVE_MAXLEN)


if __name__ == "__main__":
    main()
