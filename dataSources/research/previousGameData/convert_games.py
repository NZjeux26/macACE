"""
Hefentalf Game Converter
=========================
Converts game_moves.csv into boardstate_games.csv where every move
is stored as src:dst board indices instead of algebraic notation.

Input CSV format (no header):
    result, game_id, move_string

Output CSV format (no header):
    result, game_id, attacker_moves, defender_moves

Where attacker_moves and defender_moves are space-separated src:dst pairs:
    151:125 92:94 27:53 ...

boardState[169] is a 13x13 array with an outer off-limits ring.
    A11 = 14, K1 = 154.  Formula: 14 + (11 - row) * 13 + col_idx
"""

import csv
import re
from pathlib import Path

# ── Configuration ─────────────────────────────────────────────────────────────

INPUT_CSV  = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/game_moves.csv"
OUTPUT_CSV = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/boardstate_games.csv"

COLS    = list("abcdefghijk")
COL_IDX = {c: i for i, c in enumerate(COLS)}

# ── Board index converter ─────────────────────────────────────────────────────

def square_to_board_index(col: str, row: int) -> int:
    return 14 + (11 - row) * 13 + COL_IDX[col]


def parse_move_to_indices(token: str) -> tuple[int, int] | tuple[None, None]:
    """
    Parse 'h1-h3' or 'i10-i9xi8' into (src_idx, dst_idx).
    Capture suffix stripped — dst is where the piece lands.
    """
    m = re.match(r'^([a-k])(\d{1,2})-([a-k])(\d{1,2})', token)
    if not m:
        return None, None
    src_col, src_row = m.group(1), int(m.group(2))
    dst_col, dst_row = m.group(3), int(m.group(4))
    if src_row < 1 or src_row > 11 or dst_row < 1 or dst_row > 11:
        return None, None
    return (square_to_board_index(src_col, src_row),
            square_to_board_index(dst_col, dst_row))

# ── Parsing ───────────────────────────────────────────────────────────────────

def parse_move_string(move_string: str) -> tuple[list[str], list[str]]:
    cleaned = re.sub(r'\d+\.', '', move_string)
    cleaned = re.sub(r'\bresigned\b', '', cleaned, flags=re.IGNORECASE)
    tokens = [t for t in cleaned.split() if '-' in t]
    return tokens[0::2], tokens[1::2]


def convert_moves(tokens: list[str]) -> list[str]:
    """Convert a list of algebraic tokens to 'src:dst' index strings."""
    result = []
    for token in tokens:
        src, dst = parse_move_to_indices(token)
        if src is not None:
            result.append(f"{src}:{dst}")
    return result

# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    input_path  = Path(INPUT_CSV)
    output_path = Path(OUTPUT_CSV)

    if not input_path.exists():
        raise FileNotFoundError(f"Cannot find {INPUT_CSV!r}")

    games_read    = 0
    games_written = 0
    skipped       = 0

    with open(input_path,  newline='', encoding='utf-8') as fin, \
         open(output_path, 'w', newline='', encoding='utf-8') as fout:

        writer = csv.writer(fout)
        writer.writerow(['result', 'game_id', 'attacker_moves', 'defender_moves'])

        for row in csv.reader(fin):
            if len(row) < 3:
                skipped += 1
                continue

            result    = row[0].strip()
            game_id   = row[1].strip()
            move_str  = ','.join(row[2:]).strip()
            games_read += 1

            att_tokens, dfd_tokens = parse_move_string(move_str)
            att_moves = convert_moves(att_tokens)
            dfd_moves = convert_moves(dfd_tokens)

            if not att_moves:
                skipped += 1
                continue

            writer.writerow([
                result,
                game_id,
                ' '.join(att_moves),
                ' '.join(dfd_moves),
            ])
            games_written += 1

    print(f"\nHefentalf Game Converter")
    print(f"Input  : {INPUT_CSV}  ({games_read} rows read)")
    print(f"Output : {OUTPUT_CSV} ({games_written} games written, {skipped} skipped)")
    print(f"\nSample output:")

    # Print first 3 rows as a sanity check
    with open(output_path, newline='', encoding='utf-8') as f:
        for i, row in enumerate(csv.reader(f)):
            if i >= 3:
                break
            print(f"  [{row[0]}] game {row[1]}")
            print(f"    ATT: {row[2][:60]}...")
            print(f"    DEF: {row[3][:60]}...")


if __name__ == "__main__":
    main()
