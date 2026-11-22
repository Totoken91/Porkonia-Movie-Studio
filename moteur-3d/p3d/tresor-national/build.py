"""Canal Pork : montage du showcase « Cockring Homologué Martin Plaisir » (pas de voix : la musique de Kenny à partir de 1:30).
python3 build.py audio | video"""
import sys, subprocess, wave, numpy as np, cv2
from scipy.signal import butter, sosfilt
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, "/home/claude/channelpork")
import pork_look, pork_vhs2, pork_vhs3, pork_intro, pork_tape_audio, pork_titles
pork_look.LOGO_PATH = "/home/claude/channelpork/logo_canal.png"

SR, FPS = 48000, 25
INTRO = pork_intro.DUR
NF = 1355
BODY = NF / FPS
FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
CUT = INTRO + BODY + FREEZE
TOTAL = CUT + SNOW + NOSIG
MUSIC, M0 = "/mnt/user-data/uploads/ba260513-muizik.mp3", 90.0
CT = [9.63, 19.41, 29.68, 34.57, 41.41, 47.28]
TITLES = [(1.2, 7.4, "MUSÉE NATIONAL DE PORKONIA", "LA ROTONDE DES TRÉSORS"),
          (35.4, 41.0, "SON UNIQUE PORTEUR", "MARTIN PLAISIR")]
# les stats, en légendes discrètes accrochées à l'objet (gros plan) : (début, fin, point d'ancrage, côté, texte)
RY0, RR, Rr = 2.64 + 0.38 + 0.085 - 0.05, 0.38, 0.085
CALLOUTS = [(10.5, 18.9, "top", -1, "OR MASSIF · 24 CARATS"),
            (11.9, 18.9, "right", 1, "Ø 4,2 CM · 312 G"),
            (13.3, 18.9, "left", -1, "37,2 °C, CONSTANTE")]
CAPTIONS = [(20.6, 28.9, "VISITEURS EN LARMES : 100 % DEPUIS 1987")]
pork_vhs2.TRACKING = [e for e in pork_vhs2.TRACKING if e[1] < CUT - 0.5]
pork_vhs2.CREASE = [e for e in pork_vhs2.CREASE if e[0] < CUT - 0.3]
# pas d'incident de bande pendant un titrage
busy = [(INTRO + a - 0.3, INTRO + b + 0.3) for a, b, _, _ in TITLES] + [(INTRO + a - 0.3, INTRO + b + 0.3) for a, b, *_ in CALLOUTS + CAPTIONS] + [(INTRO + CT[2] - 0.3, INTRO + CT[3] + 0.3)]       # la fiche technique : on doit pouvoir la lire
pork_vhs2.TRACKING = [e for e in pork_vhs2.TRACKING if not any(a < e[1] and e[0] < b for a, b in busy)]
pork_vhs2.CREASE = [e for e in pork_vhs2.CREASE if not any(a < e[0] < b for a, b in busy)] + [(INTRO + c - 0.04, 3) for c in CT[:1]]
db = lambda x: 10 ** (x / 20)


def audio():
    raw = subprocess.check_output(f'ffmpeg -v error -ss {M0} -i "{MUSIC}" -ac 1 -ar {SR} -f f32le -', shell=True)
    mus = np.frombuffer(raw, np.float32).astype(np.float64)
    mus[: int(0.3 * SR)] *= np.linspace(0, 1, int(0.3 * SR))
    tc = int((BODY + FREEZE) * SR)
    mus = np.pad(mus, (0, max(0, tc + 6 * SR - len(mus))))
    # coupure de bande : la musique ralentit jusqu'à l'arrêt comme un vinyle, sous NO SIGNAL
    n = int(3.0 * SR); u = np.arange(n) / n
    speed = (1 - u) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * u * 3.0))
    pos = np.clip(tc + np.cumsum(speed), 0, len(mus) - 2)
    slow = sosfilt(butter(2, 5000, fs=SR, output="sos"), np.interp(pos, np.arange(len(mus)), mus))
    e = db(-3.0 * u); tail = int(0.25 * SR); e[-tail:] *= np.linspace(1, 0, tail) ** 2
    out = np.zeros(int(TOTAL * SR)); o = int(INTRO * SR)
    out[o:o + tc] = mus[:tc]
    out[o + tc:o + tc + n] = (slow * e)[: len(out) - o - tc]
    rng = np.random.default_rng(5)
    sn = int((SNOW + 0.08) * SR)
    nz = sosfilt(butter(2, [300, 7000], btype="band", fs=SR, output="sos"), rng.normal(0, 1, sn))
    ne = np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
    peak_m = np.abs(mus[:tc]).max()
    out[o + tc:o + tc + sn] += nz / np.sqrt(np.mean(nz ** 2)) * peak_m * db(-22) * ne
    hs = o + tc + int(SNOW * SR); hl = len(out) - hs
    hiss = sosfilt(butter(2, 4000, fs=SR, output="sos"), rng.normal(0, 1, hl))
    out[hs:] += hiss / np.sqrt(np.mean(hiss ** 2)) * peak_m * db(-50) * np.clip((hl - np.arange(hl)) / (1.2 * SR), 0, 1)
    _, _, au = pork_intro.build_intro(768, 576, FPS)
    au = au[:o] / np.abs(au).max() * peak_m * 0.75                 # jingle au niveau du morceau
    out[:len(au)] += au
    out = out / np.abs(out).max() * 0.92
    tape = pork_tape_audio.tape_track(out, SR, (), (), CUT)
    with wave.open("final_audio.wav", "w") as w:
        w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR)
        w.writeframes((np.clip(tape, -1, 1) * 32767).astype(np.int16).tobytes())
    print("audio ok", round(TOTAL, 2))


