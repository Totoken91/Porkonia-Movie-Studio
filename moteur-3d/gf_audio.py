"""La nuit, le Gras-Fond II (sans voix) : dehors, la fête s'entend à travers la coque (un battement sourd, le sonar, des bulles) ;
à bord, le morceau de Fatbass à 107 à la noire (grosse caisse sur chaque temps, basse acide, accords plaqués) ;
aux platines il casse le rythme, gratte le disque, fait monter (24,75 s) puis relance (27 s) ; on ressort, le battement s'éloigne. Fin vinyle sur NO SIGNAL."""
FILM = 36.0; OFF = 0.0
exec(open('seqaudio.py').read())
PROG = (Am9, F9, C9, G)
def club(b, ch, g=1.0, kick_=True, bass=True, stabs=True, hats=True, clap_=True):       # une mesure du morceau
    t0 = bt(b)
    for i in range(4):
        if kick_: place(drums, K2, t0 + i * BEAT, 0.95 * g)
        if hats: place(drums, HO, t0 + i * BEAT + E8, 0.16 * g)
        if clap_ and i % 2: place(drums, CL, t0 + i * BEAT, 0.45 * g)
    if hats:
        for i in range(16): place(drums, HH, t0 + i * S16, (0.05 if i % 2 else 0.08) * g)
    if bass:
        for i in range(16):
            if i % 4 == 0: continue
            m = ch["b"] + (12 if i % 8 in (3, 6) else 0) + (7 if i % 16 == 14 else 0); place(dry, acid(m + 12, S16 * 0.95), t0 + i * S16, 0.34 * g)
    if stabs:
        for i in (3, 6, 11, 14): chord(dry, [n + 12 for n in ch["p"][:4]], t0 + i * S16, 0.3, 0.30 * g, pwm); chord(wet, [n + 12 for n in ch["p"][:4]], t0 + i * S16, 0.3, 0.12 * g, pwm)
RIFF = [((0, .5, 81), (.75, .5, 84), (1.5, .5, 81), (2.5, 1, 88)), ((0, .5, 81), (.75, .5, 84), (1.5, .5, 86), (2.5, 1, 84)), ((0, .5, 79), (.75, .5, 84), (1.5, .5, 79), (2.5, 1, 91)), ((0, .5, 79), (.75, .5, 83), (1.5, 1, 86), (3, 1, 83))]
def riff(b, i, g=1.0):
    for b_, du, m in RIFF[i]: y = pwm(m, du * BEAT * 0.9); place(dry, y, bt(b, b_), 0.26 * g); place(wet, y, bt(b, b_) + 3 * S16, 0.10 * g)
# ---- tout le morceau, d'abord tel qu'on l'entend à bord
for i in range(4): club(i, PROG[i], 0.9); riff(i, i, 0.7)                                   # (0 – 9) on ne l'entendra qu'à travers la coque
for i in range(4): club(4 + i, PROG[i]); riff(4 + i, i)                                     # (9 – 18) la piste
place(wet, crash(), bt(4), 0.14)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(8) - (4 - i) * S16, 0.34 + 0.05 * i)
for i in range(3):                                                                           # (18 – 24,75) aux platines : il casse le rythme, il gratte
    b = 8 + i; ch = PROG[i]
    for k in (0, 2.5): place(drums, K2, bt(b, k), 0.85)
    place(drums, CL, bt(b, 1), 0.35); place(drums, CL, bt(b, 3), 0.35)
    for k in range(8): place(drums, HH, bt(b) + k * E8, 0.07)
    place(dry, sub(ch["b"], BAR, 0.05, 0.4), bt(b), 0.34); place(wet, pad([n + 12 for n in ch["p"][1:4]], BAR + 0.2, 1800, 0.3), bt(b), 0.14)
sc = np.random.default_rng(4)
for t in (18.6, 19.15, 20.3, 20.85, 21.4, 22.6, 23.1, 23.7, 24.2):                           # le disque gratté : un aller, un retour
    place(sfx, whoosh(0.13, 500, 2600, 1.2), t, 0.34); place(sfx, whoosh(0.11, 2400, 420, 1.2), t + 0.14, 0.30)
for i in range(1):                                                                           # (24,75 – 27) la montée
    b = 11; roll(bt(b), bt(b, 2), 0.35, 0.6, None, K2, E8); roll(bt(b, 2), bt(12), 0.30, 0.95, None, SN, S16); riser(bt(b), BAR, 0.40)
    for k in range(16): y = pwm(81 + k // 2, S16 * 0.9); place(dry, y, bt(b) + k * S16, 0.22 + 0.02 * k)
place(dry, orch(Am9["p"][:3], 0.9), bt(12), 0.85); place(wet, crash(), bt(12), 0.20)        # (27) il relance
for i in range(4): club(12 + i, PROG[i], 1.08); riff(12 + i, i, 1.1); place(wet, choir([n + 12 for n in PROG[i]["p"][1:4]], BAR + 0.2), bt(12 + i) - 0.05, 0.10)
cr = np.random.default_rng(21); n1 = cr.normal(0, 1, N)
crowd = (bp(n1, 500, 1700) + 0.5 * bp(n1, 1700, 3600)) * (0.7 + 0.5 * (lp(cr.normal(0, 1, N), 1.0) / 0.03).clip(-1, 1))
crowd *= env_t([(-3, 0), (8.9, 0), (9.1, 0.7), (17.9, 0.7), (18.3, 0.5), (24.7, 0.6), (26.9, 1.0), (27.1, 1.4), (28.5, 1.0), (31.4, 1.0), (31.6, 0), (60, 0)])
# ---- dehors (0 – 9 et 31,5 – 36) : on n'entend plus que le bas, à travers la coque ; le sonar, les bulles
w = np.zeros(N); a0 = int((PRE + 8.9) * SR); a1 = int((PRE + 9.1) * SR); b0 = int((PRE + 31.4) * SR); b1 = int((PRE + 31.7) * SR)
w[:a0] = 1; w[a0:a1] = np.linspace(1, 0, a1 - a0); w[b0:b1] = np.linspace(0, 1, b1 - b0); w[b1:] = 1
dry[:] = dry * (1 - w) + lp(dry, 220) * w * 0.5; drums[:] = drums * (1 - w) + lp(drums, 160) * w * 0.5; wet[:] = wet * (1 - w)
for t, g in ((0.8, 0.30), (3.6, 0.26), (6.4, 0.30), (32.6, 0.28), (35.2, 0.24)): place(sfx, PING, t, g); place(sfxw, PING, t, g * 1.4)
bb = np.random.default_rng(9)
for t in np.concatenate((np.sort(bb.uniform(-1.0, 9.0, 60)), np.sort(bb.uniform(31.5, 37.5, 44)))): place(sfx, blip(bb.uniform(350, 1300), bb.uniform(0.04, 0.10)), t, bb.uniform(0.04, 0.11))
deep = lp(rng.normal(0, 1, N), 90) * w                                                       # le grondement du fond
for t, g in ((9.0, 0.35), (31.5, 0.30)): gl = whoosh(0.9, 300, 2400 if t < 10 else 200, 1.5); place(sfx, gl, t - 0.45, g)      # on passe la coque
finish("gf_audio.wav", (10.0, 17.5), fx=[(crowd, (10.0, 17.5), 0.20), (deep, (1.0, 8.0), 0.30)],
       sections=[(0, 9, "dehors"), (9, 18, "la piste"), (18, 24.75, "platines"), (24.75, 27, "montée"), (27, 31.5, "relance"), (31.5, 37.0, "on remonte")])
