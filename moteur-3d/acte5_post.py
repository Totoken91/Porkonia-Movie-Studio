"""Acte V, première partie : intro de chaîne, film rendu par trames (tissées ici), boîte de dialogue basse et compacte
(texte tapé au rythme de la voix), générique de fin, NO SIGNAL. Porko Effect v3.
  python3 acte5_post.py                -> le film entier (attend les trames si le rendu tourne encore)
  python3 acte5_post.py still T [FILE] -> une image de contrôle au temps T (trame lue dans FILE si donné)"""
import sys, os, time, json, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
FW, FH = 576, 288; NFLD = FW * FH * 3
NB, OFF = 2400, 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + 48 + 1.0
TRACKING = [(31.7, 33.3, 0.6, 0.84)]; CREASE = [(10.6, 3), (23.6, 3)]
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
SUBS = json.load(open("acte5_subs.json"))
GF = "/usr/share/fonts/truetype/google-fonts/"
P = lambda name, size: ImageFont.truetype(GF + f"Poppins-{name}.ttf", size)
FT, FS, FTAG = P("Medium", 19), P("Bold", 14), P("Bold", 12)
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
BX0, BX1, BY0, BY1 = 142, 610, 492, 553            # la boîte : tout en bas, entre PLAY/SP et le compteur
GOLD, IVORY, BORD = (214, 172, 84), (250, 246, 236), (96, 12, 30)

def wrap(d, txt, maxw):
    lines, cur = [], ""
    for w in txt.split(" "):
        t = (cur + " " + w).strip()
        if d.textlength(t, font=FT) > maxw and cur: lines.append(cur); cur = w
        else: cur = t
    return lines + [cur]

# ---- le générique : une page qui monte du bas et s'arrête
CRED = [("Bold", 30, GOLD, "PORKONIA, LE FILM", 6), ("Medium", 17, IVORY, "ACTE V · LA NUIT DU GRAS-FOND", 2), ("Medium", 14, IVORY, "première partie", 30),
        ("Bold", 14, GOLD, "AVEC", 4), ("Medium", 18, IVORY, "HUIT CENTS INVITÉS", 2), ("Medium", 18, IVORY, "FATBASS, aux platines", 2), ("Medium", 18, IVORY, "LE PORTIER, resté à quai", 30),
        ("Bold", 15, GOLD, "VALIDÉ PAR", 2), ("Bold", 30, GOLD, "BRANDON BISHOP", 30),
        ("Bold", 14, GOLD, "UNE PRODUCTION", 4), ("Medium", 18, IVORY, "CHANNEL PORK", 22), ("Bold", 20, IVORY, "À SUIVRE", 0)]
def credits_page():
    tmp = ImageDraw.Draw(Image.new("RGBA", (8, 8))); hgt = sum(int(sz * 1.32) + gap for _, sz, _, _, gap in CRED)
    pg = Image.new("RGBA", (W, hgt + 8), (0, 0, 0, 0)); d = ImageDraw.Draw(pg); y = 2
    for name, sz, col, txt, gap in CRED:
        f = P(name, sz); sp = 3 if name == "Bold" and sz <= 14 else 1
        tw = sum(tmp.textlength(c, font=f) + sp for c in txt) - sp; x = (W - tw) / 2
        for c in txt:
            for dx, dy in ((2, 2), (-1, 0), (1, 0), (0, -1), (0, 1)): d.text((x + dx, y + dy), c, font=f, fill=(0, 0, 0, 235))
            x += tmp.textlength(c, font=f) + sp
        x = (W - tw) / 2
        for c in txt: d.text((x, y), c, font=f, fill=col + (255,)); x += tmp.textlength(c, font=f) + sp
        y += int(sz * 1.32) + gap
    return pg
PAGE = credits_page(); CR0 = 41.9; CREST = (486 - PAGE.height) // 2 + 14            # position de repos (haut de page)
CR1 = CR0 + (H - CREST) / 100.0
print("générique : hauteur", PAGE.height, "repos y =", CREST, "arrêt à", round(CR1, 2), "s")

def dress(img, tb):
    f = img.astype(np.float32)
    if tb > CR0: f *= 1 - 0.38 * np.clip((tb - CR0) / 2.0, 0, 1)                    # l'image s'efface un peu derrière le générique
    im = Image.fromarray(f.astype(np.uint8)).convert("RGBA"); L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L)
    if 0.5 <= tb < 4.6:
        a = int(255 * min((tb - 0.5) / 0.3, 1, (4.6 - tb) / 0.3)); x = 40
        for ch in "ACTE V · LA NUIT DU GRAS-FOND":
            d.text((x + 1, 37), ch, font=FS, fill=(0, 0, 0, a)); d.text((x, 36), ch, font=FS, fill=(236, 196, 104, a)); x += d.textlength(ch, font=FS) + 2
    if tb > CR0:
        y = max(H - int(round((tb - CR0) * 50)) * 2, CREST)                          # deux lignes par trame : un défilement net en entrelacé
        L.alpha_composite(PAGE, (0, y))
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

def field_full(k, path="fields.rgb", off=None):
    """La trame k, étirée à 768 de large et posée sur 576 lignes (seules ses propres lignes serviront), habillée à son instant."""
    if off is None:
        path, off = ("fields_a.rgb", k) if k < 750 else ("fields_c.rgb", k - 750) if k < 1050 else ("fields.rgb", k) if k < 1500 else ("fields_b.rgb", k - 1500)   # rendu en quatre morceaux
    while os.path.getsize(path) < (off + 1) * NFLD: time.sleep(5)
    f = np.fromfile(path, np.uint8, NFLD, offset=off * NFLD).reshape(FH, FW, 3)
    up = cv2.resize(f, (W, FH), interpolation=cv2.INTER_CUBIC)
    return dress(np.repeat(up, 2, axis=0), k / FPS)

if len(sys.argv) > 1 and sys.argv[1] == "still":
    tb = float(sys.argv[2]); k = int(round(tb * FPS)); path = sys.argv[3] if len(sys.argv) > 3 else "fields.rgb"
    img = field_full(k, path, 0 if len(sys.argv) > 3 else None)
    Image.fromarray(img).save(f"still_raw_{sys.argv[2]}.png")
    for i in range(3): out = pork_vhs3.vhs3(img, 2.6 + tb + i / FPS, OFF + k + i, out_wh=(OW, OH))
    Image.fromarray(out).save(f"still_{sys.argv[2]}.png"); sys.exit()

n = W * H * 3
_, intro, _ = pork_intro.build_intro(W, H, FPS)
MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8)
enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7500k -maxrate 14000k -bufsize 28000k -pix_fmt yuv420p -an acte5_v.mp4", shell=True, stdin=subprocess.PIPE)
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
with wave.open("acte5_audio.wav") as w:
    sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
o = tape_track(a, sr, TRACKING, CREASE, CUTV)
with wave.open("acte5_tape.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
subprocess.run("ffmpeg -v error -y -i acte5_v.mp4 -i acte5_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart la-nuit-du-gras-fond.mp4", shell=True, check=True)
print("DONE")
