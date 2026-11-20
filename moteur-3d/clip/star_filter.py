"""Le filtre étoile (cross filter) des clips des années 80-90 : chaque PETIT point très brillant (une paillette, une ampoule, le reflet
d'une chope, une facette de la boule) devient une étoile à 4 branches, plus 4 petites en diagonale. Les grandes surfaces claires
(néons, fenêtres, neige) n'étoilent pas. Appliqué à l'image propre, avant le Porko Effect, comme un filtre vissé devant l'objectif."""
import numpy as np, cv2
_K = {}
def _kernel(L, ang):
    if (L, ang) in _K: return _K[(L, ang)]
    n = 2 * L + 1; k = np.zeros((n, n), np.float32); c = L
    for a, ln, w in ((ang, L, 1.0), (ang + 90, L, 1.0), (ang + 45, int(L * 0.4), 0.3), (ang + 135, int(L * 0.4), 0.3)):
        r = np.deg2rad(a)
        for s in np.linspace(-ln, ln, 4 * ln + 1):
            xi, yi = int(round(c + s * np.cos(r))), int(round(c + s * np.sin(r)))
            k[yi, xi] = max(k[yi, xi], w * np.exp(-4.0 * abs(s) / ln))
    k = cv2.GaussianBlur(k, (3, 3), 0.6); _K[(L, ang)] = k; return k
def star(img, n_max=90, thr=0.07, gain=1.7, L=None, ang=18.0):
    H, W = img.shape[:2]; L = L or int(W * 0.055)
    f = img.astype(np.float32) / 255; lum = f.max(2)
    pt = lum - cv2.GaussianBlur(lum, (0, 0), 7.0)                     # ce qui dépasse de son voisinage : un point, pas une surface
    pt[lum < 0.60] = 0
    pk = (pt >= cv2.dilate(pt, np.ones((11, 11), np.uint8))) & (pt > thr)   # un seul sommet par voisinage de 11 pixels
    eye = (cv2.erode(lum, np.ones((5, 5), np.uint8)) < 0.12) & (cv2.blur(lum, (13, 13)) > 0.30)       # (20 nov. 2026) pas les reflets des yeux : un point blanc sur une petite pupille noire,
    pk &= ~cv2.dilate(eye.astype(np.uint8), np.ones((5, 5), np.uint8)).astype(bool)                       # elle-même dans un œil blanc ou une peau claire ; les spots du plafond, eux, sont sur un fond noir étendu
    ys, xs = np.nonzero(pk); order = np.argsort(-pt[ys, xs])[:n_max]
    acc = np.zeros_like(f); K = _kernel(L, ang); n = K.shape[0]
    for y, x in zip(ys[order], xs[order]):
        y0, x0 = y - L, x - L; a0, b0 = max(0, -y0), max(0, -x0); a1, b1 = min(n, H - y0), min(n, W - x0)
        col = f[y, x] * 0.5 + 0.5; k = np.clip((pt[y, x] - thr) / 0.20, 0.35, 1.0)
        acc[y0 + a0:y0 + a1, x0 + b0:x0 + b1] += K[a0:a1, b0:b1, None] * col * k
    out = 1 - (1 - f) * (1 - np.clip(acc * gain, 0, 1))
    return (np.clip(out, 0, 1) * 255).astype(np.uint8)
