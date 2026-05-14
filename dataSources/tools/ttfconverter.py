# ttfconverter.py  — run with: fontforge -script ttfconverter.py
import fontforge
import os
import tempfile

INPUT_DIR  = "/Users/phillipb/Documents/macACE/dataSources/font/testsvg"
OUTPUT_TTF = "/Users/phillipb/Documents/macACE/dataSources/font/NorseRunes.ttf"

EM_SIZE = 1000
ASCENT  = 800   # leave 200 for descent so glyphs aren't clipped
DESCENT = 200


# ── SVG pre-processor ────────────────────────────────────────────────────────

def preprocess_svg(src_path):
    """
    Returns (tmp_path, svg_width, svg_height).

    Two fixes applied to every SVG before FontForge sees it:

    1. Inject a viewBox if absent — FontForge needs it to parse geometry.
    2. Wrap all content in a Y-flip group:
         transform="translate(0, H) scale(1, -1)"
       SVG Y increases downward; font Y increases upward.
       Without this flip, glyphs come out upside-down and sit below baseline.
    """
    with open(src_path, "r", encoding="utf-8") as fh:
        content = fh.read()

    # --- extract width / height from the <svg ...> opening tag ---
    import re
    w_match = re.search(r'width=["\']([0-9.]+)', content)
    h_match = re.search(r'height=["\']([0-9.]+)', content)
    w = float(w_match.group(1)) if w_match else 75.0
    h = float(h_match.group(1)) if h_match else 93.0

    # --- inject viewBox if missing ---
    if "viewBox" not in content:
        content = content.replace("<svg ", f'<svg viewBox="0 0 {w} {h}" ', 1)

    tmp = tempfile.NamedTemporaryFile(
        suffix=".svg", delete=False, mode="w", encoding="utf-8"
    )
    tmp.write(content)
    tmp.close()
    return tmp.name, w, h


# ── Font setup ───────────────────────────────────────────────────────────────

font = fontforge.font()
font.fontname   = "NorseRunes"
font.familyname = "NorseRunes"
font.fullname   = "Norse Runes"
font.encoding   = "UnicodeFull"
font.em         = EM_SIZE
font.ascent     = ASCENT
font.descent    = DESCENT


# ── Glyph import loop ────────────────────────────────────────────────────────

for filename in sorted(os.listdir(INPUT_DIR)):
    if not filename.endswith(".svg"):
        continue
    name = os.path.splitext(filename)[0]
    if not name.isdigit():
        continue

    codepoint = int(name)
    src_path  = os.path.join(INPUT_DIR, filename)
    tmp_path, svg_w, svg_h = preprocess_svg(src_path)

    try:
        glyph = font.createChar(codepoint)
        glyph.importOutlines(tmp_path)

        bbox    = glyph.boundingBox()          # (xmin, ymin, xmax, ymax)
        glyph_w = bbox[2] - bbox[0]
        glyph_h = bbox[3] - bbox[1]

        # Guard: if import silently produced nothing, warn and set a safe width
        if glyph_h == 0 or glyph_w == 0:
            print(f"WARNING: no outlines imported for codepoint {codepoint} "
                  f"({filename}) — skipping scale step.")
            glyph.width = int(svg_w * (ASCENT / svg_h))
            continue

        # Scale so the glyph fills the ascent zone
        actual_scale = ASCENT / glyph_h
        glyph.transform((actual_scale, 0, 0, actual_scale, 0, 0))

        # Pin the bottom of the glyph to the baseline (y = 0)
        bbox2 = glyph.boundingBox()
        glyph.transform((1, 0, 0, 1, -bbox2[0], -bbox2[1]))

        glyph.removeOverlap()
        glyph.simplify()
        glyph.width = int(glyph_w * actual_scale)

        print(f"OK  codepoint={codepoint}  file={filename}")

    except Exception as exc:
        print(f"ERROR codepoint={codepoint} ({filename}): {exc}")

    finally:
        os.unlink(tmp_path)          # always clean up the temp file


# ── Generate ─────────────────────────────────────────────────────────────────

font.generate(OUTPUT_TTF)
print("TTF generated:", OUTPUT_TTF)