W, H = 768, 576


def _proj(P, t):
    """projette un point de la scène dans l'image 768x576 au temps t (caméra du gros plan, comme dans tresor.c)."""
    C0, C1 = CT[0], CT[1]; u = (t - C0) / (C1 - C0); a = -0.55 + u; dist = 2.3 - 0.4 * u
    cam = np.array([np.sin(a) * dist, RY0 + 0.75 - 0.25 * u, -np.cos(a) * dist]); tgt = np.array([0, RY0 - 0.05, 0])
    fw = tgt - cam; fw /= np.linalg.norm(fw); rt = np.cross([0, 1, 0], fw); rt /= np.linalg.norm(rt); up = np.cross(fw, rt)
    d = np.array(P) - cam; z = d @ fw; px, py = (d @ rt) / z * 2.6, (d @ up) / z * 2.6
    return (px / (512 / 384) + 1) / 2 * W, (1 - py) / 2 * H

def _anchor(kind, t):
    tl = t - CT[0]; a = -0.55 + tl / (CT[1] - CT[0]); rot = -a + 0.7 * np.sin(tl * 0.45); c, s_ = np.cos(rot), np.sin(rot)
    if kind == "top": return _proj((0, RY0 + RR + Rr * 0.7, 0), t)
    x = (RR + Rr * 0.6) * (1 if kind == "right" else -1)
    return _proj((c * x, RY0, -s_ * x), t)

_cf = {}
def _font(sz):
    if sz not in _cf: _cf[sz] = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", sz)
    return _cf[sz]
GOLD, INK = (236, 196, 100), (12, 6, 10)

def callout_layer(t, a, b, kind, side, text):
    """un point d'or sur l'objet, un trait fin qui s'en va en biais puis à l'horizontale, une courte légende : discret."""
    k = min((t - a) / 0.45, 1.0) * min((b - t) / 0.4, 1.0)
    if k <= 0: return None
    ax, ay = _anchor(kind, t); L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L)
    f = _font(18); tw = d.textlength(text, font=f)
    ex, ey = ax + side * 46, ay + (-34 if kind == "top" else 40)
    if side > 0: tx = min(ex + 40, W - 22 - tw); hx = tx - 6; ex = min(ex, hx)
    else: tx = max(ex - 40 - tw, 22); hx = tx + tw + 6; ex = max(ex, hx)
    g = min((t - a) / 0.35, 1.0); al = int(255 * min(k * 1.5, 1.0))
    pts = [(ax, ay), (ex, ey), (hx, ey)]
    seg = [(pts[0], pts[1]), (pts[1], pts[2])]
    for i, (p0, p1) in enumerate(seg):
        u = min(max(g * 2 - i, 0), 1)
        if u <= 0: continue
        p1 = (p0[0] + (p1[0] - p0[0]) * u, p0[1] + (p1[1] - p0[1]) * u)
        d.line([p0, p1], fill=INK + (al,), width=4); d.line([p0, p1], fill=GOLD + (al,), width=2)
    d.ellipse((ax - 5, ay - 5, ax + 5, ay + 5), fill=GOLD + (al,), outline=INK + (al,), width=2)
    if g >= 1:
        ta = int(255 * min((t - a - 0.35) / 0.3, 1.0) * min((b - t) / 0.4, 1.0))
        if ta > 0:
            d.text((tx, ey - 11), text, font=f, fill=GOLD + (ta,), stroke_width=3, stroke_fill=INK + (ta,))
    return L

