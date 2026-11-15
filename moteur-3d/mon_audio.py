"""06 h 00, le Porc couronné (sans voix) : six heures sonnent au beffroi sur une nappe d'avant le jour, les couleurs montent sur un roulement
et des cuivres qui s'élèvent, le premier rayon touche la couronne (13,35 s), le drapeau arrive en haut du mât sur un coup d'orchestre (15,6 s),
marche « images de synthèse 90 » autour de la statue, final quand la caméra s'élève, fin vinyle sur NO SIGNAL."""
FILM = 36.0; OFF = 2.1
exec(open('seqaudio.py').read())
# ---- la vallée avant le jour (0 – 8,85) : six coups au beffroi, une nappe, quelques clochettes
for b, ch in ((-1, Am9), (0, Am9), (1, F9), (2, Dm7)):
    place(dry, pad(ch["p"], BAR + 0.5, 900 + 300 * (b + 1), 0.9), bt(b) - 0.2, 0.22)
    place(wet, pad([n + 12 for n in ch["p"][:3]], BAR + 0.5, 2000, 1.0), bt(b) - 0.2, 0.13)
    place(dry, sub(ch["b"], BAR + 0.3), bt(b) - 0.1, 0.20)
    if b >= 0:
        for i in range(4): y = bell(mtof(ch["p"][[0, 2, 4, 2][i]] + 24), 1.6); place(dry, y, bt(b, i), 0.035 + 0.01 * b); place(wet, y, bt(b, i), 0.05 + 0.012 * b)
for k in range(6):       # la cloche du beffroi : six heures
    y = ding(294.0, 2.6) + 0.5 * ding(147.0, 2.6); place(sfx, y, 0.45 + 1.18 * k, 0.26); place(sfxw, y, 0.45 + 1.18 * k, 0.30)
# ---- les couleurs montent (8,85 – 15,6) : un roulement qui enfle, des cuivres tenus qui s'élèvent
for i, (b, ch, top) in enumerate(((3, Dm7, 62), (4, F9, 65), (5, G, 67))):
    notes = (ch["b"] + 12, ch["b"] + 19, top, top + 4 if i else top + 3, top + 7)
    place(dry, held(notes, BAR + 0.1, 0.85), bt(b), 0.10 + 0.035 * i); place(wet, choir([n + 12 for n in notes[2:]], BAR + 0.2), bt(b) - 0.05, 0.09 + 0.03 * i)
    place(dry, sub(ch["b"], BAR, 0.3, 0.4), bt(b), 0.26); place(drums, tom(105), bt(b), 0.30 + 0.08 * i)
    place(dry, brass(top + 12, BEAT * 1.9), bt(b), 0.20 + 0.05 * i); place(dry, brass(top + 14, BEAT * 1.9), bt(b, 2), 0.20 + 0.05 * i)
roll(bt(3), bt(5), 0.03, 0.16); roll(bt(5), bt(6), 0.14, 0.42); riser(bt(5), BAR, 0.07)
for i, m in enumerate((84, 88, 91, 96, 100, 103, 108, 112)): y = bell(mtof(m), 1.4); place(dry, y, 13.35 + i * 0.07, 0.07); place(wet, y, 13.35 + i * 0.07, 0.10)     # le premier rayon sur la couronne
place(wet, crash(), 13.35, 0.05)
# ---- le drapeau en haut du mât (15,6) : le coup d'orchestre, la sonnerie
t0 = bt(6)
place(dry, orch(C9["p"][:3], 1.1), t0, 0.95); place(wet, orch(C9["p"][:3], 1.1), t0, 0.35); place(wet, crash(), t0, 0.16); place(drums, K2, t0, 1.0); place(drums, STOMP, t0, 0.8)
place(dry, held((48, 55, 60, 64, 67, 72), BAR, 0.6), t0, 0.16); place(wet, choir((60, 64, 67, 72), BAR + 0.2, 0.05), t0, 0.17)
fanfare(t0, [(0, .75, 67), (.75, .25, 67), (1, 1, 72), (2, 1.5, 76), (3.5, .5, 79)], 0.9)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(7) - (4 - i) * S16, 0.32 + 0.05 * i)
# ---- autour de la statue (17,85 – 26,85) : la marche, la fanfare
for i in range(4):
    b = 7 + i; ch = MAIN[i]; groove(b, ch); fanfare(bt(b), FANF[i]); arp_bar(bt(b), ch, 0.035, 24)
    place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.32); place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.15)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(11) - (4 - i) * S16, 0.34 + 0.05 * i)
place(wet, crash(), bt(11), 0.13)
# ---- la caméra s'élève (26,85 – 33,6), puis l'accord final
for i in range(3):
    b = 11 + i; ch = MAIN[i]; groove(b, ch, lvl=1.05); lead(bt(b), BELLS[i], 0.8); fanfare(bt(b), FANF[i], 0.9)
    place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.19); place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.34)
roll(bt(13, 3), bt(14), 0.10, 0.38)
tf = bt(14)
place(dry, orch(C9["p"][:3], 1.5), tf, 1.0); place(wet, orch(C9["p"][:3], 1.5), tf, 0.4); place(wet, crash(), tf, 0.18); place(drums, K2, tf, 1.0); place(drums, STOMP, tf, 0.7)
place(dry, held((36, 48, 55, 60, 64, 67, 72, 76), 6.0, 0.55), tf, 0.15); place(wet, choir((60, 64, 67, 72, 76), 6.0, 0.08), tf, 0.20); place(dry, sub(24, 6.0, 0.02, 1.0), tf, 0.40)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)): y = bell(mtof(m), 1.6); place(dry, y, tf + i * S16, 0.10); place(wet, y, tf + i * S16, 0.12)
# ---- bruitages : le vent du plateau, le drapeau qui claque
wind = windbed() * env_t([(-3, 0.8), (8.0, 0.8), (9.0, 0.45), (17.5, 0.40), (27.0, 0.45), (30, 0.8), (60, 0.9)])
fl = np.random.default_rng(7); flap = np.zeros(N)
for t in np.arange(9.8, 35.5, 0.37):
    n_ = int(0.16 * SR); d_ = np.arange(n_) / SR; z = bp(fl.normal(0, 1, n_), 250, 1800) * np.exp(-d_ * 22) * np.minimum(d_ / 0.008, 1)
    i = int((PRE + t + fl.uniform(-0.08, 0.08)) * SR); flap[i:i + n_] += z * fl.uniform(0.5, 1.0)
finish("mon_audio.wav", (18.0, 26.5), fx=[(wind, (0.5, 8.0), 0.50), (flap, (10.0, 30.0), 0.10)],
       sections=[(0, 8.85, "vallée"), (8.85, 15.6, "couleurs"), (15.6, 17.85, "coup"), (17.85, 26.85, "statue"), (26.85, 33.6, "envol"), (33.6, 37.0, "final")])
