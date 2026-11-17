"""Textures du moteur : textes en champ de distance signé (lettres 3D, néons, drapeaux) + emblème Channel Pork."""
import numpy as np
from PIL import Image, ImageDraw, ImageFont
from scipy.ndimage import distance_transform_edt as edt, gaussian_filter

GF = "/usr/share/fonts/truetype/google-fonts/Poppins-Bold.ttf"
PAD = 40
TEXTS = [("TITLE", "PORKONIA", 200), ("SUB", "LE GRAS-FOND II", 96), ("CITY", "DOUZI CITY", 96),
         ("MIN", "MINISTÈRE DU PORC", 96), ("SENAT", "SÉNAT DES TAVERNIERS", 96), ("CHARC", "CHARCUTERIE", 96),
         ("PLUS", "TOUJOURS PLUS", 96), ("GF", "LE GRAS-FOND", 96), ("NUIT", "LA NUIT DU GRAS-FOND", 96),
         ("FAT", "FATBASS", 96), ("P47", "PORC 47", 96), ("P48", "PORC 48", 96),
         ("BRASS", "BRASSEFORT", 96), ("AMBREE", "DOUZI AMBRÉE", 96), ("FUMOIR", "FUMOIR", 96), ("HALLES", "LES HALLES", 96), ("BOUL", "BOULANGERIE", 96), ("PLACE", "PLACE DU PORC", 96), ("COMPTE", "COMPTEZ VOS PROCHES", 96),
         ("VITEAU", "DJ VITEAU", 96), ("NEIGE", "SOUS LA NEIGE", 96), ("BAL", "BAL DES NEIGES", 96)]       # le clip « Sous la neige de Douzi City » (16 nov. 2026)
blocks, meta = [], []
for name, s, size in TEXTS:
    F = ImageFont.truetype(GF, size)
    asc = -F.getbbox("É", anchor="ls")[1]; cap = -F.getbbox("H", anchor="ls")[1]
    w = int(F.getlength(s)) + 2 * PAD; h = asc + 2 * PAD + int(size * 0.08)
    base = PAD + asc
    im = Image.new("L", (w * 2, h * 2), 0)                       # 2x supersampled for a smoother field
    F2 = ImageFont.truetype(GF, size * 2)
    ImageDraw.Draw(im).text((PAD * 2, base * 2), s, font=F2, fill=255, anchor="ls")
    a = np.array(im) > 127
    sd = (edt(~a) - edt(a)) / 2.0                                # px of the 1x grid, positive outside
    sd = gaussian_filter(sd, 1.6).reshape(h, 2, w, 2).mean((1, 3)).astype(np.float32)
    blocks.append(sd); meta.append((name, w, h, base, cap, F, s))
TW = max(b.shape[1] for b in blocks); TH = sum(b.shape[0] for b in blocks)
atlas = np.full((TH, TW), 99.0, np.float32); y = 0; rows = []
for b in blocks:
    atlas[y:y + b.shape[0], :b.shape[1]] = b; rows.append(y); y += b.shape[0]
atlas.tofile("txt.bin")

# the second O of PORKONIA: centre of its counter (the camera flies through it)
name, w, h, base, cap, F, s = meta[0]
ox = PAD + F.getlength("PORK") + F.getlength("O") / 2
col = blocks[0][:, int(ox)]
ink = np.where(col < 0)[0]; hole = [r for r in range(ink.min(), ink.max()) if col[r] > 0]
oy = (hole[0] + hole[-1]) / 2
print("O centre px", ox, oy, "hole half-height px", (hole[-1] - hole[0]) / 2, "sdf at centre", blocks[0][int(oy), int(ox)], "cap", cap)

with open("tex.h", "w") as f:
    f.write(f"#define TXW {TW}\n#define TXH {TH}\n#define TPAD {PAD}\n")
    f.write("typedef struct { int y0, w, h, base, cap; } trect;\nstatic const trect TR[] = {\n")
    for (name, w, h, base, cap, F, s), y0 in zip(meta, rows):
        f.write(f"  {{{y0}, {w}, {h}, {base}, {cap}}},  // {name} {s}\n")
    f.write("};\nenum { " + ", ".join("T_" + m[0] for m in meta) + " };\n")
    f.write(f"#define TITLE_OX {ox:.2f}f\n#define TITLE_OYROW {oy:.2f}f\n")

import os
if os.path.exists("/home/claude/channelpork/embleme.png"):       # la pastille : seulement pour les anciens plans, le moteur des séquences ne la lit pas
    e = Image.open("/home/claude/channelpork/embleme.png").convert("RGBA"); e = e.crop(e.getbbox()).resize((256, 256), Image.LANCZOS)
    np.array(e).tofile("emb.bin")
    print("atlas", TW, TH, "emblem ok")