def caption_layer(t, a, b, text):
    k = min((t - a) / 0.5, 1.0, (b - t) / 0.5)
    if k <= 0: return None
    L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L); al = int(255 * k); f = _font(17)
    d.line([(36, 470), (36, 492)], fill=GOLD + (al,), width=3)
    d.text((46, 469), text, font=f, fill=GOLD + (al,), stroke_width=3, stroke_fill=INK + (al,))
    return L

def overlays(img, t):
    for a, b, lab, tit in TITLES:
        if a <= t < b:
            ov = np.array(pork_titles.canal_title(t, a, b, lab, tit, W, H)).astype(np.float32); al = ov[..., 3:4] / 255
            img = img * (1 - al) + ov[..., :3] * al
    for a, b, kind, side, text in CALLOUTS:
        if a <= t < b:
            ov = callout_layer(t, a, b, kind, side, text)
            if ov is not None: ov = np.array(ov).astype(np.float32); al = ov[..., 3:4] / 255; img = img * (1 - al) + ov[..., :3] * al
    for a, b, text in CAPTIONS:
        if a <= t < b:
            ov = caption_layer(t, a, b, text)
            if ov is not None: ov = np.array(ov).astype(np.float32); al = ov[..., 3:4] / 255; img = img * (1 - al) + ov[..., :3] * al
    return img


def video():
    enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s 1440x1080 -r {FPS} -i - -c:v libx264 -preset medium -b:v 4500k -maxrate 6000k -bufsize 9000k -pix_fmt yuv420p -an video.mp4", shell=True, stdin=subprocess.PIPE)
    fi = [0]
    def put(img, colour=1.0):
        enc.stdin.write(pork_vhs3.vhs3(np.ascontiguousarray(img), fi[0] / FPS, fi[0], out_wh=(1440, 1080), colour=colour).tobytes()); fi[0] += 1
    rng = np.random.default_rng(8)
    def snow(): return (np.clip((rng.random((H, W // 2 + 1)).repeat(2, 1)[:, :W] - 0.5) * 1.7 + 0.5, 0, 1) * 255).astype(np.uint8)[..., None].repeat(3, 2)
    DUR, frame, _ = pork_intro.build_intro(W, H, FPS)
    for k in range(int(round(INTRO * FPS))):
        t = k / FPS
        if t < 0.30: put(snow(), 0.0)
        else: put(np.clip(frame(t), 0, 255).astype(np.uint8))
    logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
    last = None
    with open("frames.rgb", "rb") as f:
        for k in range(NF):
            raw = np.frombuffer(f.read(512 * 384 * 3), np.uint8).reshape(384, 512, 3)
            img = cv2.resize(raw, (W, H), interpolation=cv2.INTER_CUBIC).astype(np.float32)
            t = k / FPS
            if t < 0.4: img *= t / 0.4
            img = overlays(img, t)
            img = img * (1 - la) + logo[..., :3] * la
            last = np.clip(img, 0, 255).astype(np.uint8); put(last)
            if k % 200 == 0: print(k, flush=True)
    for _ in range(int(FREEZE * FPS)): put(last)
    MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
    def blue(label):
        b = Image.new("RGB", (W, H), (18, 40, 200))
        if label:
            d = ImageDraw.Draw(b); fo = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=fo)
            d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=fo, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=fo, fill=(235, 240, 255))
        return np.array(b)
    b_on, b_off = blue(True), blue(False)
    for j in range(int(round((SNOW + NOSIG) * FPS))):
        lt = j / FPS
        if lt < SNOW: put(snow(), 0.0)
        else:
            im = b_on if int((lt - SNOW) * 1.2) % 2 == 0 else b_off
            put((im * np.clip((SNOW + NOSIG - lt) / 1.2, 0, 1)).astype(np.uint8))
    enc.stdin.close(); enc.wait()
    print("images", fi[0], "duree", fi[0] / FPS)


if __name__ == "__main__":
    if sys.argv[1] == "audio": audio()
    elif sys.argv[1] == "still":       # python3 build.py still t raw.rgb out.png : une image test avec les incrustations
        t = float(sys.argv[2]); raw = np.fromfile(sys.argv[3], np.uint8).reshape(384, 512, 3)
        img = cv2.resize(raw, (W, H), interpolation=cv2.INTER_CUBIC).astype(np.float32)
        logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
        img = overlays(img, t); img = img * (1 - la) + logo[..., :3] * la
        Image.fromarray(np.clip(img, 0, 255).astype(np.uint8)).save(sys.argv[4])
    else:
        video()
        subprocess.run('ffmpeg -v error -y -i video.mp4 -i final_audio.wav -map 0:v -map 1:a -c:v copy -c:a aac -b:a 192k -shortest -movflags +faststart "canal-pork-tresor-national-martin-plaisir.mp4"', shell=True, check=True)
