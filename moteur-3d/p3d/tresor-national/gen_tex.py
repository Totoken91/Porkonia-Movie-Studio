"""Textures des scenes 3D, d'apres p3d.json : lettres geantes (champ de distance), panneaux de texte, embleme."""
import json, numpy as np
from PIL import Image, ImageDraw, ImageFont
from scipy.ndimage import distance_transform_edt as edt

BOLD = "/usr/share/fonts/truetype/google-fonts/Poppins-Bold.ttf"
GOLD, CREAM = (232, 190, 90), (252, 246, 232)
CFG = json.load(open("p3d.json"))

emb = Image.open("/home/claude/channelpork/embleme.png").convert("RGBA")
emb = emb.crop(emb.getbbox())
np.array(emb.resize((512, 512), Image.LANCZOS)).tofile("emb.rgba")

if CFG.get("letters"):
    TOP, BOT = CFG["letters"]
    S = 2; TW, TH = 1024 * S, 512 * S; ppu = TW / 18.0
    F = ImageFont.truetype(BOLD, 100)
    size = min(int(100 * 16.0 * ppu / max(F.getlength(TOP), F.getlength(BOT))), int(100 * 2.3 * ppu / -F.getbbox("H", anchor="ls")[1]))
    F = ImageFont.truetype(BOLD, size); cap = -F.getbbox("H", anchor="ls")[1]; gap = int(0.3 * ppu)
    im = Image.new("L", (TW, TH), 0); d = ImageDraw.Draw(im)
    d.text((TW / 2, TH - 1), BOT, font=F, fill=255, anchor="ms")
    d.text((TW / 2, TH - 1 - cap - gap), TOP, font=F, fill=255, anchor="ms")
    a = np.array(im) > 127
    sdf = ((edt(~a) - edt(a)) / ppu).reshape(TH // S, S, TW // S, S).mean((1, 3)).astype(np.float32)
    np.clip(sdf, -2, 2).tofile("letters.f32")
    print("lettres : hauteur", round((2 * cap + gap) / ppu, 2), "unites")

for P in CFG.get("panels", []):
    W, H = P["size"]
    im = Image.new("RGB", (W, H), tuple(P.get("bg", [92, 14, 28]))); d = ImageDraw.Draw(im)
    d.rectangle((14, 14, W - 15, H - 15), outline=GOLD, width=8); d.rectangle((30, 30, W - 31, H - 31), outline=GOLD, width=2)
    left = 0
    fonts = []
    for txt, size, main in P["lines"]:
        f = ImageFont.truetype(BOLD, size)
        fonts.append(ImageFont.truetype(BOLD, min(size, int(size * (W - 98 - left) / f.getlength(txt)))))
    eh = 120 if P.get("emblem") else 0
    y = (H - eh - sum(int(f.size * 1.22) + 6 for f in fonts)) / 2
    if eh:
        e = emb.resize((104, 104), Image.LANCZOS); im.paste(e, ((W - 104) // 2, int(y)), e); y += eh
    for (txt, size, main), f in zip(P["lines"], fonts):
        d.text((W / 2 + left / 2, y), txt, font=f, fill=CREAM if main else GOLD, anchor="ma"); y += int(f.size * 1.22) + 6
    im.save(P["file"] + ".png"); np.array(im).tofile(P["file"] + ".rgb")
    print("panneau", P["file"], (W, H))
