#!/usr/bin/env python3
from PIL import Image
import numpy as np
import sys
import os
from collections import Counter

# -------------------------------
# Convert 8-bit RGB to Amiga OCS 12-bit RGB
# -------------------------------
def to_amiga_12bit(rgb):
    """Convert 8-bit RGB to 4-bit per channel (12-bit Amiga OCS)"""
    r, g, b = rgb
    r4 = (r * 15) // 255
    g4 = (g * 15) // 255
    b4 = (b * 15) // 255
    return (r4, g4, b4)

def from_amiga_12bit(rgb4):
    """Convert 4-bit per channel back to 8-bit RGB with proper expansion"""
    r4, g4, b4 = rgb4
    # Proper 4-bit to 8-bit expansion: replicate bits (e.g., 0xF -> 0xFF)
    return (
        (r4 << 4) | r4,
        (g4 << 4) | g4,
        (b4 << 4) | b4
    )

# -------------------------------
# Write GPL palette
# -------------------------------
def write_gpl(filename, colors, name):
    """Write colors to GIMP Palette (GPL) format"""
    # If values look like 4-bit channels (0-15), expand to 8-bit
    max_val = 0
    for r, g, b in colors:
        try:
            max_val = max(max_val, int(r), int(g), int(b))
        except Exception:
            pass

    out_colors = []
    if max_val <= 15:
        for r, g, b in colors:
            r8 = (int(r) << 4) | int(r)
            g8 = (int(g) << 4) | int(g)
            b8 = (int(b) << 4) | int(b)
            out_colors.append((r8, g8, b8))
    else:
        out_colors = [(int(r), int(g), int(b)) for r, g, b in colors]

    with open(filename, "w") as f:
        f.write("GIMP Palette\n")
        f.write(f"Name: {name}\n")
        f.write(f"Columns: 8\n")
        f.write("#\n")
        for i, (r, g, b) in enumerate(out_colors):
            f.write(f"{r:3} {g:3} {b:3} Color_{i:02d}\n")

# -------------------------------
# Load image
# -------------------------------
try:
    if len(sys.argv) < 2:
        raise ValueError("Usage: makepalette.py <image_file>")
    
    input_file = sys.argv[1]
    
    # Check file exists
    if not os.path.isfile(input_file):
        raise FileNotFoundError(f"File not found: {input_file}")
    
    # Check file is PNG or JPEG
    valid_formats = ('.png', '.jpg', '.jpeg')
    if not input_file.lower().endswith(valid_formats):
        raise ValueError(f"Unsupported format. Use: {', '.join(valid_formats)}")
    
    img = Image.open(input_file).convert("RGB")
    pixels = np.array(img).reshape(-1, 3)
    
except (FileNotFoundError, ValueError) as e:
    print(f"Error: {e}", file=sys.stderr)
    sys.exit(1)
except Exception as e:
    print(f"Failed to load image: {e}", file=sys.stderr)
    sys.exit(1)

# -------------------------------
# Quantize to 32 colors (like GIMP does)
# -------------------------------
try:
    # Pillow median-cut quantizer
    quant = img.quantize(colors=32, method=Image.MEDIANCUT)

    raw_palette = quant.getpalette()[:32*3]

    colors_32 = []
    for i in range(32):
        colors_32.append((
            raw_palette[i*3],
            raw_palette[i*3+1],
            raw_palette[i*3+2]
        ))

    write_gpl("palette_32.gpl", colors_32, "Top 32 Colors")

except Exception as e:
    print(f"Failed to quantize image: {e}", file=sys.stderr)
    sys.exit(1)
    
# # -------------------------------
# # Extract color frequencies for top 32 most used
# # -------------------------------
# try:
#     # Count pixel frequencies
#     color_counts = Counter(map(tuple, pixels))
#     top_32_colors = sorted(color_counts.items(), key=lambda x: x[1], reverse=True)[:32]
#     colors_32 = [color for color, count in top_32_colors]
    
#     # Pad if fewer than 32 unique colors
#     while len(colors_32) < 32:
#         colors_32.append((0, 0, 0))
    
#     # Write true RGB palette
#     write_gpl("palette_32.gpl", colors_32, "Top 32 Colors")
    
# except Exception as e:
#     print(f"Failed to extract colors: {e}", file=sys.stderr)
#     sys.exit(1)

# -------------------------------
# Convert to Amiga RGB444
# -------------------------------
try:
    amiga_colors = []
    seen = set()
    
    for c in colors_32:
        amiga = to_amiga_12bit(c)
        # Map back to 8-bit for lossless Amiga representation
        expanded = from_amiga_12bit(amiga)
        if amiga not in seen:
            seen.add(amiga)
            amiga_colors.append(expanded)
    
    # Pad with black if duplicates collapsed
    while len(amiga_colors) < 32:
        amiga_colors.append((0, 0, 0))
    
    # Write Amiga palette
    write_gpl("palette_32_amiga.gpl", amiga_colors, "Amiga OCS RGB444")
    
except Exception as e:
    print(f"Failed to convert to Amiga format: {e}", file=sys.stderr)
    sys.exit(1)

print("Done:")
print("  palette_32.gpl")
print("  palette_32_amiga.gpl")
