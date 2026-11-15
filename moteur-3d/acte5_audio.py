"""Acte V, première partie : voix 'animalese' (une syllabe synthétique par lettre, sans aucun bruit de frappe),
musique calée sur les plans (mesure = 2,25 s, temps alignés sur les hublots), bruitages, jingle, fin vinyle."""
import sys, json, wave, unicodedata, numpy as np
from scipy.signal import iirpeak, lfilter
src = open('music3.py').read()
exec(src[:src.index("# ---------------- prologue")])          # synthés et tampons communs (temps 0 = début du film)
FILM = 48.0; INTRO = 2.6; CUT = FILM + 1.0; TOTAL = INTRO + CUT + SNOW + NOSIG
OFF = -2 * BEAT                                                # la mesure 0 commence deux temps avant l'image
LINES = [(1.0, "Port-Cochon, le 14 mars. Le Gras-Fond attend au bout de la jetée."),
         (9.0, "Huit cents invités montent à bord. Il y a de la place pour trente."),
         (16.0, "Personne ne compte."),
         (22.2, "À la tombée du jour, il plonge. La jetée reste allumée."),
         (30.6, "Aux platines : Fatbass. Dans le civil, officier sonar, et allergique au sel."),
         (36.9, "Il ne passe que ses propres morceaux. Ils s'appellent tous « Porc ».")]

# ---------------------------------------------------------------- la voix
FORM = dict(a=(800, 1250), e=(480, 1750), i=(300, 2300), o=(450, 850), u=(320, 800), y=(300, 2300))
vr = np.random.default_rng(12); vo = np.zeros(N); stamps = []
def syll(ch, f0):
    base = unicodedata.normalize('NFD', ch.lower())[0]
    n = int(0.095 * SR); d = np.arange(n) / SR
    f = f0 * (1 + 0.05 * np.sin(2 * np.pi * 9 * d)); ph = np.cumsum(f) / SR
    g = 2 * (ph % 1) - 1
    vowel = base in FORM
    F1, F2 = FORM[base] if vowel else (380 + 22 * (ord(base) % 7), 1150 + 80 * (ord(base) % 9))
    y = 0
    for fc, q, a in ((F1, 6, 1.0), (F2, 9, 0.6 if vowel else 0.35)):
        b_, a_ = iirpeak(fc, q, fs=SR); y = y + lfilter(b_, a_, g) * a
    env = 0.5 - 0.5 * np.cos(np.pi * np.minimum(d / 0.014, 1))             # entrée douce
    env = env * np.exp(-d * 20) * (0.5 + 0.5 * np.cos(np.pi * np.clip((d - 0.060) / 0.035, 0, 1)))      # sortie jusqu'à zéro : pas de clic
    return y * env * (1.0 if vowel else 0.8)
for t0, txt in LINES:
    t = t0; chars = []
    for k, ch in enumerate(txt):
        chars.append(round(t, 3))
        if ch.isalnum():
            fall = 1 - 0.18 * (k / len(txt)) ** 2
            place(vo, syll(ch, 118 * fall * (1 + vr.uniform(-0.10, 0.12))), t, 1.0); t += 0.062
        elif ch == ' ': t += 0.045
        elif ch == ',': t += 0.20
        elif ch in '.:': t += 0.34
        else: t += 0.03
    stamps.append(dict(t0=t0, t1=round(t, 3), text=txt, chars=chars))
vo = lp(vo, 4200, 4)                                            # voix ronde, rien au-dessus
json.dump(stamps, open('acte5_subs.json', 'w'), ensure_ascii=False)
print([(s['t0'], s['t1']) for s in stamps])

# ---------------------------------------------------------------- la musique
CHS = [Am9, F9, C9, G]
MEL = [[(0, 1, 69), (1, .5, 72), (1.5, .5, 76), (2, 1, 74), (3, 1, 72)], [(0, 1.5, 69), (1.5, .5, 65), (2, 2, 72)],
       [(0, 1, 67), (1, .5, 72), (1.5, .5, 76), (2, 1, 79), (3, 1, 76)], [(0, 1.5, 74), (1.5, .5, 71), (2, 2, 67)]]
