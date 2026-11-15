"""Montage du film entier « 24 heures à Porkonia » (sans voix) : les sept séquences bout à bout sous une seule bande
(une intro, un compteur de magnétoscope continu, un seul générique, un seul NO SIGNAL). Le travail se fait par tronçons, un par séquence,
pour pouvoir reprendre après une coupure :
  python3 filmpost.py K        -> le tronçon K (0 à 6) : film_K.mp4
  python3 filmpost.py audio    -> la bande-son entière : film_tape.wav
  python3 filmpost.py join     -> colle les tronçons et la bande-son : 24-heures-a-porkonia.mp4"""
import sys, os, time, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont
src = open("seqpost.py").read(); exec(src[src.index("W, H, FPS"):src.index("NAME = sys.argv[1]")])       # W, H, FPS, OW, OH, FW, FH, NFLD, SEQS
SEQS["city"] = dict(nb=2075, src="dc3.rgb", audio="city_audio.wav", tracking=[(21.2, 22.3, 0.6, 0.84)], crease=[(12.3, 3), (38.9, 3)],
                    titles=[(10.6, 15.2, "05 H 00 · CAPITALE", "DOUZI CITY"), (20.4, 25.0, "TERMINUS DU TRAMWAY", "PLACE DU PORC")])
ORDER = ["mon", "par", "cub", "tav", "gf", "city", "fin"]; OUT = "24-heures-a-porkonia.mp4"
START = np.concatenate(([0], np.cumsum([SEQS[n]["nb"] for n in ORDER]))).astype(int)      # la première trame de chaque séquence
NB, OFF = int(START[-1]), 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + NB / FPS + 1.0
SRC = [(int(START[i]), int(START[i + 1]), SEQS[n]["src"]) for i, n in enumerate(ORDER)]
TITLES, TRACKING, CREASE, CRED = [], [], [], None
for i, n in enumerate(ORDER):
    t0 = START[i] / FPS; c = SEQS[n]
    TITLES += [(a + t0, b + t0, l, t) for a, b, l, t in c["titles"]]
    TRACKING += [(a + t0, b + t0, x, y) for a, b, x, y in c["tracking"]]
    CREASE += [(a + t0, k) for a, k in c["crease"]] + ([(2.6 + t0, 3)] if i else [])                # un pli de bande à chaque changement de séquence
    if c.get("credits"): CRED = (c["credits"][0] + t0, c["credits"][1] + t0, c["credits"][2])
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
GF = "/usr/share/fonts/truetype/google-fonts/"
F1 = ImageFont.truetype(GF + "Poppins-Medium.ttf", 15); F2 = ImageFont.truetype(GF + "Poppins-Bold.ttf", 27)
X0, Y0, BH = 36, 388, 66
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
def credits_layer(lines):
    fa = ImageFont.truetype(GF + "Poppins-Medium.ttf", 17); fb = ImageFont.truetype(GF + "Poppins-Bold.ttf", 25)
    hh = sum(16 if not s_ else (40 if (s_.startswith("*") or i == 0) else 25) for i, s_ in enumerate(lines)) + 24
    L = Image.new("RGBA", (W, hh), (0, 0, 0, 0)); d = ImageDraw.Draw(L); y = 6
    for i, s_ in enumerate(lines):
        if not s_: y += 16; continue
        big = s_.startswith("*") or i == 0; t = s_.lstrip("*"); f = fb if big else fa
        if big:
            tw = d.textlength(t, font=f); d.text(((W - tw) / 2, y), t, font=f, fill=(250, 246, 236, 255), stroke_width=3, stroke_fill=(20, 8, 10, 230)); y += 40
        else:
            tw = sum(d.textlength(c, font=f) + 2 for c in t); x = (W - tw) / 2
            for c in t: d.text((x, y), c, font=f, fill=(236, 196, 104, 255), stroke_width=2, stroke_fill=(20, 8, 10, 230)); x += d.textlength(c, font=f) + 2
            y += 25
    return np.array(L).astype(np.float32)
ease = lambda x: (lambda v: v * v * (3 - 2 * v))(min(max(x, 0.0), 1.0))

