"""08 h 00, la Grande Parade (sans voix) : la marche « images de synthèse 90 » à 107 à la noire, un pas par temps, caisse claire et fanfare ;
la foule le long des gradins, un coup d'orchestre quand la tribune salue (22,5 s), final sur les rangs jusqu'à l'horizon, fin vinyle sur NO SIGNAL."""
FILM = 36.0; OFF = 0.0
exec(open('seqaudio.py').read())
def pas(t0, t1, g=0.5):       # le pas du défilé : un coup sourd par temps, un roulement de caisse claire
    t = t0
    while t < t1 - 0.02:
        place(drums, K, t, g * 0.8); place(drums, SN, t + BEAT / 2, g * 0.22); place(drums, SN, t + BEAT * 0.75, g * 0.16); t += BEAT
# ---- le meneur, de face (0 – 9) : on entre directement dans la marche
place(dry, orch(C9["p"][:3], 0.9), 0.0, 0.8); place(wet, crash(), 0.0, 0.12); place(drums, K2, 0.0, 0.9)
for i in range(4):
    b = i; ch = MAIN[i]; groove(b, ch, lvl=0.9); fanfare(bt(b), FANF[i], 0.85); place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.28)
pas(0.0, 9.0, 0.45)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(4) - (4 - i) * S16, 0.32 + 0.05 * i)
# ---- le long des gradins (9 – 18) : la mélodie passe aux cloches, la foule
for i in range(4):
    b = 4 + i; ch = MAIN[i]; groove(b, ch); lead(bt(b), BELLS[i]); arp_bar(bt(b), ch, 0.04)
    place(wet, pad([n + 12 for n in ch["p"][1:4]], BAR + 0.2, 2400, 0.2), bt(b), 0.10)
pas(9.0, 18.0, 0.40)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(8) - (4 - i) * S16, 0.34 + 0.05 * i)
place(wet, crash(), bt(8), 0.12)
# ---- la tribune (18 – 27) : la fanfare revient, la tribune salue sur un coup d'orchestre
for i in range(4):
    b = 8 + i; ch = MAIN[i]; groove(b, ch); fanfare(bt(b), FANF[i]); arp_bar(bt(b), ch, 0.03, 24)
    place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.33); place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.16)
pas(18.0, 27.0, 0.45)
place(dry, orch(C9["p"][:3], 0.9), bt(10), 0.55); place(wet, crash(), bt(10), 0.14)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(12) - (4 - i) * S16, 0.34 + 0.05 * i)
# ---- les rangs, l'arc, l'horizon (27 – 36)
for i in range(3):
    b = 12 + i; ch = MAIN[i]; groove(b, ch, lvl=1.05); fanfare(bt(b), FANF[i], 0.95); lead(bt(b), BELLS[i], 0.7)
    place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.35); place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.19)
pas(27.0, 33.75, 0.5); roll(bt(14, 3), bt(15), 0.10, 0.38)
tf = bt(15)
place(dry, orch(C9["p"][:3], 1.5), tf, 1.0); place(wet, orch(C9["p"][:3], 1.5), tf, 0.4); place(wet, crash(), tf, 0.18); place(drums, K2, tf, 1.0); place(drums, STOMP, tf, 0.7)
place(dry, held((36, 48, 55, 60, 64, 67, 72, 76), 6.0, 0.55), tf, 0.15); place(wet, choir((60, 64, 67, 72, 76), 6.0, 0.08), tf, 0.20); place(dry, sub(24, 6.0, 0.02, 1.0), tf, 0.40)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)): y = bell(mtof(m), 1.6); place(dry, y, tf + i * S16, 0.10); place(wet, y, tf + i * S16, 0.12)
pas(33.75, 37.0, 0.5)
# ---- bruitages : la foule (une rumeur qui enfle), les sabots sur le marbre
cr = np.random.default_rng(11); n1 = cr.normal(0, 1, N)
crowd = (bp(n1, 500, 1600) + 0.6 * bp(n1, 1600, 3600)) * (0.7 + 0.5 * (lp(cr.normal(0, 1, N), 0.8) / 0.02).clip(-1, 1))
crowd *= env_t([(-3, 0.5), (8.8, 0.55), (9.1, 1.0), (17.9, 1.0), (18.2, 0.7), (22.3, 0.7), (22.7, 1.1), (26.8, 0.9), (27.2, 0.5), (60, 0.5)])
hoof = np.zeros(N)
for t in np.arange(0.0, 37.0, BEAT):
    for dt in (0.0, 0.03, 0.07):
        n_ = int(0.05 * SR); d_ = np.arange(n_) / SR; i = int((PRE + t + dt) * SR); hoof[i:i + n_] += bp(cr.normal(0, 1, n_), 1200, 4200) * np.exp(-d_ * 90)
finish("par_audio.wav", (9.5, 17.5), fx=[(crowd, (9.5, 17.5), 0.30), (hoof, (1.0, 8.0), 0.16)],
       sections=[(0, 9, "meneur"), (9, 18, "gradins"), (18, 27, "tribune"), (27, 33.75, "rangs"), (33.75, 37.0, "final")])
