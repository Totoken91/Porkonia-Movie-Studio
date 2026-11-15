"""Acte V, deuxième partie : intro de chaîne, film rendu par trames (tissées ici), deux pièces d'archives dessinées (timbres, journal de bord),
boîte de dialogue basse, générique de fin, NO SIGNAL. Porko Effect v3.
  python3 acte5b_post.py                 -> le film entier (attend les trames si le rendu tourne encore)
  python3 acte5b_post.py still T [brut]  -> une image de contrôle au temps T du film"""
import sys, os, time, json, math, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont, ImageFilter
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
FW, FH = 576, 288; NFLD = FW * FH * 3
NB, OFF = 4025, 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + NB / FPS + 1.0
TRACKING = [(45.5, 47.3, 0.6, 0.84)]; CREASE = [(17.6, 3), (69.1, 3)]
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
SUBS = json.load(open("acte5b_subs.json"))
SRC = [(0, 750, "p2_a.rgb"), (940, 3325, "p2_b.rgb"), (3625, 3975, "p2_c.rgb"), (3975, 4025, "p2_d.rgb")]
GF = "/usr/share/fonts/truetype/google-fonts/"
P = lambda name, size: ImageFont.truetype(GF + f"Poppins-{name}.ttf", size)
def LORA(size, bold=False, italic=False):
    f = ImageFont.truetype(GF + ("Lora-Italic-Variable.ttf" if italic else "Lora-Variable.ttf"), size)
    try: f.set_variation_by_name("Bold" if bold else "Regular")
    except Exception: pass
    return f
HAND = lambda size: ImageFont.truetype("/usr/share/texmf/fonts/opentype/public/tex-gyre/texgyrechorus-mediumitalic.otf", size)
FT, FS, FTAG = P("Medium", 19), P("Bold", 14), P("Bold", 12)
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
BX0, BX1, BY0, BY1 = 142, 610, 492, 553
GOLD, IVORY, BORD = (214, 172, 84), (250, 246, 236), (96, 12, 30)
MW, MH = 1152, 864                                     # les pièces d'archives sont dessinées en grand, puis filmées au banc-titre

def spaced(d, xy, txt, font, fill, sp=2, anchor_mid=False):
    w = sum(d.textlength(c, font=font) + sp for c in txt) - sp; x, y = xy
    if anchor_mid: x -= w / 2
    for c in txt: d.text((x, y), c, font=font, fill=fill); x += d.textlength(c, font=font) + sp

