"""
Hefentalf Opening Heatmap Generator
=====================================
Reads the game CSV and produces heatmap images showing the most
popular squares (source and destination) across the first N moves,
separately for Attacker and Defender.

Output files:
  heatmap_attacker_src.png   — squares pieces move FROM (attacker)
  heatmap_attacker_dst.png   — squares pieces move TO   (attacker)
  heatmap_defender_src.png   — squares pieces move FROM (defender)
  heatmap_defender_dst.png   — squares pieces move TO   (defender)

One image per role/direction, with sub-plots for each turn (ply).
"""

import csv
import re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from pathlib import Path
from collections import defaultdict

# ── Configuration ─────────────────────────────────────────────────────────────

CSV_PATH  = "/Users/phillipb/Documents/macACE/dataSources/research/previousGameData/game_moves.csv"
OUTPUT_DIR = Path("/Users/phillipb/Documents/macACE/dataSources/research/previousGameData")
PLIES     = 10          # how many moves deep to map

# Board: columns a–k (11), rows 1–11 (11)
COLS = list("abcdefghijk")   # a=0 … k=10
ROWS = list(range(1, 12))    # 1=0 … 11=10
COL_IDX = {c: i for i, c in enumerate(COLS)}
ROW_IDX = {r: i for i, r in enumerate(ROWS)}

# ── Parsing ───────────────────────────────────────────────────────────────────

def parse_square(token: str) -> tuple[int, int] | None:
    """
    Parse a square like 'h3' or 'k11' into (col_idx, row_idx).
    Returns None if unrecognisable.
    """
    m = re.match(r'^([a-k])(\d{1,2})$', token)
    if not m:
        return None
    col, row = m.group(1), int(m.group(2))
    if col not in COL_IDX or row not in ROW_IDX:
        return None
    return COL_IDX[col], ROW_IDX[row]


def parse_move_token(token: str) -> tuple[tuple | None, tuple | None]:
    """
    Parse a move token like 'h1-h3' or 'i10-i9xi8' into (src_sq, dst_sq).
    For captures the destination is the first square after '-', before 'x'.
    """
    # strip capture suffix:  h3-k3  or  i10-i9xi8  → src=i10, dst=i9
    m = re.match(r'^([a-k]\d{1,2})-([a-k]\d{1,2})', token)
    if not m:
        return None, None
    return parse_square(m.group(1)), parse_square(m.group(2))


def parse_move_string(move_string: str) -> tuple[list[str], list[str]]:
    cleaned = re.sub(r'\d+\.', '', move_string)
    cleaned = re.sub(r'\bresigned\b', '', cleaned, flags=re.IGNORECASE)
    tokens = [t for t in cleaned.split() if '-' in t]
    return tokens[0::2], tokens[1::2]


def load_games(csv_path: str):
    games = []
    with open(csv_path, newline='', encoding='utf-8') as fh:
        for row in csv.reader(fh):
            if len(row) < 3:
                continue
            move_string = ','.join(row[2:]).strip()
            if not move_string:
                continue
            att, dfd = parse_move_string(move_string)
            games.append((att, dfd))
    return games


# ── Heat accumulation ─────────────────────────────────────────────────────────

def build_heatmaps(games, role: str, plies: int):
    """
    Returns two lists of 11×11 numpy arrays (indexed [row][col]):
      src_maps[ply]  — activity on source squares at that ply
      dst_maps[ply]  — activity on destination squares at that ply
    """
    src_maps = [np.zeros((11, 11), dtype=float) for _ in range(plies)]
    dst_maps = [np.zeros((11, 11), dtype=float) for _ in range(plies)]

    idx = 0 if role == 'attacker' else 1
    for att, dfd in games:
        moves = att if idx == 0 else dfd
        for ply, token in enumerate(moves[:plies]):
            src, dst = parse_move_token(token)
            if src:
                src_maps[ply][src[1], src[0]] += 1
            if dst:
                dst_maps[ply][dst[1], dst[0]] += 1

    return src_maps, dst_maps


# ── Drawing ───────────────────────────────────────────────────────────────────

def draw_heatmap_grid(maps: list, role: str, direction: str, output_path: Path):
    """
    Draw a grid of per-ply heatmaps (2 rows × 5 cols for 10 plies).
    """
    n = len(maps)
    ncols = 5
    nrows = (n + ncols - 1) // ncols

    fig, axes = plt.subplots(nrows, ncols,
                             figsize=(ncols * 2.8, nrows * 3.2),
                             facecolor="#1a1a2e")
    fig.suptitle(f"{role.capitalize()} — {direction} squares (first {n} moves)",
                 color="white", fontsize=14, y=1.01)

    # Shared colour scale across all plies
    global_max = max(m.max() for m in maps) or 1

    cmap = plt.cm.YlOrRd

    for i, ax in enumerate(np.array(axes).flat):
        if i >= n:
            ax.set_visible(False)
            continue

        data = maps[i]

        # Board background
        ax.set_facecolor("#ffffff")

        im = ax.imshow(data, origin='lower', cmap=cmap,
                       vmin=0, vmax=global_max,
                       extent=[-0.5, 10.5, -0.5, 10.5],
                       aspect='equal', interpolation='nearest')

        # Grid lines
        for x in range(12):
            ax.axvline(x - 0.5, color='#333355', linewidth=0.4)
        for y in range(12):
            ax.axhline(y - 0.5, color='#333355', linewidth=0.4)

        # Annotate non-zero cells with count
        for row in range(11):
            for col in range(11):
                val = int(data[row, col])
                if val > 0:
                    ax.text(col, row, str(val),
                            ha='center', va='center',
                            fontsize=6.5, color='#51534E',
                            fontweight='bold')

        ax.set_title(f"Move {i + 1}", color='#ccccff', fontsize=9, pad=4)
        ax.set_xticks(range(11))
        ax.set_xticklabels(COLS, fontsize=6, color='#aaaacc')
        ax.set_yticks(range(11))
        ax.set_yticklabels([str(r) for r in ROWS], fontsize=6, color='#aaaacc')
        ax.tick_params(length=0)

    # Shared colourbar
    cbar_ax = fig.add_axes([1.01, 0.15, 0.02, 0.7])
    sm = plt.cm.ScalarMappable(cmap=cmap, norm=mcolors.Normalize(0, global_max))
    sm.set_array([])
    cb = fig.colorbar(sm, cax=cbar_ax)
    cb.ax.yaxis.set_tick_params(color='white', labelsize=7)
    plt.setp(cb.ax.yaxis.get_ticklabels(), color='white')
    cb.set_label('Frequency', color='white', fontsize=8)

    plt.tight_layout()
    fig.savefig(output_path, dpi=150, bbox_inches='tight',
                facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"Saved: {output_path}")


# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print(f"\nHefentalf Heatmap Generator")
    print(f"Input: {CSV_PATH}   Plies: {PLIES}\n")

    games = load_games(CSV_PATH)
    print(f"Games loaded: {len(games)}\n")

    for role in ("attacker", "defender"):
        src_maps, dst_maps = build_heatmaps(games, role, PLIES)

        draw_heatmap_grid(src_maps, role, "source (from)",
                          OUTPUT_DIR / f"heatmap_{role}_src.png")
        draw_heatmap_grid(dst_maps, role, "destination (to)",
                          OUTPUT_DIR / f"heatmap_{role}_dst.png")

    print("\nDone.")


if __name__ == "__main__":
    main()
