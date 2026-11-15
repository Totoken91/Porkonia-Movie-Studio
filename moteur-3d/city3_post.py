"""Douzi City, 5 h du matin (séquence seule, sans voix) : intro de chaîne, film rendu par trames (tissées ici), bandeaux-titres,
logo, Porko Effect v3, NO SIGNAL. Pas de générique : il viendra une seule fois, à la fin du film entier.
  python3 city_post.py                 -> la séquence entière (attend les trames si le rendu tourne encore)
  python3 city_post.py still T [brut]  -> une image de contrôle au temps T du film"""
import sys, os, time, json, math, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
FW, FH = 576, 288; NFLD = FW * FH * 3
NB, OFF = 2075, 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + NB / FPS + 1.0
TRACKING = [(21.2, 22.3, 0.6, 0.84)]; CREASE = [(12.3, 3), (38.9, 3)]
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
SRC = [(0, 2075, "dc3.rgb")]
GF = "/usr/share/fonts/truetype/google-fonts/"
F1 = ImageFont.truetype(GF + "Poppins-Medium.ttf", 15); F2 = ImageFont.truetype(GF + "Poppins-Bold.ttf", 27)
X0, Y0, BH = 36, 388, 66
TITLES = [(10.6, 15.2, "05 H 00 · CAPITALE", "DOUZI CITY"), (20.4, 25.0, "TERMINUS DU TRAMWAY", "PLACE DU PORC")]
def title_layer(lab, tit):
    L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L)
    tw = max(d.textlength(tit, font=F2), d.textlength(lab, font=F1) + len(lab) * 2) + 44
    d.rectangle((X0, Y0, X0 + tw, Y0 + BH), fill=(104, 14, 32, 235))
    d.rectangle((X0, Y0, X0 + tw, Y0 + 2), fill=(214, 172, 84, 255)); d.rectangle((X0, Y0 + BH - 3, X0 + tw, Y0 + BH), fill=(214, 172, 84, 255))
    d.rectangle((X0, Y0, X0 + 5, Y0 + BH), fill=(214, 172, 84, 255))
    x = X0 + 22
    for ch in lab:
        d.text((x, Y0 + 8), ch, font=F1, fill=(226, 186, 96, 255)); x += d.textlength(ch, font=F1) + 2
    d.text((X0 + 24, Y0 + 27), tit, font=F2, fill=(0, 0, 0, 200)); d.text((X0 + 22, Y0 + 25), tit, font=F2, fill=(250, 246, 236, 255))
    return np.array(L).astype(np.float32), int(tw)
TL = [title_layer(l, t) for _, _, l, t in TITLES]
ease = lambda x: (lambda v: v * v * (3 - 2 * v))(min(max(x, 0.0), 1.0))
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255

def dress(img, tb):
    f = img.astype(np.float32)
    if tb < 0.5: f *= tb / 0.5
    for (a, b, _, _), (lay, tw) in zip(TITLES, TL):
        if a <= tb < b:
            k = ease((tb - a) / 0.35) * ease((b - tb) / 0.3); xr = X0 + int((tw + 2) * k)
            al = lay[:, :xr, 3:4] / 255; f[:, :xr] = f[:, :xr] * (1 - al) + lay[:, :xr, :3] * al
    lk = la * ease((tb - 0.15) / 0.6)
    return np.clip(f * (1 - lk) + logo[..., :3] * lk, 0, 255).astype(np.uint8)

def field_full(k):
    """L'image de la trame k sur 576 lignes (seules ses propres lignes serviront), habillée à son instant."""
    tb = k / FPS
    for a, b, path in SRC:
        if a <= k < b:
            while not os.path.exists(path) or os.path.getsize(path) < (k - a + 1) * NFLD: time.sleep(5)
            f = np.fromfile(path, np.uint8, NFLD, offset=(k - a) * NFLD).reshape(FH, FW, 3)
            return dress(np.repeat(cv2.resize(f, (W, FH), interpolation=cv2.INTER_CUBIC), 2, axis=0), tb)

if len(sys.argv) > 1 and sys.argv[1] == "still":
    tb = float(sys.argv[2]); k = int(round(tb * FPS)); img = field_full(k)
    if len(sys.argv) > 3: Image.fromarray(img).save(f"stillc_raw_{sys.argv[2]}.png"); sys.exit()
    for i in range(3): out = pork_vhs3.vhs3(img, 2.6 + tb + i / FPS, OFF + k + i, out_wh=(OW, OH))
    Image.fromarray(out).save(f"stillc_{sys.argv[2]}.png"); sys.exit()

_, intro, _ = pork_intro.build_intro(W, H, FPS)
MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8)
enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7500k -maxrate 15000k -bufsize 30000k -pix_fmt yuv420p -an city3_v.mp4", shell=True, stdin=subprocess.PIPE)
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
with wave.open("city_audio.wav") as w:
    sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
o = tape_track(a, sr, TRACKING, CREASE, CUTV)
with wave.open("city3_tape.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
subprocess.run("ffmpeg -v error -y -i city3_v.mp4 -i city3_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart douzi-city-5h-v3.mp4", shell=True, check=True)
print("DONE")
