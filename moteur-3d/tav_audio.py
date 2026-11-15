"""12 h 00, la Douzi Ambrée (sans voix) : midi sonne au clocher sur une valse de taverne (basse, contretemps, un air sifflé),
la chope tirée (la musique s'efface, le filet de bière monte en hauteur, un tintement quand elle est pleine),
toute la salle lève sa chope (22,2 s : un cri, les chopes qui tintent), puis on plonge dans le verre : la musique s'assourdit,
des bulles, un bourdon, et tout au fond le sonar (34,7 s). Fin vinyle sur NO SIGNAL."""
FILM = 36.0; OFF = 0.0
exec(open('seqaudio.py').read())
CH = (C9, F9, G, C9)
AIR = [((0, 1.5, 76), (1.5, .5, 79), (2, 1, 84), (3, 1, 79)), ((0, 1.5, 81), (1.5, .5, 77), (2, 2, 72)), ((0, 1, 74), (1, 1, 79), (2, 1, 83), (3, 1, 86)), ((0, 3, 84), (3, 1, 79))]
def valse(b, ch, g=1.0, air=None, fn=whistle):       # une mesure : la basse sur 1 et 3, l'accord à contretemps, l'air
    t0 = bt(b)
    for i in (0, 2): place(dry, bassn(ch["b"] + (0 if i == 0 else 7), BEAT * 0.9), t0 + i * BEAT, 0.42 * g)
    for i in (1, 3): chord(dry, ch["p"][:4], t0 + i * BEAT, 0.35, 0.26 * g, ep); chord(wet, ch["p"][:4], t0 + i * BEAT, 0.35, 0.07 * g, ep)
    for i in range(8): place(drums, HH, t0 + i * E8, (0.05 if i % 2 else 0.09) * g)
    for i in (1, 3): place(drums, RIM, t0 + i * BEAT, 0.16 * g)
    if air:
        for b_, du, m in air: y = fn(m, du * BEAT * 0.95); place(dry, y, t0 + b_ * BEAT, 0.20 * g); place(wet, y, t0 + b_ * BEAT, 0.10 * g)
# ---- on entre (0 – 9) : midi sonne, la valse
for k in range(12): y = ding(262.0, 2.2) + 0.5 * ding(131.0, 2.2); place(sfx, y, 0.25 + 0.70 * k, 0.11); place(sfxw, y, 0.25 + 0.70 * k, 0.20)
for i in range(4): valse(i, CH[i], 0.85 + 0.05 * i, AIR[i])
for i in range(4): place(wet, pad([n + 12 for n in CH[i]["p"][1:4]], BAR + 0.2, 2200, 0.2), bt(i), 0.07)
# ---- au comptoir (9 – 18) : la musique s'efface, on tire la chope
for i in range(4): valse(4 + i, CH[i], 0.62)
t0, t1 = 10.4, 15.3; n_ = int((t1 - t0) * SR); u_ = np.arange(n_) / n_; x_ = rng.normal(0, 1, n_); pour = np.zeros(n_)
for bc in np.geomspace(300, 2600, 8): pour += bp(x_, bc / 1.3, bc * 1.3) * np.exp(-(np.log((350 * (6.0 ** u_)) / bc) / 0.45) ** 2)       # le filet : de plus en plus aigu à mesure que la chope se remplit
pour *= np.minimum(u_ / 0.04, 1) * np.minimum((1 - u_) / 0.03, 1) * (0.8 + 0.2 * np.sin(2 * np.pi * 9 * u_ * (t1 - t0)))
place(sfx, pour, t0, 0.55); place(sfxw, pour, t0, 0.10)
fz = bp(rng.normal(0, 1, int(3.0 * SR)), 4000, 9000) * np.exp(-np.arange(int(3.0 * SR)) / SR * 1.3); place(sfx, fz, 14.6, 0.10)                 # la mousse qui crépite
y = ding(1568.0, 1.2); place(sfx, y, 15.65, 0.16); place(sfxw, y, 15.65, 0.2)
place(drums, RIM, 10.05, 0.3); place(drums, RIM, 15.35, 0.3)                                                                                       # la manette
# ---- toute la salle lève sa chope (18 – 27)
for i in range(4): valse(8 + i, CH[i], 1.0, AIR[i], pwm); place(dry, orch(CH[i]["p"][:3], 0.4), bt(8 + i), 0.22); place(wet, choir([n + 12 for n in CH[i]["p"][1:4]], BAR + 0.2), bt(8 + i) - 0.05, 0.10)
tc = 22.2
hey = bp(rng.normal(0, 1, int(1.3 * SR)), 500, 2600) * np.exp(-np.arange(int(1.3 * SR)) / SR * 2.6) * np.minimum(np.arange(int(1.3 * SR)) / SR / 0.06, 1)
place(sfx, hey, tc - 0.15, 0.42); place(sfxw, hey, tc - 0.15, 0.25); place(wet, choir((64, 67, 72, 76), 1.2, 0.05), tc - 0.1, 0.30); place(dry, orch(C9["p"][:3], 0.8), tc, 0.6); place(wet, crash(), tc, 0.10)
cl = np.random.default_rng(3)
for k in range(14): y = bell(mtof(96 + cl.integers(0, 12)), 0.5); t = tc + 0.15 + cl.uniform(0, 1.3); place(sfx, y, t, 0.09); place(sfxw, y, t, 0.10)     # les chopes tintent
for k in range(9): place(sfx, blip(190 - 12 * (k % 3), 0.11), 25.3 + 0.19 * k, 0.16)                                                                # on boit
# ---- on plonge (27 – 36) : la valse s'éloigne sous la surface, les bulles, le bourdon, le sonar
for i in range(2): valse(12 + i, CH[i], 0.7 - 0.25 * i, AIR[i])
for i in range(2): valse(14 + i, CH[2 + i], 0.30)
for i, ch in enumerate((Am9, F9)):
    place(dry, pad([ch["b"] + 12, ch["b"] + 19] + ch["p"][:3], 2 * BAR + 0.5, 600, 1.2), bt(14) - 0.2 + i * BAR, 0.11); place(dry, sub(ch["b"] - 12, 2 * BAR, 0.8, 0.8), bt(14) + i * BAR, 0.11)