ACID = [0, 0, 12, 0, 7, 0, 12, 10, 0, 0, 12, 0, 3, 7, 12, 0]
deep, pdh = B(), B()
for b in range(23):
    t0 = OFF + b * BAR; ch = CHS[b % 4]
    above = b < 10; dive = 10 <= b < 14; under = b >= 14
    if above:                                                   # le crépuscule : nappe, arpège, cloches
        place(dry, pad(ch["p"], BAR + 0.5, 1100, 0.6), t0, 0.42); place(dry, bassn(ch["b"], 1.8), t0, 0.30)
        for i in range(8):
            y = fm(mtof(ch["p"][[0, 2, 4, 3, 1, 2, 4, 2][i]] + 12), 0.9, 1.0, 1.6, 3.0, 6.0); place(dry, y, t0 + i * BEAT / 2, 0.09); place(wet, y, t0 + i * BEAT / 2, 0.11)
        if b < 4:
            for bt, du, m in BELLS[b % 4]: y = bell(mtof(m), du * BEAT + 0.8); place(wet, y, t0 + bt * BEAT, 0.10); place(dry, y, t0 + bt * BEAT, 0.04)
        if 4 <= b < 10:                                         # la file : un air qu'on siffle
            for bt, du, m in MEL[b % 4]: y = whistle(m + 12, du * BEAT * 0.92); place(dry, y, t0 + bt * BEAT, 0.085); place(wet, y, t0 + bt * BEAT, 0.07)
    if dive:                                                    # il part : la nappe s'ouvre puis retombe
        place(dry, pad(ch["p"], BAR + 0.5, 1500 if b < 12 else 700, 0.5), t0, 0.46 if b < 12 else 0.30); place(dry, bassn(ch["b"], 2.0), t0, 0.30)
        if b < 12:
            for i in range(8):
                y = fm(mtof(ch["p"][[0, 2, 4, 3, 1, 2, 4, 2][i]] + 12), 0.9, 1.0, 1.6, 3.0, 6.0); place(wet, y, t0 + i * BEAT / 2, 0.10)
    if under:                                                   # sous l'eau : nappe grave, le sonar pour mélodie
        place(deep, pad([n - 12 for n in ch["p"][:4]], BAR + 0.6, 520, 0.7), t0, 0.50); place(deep, bassn(ch["b"] - 12, 2.1), t0, 0.34)
        if b % 2 == 0: place(sfxw, PING, t0, 0.20); place(sfxw, PING, t0 + 3 * BEAT / 2, 0.07)
        for i in (0, 3, 6): y = fm(mtof(ch["p"][[4, 2, 3][i // 3]] + 12), 1.2, 1.0, 1.2, 2.5, 5.0); place(wet, y, t0 + i * BEAT / 2, 0.06)
    # la fête, dans la coque : grosse caisse sur chaque temps (les hublots battent avec), basse à contretemps, ligne acide
    for bt in range(4):
        place(pdr, K2, t0 + bt * BEAT, 0.9); place(pd, bassn(ch["b"], 0.24), t0 + (bt + 0.5) * BEAT, 0.5)
    for bt in (1, 3): place(pdh, CL, t0 + bt * BEAT, 0.45)
    for i in range(16): place(pdh, acid(ch["b"] + 24 + ACID[i], S16 * 0.9), t0 + i * S16, 0.16)
    for i in range(8): place(pdh, HH, t0 + (i + 0.5) * BEAT / 2, 0.10)

# ---------------------------------------------------------------- les bruitages
tb = np.arange(N) / SR - PRE
env_t = lambda pts: np.interp(tb, [p[0] for p in pts], [p[1] for p in pts])
n_ = int(34 * SR); d_ = np.arange(n_) / SR                                        # la mer
sea = lp(rng.normal(0, 1, n_), 500) * (0.55 + 0.45 * np.sin(2 * np.pi * 0.17 * d_)) * 1.6 + bp(rng.normal(0, 1, n_), 900, 3500) * (0.3 + 0.3 * np.sin(2 * np.pi * 0.11 * d_ + 1)) * 0.25
place(sfx, sea, -2.5, 0.10)
sfx *= env_t([(-3, 1), (29.6, 1), (30.0, 0), (99, 0)])
hub = np.zeros(N)                                                                 # le brouhaha de la file : des voix sans mots, loin
for k in range(14):
    f0 = rng.uniform(260, 620); x = bp(rng.normal(0, 1, N), f0, f0 * 2.2)
    m = np.clip(lp(rng.normal(0, 1, N), rng.uniform(2.5, 4.5)) * 40, 0, 1)
    hub += x * m
hub = lp(hub, 1800) * env_t([(-3, 0.25), (7.6, 0.35), (8.2, 1.0), (14.8, 1.0), (15.2, 0.8), (20.8, 0.8), (21.2, 0.0), (99, 0)])
hub = hub / (np.abs(hub).max() + 1e-9)
y = brass(36, 2.6) + brass(43, 2.6) * 0.6; place(dry, y, 21.3, 0.5); place(wet, y, 21.3, 0.4)   # la corne
place(sfx2 := np.zeros(N), whoosh(5.5, 1800, 180, 1.0), 23.0, 0.42)                               # il s'enfonce : l'eau se referme
place(sfx2, whoosh(2.2, 400, 2600, 1.5), 24.2, 0.22)
for i in range(90):
    tt = 23.6 + 6.0 * rng.random() ** 0.8; place(sfxw, blip(rng.uniform(200, 900), rng.uniform(0.06, 0.12)), tt, rng.uniform(0.03, 0.07))
n2 = int(19 * SR); d2 = np.arange(n2) / SR                                         # sous l'eau : le grondement, les bulles
place(sfx2, lp(rng.normal(0, 1, n2), 110, 4) * (0.7 + 0.3 * np.sin(2 * np.pi * 0.09 * d2)) * 4.0, 29.8, 0.16)
for i in range(110):
    tt = 30.0 + 18.5 * rng.random(); place(sfxw, blip(rng.uniform(250, 1300), rng.uniform(0.06, 0.12)), tt, rng.uniform(0.012, 0.04) * (1 - 0.5 * (tt - 30) / 18.5))

# ---------------------------------------------------------------- le mélange
ps = pd + pdr
far = env_t([(-3, 0.55), (7.5, 0.60), (8.2, 0.75), (14.5, 0.9), (21.0, 0.9), (22.5, 0.75), (27.0, 0.0), (30.2, 0.0), (30.5, 1.25), (99, 1.25)])     # ce qui passe à travers la coque
near = env_t([(-3, 0), (14.4, 0), (15.2, 0.50), (20.4, 0.62), (21.2, 0.0), (99, 0)])                                                         # ce qui sort par la porte
party = lp(ps, 240, 4) * db(-4) * far + (lp(ps, 1400) + lp(pdh, 2600)) * db(-9) * near + lp(pdh, 700, 4) * db(-10) * env_t([(-3, 0), (30.2, 0), (30.6, 1), (99, 1)])
mus = dry + reverb(wet) * 0.9 + party + lp(deep, 900) * 0.9
fxs = sfx + sfx2 + reverb(sfxw, 1.8, 5000) * 0.9
ref = np.percentile(np.abs(mus[: int((PRE + FILM) * SR)]), 99.9); mus = mus / ref * 0.8; fxs = fxs / ref * 0.8
vref = rms(mus[int((PRE + 2) * SR): int((PRE + 14) * SR)])
fxs = fxs + hub * vref * 0.30
act = np.abs(vo) > 0.02 * np.abs(vo).max(); vo = vo / rms(vo[act]) * vref * db(6.5)
env = np.convolve((np.abs(vo) > vref * 0.1).astype(float), np.ones(int(0.3 * SR)) / int(0.3 * SR), mode="same")
mix = np.tanh(((mus + fxs) * db(-5.0 * np.clip(env * 2, 0, 1)) + vo) * 0.55)
for a, b in ((0, 8), (8, 15), (15, 21), (21, 30), (30, 48)):
    print("plan %2d-%2d  niveau %.1f dB" % (a, b, 20 * np.log10(rms(mix[int((PRE + a) * SR): int((PRE + b) * SR)]))))
tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n                # la bande s'arrête comme un vinyle
speed = (1 - uu) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * uu * D))
slow = lp(np.interp(np.clip(tc + np.cumsum(speed), 0, N - 2), np.arange(N), mix), 5000)
e = db(-3.0 * uu); tl = int(0.25 * SR); e[-tl:] *= np.linspace(1, 0, tl) ** 2
body = np.zeros(int((PRE + CUT + SNOW + NOSIG) * SR)); body[:tc] = mix[:tc]; body[tc:tc + n] = slow * e
sn = int((SNOW + 0.08) * SR); noise = bp(rng.normal(0, 1, sn), 300, 7000)
body[tc:tc + sn] += noise / rms(noise) * db(-25) * np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
_, _, jing = pork_intro.build_intro(64, 48, 25); jing = jing[: int(INTRO * SR)]
jing = np.tanh(jing / rms(jing[int(0.4 * SR): int(2.0 * SR)]) * vref * db(2.0) * 0.55)
out = np.zeros(int(round(TOTAL * SR))); out[: len(jing)] = jing
b0 = body[int(PRE * SR):]; b0[: int(0.3 * SR)] *= np.linspace(0, 1, int(0.3 * SR)); out[len(jing): len(jing) + len(b0)] += b0[: len(out) - len(jing)]
print("total", TOTAL, "peak", np.abs(out).max().round(3), "voix/fond dB", round(20 * np.log10(rms(vo[act]) / vref), 1))
with wave.open("acte5_audio.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
# la voix seule, pour vérifier qu'il n'y a plus de frappe : énergie au-dessus de 5 kHz et forme des débuts de syllabe
hv = hp(vo, 5000, 4); print("voix : part d'énergie au-dessus de 5 kHz = %.5f" % (np.sum(hv ** 2) / np.sum(vo ** 2)))