def dress(img, tb):
    f = img.astype(np.float32)
    if tb < 0.5: f *= tb / 0.5
    if CRED and CRED[0] <= tb < CRED[1]:                        # le générique défile
        c0, c1 = CRED[0], CRED[1]; k = ease((tb - c0) / 0.5) * ease((c1 - tb) / 0.5); f *= 1 - 0.30 * k
        y = int(round(H - 40 - (tb - c0) / (c1 - c0) * (H - 40 + CRL.shape[0] - 30))); a, b = max(y, 0), min(y + CRL.shape[0], H - 96)
        if b > a:
            lay = CRL[a - y:b - y]; al = lay[..., 3:4] / 255 * np.clip((np.arange(a, b)[:, None, None] - 4) / 30.0, 0, 1)
            f[a:b] = f[a:b] * (1 - al) + lay[..., :3] * al
    for (a, b, _, _), (lay, tw) in zip(TITLES, TL):
        if a <= tb < b:
            k = ease((tb - a) / 0.35) * ease((b - tb) / 0.3); xr = X0 + int((tw + 2) * k)
            al = lay[:, :xr, 3:4] / 255; f[:, :xr] = f[:, :xr] * (1 - al) + lay[:, :xr, :3] * al
    lk = la * ease((tb - 0.15) / 0.6)
    return np.clip(f * (1 - lk) + logo[..., :3] * lk, 0, 255).astype(np.uint8)

def field_full(k):
    tb = k / FPS
    for a, b, path in SRC:
        if a <= k < b:
            f = np.fromfile(path, np.uint8, NFLD, offset=(k - a) * NFLD).reshape(FH, FW, 3)
            return dress(np.repeat(cv2.resize(f, (W, FH), interpolation=cv2.INTER_CUBIC), 2, axis=0), tb)

def chunk(K):
    global TL, CRL, logo, la
    TL = [title_layer(l, t) for _, _, l, t in TITLES]; CRL = credits_layer(CRED[2]) if CRED else None
    logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
    f0 = 0 if K == 0 else OFF + int(START[K]); f1 = NF if K == len(ORDER) - 1 else OFF + int(START[K + 1])
    _, intro, _ = pork_intro.build_intro(W, H, FPS)
    MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
    def blue(label):
        b = Image.new("RGB", (W, H), (18, 40, 200))
        if label:
            d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
            d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
        return np.array(b)
    B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8)
    enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7500k -maxrate 15000k -bufsize 30000k -pix_fmt yuv420p -an film_{K}_tmp.mp4", shell=True, stdin=subprocess.PIPE)
    t_start = time.time(); first = set(int(x) for x in START[:-1])
    for fi in range(f0, f1):
        col = 1.0
        if fi < OFF: img = np.clip(intro(fi / FPS), 0, 255).astype(np.uint8); col = 0.0 if fi < 15 else 1.0
        elif fi - OFF < NB:
            k = fi - OFF; full = field_full(k); p = k & 1
            if k in first: woven[:] = full                           # un changement de séquence : l'image entière
            else: woven[p::2] = full[p::2]
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
        if (fi - f0) % 200 == 0: print(K, fi - f0, f1 - f0, round(time.time() - t_start), flush=True)
    enc.stdin.close(); enc.wait(); os.replace(f"film_{K}_tmp.mp4", f"film_{K}.mp4"); print("DONE", K, flush=True)

def audio():
    parts = []; sr = None
    for i, n in enumerate(ORDER):
        with wave.open(SEQS[n]["audio"]) as w:
            sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
        n0 = int(round(2.6 * sr)); n1 = n0 + int(round(SEQS[n]["nb"] / FPS * sr))
        seg = (a[:n1] if i == 0 else a[n0:] if i == len(ORDER) - 1 else a[n0:n1]).copy()
        if i < len(ORDER) - 1: m = int(0.06 * sr); seg[-m:] *= np.linspace(1, 0, m)[:, None]          # pas de clic au changement de séquence
        parts.append(seg)
    a = np.concatenate(parts); want = int(round(NF / FPS * sr)); a = a[:want] if len(a) >= want else np.concatenate((a, np.zeros((want - len(a), 2))))
    o = tape_track(a, sr, TRACKING, CREASE, CUTV)
    with wave.open("film_tape.wav", "w") as w:
        w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
    print("audio", len(a) / sr, "s", flush=True)

def join():
    with open("film_list.txt", "w") as f:
        for K in range(len(ORDER)): f.write(f"file 'film_{K}.mp4'\n")
    subprocess.run("ffmpeg -v error -y -f concat -safe 0 -i film_list.txt -c copy film_v.mp4", shell=True, check=True)
    subprocess.run(f'ffmpeg -v error -y -i film_v.mp4 -i film_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart {OUT}', shell=True, check=True)
    print("DONE", OUT, flush=True)

if __name__ == "__main__":
    a = sys.argv[1]
    if a == "audio": audio()
    elif a == "join": join()
    elif a == "info": print(NB, NF, NF / FPS, list(START), len(TITLES), CRED[:2])
    else: chunk(int(a))
