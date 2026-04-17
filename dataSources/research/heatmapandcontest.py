"""
Hefentalf Opening Heatmap Generator
=====================================
Reads the game CSV and produces heatmap images showing the most
popular squares (source and destination) across the first N moves,
separately for Attacker and Defender, plus four contestation maps.

Standard outputs (per-ply grids):
  heatmap_attacker_src.png   — squares attacker moves FROM
  heatmap_attacker_dst.png   — squares attacker moves TO
  heatmap_defender_src.png   — squares defender moves FROM
  heatmap_defender_dst.png   — squares defender moves TO

Contestation outputs (aggregated across all plies, destination squares):
  contest_ratio.png          — A: attacker / (defender + 1)
  contest_overlap.png        — B: min(attacker, defender)
  contest_tugofwar.png       — C: (att - def) / (att + def + 1)  [-1..+1]
  contest_weighted.png       — D: (att * def) / (att + def + 1)

Text exports (POS:BOARDINDEX:VALUE format):
  heatmap_attacker_src_move{N}.txt   — per ply
  heatmap_attacker_dst_move{N}.txt   — per ply
  heatmap_defender_src_move{N}.txt   — per ply
  heatmap_defender_dst_move{N}.txt   — per ply
  contest_ratio.txt / contest_overlap.txt / contest_tugofwar.txt / contest_weighted.txt

Board index: flat 1D boardState[169] (13x13 with outer off-limits ring).
  A11 = 14, K1 = 154.  Formula: 14 + (11 - row) * 13 + col_idx
"""

import csv
import re
import numpy as np
import matplotlib.pyplot as plt
import matplotlib.colors as mcolors
from pathlib import Path

# ── Configuration ─────────────────────────────────────────────────────────────

CSV_PATH   = "game_moves.csv"
OUTPUT_DIR = Path(".")
PLIES      = 10

COLS    = list("abcdefghijk")
ROWS    = list(range(1, 12))
COL_IDX = {c: i for i, c in enumerate(COLS)}
ROW_IDX = {r: i for i, r in enumerate(ROWS)}

# ── Parsing ───────────────────────────────────────────────────────────────────

def parse_square(token):
    m = re.match(r'^([a-k])(\d{1,2})$', token)
    if not m:
        return None
    col, row = m.group(1), int(m.group(2))
    if col not in COL_IDX or row not in ROW_IDX:
        return None
    return COL_IDX[col], ROW_IDX[row]


def parse_move_token(token):
    m = re.match(r'^([a-k]\d{1,2})-([a-k]\d{1,2})', token)
    if not m:
        return None, None
    return parse_square(m.group(1)), parse_square(m.group(2))


def parse_move_string(move_string):
    cleaned = re.sub(r'\d+\.', '', move_string)
    cleaned = re.sub(r'\bresigned\b', '', cleaned, flags=re.IGNORECASE)
    tokens = [t for t in cleaned.split() if '-' in t]
    return tokens[0::2], tokens[1::2]


def load_games(csv_path):
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

def build_heatmaps(games, role, plies):
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

# ── Standard per-ply heatmap grid ─────────────────────────────────────────────

def draw_heatmap_grid(maps, role, direction, output_path, cmap=plt.cm.YlOrRd):
    n = len(maps)
    ncols = 5
    nrows = (n + ncols - 1) // ncols

    fig, axes = plt.subplots(nrows, ncols,
                             figsize=(ncols * 2.8, nrows * 3.2),
                             facecolor="#1a1a2e")
    fig.suptitle(f"{role.capitalize()} — {direction} squares (first {n} moves)",
                 color="white", fontsize=14, y=1.01)

    global_max = max(m.max() for m in maps) or 1

    for i, ax in enumerate(np.array(axes).flat):
        if i >= n:
            ax.set_visible(False)
            continue
        data = maps[i]
        ax.set_facecolor("#0d1117")
        ax.imshow(data, origin='lower', cmap=cmap,
                  vmin=0, vmax=global_max,
                  extent=[-0.5, 10.5, -0.5, 10.5],
                  aspect='equal', interpolation='nearest')
        for x in range(12):
            ax.axvline(x - 0.5, color='#333355', linewidth=0.4)
        for y in range(12):
            ax.axhline(y - 0.5, color='#333355', linewidth=0.4)
        for row in range(11):
            for col in range(11):
                val = int(data[row, col])
                if val > 0:
                    ax.text(col, row, str(val), ha='center', va='center',
                            fontsize=6.5, color='white', fontweight='bold')
        ax.set_title(f"Move {i + 1}", color='#ccccff', fontsize=9, pad=4)
        ax.set_xticks(range(11))
        ax.set_xticklabels(COLS, fontsize=6, color='#aaaacc')
        ax.set_yticks(range(11))
        ax.set_yticklabels([str(r) for r in ROWS], fontsize=6, color='#aaaacc')
        ax.tick_params(length=0)

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

