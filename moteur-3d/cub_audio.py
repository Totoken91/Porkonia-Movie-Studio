"""Le pèlerinage, le Cube de Porc (sans voix) : un bourdon et un glas sur la vallée, la procession (un pas par temps, un chœur),
celui qui refait le tour (une voix seule, sifflée), la montée le long de la paroi (un roulement qui enfle),
le sommet sur un coup d'orchestre (33,75 s), fin vinyle sur NO SIGNAL."""
FILM = 36.0; OFF = 0.0
exec(open('seqaudio.py').read())
def glas(t, g=1.0):       # la grosse cloche de la vallée
    y = ding(196.0, 3.2) + 0.7 * ding(98.0, 3.2); place(sfx, y, t, 0.20 * g); place(sfxw, y, t, 0.30 * g)
def pas(t0, t1, g=0.4):   # le pas des pèlerins dans la neige : un coup sourd par temps, la neige qui crisse
    r = np.random.default_rng(11); t = t0; k = 0
    while t < t1 - 0.02:
        place(drums, tom(62 if k % 2 == 0 else 55), t, g * (0.9 if k % 4 == 0 else 0.6))
        n_ = int(0.11 * SR); d_ = np.arange(n_) / SR; z = bp(r.normal(0, 1, n_), 700, 3200) * np.exp(-d_ * 30) * np.minimum(d_ / 0.01, 1)
        place(sfx, z, t + 0.02, 0.05 * g); place(sfx, z[::-1] * 0.6, t + BEAT / 2, 0.03 * g); t += BEAT; k += 1
# ---- on passe la crête (0 – 9) : le bourdon, le glas, le chœur qui monte
for i, ch in enumerate((Am9, F9, Dm7, Em7)):
    place(dry, pad([ch["b"] + 12, ch["b"] + 19] + ch["p"][:3], BAR + 0.5, 700 + 250 * i, 0.9), bt(i) - 0.15, 0.26)
    place(dry, sub(ch["b"], BAR + 0.3, 0.5, 0.6), bt(i) - 0.1, 0.30); place(dry, sub(ch["b"] - 12, BAR + 0.3, 0.6, 0.6), bt(i) - 0.1, 0.16)
    place(wet, choir(ch["p"][:4], BAR + 0.4, 0.8), bt(i) - 0.1, 0.07 + 0.035 * i)
    glas(bt(i) + 0.05, 1.0 - 0.1 * i)
place(wet, crash(), 0.0, 0.05)
for i, f in enumerate((120, 100, 82, 66)): place(drums, tom(f), bt(4) - (4 - i) * E8, 0.20 + 0.05 * i)
# ---- dans la file, vers la porte (9 – 18) : la procession
PROC = (Am9, F9, C9, G); MEL = ((0, 2, 69), (2, 1, 72), (3, 1, 71)), ((0, 2, 69), (2, 2, 65)), ((0, 2, 67), (2, 1, 72), (3, 1, 76)), ((0, 3, 74), (3, 1, 71))
for i, ch in enumerate(PROC):
    b = 4 + i
    place(wet, choir(ch["p"][:4], BAR + 0.3, 0.3), bt(b) - 0.05, 0.20); place(dry, choir([n - 12 for n in ch["p"][:3]], BAR + 0.2, 0.3), bt(b) - 0.05, 0.10)
    place(dry, held((ch["b"] + 12, ch["b"] + 19, ch["p"][1]), BAR, 0.5), bt(b), 0.07); place(dry, sub(ch["b"], BAR, 0.1, 0.5), bt(b), 0.30)
    for b_, du, m in MEL[i]: y = bell(mtof(m), du * BEAT + 0.8); place(dry, y, bt(b, b_), 0.09); place(wet, y, bt(b, b_), 0.10)
    if i % 2 == 0: glas(bt(b), 0.55)
