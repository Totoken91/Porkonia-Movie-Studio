"""Plan pilote monté : intro de chaîne, plan 3D avec boîte de dialogue (texte tapé au rythme de la voix), carton, NO SIGNAL. Porko Effect v3."""
import sys, json, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
NB, OFF = 1100, 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + 22 + 1.0
TRACKING = [(21.4, 23.0, 0.6, 0.84)]; CREASE = [(8.3, 3), (16.6, 3)]
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
SUBS = json.load(open("pilot_subs.json"))
GF = "/usr/share/fonts/truetype/google-fonts/"
FT = ImageFont.truetype(GF + "Poppins-Medium.ttf", 23); FS = ImageFont.truetype(GF + "Poppins-Bold.ttf", 14); FB = ImageFont.truetype(GF + "Poppins-Bold.ttf", 27)
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255
def wrap(d, txt, maxw):
    lines, cur = [], ""
    for w in txt.split(" "):
        t = (cur + " " + w).strip()
        if d.textlength(t, font=FT) > maxw and cur: lines.append(cur); cur = w
        else: cur = t
    return lines + [cur]
def dress(img, tb):
    im = Image.fromarray(img).convert("RGBA"); L = Image.new("RGBA", (W, H), (0, 0, 0, 0)); d = ImageDraw.Draw(L)
    if 0.5 <= tb < 4.6:
        a = int(255 * min((tb - 0.5) / 0.3, 1, (4.6 - tb) / 0.3)); x = 40
        for ch in "ACTE V · LA NUIT DU GRAS-FOND":
            d.text((x + 1, 37), ch, font=FS, fill=(0, 0, 0, a)); d.text((x, 36), ch, font=FS, fill=(236, 196, 104, a)); x += d.textlength(ch, font=FS) + 2
    for s in SUBS:
        if s["t0"] - 0.15 <= tb < s["t1"] + 0.9:
            nch = sum(1 for c in s["chars"] if c <= tb)
            d.rounded_rectangle((60, 372, W - 60, 458), 16, fill=(96, 12, 30, 225), outline=(214, 172, 84, 255), width=3)
            d.rounded_rectangle((84, 360, 208, 384), 10, fill=(214, 172, 84, 255)); d.text((98, 362), "LE NARRATEUR", font=FS, fill=(60, 8, 18, 255))
            full = wrap(d, s["text"], W - 180); shown = s["text"][:nch]; y = 392; k = 0
            for ln in full:
                part = shown[k:k + len(ln)]; d.text((88, y), part, font=FT, fill=(250, 246, 236, 255)); k += len(ln) + 1; y += 30
            if tb > s["t1"] and int(tb * 3) % 2 == 0: d.polygon([(W - 96, 436), (W - 82, 436), (W - 89, 446)], fill=(236, 196, 104, 255))
    if tb >= 21.2:
        tw = d.textlength("À SUIVRE", font=FB); d.text(((W - tw) / 2 + 2, 262), "À SUIVRE", font=FB, fill=(0, 0, 0, 220)); d.text(((W - tw) / 2, 260), "À SUIVRE", font=FB, fill=(250, 246, 236, 255))
    im.alpha_composite(L); f = np.array(im.convert("RGB")).astype(np.float32)
    if tb < 0.5: f *= tb / 0.5
    return np.clip(f * (1 - la) + logo[..., :3] * la, 0, 255).astype(np.uint8)
n = W * H * 3
_, intro, _ = pork_intro.build_intro(W, H, FPS)
MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); last = None
enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7000k -maxrate 14000k -bufsize 28000k -pix_fmt yuv420p -an pilot_v.mp4", shell=True, stdin=subprocess.PIPE)
for fi in range(NF):
    col = 1.0
    if fi < OFF: img = np.clip(intro(fi / FPS), 0, 255).astype(np.uint8); col = 0.0 if fi < 15 else 1.0
    elif fi - OFF < NB: last = img = dress(np.fromfile("fr_P.rgb", np.uint8, n, offset=(fi - OFF) * n).reshape(H, W, 3), (fi - OFF) / FPS)
    elif fi - OFF < NB + 50: img = last
    else:
        lt = (fi - OFF - NB - 50) / FPS
        if lt < SNOW:
            s = np.random.default_rng(fi).random((H, W // 2 + 1), dtype=np.float32).repeat(2, 1)[:, :W]
            img = (np.clip((s - 0.5) * 2.8 + 0.5, 0, 1) * 255).astype(np.uint8)[..., None].repeat(3, 2); col = 0.0
        else: img = ((B_ON if int((lt - SNOW) * 1.2) % 2 == 0 else B_OFF) * np.clip((SNOW + NOSIG - lt) / 1.2, 0, 1)).astype(np.uint8)
    enc.stdin.write(pork_vhs3.vhs3(img, fi / FPS, fi, out_wh=(OW, OH), colour=col).tobytes())
enc.stdin.close(); enc.wait()
with wave.open("pilot_audio.wav") as w:
    sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
o = tape_track(a, sr, TRACKING, CREASE, CUTV)
with wave.open("pilot_tape.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
subprocess.run("ffmpeg -v error -y -i pilot_v.mp4 -i pilot_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart gras-fond-plan-pilote.mp4", shell=True, check=True)
print("DONE")
