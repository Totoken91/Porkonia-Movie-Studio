"""Contrôle qualité automatique d'un rendu de trames (576 x 288 rgb24, 50 trames/s), sans regarder la vidéo.

  python3 qc.py trames.rgb [zstat.log] [--cuts 4.2,9.8] [--t0 0] [--png qc.png]

Ce qu'il cherche (demande de Kenny du 16 nov. 2026 : « vérifier systématiquement que tout va bien ») :
  - trame noire, cramée, ou figée (identique à la précédente de même parité) ;
  - POP : une zone qui change d'un coup entre deux trames puis reste comme ça (objet qui apparaît ou disparaît), hors des coupes ;
  - FLASH : une zone qui change sur une seule trame puis revient (objet qui traverse le champ, rayon fou) ;
  - LUCIOLES : des pixels isolés aberrants (rayons qui fuient) ;
  - CLIP : la caméra entre dans un objet (avec le fichier zstat : lancer le moteur avec ZSTAT=1, stderr dans zstat.log).
Les coupes du montage sont trouvées toutes seules (l'image entière change) ; --cuts ajoute celles qu'on connaît (en secondes).
Sortie : un rapport trame par trame, et une planche PNG des trames suspectes, zone entourée en rouge. Code de sortie 1 s'il y a une alerte.
"""
import sys, argparse, numpy as np
from scipy.ndimage import median_filter

FW, FH, FPS = 576, 288, 50
NF = FW * FH * 3
B = 24                                   # taille des blocs comparés (pixels)


def load(path):
    a = np.memmap(path, np.uint8, "r")
    n = len(a) // NF
    if len(a) % NF: print(f"ATTENTION : {len(a) % NF} octets en trop à la fin (trame coupée ?)")
    return a[: n * NF].reshape(n, FH, FW, 3), n