pas(9.0, 18.0, 0.42)
for i, f in enumerate((120, 100, 82, 66)): place(drums, tom(f), bt(8) - (4 - i) * E8, 0.20 + 0.05 * i)
# ---- au pied du Cube (18 – 27) : celui qui refait le tour ; une voix seule
TOUR = (Dm7, Am9, F9, Em7); SOLO = ((0, 1.5, 74), (1.5, .5, 77), (2, 2, 76)), ((0, 1.5, 72), (1.5, .5, 76), (2, 2, 71)), ((0, 1, 69), (1, 1, 72), (2, 1, 77), (3, 1, 76)), ((0, 3, 75), (3, 1, 71))
for i, ch in enumerate(TOUR):
    b = 8 + i
    place(wet, choir(ch["p"][:4], BAR + 0.3, 0.5), bt(b) - 0.05, 0.11); place(dry, pad([ch["b"] + 12] + ch["p"][:3], BAR + 0.3, 900, 0.5), bt(b) - 0.05, 0.20)
    place(dry, sub(ch["b"], BAR, 0.1, 0.5), bt(b), 0.28)
    for b_, du, m in SOLO[i]: y = whistle(m, du * BEAT * 0.95); place(dry, y, bt(b, b_), 0.20); place(wet, y, bt(b, b_), 0.16)
    for s in range(8): y = bell(mtof(ch["p"][(s * 2) % 4] + 24), 0.5); place(wet, y, bt(b) + s * E8, 0.018)        # les cierges
pas(18.0, 27.0, 0.30)
glas(bt(10), 0.5)
# ---- le long de la paroi (27 – 33,75) : on s'élève
for i, (ch, top) in enumerate(((F9, 65), (G, 67), (Em7, 71))):
    b = 12 + i; notes = (ch["b"] + 12, ch["b"] + 19, top, top + 4, top + 7)
    place(dry, held(notes, BAR + 0.1, 0.9), bt(b), 0.10 + 0.04 * i); place(wet, choir([n + 12 for n in notes[2:]], BAR + 0.2, 0.3), bt(b) - 0.05, 0.10 + 0.04 * i)
    place(dry, sub(ch["b"], BAR, 0.2, 0.4), bt(b), 0.30); place(drums, tom(95), bt(b), 0.30 + 0.08 * i)
    place(dry, brass(top + 12, BEAT * 1.9), bt(b), 0.18 + 0.05 * i); place(dry, brass(top + 14, BEAT * 1.9), bt(b, 2), 0.18 + 0.05 * i)
roll(bt(12), bt(14), 0.03, 0.18); roll(bt(14), bt(15), 0.16, 0.45); riser(bt(13), 2 * BAR, 0.09)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(15) - (4 - i) * S16, 0.34 + 0.05 * i)
# ---- le sommet (33,75) : le soleil, le drapeau
tf = bt(15)
place(dry, orch(C9["p"][:3], 1.5), tf, 1.0); place(wet, orch(C9["p"][:3], 1.5), tf, 0.4); place(wet, crash(), tf, 0.18); place(drums, K2, tf, 1.0); place(drums, STOMP, tf, 0.7)
place(dry, held((36, 48, 55, 60, 64, 67, 72, 76), 6.0, 0.55), tf, 0.15); place(wet, choir((60, 64, 67, 72, 76), 6.0, 0.08), tf, 0.22); place(dry, sub(24, 6.0, 0.02, 1.0), tf, 0.40)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)): y = bell(mtof(m), 1.6); place(dry, y, tf + i * S16, 0.10); place(wet, y, tf + i * S16, 0.12)
fanfare(tf + BEAT, [(0, .75, 67), (.75, .25, 67), (1, 2, 72)], 0.8)
# ---- bruitages : le vent de la vallée, le drapeau du sommet
wind = windbed() * env_t([(-3, 0.9), (8.5, 0.9), (9.5, 0.35), (26.5, 0.30), (29.0, 0.6), (33.5, 1.0), (60, 1.0)])
fl = np.random.default_rng(5); flap = np.zeros(N)
for t in np.arange(33.9, 37.5, 0.33):
    n_ = int(0.2 * SR); d_ = np.arange(n_) / SR; z = bp(fl.normal(0, 1, n_), 180, 1400) * np.exp(-d_ * 18) * np.minimum(d_ / 0.008, 1)
    i = int((PRE + t + fl.uniform(-0.07, 0.07)) * SR); flap[i:i + n_] += z * fl.uniform(0.5, 1.0)
finish("cub_audio.wav", (10.0, 17.5), fx=[(wind, (0.5, 8.0), 0.55), (flap, (34.0, 36.5), 0.12)],
       sections=[(0, 9, "crête"), (9, 18, "procession"), (18, 27, "le tour"), (27, 33.75, "paroi"), (33.75, 37.0, "sommet")])
