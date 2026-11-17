"""Montage du clip « Sous la neige de Douzi City » (DJ Viteau) : les plans 3D rendus par trames (r/pNNN.rgb), les effets de régie calés sur les temps
(zoom, miroir, mosaïque, kaléidoscope, écran dans l'écran, couleurs qui tournent, traînées), l'habillage Canal Pork (titrages au cochon 3D, logo),
le Porko Effect v3 doux, puis gel, neige, NO SIGNAL.
  python3 clip_post.py stills OUT.png t1 t2 ...   planche d'images fixes (t : secondes du morceau) ; les trames manquantes sont rendues à la demande
  python3 clip_post.py OUT.mp4 A B                les images A à B-1 du film final (intro comprise), sans le son"""
import sys, os, time, math, subprocess, numpy as np, cv2
sys.path.insert(0, "/home/claude/clip"); sys.path.insert(0, "/home/claude/channelpork")
import pork_look, pork_vhs2, pork_vhs3, pork_titles
pork_look.LOGO_PATH = "/home/claude/channelpork/logo_canal.png"
from PIL import Image, ImageDraw, ImageFont
from clip_edl import SHOTS, shot_at, P, PH, DUR

STILLS = sys.argv[1] == "stills"
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
FW, FH = 576, 288; NFLD = FW * FH * 3
R, ENG = "/home/claude/clip/r", "/home/claude/porkonia3d"
NB = int(round(DUR * FPS))                 # 13 411 trames de musique
INTRO = 5.4; OFF = int(INTRO * FPS)        # l'intro de la chaîne
FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + int(FREEZE * FPS) + int(round((SNOW + NOSIG) * FPS))
pork_vhs2.TRACKING = []                    # pas d'épisode de tracking (Kenny, 15 nov. 2026)
pork_vhs2.CREASE = [(INTRO + t, n) for t, n in [(17.3, 2), (58.9, 3), (93.1, 2), (151.6, 3), (188.4, 2), (228.7, 3)]]
pork_vhs3.OSD_UNTIL = INTRO + 3.0
TITLES = [(1.2, 5.2, "CANAL PORK PRÉSENTE", "SOUS LA NEIGE DE DOUZI CITY"),      # en secondes du morceau ; chaque carte reste sur son plan
          (24.2, 27.8, "AUX PLATINES", "DJ VITEAU"),
          (64.0, 68.6, "SUR LA PISTE", "LES RANGA BOYZ"),
          (79.55, 82.0, "LEUR PAS LÉGENDAIRE", "LE RANGA STYLE"),
          (148.6, 152.9, "SUR LE DANCEFLOOR", "MARTIN PLAISIR"),
          (261.75, 264.2, "DJ VITEAU", "SOUS LA NEIGE DE DOUZI CITY")]
os.makedirs(R, exist_ok=True)

def render_cmd(sh, a, b):
    """La commande du moteur pour les images a à b-1 du plan (une image = une trame de 576 x 288)."""
    p0 = (sh["k0"] & 1) if sh["mult"] == 1 else 0
    env = dict(os.environ, CLIP=f'{sh["t0"]:.4f} {sh["D"]:.4f} {sh["e"]} {sh["th"]} {sh["pat"]} {sh["var"]} {sh["mult"]} {sh["s0"]}')
    if sh["mult"] > 1: env["CLIPPAR0"] = "1"
    return ["./filmclip", str(p0 + a), str(p0 + b), f'{900 + 100 * sh["K"] + 1 - p0 / 50:.2f}', "noaa"], env
def field(sh, idx):
    path = f'{R}/p{sh["i"]:03d}.rgb'
    if os.path.exists(path) and os.path.getsize(path) >= (idx + 1) * NFLD: return np.fromfile(path, np.uint8, NFLD, offset=idx * NFLD).reshape(FH, FW, 3)
    if not STILLS:
        while not os.path.exists(path) or os.path.getsize(path) < (idx + 1) * NFLD: time.sleep(5)
        return np.fromfile(path, np.uint8, NFLD, offset=idx * NFLD).reshape(FH, FW, 3)
    c = f'{R}/s{sh["i"]:03d}_{idx}.rgb'
    if not os.path.exists(c) or os.path.getsize(c) != NFLD:
        cmd, env = render_cmd(sh, idx, idx + 1)
        with open(c, "wb") as f: subprocess.run(cmd, cwd=ENG, env=env, stdout=f, stderr=subprocess.DEVNULL, check=True)
    return np.fromfile(c, np.uint8).reshape(FH, FW, 3)