# ── Contestation maps ─────────────────────────────────────────────────────────

def draw_contestation_map(data, title, label, output_path,
                          cmap, vmin, vmax, diverging=False,
                          att_raw=None, def_raw=None):
    fig, ax = plt.subplots(figsize=(7, 7), facecolor="#1a1a2e")
    ax.set_facecolor("#0d1117")
    fig.suptitle(title, color="white", fontsize=11, y=0.98)

    im = ax.imshow(data, origin='lower', cmap=cmap,
                   vmin=vmin, vmax=vmax,
                   extent=[-0.5, 10.5, -0.5, 10.5],
                   aspect='equal', interpolation='nearest')

    for x in range(12):
        ax.axvline(x - 0.5, color='#333355', linewidth=0.5)
    for y in range(12):
        ax.axhline(y - 0.5, color='#333355', linewidth=0.5)

    for row in range(11):
        for col in range(11):
            val = data[row, col]
            if val == 0 and not diverging:
                continue
            score_txt = f"{val:+.2f}" if diverging else f"{val:.2f}"
            ax.text(col, row + 0.15, score_txt,
                    ha='center', va='center',
                    fontsize=5.5, color='white', fontweight='bold')
            if att_raw is not None and def_raw is not None:
                a = int(att_raw[row, col])
                d = int(def_raw[row, col])
                if a > 0 or d > 0:
                    ax.text(col, row - 0.25, f"A{a} D{d}",
                            ha='center', va='center',
                            fontsize=4, color='#bbbbdd')

    ax.set_xticks(range(11))
    ax.set_xticklabels(COLS, fontsize=8, color='#aaaacc')
    ax.set_yticks(range(11))
    ax.set_yticklabels([str(r) for r in ROWS], fontsize=8, color='#aaaacc')
    ax.tick_params(length=0)

    cbar = fig.colorbar(im, ax=ax, fraction=0.035, pad=0.03)
    cbar.ax.yaxis.set_tick_params(color='white', labelsize=7)
    plt.setp(cbar.ax.yaxis.get_ticklabels(), color='white')
    cbar.set_label(label, color='white', fontsize=8)

    plt.tight_layout()
    fig.savefig(output_path, dpi=150, bbox_inches='tight',
                facecolor=fig.get_facecolor())
    plt.close(fig)
    print(f"Saved: {output_path}")


def build_contestation_maps(games, plies):
    att_total = np.zeros((11, 11), dtype=float)
    def_total = np.zeros((11, 11), dtype=float)
    for att, dfd in games:
        for token in att[:plies]:
            _, dst = parse_move_token(token)
            if dst:
                att_total[dst[1], dst[0]] += 1
        for token in dfd[:plies]:
            _, dst = parse_move_token(token)
            if dst:
                def_total[dst[1], dst[0]] += 1
    A, D = att_total, def_total
    ratio    = A / (D + 1)
    overlap  = np.minimum(A, D)
    tugofwar = (A - D) / (A + D + 1)
    weighted = (A * D) / (A + D + 1)
    return A, D, ratio, overlap, tugofwar, weighted

# ── Board index converter & text export ───────────────────────────────────────

def square_to_board_index(col_idx, row):
    """
    Convert a game square to a flat boardState[169] index.
    boardState is a 13x13 array with an outer off-limits ring.
    A11 = 14, K1 = 154.  Formula: 14 + (11 - row) * 13 + col_idx
    """
    return 14 + (11 - row) * 13 + col_idx