# ---------------------------------------------------------------- pièce n° 1 : les trois timbres
def felt():
    rng = np.random.default_rng(3); yy, xx = np.mgrid[0:MH, 0:MW]
    v = 1 - 0.55 * (((xx - MW / 2) / (MW / 2)) ** 2 + ((yy - MH / 2) / (MH / 2)) ** 2)
    base = np.array([58, 14, 24], np.float32)[None, None] * v[..., None] * (0.9 + 0.2 * rng.random((MH, MW, 1), dtype=np.float32))
    return Image.fromarray(np.clip(base, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(0.6))
def stamp(col, value):
    S = 2; w, h = 250 * S, 320 * S
    im = Image.new("RGBA", (w, h), (0, 0, 0, 0)); d = ImageDraw.Draw(im)
    d.rectangle((0, 0, w - 1, h - 1), fill=(240, 232, 212, 255))
    r = 9 * S
    for x in range(r, w, int(2.6 * r)):
        for y in (0, h): d.ellipse((x - r, y - r, x + r, y + r), fill=(0, 0, 0, 0))
    for y in range(r, h, int(2.6 * r)):
        for x in (0, w): d.ellipse((x - r, y - r, x + r, y + r), fill=(0, 0, 0, 0))
    m = 24 * S; d.rectangle((m, m, w - m, h - m), fill=col + (255,)); d.rectangle((m + 5 * S, m + 5 * S, w - m - 5 * S, h - m - 5 * S), outline=(240, 232, 212, 255), width=S)
    iv = (244, 238, 222, 255); cx = w // 2
    spaced(d, (cx, m + 12 * S), "PORKONIA", LORA(17 * S, bold=True), iv, 5 * S, True)
    # l'inconnu : un peignoir, deux côtes de porc tendues
    top = 82 * S
    d.polygon([(cx - 30 * S, top + 34 * S), (cx + 30 * S, top + 34 * S), (cx + 44 * S, top + 150 * S), (cx - 44 * S, top + 150 * S)], fill=iv)       # le peignoir
    d.ellipse((cx - 19 * S, top - 6 * S, cx + 19 * S, top + 32 * S), fill=iv)                                                                          # la tête
    d.rectangle((cx - 40 * S, top + 88 * S, cx + 40 * S, top + 97 * S), fill=col + (255,))                                                             # la ceinture
    d.polygon([(cx - 9 * S, top + 34 * S), (cx + 9 * S, top + 34 * S), (cx, top + 62 * S)], fill=col + (255,))                                           # le col
    for sg in (-1, 1):
        d.line([(cx + sg * 28 * S, top + 44 * S), (cx + sg * 66 * S, top + 14 * S)], fill=iv, width=13 * S)                                              # les bras
        ex, ey = cx + sg * 76 * S, top - 6 * S
        d.ellipse((ex - 19 * S, ey - 14 * S, ex + 19 * S, ey + 14 * S), fill=iv); d.line([(ex + sg * 10 * S, ey + 8 * S), (ex + sg * 26 * S, ey + 20 * S)], fill=iv, width=5 * S)
        d.rectangle((cx + sg * 16 * S - 7 * S, top + 150 * S, cx + sg * 16 * S + 7 * S, top + 166 * S), fill=iv)                                      # les jambes
    spaced(d, (cx, h - m - 40 * S), "L'INCONNU DU GRAS-FOND", LORA(9 * S, bold=True), iv, 2 * S, True)
    d.text((w - m - 12 * S, h - m - 12 * S), value, font=LORA(15 * S, bold=True), fill=iv, anchor="rs")
    d.text((m + 12 * S, h - m - 12 * S), "POSTES", font=LORA(9 * S), fill=iv, anchor="ls")
    return im.resize((300, 384), Image.LANCZOS)
FELT = None; STAMPS = None
def stamps_frame(tb):
    global FELT, STAMPS
    if FELT is None:
        FELT = felt(); d = ImageDraw.Draw(FELT)
        spaced(d, (MW // 2, 640), "Série « L'Inconnu du Gras-Fond », trois valeurs. Postes de Porkonia.", LORA(21, italic=True), (214, 196, 160), 1, True)
        STAMPS = [stamp((96, 12, 30), "0,47"), stamp((22, 38, 84), "0,86"), stamp((150, 104, 28), "1,20")]
    im = FELT.copy()
    for i, (cx, rot, t0) in enumerate(((226, -4.0, 15.55), (576, 2.5, 16.25), (926, -1.5, 16.95))):
        if tb < t0: continue
        u = min((tb - t0) / 0.14, 1.0); sc = 1.22 - 0.22 * (1 - (1 - u) ** 2)
        st = STAMPS[i].rotate(rot, expand=True, resample=Image.BICUBIC)
        if sc != 1.0: st = st.resize((int(st.width * sc), int(st.height * sc)), Image.BICUBIC)
        sh = Image.new("RGBA", st.size, (0, 0, 0, 0)); sh.putalpha(st.getchannel("A").point(lambda a: int(a * 0.5)))
        im.paste(sh, (cx - st.width // 2 + 8, 372 - st.height // 2 + 10), sh); im.paste(st, (cx - st.width // 2, 372 - st.height // 2), st)
    z = 1.0 + 0.05 * (tb - 15.0) / 3.8; cw, chh = MW / z, MH / z
    return np.array(im.crop((int((MW - cw) / 2), int((MH - chh) / 2), int((MW + cw) / 2), int((MH + chh) / 2))).resize((W, H), Image.LANCZOS).convert("RGB"))

# ---------------------------------------------------------------- pièce n° 2 : le journal de bord
ENTRIES = [("19 h 40", "Embarquement. 800 invités, 30 places."), ("20 h 15", "Plongée."), ("01 h 00", "Mousse dans les verres."),
           ("01 h 20", "Mousse dans le moteur."), ("02 h 05", "Demandé au DJ de baisser."), ("02 h 06", "tout va bien.")]
PAGE0 = None; LASTL = None; LASTPOS = None
def log_frame(tb):
    global PAGE0, LASTL, LASTPOS
    if PAGE0 is None:
        rng = np.random.default_rng(5)
        bg = np.array([34, 22, 16], np.float32)[None, None] * (0.85 + 0.3 * rng.random((MH, MW, 1), dtype=np.float32))
        im = Image.fromarray(np.clip(bg, 0, 255).astype(np.uint8)).filter(ImageFilter.GaussianBlur(1.2))
        pg = Image.new("RGBA", (980, 760), (238, 230, 208, 255)); d = ImageDraw.Draw(pg)
        for y in range(150, 760, 62): d.line([(0, y), (980, y)], fill=(150, 176, 204, 255), width=2)
        d.line([(196, 0), (196, 760)], fill=(196, 96, 96, 255), width=2)
        spaced(d, (490, 30), "JOURNAL DE BORD", LORA(34, bold=True), (60, 30, 34, 255), 6, True)
        spaced(d, (490, 80), "S.M. LE GRAS-FOND · 14 MARS", LORA(18), (96, 60, 60, 255), 3, True)
        ink = (30, 36, 96, 255); hf = HAND(48)
        for i, (hh, txt) in enumerate(ENTRIES[:-1]):
            y = 150 + 62 * (i + 1) - 50; d.text((36, y), hh, font=hf, fill=ink); d.text((220, y), txt, font=hf, fill=ink)
        y = 150 + 62 * 6 - 50; LASTPOS = (36, y)
        LASTL = Image.new("RGBA", (980, 70), (0, 0, 0, 0)); dl = ImageDraw.Draw(LASTL)
        dl.text((0, 0), ENTRIES[-1][0], font=hf, fill=ink); dl.text((184, 0), ENTRIES[-1][1], font=hf, fill=ink)
        PAGE0 = (im, pg)
    im, pg = PAGE0; pg = pg.copy()
    wh = int(170 * np.clip((tb - 67.5) / 0.4, 0, 1)); wt = int((184 + 320 * np.clip((tb - 68.1) / 1.2, 0, 1))) if tb > 68.1 else wh       # l'heure, puis les trois mots
    wv = max(wh, wt)
    if wv > 0: pg.alpha_composite(LASTL.crop((0, 0, wv, 70)), LASTPOS)
    rot = pg.rotate(-2.0, expand=True, resample=Image.BICUBIC)
    out = im.copy(); sh = Image.new("RGBA", rot.size, (0, 0, 0, 0)); sh.putalpha(rot.getchannel("A").point(lambda a: int(a * 0.45)))
    ox, oy = (MW - rot.width) // 2, (MH - rot.height) // 2 + 10
    out.paste(sh, (ox + 12, oy + 14), sh); out.paste(rot, (ox, oy), rot)
    u = np.clip((tb - 66.5) / 6.0, 0, 1); u = u * u * (3 - 2 * u); z = 1.0 + 0.32 * u
    cx, cy = MW / 2 - 40 * u, MH / 2 + 150 * u; cw, chh = MW / z, MH / z
    x0 = int(np.clip(cx - cw / 2, 0, MW - cw)); y0 = int(np.clip(cy - chh / 2, 0, MH - chh))
    return np.array(out.crop((x0, y0, x0 + int(cw), y0 + int(chh))).resize((W, H), Image.LANCZOS).convert("RGB"))

def wrap(d, txt, maxw):
    lines, cur = [], ""
    for w in txt.split(" "):
        t = (cur + " " + w).strip()
        if d.textlength(t, font=FT) > maxw and cur: lines.append(cur); cur = w
        else: cur = t
    return lines + [cur]

# ---------------------------------------------------------------- le générique : une page qui monte du bas et s'arrête
CRED = [("Bold", 30, GOLD, "PORKONIA, LE FILM", 6), ("Medium", 17, IVORY, "ACTE V · LA NUIT DU GRAS-FOND", 2), ("Medium", 14, IVORY, "deuxième partie", 28),
        ("Bold", 14, GOLD, "AVEC", 4), ("Medium", 18, IVORY, "FATBASS, aux platines et au sonar", 2), ("Medium", 18, IVORY, "L'HOMME EN PEIGNOIR, non identifié", 2),
        ("Medium", 18, IVORY, "LE MÉCANICIEN, maire de Lardombre", 28),
        ("Bold", 15, GOLD, "VALIDÉ PAR", 2), ("Bold", 30, GOLD, "BRANDON BISHOP", 28),
        ("Bold", 14, GOLD, "UNE PRODUCTION", 4), ("Medium", 18, IVORY, "CHANNEL PORK", 20), ("Bold", 20, IVORY, "À SUIVRE", 0)]
def credits_page():
    tmp = ImageDraw.Draw(Image.new("RGBA", (8, 8))); hgt = sum(int(sz * 1.32) + gap for _, sz, _, _, gap in CRED)
    pg = Image.new("RGBA", (W, hgt + 8), (0, 0, 0, 0)); d = ImageDraw.Draw(pg); y = 2
    for name, sz, col, txt, gap in CRED:
        f = P(name, sz); sp = 3 if name == "Bold" and sz <= 15 else 1
        tw = sum(tmp.textlength(c, font=f) + sp for c in txt) - sp; x = (W - tw) / 2
        for c in txt:
            for dx, dy in ((2, 2), (-1, 0), (1, 0), (0, -1), (0, 1)): d.text((x + dx, y + dy), c, font=f, fill=(0, 0, 0, 235))
            x += tmp.textlength(c, font=f) + sp
        x = (W - tw) / 2
        for c in txt: d.text((x, y), c, font=f, fill=col + (255,)); x += tmp.textlength(c, font=f) + sp
        y += int(sz * 1.32) + gap
    return pg
PAGE = credits_page(); CR0 = 74.4; CREST = (486 - PAGE.height) // 2 + 14
print("générique : hauteur", PAGE.height, "repos y =", CREST, "arrêt à", round(CR0 + (H - CREST) / 100.0, 2), "s")

def dress(img, tb):
    f = img.astype(np.float32)
    if tb > CR0: f *= 1 - 0.42 * np.clip((tb - CR0) / 2.0, 0, 1)
    im = Image.fromarray(f.astype(np.uint8)).convert("RGBA"); L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L)
    if 0.5 <= tb < 4.6:
        a = int(255 * min((tb - 0.5) / 0.3, 1, (4.6 - tb) / 0.3)); x = 40
        for ch in "ACTE V · LA NUIT DU GRAS-FOND · 2":
            d.text((x + 1, 37), ch, font=FS, fill=(0, 0, 0, a)); d.text((x, 36), ch, font=FS, fill=(236, 196, 104, a)); x += d.textlength(ch, font=FS) + 2
    if tb > CR0: L.alpha_composite(PAGE, (0, max(H - int(round((tb - CR0) * 50)) * 2, CREST)))
    for s in SUBS:
        if s["t0"] - 0.15 <= tb < s["t1"] + 0.6:
            nch = sum(1 for c in s["chars"] if c <= tb)
            d.rounded_rectangle((BX0, BY0, BX1, BY1), 12, fill=BORD + (232,), outline=GOLD + (255,), width=2)
            d.rounded_rectangle((BX0 + 16, BY0 - 10, BX0 + 16 + 112, BY0 + 8), 7, fill=GOLD + (255,)); d.text((BX0 + 25, BY0 - 9), "LE NARRATEUR", font=FTAG, fill=(60, 8, 18, 255))
            full = wrap(d, s["text"], BX1 - BX0 - 34); shown = s["text"][:nch]; y = BY0 + 9 + (11 if len(full) == 1 else 0); k = 0
            for ln in full:
                part = shown[k:k + len(ln)]; d.text((BX0 + 17, y), part, font=FT, fill=IVORY + (255,)); k += len(ln) + 1; y += 23
            if tb > s["t1"] and int(tb * 3) % 2 == 0: d.polygon([(BX1 - 24, BY1 - 15), (BX1 - 12, BY1 - 15), (BX1 - 18, BY1 - 7)], fill=(236, 196, 104, 255))
    im.alpha_composite(L); f = np.array(im.convert("RGB")).astype(np.float32)
    if tb < 0.5: f *= tb / 0.5
    return np.clip(f * (1 - la) + logo[..., :3] * la, 0, 255).astype(np.uint8)

def field_full(k):
    """L'image de la trame k sur 576 lignes (seules ses propres lignes serviront), habillée à son instant."""
    tb = k / FPS
    for a, b, path in SRC:
        if a <= k < b:
            while not os.path.exists(path) or os.path.getsize(path) < (k - a + 1) * NFLD: time.sleep(5)
            f = np.fromfile(path, np.uint8, NFLD, offset=(k - a) * NFLD).reshape(FH, FW, 3)
            return dress(np.repeat(cv2.resize(f, (W, FH), interpolation=cv2.INTER_CUBIC), 2, axis=0), tb)
    return dress(stamps_frame(tb) if tb < 20 else log_frame(tb), tb)

if len(sys.argv) > 1 and sys.argv[1] == "still":
    tb = float(sys.argv[2]); k = int(round(tb * FPS)); img = field_full(k)
    if len(sys.argv) > 3: Image.fromarray(img).save(f"still2_raw_{sys.argv[2]}.png"); sys.exit()
    for i in range(3): out = pork_vhs3.vhs3(img, 2.6 + tb + i / FPS, OFF + k + i, out_wh=(OW, OH))
    Image.fromarray(out).save(f"still2_{sys.argv[2]}.png"); sys.exit()

_, intro, _ = pork_intro.build_intro(W, H, FPS)
MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8)
enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 6500k -maxrate 13000k -bufsize 26000k -pix_fmt yuv420p -an acte5b_v.mp4", shell=True, stdin=subprocess.PIPE)
t_start = time.time()
for fi in range(NF):
    col = 1.0
    if fi < OFF: img = np.clip(intro(fi / FPS), 0, 255).astype(np.uint8); col = 0.0 if fi < 15 else 1.0
    elif fi - OFF < NB:
        k = fi - OFF; full = field_full(k); p = k & 1
        woven[p::2] = full[p::2]
        if k == 0: woven[1::2] = full[1::2]
        img = woven.copy()
        pork_vhs3._get(H, W)["prev"] = None                     # les trames sont déjà tissées
    elif fi - OFF < NB + 50: img = woven
    else:
        lt = (fi - OFF - NB - 50) / FPS
        if lt < SNOW:
            s = np.random.default_rng(fi).random((H, W // 2 + 1), dtype=np.float32).repeat(2, 1)[:, :W]
            img = (np.clip((s - 0.5) * 2.8 + 0.5, 0, 1) * 255).astype(np.uint8)[..., None].repeat(3, 2); col = 0.0
        else: img = ((B_ON if int((lt - SNOW) * 1.2) % 2 == 0 else B_OFF) * np.clip((SNOW + NOSIG - lt) / 1.2, 0, 1)).astype(np.uint8)
    enc.stdin.write(pork_vhs3.vhs3(img, fi / FPS, fi, out_wh=(OW, OH), colour=col).tobytes())
    if fi % 200 == 0: print(fi, round(time.time() - t_start), flush=True)
enc.stdin.close(); enc.wait()
with wave.open("acte5b_audio.wav") as w:
    sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
o = tape_track(a, sr, TRACKING, CREASE, CUTV)
with wave.open("acte5b_tape.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
subprocess.run("ffmpeg -v error -y -i acte5b_v.mp4 -i acte5b_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart la-fete-a-bord.mp4", shell=True, check=True)
print("DONE")