# ---------------------------------------------------------------- les effets de régie
CX, CY = W / 2, H / 2
_yy, _xx = np.mgrid[0:H, 0:W].astype(np.float32); _rr = np.hypot(_xx - CX, _yy - CY); _aa = np.arctan2(_yy - CY, _xx - CX)
def kal(img, n, rot):                      # un kaléidoscope à n secteurs, qui tourne
    sa = 2 * math.pi / n; a = np.mod(_aa + rot, sa); a = np.where(a > sa / 2, sa - a, a) - sa / 4 - math.pi / 2
    return cv2.remap(img, (CX + _rr * np.cos(a) * 0.85).astype(np.float32), (CY + _rr * np.sin(a) * 0.85).astype(np.float32), cv2.INTER_LINEAR, borderMode=cv2.BORDER_REFLECT)
def mosaic(img, n):
    if n <= 1: return img
    s = cv2.resize(img, (W // n, H // n), interpolation=cv2.INTER_AREA); return np.ascontiguousarray(np.tile(s, (n, n, 1))[:H, :W])
def zoom(img, z):
    if z <= 1.001: return img
    return cv2.warpAffine(img, cv2.getRotationMatrix2D((CX, CY), 0, z), (W, H), flags=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REFLECT)
def rgbsplit(img, d):
    if d < 1: return img
    out = img.copy(); out[:, d:, 0] = img[:, :-d, 0]; out[:, :-d, 2] = img[:, d:, 2]; return out
def hue(img, deg):
    a = math.radians(deg); c, s = math.cos(a), math.sin(a)
    yiq = np.array([[0.299, 0.587, 0.114], [0.596, -0.274, -0.322], [0.211, -0.523, 0.312]]); rot = np.array([[1, 0, 0], [0, c, -s], [0, s, c]])
    return np.clip(cv2.transform(img.astype(np.float32), (np.linalg.inv(yiq) @ rot @ yiq).astype(np.float32)), 0, 255).astype(np.uint8)
def droste(img):                           # l'écran dans l'écran, trois fois
    out = img.copy()
    for i in (1, 2, 3):
        w, h = int(W * 0.6 ** i) & ~1, int(H * 0.6 ** i) & ~1; x0, y0 = (W - w) // 2, (H - h) // 2
        out[y0:y0 + h, x0:x0 + w] = cv2.resize(img, (w, h), interpolation=cv2.INTER_AREA); cv2.rectangle(out, (x0, y0), (x0 + w - 1, y0 + h - 1), (250, 246, 236), 2)
    return out
def apply_fx(img, sh, k):
    t = k / FPS; B = (t - PH) / P; bf = B - math.floor(B); pulse = math.exp(-4.5 * bf); bi = int(math.floor(B)); u = (k - sh["k0"]) / max(sh["nf"], 1); fx = sh["fx"]
    for f in fx:
        if f.startswith("kal"): img = kal(img, int(f[3:]), 0.22 * B)
    if "mosb" in fx: img = mosaic(img, (1, 2, 3, 4)[int(math.floor(B * 2)) % 4])
    if "mirror" in fx: img = img.copy(); img[:, W // 2:] = img[:, W // 2 - 1::-1]
    if "mirrorb" in fx:
        img = img.copy()
        if bi & 1: img[:, :W // 2] = img[:, :W // 2 - 1:-1]
        else: img[:, W // 2:] = img[:, W // 2 - 1::-1]
    if "droste" in fx: img = droste(img)
    z = 1.0
    if "punch" in fx: z += 0.07 * pulse
    if "zoomin" in fx: z += 0.30 * u ** 3
    img = zoom(img, z)
    if "rgb" in fx: img = rgbsplit(img, int(round(9 * pulse * pulse)))
    if "hue" in fx: img = hue(img, 60.0 * (bi % 6))
    if "flashb" in fx: img = np.clip(img.astype(np.float32) + 55 * pulse * pulse, 0, 255).astype(np.uint8)
    if "bars" in fx: img = img.copy(); img[:40] = 0; img[H - 40:] = 0
    return img

# ---------------------------------------------------------------- l'habillage
ease = lambda x: (lambda v: v * v * (3 - 2 * v))(min(max(x, 0.0), 1.0))
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
def dress(img, t):
    f = img.astype(np.float32)
    for a, b, lab, tit in TITLES:
        if a <= t < b:
            lay = np.asarray(pork_titles.canal_title(t, a, b, lab, tit, W, H)).astype(np.float32); al = lay[..., 3:4] / 255; f = f * (1 - al) + lay[..., :3] * al
    lk = la * ease((t + 0.4) / 0.6)
    return np.clip(f * (1 - lk) + logo[..., :3] * lk, 0, 255).astype(np.uint8)

# ---------------------------------------------------------------- une trame du clip, habillée, sur 576 lignes
_st = dict(prev=None, last=None, shot=-1)
def body_full(k):
    sh = shot_at(k); j = k - sh["k0"]
    full = np.repeat(cv2.resize(field(sh, j // sh["mult"]), (W, FH), interpolation=cv2.INTER_CUBIC), 2, axis=0)
    full = apply_fx(full, sh, k)
    if "trail" in sh["fx"] and _st["prev"] is not None and _st["shot"] == sh["i"]: full = np.maximum(full, (_st["prev"].astype(np.float32) * 0.88).astype(np.uint8))
    if _st["shot"] != sh["i"]: _st["last"] = _st["prev"]; _st["shot"] = sh["i"]
    _st["prev"] = full
    if sh["tr"] == "flash" and j < 7: full = np.clip(full.astype(np.float32) + 215 * (1 - j / 7) ** 2, 0, 255).astype(np.uint8)
    if sh["tr"] == "fade" and j < 22:
        a = ease(j / 22); src = _st["last"] if (_st["last"] is not None and sh["i"] > 0) else np.zeros_like(full)
        full = cv2.addWeighted(full, a, src, 1 - a, 0)
    return dress(full, k / FPS)

def label_sheet(out, times):
    """Planche : une image par instant, telle qu'elle sortira (effets, titrages, Porko Effect), avec sa légende."""
    font = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf", 17); tw, th, cap = 480, 360, 30; cols = 4; rows = -(-len(times) // cols)
    sheet = Image.new("RGB", (cols * tw, rows * (th + cap)), (14, 8, 12)); d = ImageDraw.Draw(sheet)
    for n, (t, txt) in enumerate(times):
        k = min(int(round(t * FPS)), NB - 1); _st.update(prev=None, last=None, shot=-1); img = body_full(k)
        pork_vhs3._get(H, W)["prev"] = None
        fr = pork_vhs3.vhs3(img, INTRO + t, OFF + k, out_wh=(OW, OH), colour=1.0)
        x, y = (n % cols) * tw, (n // cols) * (th + cap); sheet.paste(Image.fromarray(fr).resize((tw, th), Image.LANCZOS), (x, y))
        d.text((x + 6, y + th + 5), f"{int(t // 60)}:{t % 60:04.1f}  {txt}", font=font, fill=(236, 196, 104))
        print(n, round(t, 1), txt, flush=True)
    sheet.save(out)

if STILLS and __name__ == "__main__":
    label_sheet(sys.argv[2], [(float(a.split("=")[0]), a.split("=")[1] if "=" in a else "") for a in sys.argv[3:]]); sys.exit(0)

# ---------------------------------------------------------------- le film : l'intro de la chaîne, le clip, le gel, la neige, NO SIGNAL
import pork_intro
CUTP = 265.4; NBC = int(round(CUTP * FPS))          # l'image se fige là ; une seconde plus tard, la bande s'arrête (le son ralentit comme un vinyle)
NF = OFF + NBC + int(FREEZE * FPS) + int(round((SNOW + NOSIG) * FPS))
_, intro_frame, _ = pork_intro.build_intro(W, H, FPS)
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype("/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf", 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8); _first = [True]
def frame(fi):                                      # -> (image 768 x 576, couleur ou non, trames déjà tissées ou non)
    if fi < OFF: t = fi / FPS; return np.clip(intro_frame(t), 0, 255).astype(np.uint8), (0.0 if t < 0.30 else 1.0), False
    k = fi - OFF
    if k < NBC or _first[0]:
        full = body_full(min(k, NBC - 1)); p = k & 1
        if _first[0]: woven[:] = full; _first[0] = False
        else: woven[p::2] = full[p::2]
    if k < NBC + int(FREEZE * FPS): return woven.copy(), 1.0, True
    lt = (k - NBC - int(FREEZE * FPS)) / FPS
    if lt < SNOW:
        sn = np.random.default_rng(fi).random((H, W // 2 + 1), dtype=np.float32).repeat(2, 1)[:, :W]
        return (np.clip((sn - 0.5) * 2.8 + 0.5, 0, 1) * 255).astype(np.uint8)[..., None].repeat(3, 2), 0.0, True
    return ((B_ON if int((lt - SNOW) * 1.2) % 2 == 0 else B_OFF) * np.clip((SNOW + NOSIG - lt) / 1.2, 0, 1)).astype(np.uint8), 1.0, True

if __name__ == "__main__":
    OUT, A, B = sys.argv[1], int(sys.argv[2]), min(int(sys.argv[3]), NF)
    enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7500k -maxrate 15000k -bufsize 30000k -pix_fmt yuv420p -an {OUT}", shell=True, stdin=subprocess.PIPE)
    t_start = time.time()
    for fi in range(A, B):
        img, col, wov = frame(fi)
        if wov: pork_vhs3._get(H, W)["prev"] = None                # les trames sont déjà tissées
        enc.stdin.write(pork_vhs3.vhs3(img, fi / FPS, fi, out_wh=(OW, OH), colour=col).tobytes())
        if (fi - A) % 250 == 0: print(fi, round(time.time() - t_start), flush=True)
    enc.stdin.close(); enc.wait(); print("DONE", NF, flush=True)
