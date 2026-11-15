"""Montage d'une séquence de « 24 heures à Porkonia » (sans voix) : intro de chaîne, film rendu par trames (tissées ici), bandeaux-titres,
logo, Porko Effect v3, NO SIGNAL. Pas de générique : il viendra une seule fois, à la fin du film entier.
  python3 seqpost.py NOM     -> la séquence entière (attend les trames si le rendu tourne encore) ; NOM : une clé de SEQS"""
import sys, os, time, json, math, subprocess, numpy as np, cv2, wave
sys.path.insert(0, "/home/claude/channelpork"); sys.path.insert(0, ".")
import pork_look, pork_vhs2, pork_vhs3, pork_intro
from pork_tape_audio import tape_track
from PIL import Image, ImageDraw, ImageFont
W, H, FPS, OW, OH = 768, 576, 50, 1440, 1080
FW, FH = 576, 288; NFLD = FW * FH * 3
SEQS = {
    "mon": dict(nb=1800, src="mon.rgb", audio="mon_audio.wav", out="porc-couronne-6h.mp4", tracking=[(20.1, 21.2, 0.6, 0.84)], crease=[(11.4, 3), (29.4, 3)],
                titles=[(1.2, 6.0, "CHANNEL PORK PRÉSENTE", "24 HEURES À PORKONIA"), (18.7, 23.7, "06 H 00 · MONUMENT NATIONAL", "LE PORC COURONNÉ")]),
    "cub": dict(nb=1800, src="cub.rgb", audio="cub_audio.wav", out="cube-de-porc-pelerinage.mp4", tracking=[(20.3, 21.3, 0.6, 0.84)], crease=[(11.6, 3), (29.6, 3)],
                titles=[(1.6, 6.6, "PÈLERINAGE", "LE CUBE DE PORC · 120 M"), (10.4, 15.0, "AVANT DE QUITTER LA VALLÉE", "COMPTEZ VOS PROCHES"),
                        (20.2, 24.6, "REGISTRE DES ARRIVÉES", "CELUI QUI REFAIT LE TOUR"), (28.0, 32.2, "LE CUBE DE PORC", "UNE VIANDE QUI NE POURRIT JAMAIS")]),
    "tav": dict(nb=1800, src="tav.rgb", audio="tav_audio.wav", out="douzi-ambree-12h.mp4", tracking=[(20.3, 21.3, 0.6, 0.84)], crease=[(11.6, 3), (29.6, 3)],
                titles=[(1.4, 6.4, "12 H 00 · TAVERNE DE BRASSEFORT", "LA DOUZI AMBRÉE"), (10.6, 15.4, "AU COMPTOIR", "UNE CHOPE BIEN TIRÉE"),
                        (21.2, 25.6, "DEVISE D'ÉTAT", "PORC · BIÈRE · TOUJOURS PLUS"), (28.0, 31.6, "ON PLONGE", "AU FOND DU VERRE")]),
    "gf": dict(nb=1800, src="gf.rgb", audio="gf_audio.wav", out="nuit-du-gras-fond-fatbass.mp4", tracking=[(20.3, 21.3, 0.6, 0.84)], crease=[(11.6, 3), (29.6, 3)],
               titles=[(1.6, 6.4, "LA NUIT · AU FOND DU VERRE", "LE GRAS-FOND II"), (10.6, 15.4, "À BORD · LA PISTE", "LA NUIT DU GRAS-FOND"),
                       (19.4, 24.2, "04 H 12 · AUX PLATINES", "FATBASS"), (32.3, 35.6, "JUSQU'AU JOUR", "TOUJOURS PLUS")]),
    "fin": dict(nb=2500, src="fin.rgb", audio="fin_audio.wav", out="lever-du-soleil-generique.mp4", tracking=[(20.3, 21.3, 0.6, 0.84)], crease=[(11.6, 3), (31.8, 3)],
                titles=[(1.4, 6.2, "05 H 59 · MONUMENT NATIONAL", "LE JOUR SE LÈVE"), (31.6, 35.4, "06 H 00 · LE LENDEMAIN", "LE SOLEIL DANS LE O"),
                        (47.3, 50.0, "PORC · BIÈRE · TOUJOURS PLUS", "FIN DES ÉMISSIONS")],
                credits=(36.6, 47.2, ["24 HEURES À PORKONIA", "", "UNE PRODUCTION", "*CHANNEL PORK", "", "IMAGES ET MOTEUR DE RENDU", "*FABLE 5.2", "",
                                      "MUSIQUE", "*SYNTHÉTISEUR NUMÉRIQUE", "", "PORCS DE MARCHE", "*38 416", "", "CITOYENS MODÉLISÉS", "*12, À TOUR DE RÔLE", "",
                                      "PÈLERIN EN TROP", "*1", "", "AUX PLATINES", "*FATBASS", "", "AVEC LE CONCOURS DU", "*MINISTÈRE DU PORC"])),
    "par": dict(nb=1800, src="par.rgb", audio="par_audio.wav", out="grande-parade-8h.mp4", tracking=[(20.3, 21.3, 0.6, 0.84)], crease=[(11.6, 3), (29.6, 3)],
                titles=[(1.6, 6.6, "08 H 00 · DÉFILÉ OFFICIEL", "LA GRANDE PARADE"), (19.0, 23.6, "TRIBUNE D'HONNEUR", "LE MENEUR CHROMÉ"), (31.9, 35.6, "RANG APRÈS RANG", "38 416 PORCS")]),
}
NAME = sys.argv[1]; CFG = SEQS[NAME]
NB, OFF = CFG["nb"], 130; FREEZE, SNOW, NOSIG = 1.0, 0.45, 3.55
NF = OFF + NB + 50 + 200; CUTV = 2.6 + NB / FPS + 1.0
TRACKING, CREASE = CFG["tracking"], CFG["crease"]
pork_vhs2.TRACKING, pork_vhs2.CREASE = TRACKING, CREASE
SRC = [(0, NB, CFG["src"])]
GF = "/usr/share/fonts/truetype/google-fonts/"
F1 = ImageFont.truetype(GF + "Poppins-Medium.ttf", 15); F2 = ImageFont.truetype(GF + "Poppins-Bold.ttf", 27)
X0, Y0, BH = 36, 388, 66
TITLES = CFG["titles"]
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
CRED = CFG.get("credits")
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
CRL = credits_layer(CRED[2]) if CRED else None
ease = lambda x: (lambda v: v * v * (3 - 2 * v))(min(max(x, 0.0), 1.0))
logo = np.array(pork_look.logo_layer((W, H), 176, "corner")).astype(np.float32); la = logo[..., 3:4] / 255

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
    """L'image de la trame k sur 576 lignes (seules ses propres lignes serviront), habillée à son instant."""
    tb = k / FPS
    for a, b, path in SRC:
        if a <= k < b:
            while not os.path.exists(path) or os.path.getsize(path) < (k - a + 1) * NFLD: time.sleep(5)
            f = np.fromfile(path, np.uint8, NFLD, offset=(k - a) * NFLD).reshape(FH, FW, 3)
            return dress(np.repeat(cv2.resize(f, (W, FH), interpolation=cv2.INTER_CUBIC), 2, axis=0), tb)