def blocks(g):                           # moyenne par bloc B x B d'une image en niveaux de gris
    h, w = FH // B * B, FW // B * B
    return g[:h, :w].reshape(h // B, B, w // B, B).mean((1, 3))


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("frames"); ap.add_argument("zstat", nargs="?")
    ap.add_argument("--cuts", default=""); ap.add_argument("--t0", type=float, default=0.0)
    ap.add_argument("--png", default="qc.png"); ap.add_argument("--pop", type=float, default=38.0)
    a = ap.parse_args()
    fr, n = load(a.frames)
    print(f"{a.frames} : {n} trames, {n / FPS:.2f} s")
    known = [int(round((float(c) - a.t0) * FPS)) for c in a.cuts.split(",") if c.strip()]
    alerts, cuts = [], []
    def alert(i, kind, msg, box=None): alerts.append((i, kind, msg, box))
    W3 = np.float32([0.3, 0.6, 0.1]); BL = []; FF = []
    for i in range(n):                    # 1. trames seules ; on ne garde que les moyennes par bloc
        g = fr[i].astype(np.float32) @ W3; m = g.mean(); BL.append(blocks(g))
        if m < 4: alert(i, "NOIRE", f"moyenne {m:.1f}")
        elif (g > 250).mean() > 0.6: alert(i, "CRAMÉE", f"{100 * (g > 250).mean():.0f} % blanc")
        if i % 5 == 0 or i == n - 1:      # les lucioles coûtent un filtre médian : une trame sur cinq
            FF.append((i, int((np.abs(g - median_filter(g, 3)) > 120).sum())))
    # lucioles : la neige et les étoiles en font toujours un peu ; on cherche un nombre qui s'envole par rapport aux trames voisines
    for k, (i, ff) in enumerate(FF):
        ref = np.median([f for _, f in FF[max(0, k - 6):k + 7]])
        if ff > 2.5 * ref + 80: alert(i, "LUCIOLES", f"{ff} pixels isolés (d'habitude {ref:.0f})")
    # 2. de trame en trame (même parité : f et f+2, sinon l'entrelacement ressemble à du mouvement)
    D = [None, None] + [np.abs(BL[i] - BL[i - 2]) for i in range(2, n)]
    skip = set()
    for i in range(2, n):
        d = D[i]; frac = (d > 25).mean()
        if frac > 0.45 or any(abs(i - k) <= 2 for k in known):
            if not cuts or i - cuts[-1] > 3: cuts.append(i)
            continue
        if d.max() == 0 and i > 3 and D[i - 1] is not None and D[i - 1].max() == 0: alert(i, "FIGÉE", "identique aux trames précédentes")
        if any(0 < i - c <= 3 for c in cuts): continue
        # un bloc qui saute beaucoup plus que d'habitude, à cette trame et à ce endroit
        ctx = [D[j] for j in (i - 4, i - 3, i + 3, i + 4) if 2 <= j < n]
        base = np.max(ctx, 0) if ctx else np.zeros_like(d)
        spike = d - 2.2 * base - 6
        for (sy, sx) in [k for k in skip if k[0] == i]: spike[sx] = 0
        nsp = (spike > a.pop / 2).mean()
        if nsp > 0.06: continue          # beaucoup de zones à la fois : un changement de lumière voulu (dalles qui changent au temps, éclair), pas un objet
        if spike.max() > a.pop:
            by, bx = np.unravel_index(np.argmax(spike), spike.shape)
            back = i + 2 < n and D[i + 2] is not None and D[i + 2][by, bx] > 0.6 * d[by, bx]
            if back:
                for yy, xx in zip(*np.nonzero(spike > a.pop / 2)):
                    for di in (1, 2, 3): skip.add((i + di, (yy, xx)))      # le retour à la normale n'est pas un second défaut
            alert(i, "FLASH" if back else "POP", f"bloc ({bx * B},{by * B}) saute de {d[by, bx]:.0f} (d'habitude {base[by, bx]:.0f})", (bx * B, by * B))
    # 3. caméra dans un objet
    if a.zstat:
        for line in open(a.zstat):
            if line.startswith("ZSTAT"):
                _, f, near, sky = line.split(); near = float(near)
                if near > 0.05: alert(int(f), "CLIP", f"{100 * near:.0f} % de l'image à moins de 12 cm de la caméra")
    # rapport
    alerts.sort()
    print("coupes vues :", ", ".join(f"{(c / FPS) + a.t0:.2f}s" for c in cuts) or "aucune")
    if not alerts: print("RAS : aucune alerte"); return 0
    merged = []                                   # une alerte par série de trames voisines
    for al in alerts:
        if merged and merged[-1][1] == al[1] and al[0] - merged[-1][-1] <= 2: merged[-1][-1] = al[0]
        else: merged.append([al[0], al[1], al[2], al[3], al[0]])
    for i, kind, msg, box, j in merged:
        span = f"trames {i}-{j}" if j > i else f"trame {i}"
        print(f"  {kind:9s} {span:16s} t={i / FPS + a.t0:7.2f}s  {msg}")
    try:
        from PIL import Image, ImageDraw
        sel = merged[:24]; W2, H2 = 384, 192
        sh = Image.new("RGB", (W2 * 4, H2 * ((len(sel) + 3) // 4)), (20, 20, 20))
        for k, (i, kind, msg, box, j) in enumerate(sel):
            im = Image.fromarray(np.ascontiguousarray(fr[i])); d = ImageDraw.Draw(im)
            if box: d.rectangle((box[0], box[1], box[0] + B, box[1] + B), outline=(255, 0, 0), width=3)
            d.rectangle((0, 0, 330, 18), fill=(0, 0, 0)); d.text((4, 3), f"{kind} f{i} t={i / FPS + a.t0:.2f}s", fill=(255, 220, 0))
            sh.paste(im.resize((W2, H2)), ((k % 4) * W2, (k // 4) * H2))
        sh.save(a.png); print("planche :", a.png)
    except Exception as e: print("planche impossible :", e)
    return 1


if __name__ == "__main__":
    sys.exit(main())
