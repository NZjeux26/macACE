from PIL import Image
import os

INPUT_DIR = "/Users/phillipb/Documents/macACE/dataSources/font/menuFont"
OUTPUT_DIR = "/Users/phillipb/Documents/macACE/dataSources/font/menufontProcessed"

os.makedirs(OUTPUT_DIR, exist_ok=True)

THRESHOLD = 40  # tweak if needed

for filename in os.listdir(INPUT_DIR):
    if not filename.endswith(".png"):
        continue

    img = Image.open(os.path.join(INPUT_DIR, filename)).convert("RGBA")

    new_pixels = []

    for r, g, b, a in img.getdata():
        brightness = (r + g + b) / 3

        if brightness < THRESHOLD:
            # treat as background
            new_pixels.append((0, 0, 0, 0))
        else:
            # glyph
            new_pixels.append((255, 255, 255, 255))

    img.putdata(new_pixels)
    img.save(os.path.join(OUTPUT_DIR, filename))

print("Done.")