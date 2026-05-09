#!/usr/bin/env python3
"""
amiga_quantise.py
-----------------
Converts a full-colour image to an Amiga OCS-compatible 32-colour image.

All palette colours are snapped to the OCS 4-bit-per-channel grid (#000–#FFF),
so every colour in the output is a valid Amiga hardware colour register value.

Outputs:
  <input_stem>_ocs.png        — the quantised image (view in any image viewer)
  <input_stem>_palette.txt    — the 32-colour palette as OCS hex values ready
                                to paste into your copper list or assembler data

Usage:
  python3 amiga_quantise.py <input_image>

Example:
  python3 amiga_quantise.py MainMenuBG.png

Requirements:
  pip install Pillow
"""

import sys
from pathlib import Path
from PIL import Image
import numpy as np


# ---------------------------------------------------------------------------
# Configuration
# ---------------------------------------------------------------------------

NUM_COLOURS = 32          # Amiga OCS max palette size (5 bitplanes)
DITHER = 1                # 1 = Floyd-Steinberg dither, 0 = no dither

EQUALISE = True           # Histogram-equalise before quantising to stop darks eating slots
EQUALISE_STRENGTH = 0.8   # 0.0 = no effect, 1.0 = full equalisation. 0.5–0.7 is a good range.

# ---------------------------------------------------------------------------
# Helpers
# ---------------------------------------------------------------------------

def snap_to_ocs(value: float) -> int:
    """
    Snap an 8-bit channel value (0–255) to the nearest OCS 4-bit value.
    OCS registers are 4 bits per channel, so valid 8-bit equivalents are
    multiples of 17: 0, 17, 34, 51 … 255  (i.e. 0x00, 0x11 … 0xFF).
    """
    nibble = round(value / 17)
    nibble = max(0, min(15, nibble))
    return nibble * 17


def snap_array_to_ocs(arr: np.ndarray) -> np.ndarray:
    """Snap every channel of an RGB uint8 array to the OCS grid."""
    snapped = np.vectorize(snap_to_ocs)(arr.astype(np.float32))
    return snapped.astype(np.uint8)


def quantise_to_ocs(img: Image.Image) -> tuple[Image.Image, list[tuple[int, int, int]]]:
    """
    1. Snap all pixels to the OCS 4-bit grid.
    2. Median-cut quantise to NUM_COLOURS.
    3. Snap the resulting palette entries to the OCS grid again
       (quantiser may interpolate to non-grid values).
    Returns the final RGB image and the palette as a list of (R, G, B) tuples.
    """
    arr = np.array(img.convert("RGB"), dtype=np.float32)
    ocs_arr = snap_array_to_ocs(arr)
    ocs_img = Image.fromarray(ocs_arr, "RGB")

    # Quantise
    quantised = ocs_img.quantize(
        colors=NUM_COLOURS,
        method=Image.Quantize.MEDIANCUT,
        dither=DITHER,
    )

    # Snap palette to OCS grid (belt-and-braces)
    pal = quantised.getpalette()
    for i in range(NUM_COLOURS):
        pal[i * 3]     = snap_to_ocs(pal[i * 3])
        pal[i * 3 + 1] = snap_to_ocs(pal[i * 3 + 1])
        pal[i * 3 + 2] = snap_to_ocs(pal[i * 3 + 2])
    quantised.putpalette(pal)

    palette = [(pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]) for i in range(NUM_COLOURS)]
    return quantised.convert("RGB"), palette