bb = np.random.default_rng(9)
for t in np.sort(bb.uniform(31.6, 37.0, 46)): place(sfx, blip(bb.uniform(500, 1500), bb.uniform(0.04, 0.09)), t, bb.uniform(0.05, 0.13));
gl = whoosh(1.4, 1800, 250, 1.5); place(sfx, gl, 31.3, 0.30); place(sfxw, gl, 31.3, 0.2)                                                           # on passe la paroi du verre
for t, g in ((34.7, 0.30), (36.1, 0.22)): place(sfx, PING, t, g); place(sfxw, PING, t, g * 1.3)
# ---- bruitages continus : la rumeur de la salle, le feu
cr = np.random.default_rng(21); n1 = cr.normal(0, 1, N)
crowd = (bp(n1, 300, 1100) + 0.5 * bp(n1, 1100, 2800)) * (0.7 + 0.5 * (lp(cr.normal(0, 1, N), 1.4) / 0.03).clip(-1, 1))
crowd *= env_t([(-3, 0.8), (8.8, 0.9), (9.3, 0.45), (17.8, 0.45), (18.2, 0.9), (22.0, 0.8), (22.3, 1.3), (24.5, 1.0), (27.0, 0.8), (31.5, 0.6), (32.2, 0.0), (60, 0.0)])
fire = np.zeros(N); fr = np.random.default_rng(13)
for t in np.sort(fr.uniform(-2, 32, 260)):
    n_ = int(0.02 * SR); z = fr.normal(0, 1, n_) * np.exp(-np.arange(n_) / SR * 260); i = int((PRE + t) * SR); fire[i:i + n_] += z * fr.uniform(0.2, 1.0)
fire = bp(fire, 900, 6000) * env_t([(-3, 1), (31.5, 1), (32.2, 0), (60, 0)])
# la musique s'assourdit quand on passe sous la surface : on filtre le mélange sec après 31,9 s
k0 = int((PRE + 31.7) * SR); k1 = int((PRE + 32.5) * SR); w = np.zeros(N); w[k1:] = 1; w[k0:k1] = np.linspace(0, 1, k1 - k0)
dry[:] = dry * (1 - w) + lp(dry, 500) * w * 0.9; drums[:] = drums * (1 - w) + lp(drums, 350) * w
finish("tav_audio.wav", (19.0, 26.5), fx=[(crowd, (1.0, 8.0), 0.22), (fire, (1.0, 8.0), 0.06)],
       sections=[(0, 9, "la salle"), (9, 18, "la chope"), (18, 27, "santé"), (27, 31.9, "le verre"), (31.9, 37.0, "le fond")])
