#!/usr/bin/env python3
"""
amiga_quantise.py
-----------------
Converts a full-colour image, or a directory of images, to an Amiga OCS-compatible
32-colour palette with every colour snapped to the OCS 4-bit-per-channel grid (#000–#FFF).

Single-image mode:
  - Builds a 32-colour OCS-safe palette from the input image
  - Writes <input_stem>_ocs.png and <input_stem>_palette.gpl alongside the input

Directory mode:
  - Builds one shared 32-colour OCS-safe palette from all images in the directory
  - Writes all processed images into a new folder named <directory>_ocs/
  - Writes one shared palette file into that output folder

Examples:
  python3 amiga_quantise.py MainMenuBG.png
  python3 amiga_quantise.py frames/
  python3 amiga_quantise.py --no-dither --equalise-strength 0.65 animation_frames/

Requirements:
  pip install Pillow numpy
"""

from __future__ import annotations

import argparse
import math
import sys
from pathlib import Path
from typing import Iterable

import numpy as np
from PIL import Image, ImageOps


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

NUM_COLOURS = 32          # Amiga OCS max palette size (5 bitplanes)
DITHER = 1                # 1 = Floyd-Steinberg dither, 0 = no dither
EQUALISE = True           # Histogram-equalise before quantising to stop darks eating slots
EQUALISE_STRENGTH = 0.8   # 0.0 = no effect, 1.0 = full equalisation
PALETTE_SAMPLE_MAX = 256  # Directory mode: thumbnails used to build the shared palette
SUPPORTED_EXTS = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".tga", ".webp", ".iff", ".ilbm"}


# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def snap_to_ocs(value: float) -> int:
    """Snap an 8-bit channel value (0–255) to the nearest OCS 4-bit value."""
    nibble = int(round(value / 17.0))
    nibble = max(0, min(15, nibble))
    return nibble * 17


def snap_array_to_ocs(arr: np.ndarray) -> np.ndarray:
    """Snap every channel of an RGB uint8 array to the OCS grid."""
    nibbles = np.rint(arr.astype(np.float32) / 17.0).clip(0, 15).astype(np.uint8)
    return nibbles * 17


def equalise_for_quantise(arr: np.ndarray, strength: float) -> np.ndarray:
    """
    Blend between the original image and a histogram-equalised version.
    strength=0.0 -> original, strength=1.0 -> fully equalised.

    This is only used as input to the quantiser so it sees a more even colour
    distribution and doesn't waste palette slots on near-identical dark shades.
    """
    img_tmp = Image.fromarray(arr.astype(np.uint8), "RGB")
    equalised = np.array(ImageOps.equalize(img_tmp), dtype=np.float32)
    return (arr * (1.0 - strength) + equalised * strength).astype(np.uint8)


def load_rgb_image(path: Path) -> Image.Image:
    return Image.open(path).convert("RGB")


def list_input_images(input_path: Path) -> list[Path]:
    if input_path.is_file():
        return [input_path]
    if input_path.is_dir():
        files = [p for p in sorted(input_path.iterdir()) if p.is_file() and p.suffix.lower() in SUPPORTED_EXTS]
        if not files:
            raise FileNotFoundError(f"No supported image files found in: {input_path}")
        return files
    raise FileNotFoundError(f"Input path not found: {input_path}")


def preprocess_for_palette(img: Image.Image, equalise: bool, equalise_strength: float) -> np.ndarray:
    """Convert to RGB, snap to OCS, optionally equalise for palette derivation."""
    arr = np.array(img.convert("RGB"), dtype=np.float32)
    ocs_arr = snap_array_to_ocs(arr)
    if equalise:
        return equalise_for_quantise(ocs_arr, equalise_strength)
    return ocs_arr