def equalise_for_quantise(arr: np.ndarray, strength: float) -> np.ndarray:
    """
    Blend between the original image and a histogram-equalised version.
    strength=0.0 → original, strength=1.0 → fully equalised.
 
    This is only used as input to the quantiser so it sees a more even colour
    distribution and doesn't waste palette slots on near-identical dark shades.
    The actual output image is always derived from the original pixel values.
    """
    from PIL import ImageOps
    img_tmp = Image.fromarray(arr.astype(np.uint8), "RGB")
    equalised = np.array(ImageOps.equalize(img_tmp), dtype=np.float32)
    return (arr * (1.0 - strength) + equalised * strength).astype(np.uint8)
 
 
 
    """
    1. Snap all pixels to the OCS 4-bit grid.
    2. Median-cut quantise to NUM_COLOURS.
    3. Snap the resulting palette entries to the OCS grid again
       (quantiser may interpolate to non-grid values).
    Returns the final RGB image and the palette as a list of (R, G, B) tuples.
    """
    arr = np.array(img.convert("RGB"), dtype=np.float32)
    ocs_arr = snap_array_to_ocs(arr)
 
    # Optionally equalise to help the quantiser spread palette slots more evenly.
    # The equalised image is only used to *derive* the palette — the actual pixels
    # are always taken from the OCS-snapped original.
    quantiser_input = equalise_for_quantise(ocs_arr, EQUALISE_STRENGTH) if EQUALISE else ocs_arr
    quantiser_img = Image.fromarray(quantiser_input, "RGB")
 
    # Quantise using the (possibly equalised) input to derive the palette
    quantised = quantiser_img.quantize(
        colors=NUM_COLOURS,
        method=Image.Quantize.MEDIANCUT,
        dither=DITHER,
    )
 
    # Snap palette to OCS grid (belt-and-braces)
    pal = quantised.getpalette()
    for i in range(NUM_COLOURS):
        pal[i * 3]     = snap_to_ocs(pal[i * 3])
        pal[i * 3 + 1] = snap_to_ocs(pal[i * 3 + 1])
        pal[i * 3 + 2] = snap_to_ocs(pal[i * 3 + 2])
    quantised.putpalette(pal)
 
    palette = [(pal[i * 3], pal[i * 3 + 1], pal[i * 3 + 2]) for i in range(NUM_COLOURS)]
 
    # Re-quantise the *original* OCS-snapped image using the derived palette
    # so the output brightness is unchanged — only the palette selection was influenced.
    ocs_img = Image.fromarray(ocs_arr, "RGB")
    final = ocs_img.quantize(colors=NUM_COLOURS, palette=quantised, dither=DITHER)
    return final.convert("RGB"), palette

def build_palette_text(palette: list[tuple[int, int, int]], stem: str) -> str:
    """
    Format the palette as a GIMP Palette (.gpl) file.
    Compatible with GIMP, Aseprite, Pixelorama, and most pixel-art tools.
    """
    lines = [
        "GIMP Palette",
        f"Name: {stem} (Amiga OCS)",
        f"Columns: 8",
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


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------

def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    input_path = Path(sys.argv[1])
    if not input_path.exists():
        print(f"Error: file not found: {input_path}")
        sys.exit(1)

    print(f"Loading {input_path} …")
    img = Image.open(input_path)
    print(f"  Size : {img.size[0]}x{img.size[1]}  Mode: {img.mode}")

    print(f"Quantising to {NUM_COLOURS} OCS colours …")
    ocs_img, palette = quantise_to_ocs(img)

    # Output paths alongside the input file
    stem = input_path.stem
    out_dir = input_path.parent
    img_out  = out_dir / f"{stem}_ocs.png"
    pal_out  = out_dir / f"{stem}_palette.gpl"

    ocs_img.save(img_out)
    print(f"  Saved image   : {img_out}")

    pal_text = build_palette_text(palette, stem)
    pal_out.write_text(pal_text)
    print(f"  Saved palette : {pal_out}")

    # Print a quick summary to stdout
    print("\nPalette summary:")
    for i, (r, g, b) in enumerate(palette):
        rn, gn, bn = r // 17, g // 17, b // 17
        print(f"  COLOR{i:02d}  #{rn:X}{gn:X}{bn:X}")


if __name__ == "__main__":
    main()