_, intro, _ = pork_intro.build_intro(W, H, FPS)
MONO = "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf"
def blue(label):
    b = Image.new("RGB", (W, H), (18, 40, 200))
    if label:
        d = ImageDraw.Draw(b); f = ImageFont.truetype(MONO, 30); lw = d.textlength("NO SIGNAL", font=f)
        d.text(((W - lw) / 2 + 2, H / 2 - 15), "NO SIGNAL", font=f, fill=(0, 0, 60)); d.text(((W - lw) / 2, H / 2 - 17), "NO SIGNAL", font=f, fill=(235, 240, 255))
    return np.array(b)
B_ON, B_OFF = blue(True), blue(False); woven = np.zeros((H, W, 3), np.uint8)
enc = subprocess.Popen(f"ffmpeg -v error -y -f rawvideo -pix_fmt rgb24 -s {OW}x{OH} -r {FPS} -i - -c:v libx264 -preset medium -b:v 7500k -maxrate 15000k -bufsize 30000k -pix_fmt yuv420p -an {NAME}_v.mp4", shell=True, stdin=subprocess.PIPE)
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
with wave.open(CFG["audio"]) as w:
    sr = w.getframerate(); a = np.frombuffer(w.readframes(w.getnframes()), np.int16).reshape(-1, 2).astype(np.float64) / 32768
o = tape_track(a, sr, TRACKING, CREASE, CUTV)
with wave.open(f"{NAME}_tape.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(sr); w.writeframes((o * 32767).astype(np.int16).tobytes())
subprocess.run(f'ffmpeg -v error -y -i {NAME}_v.mp4 -i {NAME}_tape.wav -map 0:v -map 1:a -c:v copy -af loudnorm=I=-15:TP=-1.5 -ar 48000 -c:a aac -b:a 192k -movflags +faststart {CFG["out"]}', shell=True, check=True)
print("DONE")