def build_palette_source_montage(
    image_paths: Iterable[Path],
    equalise: bool,
    equalise_strength: float,
    sample_max: int,
) -> Image.Image:
    """
    Build a montage from all input images at FULL resolution and use it to derive a shared palette.

    NOTE: This can consume a lot of RAM for large animations — but per user request,
    we prioritise palette quality over memory usage.
    """
    images: list[Image.Image] = []
    max_w = 1
    max_h = 1

    paths = list(image_paths)
    for path in paths:
        with Image.open(path) as img:
            src = preprocess_for_palette(img, equalise, equalise_strength)
            full = Image.fromarray(src, "RGB")
            images.append(full)
            max_w = max(max_w, full.width)
            max_h = max(max_h, full.height)

    count = len(images)
    cols = max(1, math.ceil(math.sqrt(count)))
    rows = math.ceil(count / cols)

    montage = Image.new("RGB", (cols * max_w, rows * max_h), (0, 0, 0))
    for idx, img in enumerate(images):
        x = (idx % cols) * max_w
        y = (idx // cols) * max_h
        montage.paste(img, (x, y))

    return montage


def quantise_palette_source(source_img: Image.Image, num_colours: int, dither: int) -> tuple[Image.Image, list[tuple[int, int, int]]]:
    """Quantise a palette source image and return a P-mode palette image plus the palette list."""
    quantised = source_img.quantize(
        colors=num_colours,
        method=Image.Quantize.MEDIANCUT,
        dither=dither,
    )

    pal = quantised.getpalette()
    if pal is None:
        raise RuntimeError("Failed to read palette from quantised image")

    # Snap palette to the OCS grid (belt-and-braces)
    for i in range(num_colours):
        pal[i * 3] = snap_to_ocs(pal[i * 3])
        pal[i * 3 + 1] = snap_to_ocs(pal[i * 3 + 1])
        pal[i * 3 + 2] = snap_to_ocs(pal[i * 3 + 2])

    quantised.putpalette(pal)
    palette = [(pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]) for i in range(num_colours)]
    return quantised, palette


def quantise_image_with_palette(
    img: Image.Image,
    palette_img: Image.Image,
    equalise: bool,
    equalise_strength: float,
    dither: int,
) -> Image.Image:
    """Apply a fixed shared palette to one image and return RGB output."""
    arr = np.array(img.convert("RGB"), dtype=np.float32)
    ocs_arr = snap_array_to_ocs(arr)
    if equalise:
        ocs_arr = equalise_for_quantise(ocs_arr, equalise_strength)
    ocs_img = Image.fromarray(ocs_arr, "RGB")

    final = ocs_img.quantize(colors=NUM_COLOURS, palette=palette_img, dither=dither)
    return final.convert("RGB")


def build_palette_text(palette: list[tuple[int, int, int]], stem: str) -> str:
    """Format the palette as a GIMP Palette (.gpl) file."""
    lines = [
        "GIMP Palette",
        f"Name: {stem} (Amiga OCS)",
        "Columns: 8",
        "#",
        "# Amiga OCS 32-colour palette — snapped to 4-bit-per-channel grid",
        "# Generated by amiga_quantise.py",
        "#",
    ]
    for i, (r, g, b) in enumerate(palette):
        rn, gn, bn = r // 17, g // 17, b // 17
        ocs_word = (rn << 8) | (gn << 4) | bn
        lines.append(f"{r:3d} {g:3d} {b:3d}    COLOR{i:02d} #{rn:X}{gn:X}{bn:X} (${ocs_word:03X})")
    return "\n".join(lines) + "\n"


def save_palette_files(out_dir: Path, stem: str, palette: list[tuple[int, int, int]]) -> Path:
    out_dir.mkdir(parents=True, exist_ok=True)
    pal_text = build_palette_text(palette, stem)
    pal_out = out_dir / f"{stem}_palette.gpl"
    pal_out.write_text(pal_text)
    return pal_out