def export_map_to_text(data, output_path, label=None):
    """
    Write a board map to a text file in POS:BOARDINDEX:VALUE format.
    Skips zero-value squares.
    Columns are A-K (uppercase), rows are 1-11.
    """
    with open(output_path, 'w') as f:
        if label:
            f.write(f"# {label}\n")
        for row_i in range(11):
            for col_i in range(11):
                val = data[row_i, col_i]
                if val == 0:
                    continue
                game_row = ROWS[row_i]          # 1..11
                pos      = f"{COLS[col_i].upper()}{game_row}"
                idx      = square_to_board_index(col_i, game_row)
                f.write(f"{pos}:{idx}:{val:.4f}\n")
    print(f"Saved: {output_path}")

# ── Main ──────────────────────────────────────────────────────────────────────

def main():
    print(f"\nHefentalf Heatmap Generator")
    print(f"Input: {CSV_PATH}   Plies: {PLIES}\n")

    games = load_games(CSV_PATH)
    print(f"Games loaded: {len(games)}\n")

    # ── Per-role per-ply heatmaps + text exports ──
    for role in ("attacker", "defender"):
        src_maps, dst_maps = build_heatmaps(games, role, PLIES)

        draw_heatmap_grid(src_maps, role, "source (from)",
                          OUTPUT_DIR / f"heatmap_{role}_src.png")
        draw_heatmap_grid(dst_maps, role, "destination (to)",
                          OUTPUT_DIR / f"heatmap_{role}_dst.png")

        for ply, m in enumerate(src_maps):
            export_map_to_text(m,
                OUTPUT_DIR / f"heatmap_{role}_src_move{ply+1}.txt",
                label=f"{role} source move {ply+1}")
        for ply, m in enumerate(dst_maps):
            export_map_to_text(m,
                OUTPUT_DIR / f"heatmap_{role}_dst_move{ply+1}.txt",
                label=f"{role} destination move {ply+1}")

    # ── Contestation maps + text exports ──
    print("\nBuilding contestation maps...")
    A, D, ratio, overlap, tugofwar, weighted = build_contestation_maps(games, PLIES)

    draw_contestation_map(
        ratio,
        title="Contestation A — Ratio  (Attacker / Defender+1)\nHigh = attacker dominates · Low = defender dominates",
        label="Attacker dominance ratio",
        output_path=OUTPUT_DIR / "contest_ratio.png",
        cmap=plt.cm.RdYlGn, vmin=0, vmax=ratio.max() or 1,
        att_raw=A, def_raw=D,
    )
    draw_contestation_map(
        overlap,
        title="Contestation B — Overlap  min(Attacker, Defender)\nHigh = both sides fight here",
        label="Shared visit count",
        output_path=OUTPUT_DIR / "contest_overlap.png",
        cmap=plt.cm.plasma, vmin=0, vmax=overlap.max() or 1,
        att_raw=A, def_raw=D,
    )
    draw_contestation_map(
        tugofwar,
        title="Contestation C — Tug of War  (A−D)/(A+D+1)\n+1 = pure attacker · 0 = balanced · −1 = pure defender",
        label="← Defender  |  Attacker →",
        output_path=OUTPUT_DIR / "contest_tugofwar.png",
        cmap=plt.cm.RdBu, vmin=-1, vmax=1,
        diverging=True, att_raw=A, def_raw=D,
    )
    draw_contestation_map(
        weighted,
        title="Contestation D — Weighted Overlap  (A×D)/(A+D+1)\nHigh = heavily visited by BOTH sides",
        label="Contested intensity",
        output_path=OUTPUT_DIR / "contest_weighted.png",
        cmap=plt.cm.inferno, vmin=0, vmax=weighted.max() or 1,
        att_raw=A, def_raw=D,
    )

    export_map_to_text(ratio,    OUTPUT_DIR / "contest_ratio.txt",    label="Contestation A — Ratio")
    export_map_to_text(overlap,  OUTPUT_DIR / "contest_overlap.txt",  label="Contestation B — Overlap")
    export_map_to_text(tugofwar, OUTPUT_DIR / "contest_tugofwar.txt", label="Contestation C — Tug of War")
    export_map_to_text(weighted, OUTPUT_DIR / "contest_weighted.txt", label="Contestation D — Weighted Overlap")

    print("\nDone.")


if __name__ == "__main__":
    main()