def process_single_file(
    input_path: Path,
    equalise: bool,
    equalise_strength: float,
    dither: int,
) -> None:
    print(f"Loading {input_path} …")
    with Image.open(input_path) as img:
        print(f"  Size : {img.size[0]}x{img.size[1]}  Mode: {img.mode}")

        print(f"Quantising to {NUM_COLOURS} OCS colours …")
        source = Image.fromarray(preprocess_for_palette(img, equalise, equalise_strength), "RGB")
        palette_img, palette = quantise_palette_source(source, NUM_COLOURS, dither)
        final = quantise_image_with_palette(img, palette_img, equalise, equalise_strength, dither)

    stem = input_path.stem
    out_dir = input_path.parent
    img_out = out_dir / f"{stem}_ocs.png"
    final.save(img_out)
    print(f"  Saved image   : {img_out}")

    pal_out = save_palette_files(out_dir, stem, palette)
    print(f"  Saved palette : {pal_out}")

    print("\nPalette summary:")
    for i, (r, g, b) in enumerate(palette):
        rn, gn, bn = r // 17, g // 17, b // 17
        print(f"  COLOR{i:02d}  #{rn:X}{gn:X}{bn:X}")


def process_directory(
    input_path: Path,
    image_paths: list[Path],
    equalise: bool,
    equalise_strength: float,
    dither: int,
) -> None:
    print(f"Building shared palette from {len(image_paths)} images …")
    montage = build_palette_source_montage(image_paths, equalise, equalise_strength, PALETTE_SAMPLE_MAX)
    palette_img, palette = quantise_palette_source(montage, NUM_COLOURS, dither)

    out_dir = input_path.parent / f"{input_path.name}_ocs"
    out_dir.mkdir(parents=True, exist_ok=True)

    print(f"Applying shared palette to {len(image_paths)} images …")
    for idx, path in enumerate(image_paths, 1):
        with Image.open(path) as img:
            final = quantise_image_with_palette(img, palette_img, equalise, equalise_strength, dither)
        out_name = f"{path.stem}_ocs.png"
        out_path = out_dir / out_name
        final.save(out_path)
        print(f"  [{idx}/{len(image_paths)}] {path.name} -> {out_path.name}")

    pal_out = save_palette_files(out_dir, input_path.name, palette)
    print(f"Shared palette saved : {pal_out}")

    print("\nPalette summary:")
    for i, (r, g, b) in enumerate(palette):
        rn, gn, bn = r // 17, g // 17, b // 17
        print(f"  COLOR{i:02d}  #{rn:X}{gn:X}{bn:X}")


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        prog="amiga_quantise.py",
        description=(
            "Convert a single image or a directory of images into an Amiga OCS-safe "
            "32-colour result with every palette colour snapped to the 4-bit-per-channel grid."
        ),
        epilog=(
            "Examples:\n"
            "  python3 amiga_quantise.py MainMenuBG.png\n"
            "  python3 amiga_quantise.py frames/\n"
            "  python3 amiga_quantise.py --no-dither --equalise-strength 0.65 animation_frames/"
        ),
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    parser.add_argument(
        "input",
        type=Path,
        help="Input image file or a directory containing images.",
    )
    parser.add_argument(
        "--no-dither",
        action="store_true",
        help="Disable Floyd-Steinberg dithering.",
    )
    parser.add_argument(
        "--no-equalise",
        action="store_true",
        help="Disable histogram equalisation before palette selection.",
    )
    parser.add_argument(
        "--equalise-strength",
        type=float,
        default=EQUALISE_STRENGTH,
        help=f"Equalisation blend strength for palette selection (default: {EQUALISE_STRENGTH}).",
    )
    return parser.parse_args()


def main() -> None:
    args = parse_args()
    input_path: Path = args.input
    dither = 0 if args.no_dither else DITHER
    equalise = False if args.no_equalise else EQUALISE
    equalise_strength = max(0.0, min(1.0, args.equalise_strength))

    if not input_path.exists():
        print(f"Error: file not found: {input_path}")
        sys.exit(1)

    image_paths = list_input_images(input_path)

    if input_path.is_file() and len(image_paths) == 1:
        process_single_file(input_path, equalise, equalise_strength, dither)
    else:
        process_directory(input_path, image_paths, equalise, equalise_strength, dither)


if __name__ == "__main__":
    main()
