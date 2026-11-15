"""Douzi City, 5 h du matin (sans voix) : jingle de chaîne, nappe et clochettes au-dessus des toits, passage dans le O du titre (7,0 s),
groove « images de synthèse 90 » derrière le tramway, fanfare sur la place, boîte à musique devant la vitrine, montée sous l'enseigne,
final au lever du jour, fin vinyle sur NO SIGNAL. Temps = secondes du film (0 = première image après l'intro)."""
import sys, wave, numpy as np
from scipy.signal import iirpeak, lfilter
src = open('music3.py').read()
exec(src[:src.index("# ---------------- prologue")])
FILM = 41.5; INTRO = 2.6; CUT = FILM + 1.0; TOTAL = INTRO + CUT + SNOW + NOSIG
N = int((PRE + FILM + 12) * SR); B = lambda: np.zeros(N)
dry, wet, drums, sfx, sfxw = B(), B(), B(), B(), B()
tb = np.arange(N) / SR - PRE
env_t = lambda pts: np.interp(tb, [p[0] for p in pts], [p[1] for p in pts])
OFF = 0.25                                                      # une barre de mesure tombe sur le passage dans le O
bt = lambda b, beat=0.0: OFF + b * BAR + beat * BEAT
E8 = BEAT / 2

def ep(m, dur=0.9):                                             # piano électrique FM
    return fm(mtof(m), dur, 1.0, 1.6, 3.0, 7.0) + 0.22 * fm(mtof(m), dur, 14.0, 0.6, 9.0, 20.0)
def chord(buf, notes, t, dur, g, fn):
    for n in notes: place(buf, fn(n, dur), t, g / len(notes))
def choir(notes, dur, a=0.4):
    y = sum(saw(mtof(n), dur, (-0.10, 0.0, 0.09)) for n in notes) / len(notes); out = 0
    for fc, q, g in ((700, 5, 1.0), (1100, 6, 0.6), (2600, 8, 0.25)):
        b_, a_ = iirpeak(fc, q, fs=SR); out = out + lfilter(b_, a_, y) * g
    return out * ar(len(y), a, 0.6)
def sub(m, dur, a=0.4, r=0.5):
    n = int(dur * SR); return np.sin(2 * np.pi * mtof(m) * np.arange(n) / SR) * ar(n, a, r)
def held(notes, dur, tail=0.35):                                # accord de cuivres tenu, qui retombe
    y = sum(brass(m, dur) for m in notes); return y * np.linspace(1, tail, len(y))

# ---------------------------------------------------------------- A · au-dessus des toits (0 – 7,0)
for b, ch in ((0, Am9), (1, F9)):
    place(dry, pad(ch["p"], BAR + 0.5, 1100 + 500 * b, 0.9), bt(b) - 0.2, 0.25)
    place(wet, pad([n + 12 for n in ch["p"][:3]], BAR + 0.5, 2200, 1.0), bt(b) - 0.2, 0.16)
    place(dry, sub(ch["b"], BAR + 0.3), bt(b) - 0.1, 0.22)
    for i in range(8):
        y = bell(mtof(ch["p"][[0, 2, 4, 2, 3, 1, 4, 2][i]] + 24), 1.4); g = 0.05 + 0.012 * (b * 8 + i)
        place(dry, y, bt(b, i * 0.5), g); place(wet, y, bt(b, i * 0.5), g * 1.2)
for k, ch in enumerate((Dm7, G)):
    t = bt(2, 2 * k)
    place(dry, pad(ch["p"], 2 * BEAT + 0.4, 1800 + 900 * k, 0.3), t - 0.1, 0.40)
    place(wet, pad([n + 12 for n in ch["p"][:3]], 2 * BEAT + 0.4, 3000, 0.3), t - 0.1, 0.20)
    place(dry, sub(ch["b"], 2 * BEAT + 0.2, 0.2, 0.2), t, 0.34)
    for i in range(8):
        y = bell(mtof(ch["p"][i % 5] + 24 + 12 * (i // 5)), 1.2); g = 0.10 + 0.010 * (k * 8 + i)
        place(dry, y, t + i * S16, g * 0.6); place(wet, y, t + i * S16, g * 0.7)
PENT = [84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 110, 112, 115]
for i, m in enumerate(PENT): place(wet, bell(mtof(m), 0.9), 5.95 + i * 0.075, 0.025 + 0.006 * i)       # les paillettes du titre
riser(4.9, 2.1, 0.045)
wh = whoosh(1.7, 400, 5200, 2.5); place(sfx, wh / np.abs(wh).max(), 6.15, 0.55)                                                   # on passe dans le O

# ---------------------------------------------------------------- le coup d'orchestre et la descente vers les rails (7,0 – 9,25)
place(dry, orch(C9["p"][:3], 1.1), 7.0, 0.95); place(wet, orch(C9["p"][:3], 1.1), 7.0, 0.35); place(wet, crash(), 7.0, 0.16)
place(drums, K2, 7.0, 1.0); place(drums, STOMP, 7.0, 0.8)
place(dry, held((48, 55, 60, 64, 67, 72), 2.1), 7.0, 0.17); place(wet, choir((60, 64, 67, 72), 2.2, 0.05), 7.0, 0.16)
n_ = int(1.8 * SR); d_ = np.arange(n_) / SR
place(dry, np.sin(2 * np.pi * np.cumsum(34 + 60 * np.exp(-d_ * 2.4)) / SR) * ar(n_, 0.01, 0.6), 7.0, 0.55)   # la chute
for i, f in enumerate((210, 210, 170, 170, 135, 135, 105, 105)): place(drums, tom(f), bt(3, 2) + i * S16, 0.30 + 0.045 * i)
roll(bt(3, 3), bt(4), 0.06, 0.30)

# ---------------------------------------------------------------- le groove
def groove(b, ch, ch2=None, lvl=1.0, last=16):
    t0 = bt(b); ch2 = ch2 or ch
    for s in (0, 6, 8):
        if s < last: place(drums, K, t0 + s * S16, (0.95 if s != 6 else 0.6) * lvl)
    for s in (4, 12):
        if s < last: place(drums, SN, t0 + s * S16, 0.50 * lvl)
    if last > 12: place(drums, CL, t0 + 12 * S16, 0.22 * lvl)
    for s in range(0, min(last, 16), 2): place(drums, HH, t0 + s * S16, 0.05 if s % 4 else 0.035)
    if last > 14: place(drums, HO, t0 + 14 * S16, 0.045)
    for s, o in ((0, 0), (3, 0), (6, 12), (8, 0), (10, 7), (12, 0), (14, 12), (15, 10)):
        if s < last: place(dry, bassn((ch if s < 8 else ch2)["b"] + o, S16 * (2.6 if s in (0, 8) else 1.5)), t0 + s * S16, 0.42 * lvl)
    for s in (0, 6, 12):
        if s < last:
            c = (ch if s < 8 else ch2)["p"][:4]; chord(dry, c, t0 + s * S16, 0.7, 0.30 * lvl, ep); chord(wet, c, t0 + s * S16, 0.7, 0.10 * lvl, ep)
def lead(t0, notes, g=1.0):                                     # la voix néon de la ville, doublée de cloches
    for b_, du, m in notes:
        place(dry, pwm(m, du * BEAT), t0 + b_ * BEAT, 0.55 * g); y = bell(mtof(m + 12), du * BEAT + 0.6)
        place(dry, y, t0 + b_ * BEAT, 0.09 * g); place(wet, y, t0 + b_ * BEAT, 0.10 * g)
def fanfare(t0, notes, g=1.0):
    for b_, du, m in notes:
        y = brass(m, du * BEAT * 0.95); place(dry, y, t0 + b_ * BEAT, 0.50 * g)
        place(dry, brass(m - 12, du * BEAT * 0.95), t0 + b_ * BEAT, 0.30 * g); place(wet, y, t0 + b_ * BEAT, 0.20 * g)

# ---------------------------------------------------------------- B · l'avenue, derrière le tramway (9,25 – 18,25)
for i in range(4):
    b = 4 + i; ch = MAIN[i]; groove(b, ch); lead(bt(b), BELLS[i]); arp_bar(bt(b), ch, 0.045)
    place(wet, pad([n + 12 for n in ch["p"][1:4]], BAR + 0.2, 2400, 0.2), bt(b), 0.10)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(8) - (4 - i) * S16, 0.32 + 0.05 * i)
place(wet, crash(), bt(8), 0.11)

# ---------------------------------------------------------------- C · la place, le Porc couronné (18,25 – 28,94)
for i in range(5):
    b = 8 + i; ch = MAIN[i % 4]; stop = i == 4
    groove(b, ch, last=12 if stop else 16)
    fanfare(bt(b), [(0, .75, 67), (.75, .25, 67), (1, 1, 72), (2, 1, 76)] if stop else FANF[i])
    place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.34); place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR * (0.75 if stop else 1) + 0.25), bt(b) - 0.05, 0.17)
    if not stop: arp_bar(bt(b), ch, 0.032, 24)
t_stop = bt(12, 3)                                              # l'arrêt net, sur le quatrième temps
place(dry, orch(C9["p"][:3], 0.8), t_stop, 0.8); place(wet, orch(C9["p"][:3], 0.8), t_stop, 0.4); place(drums, K, t_stop, 1.0); place(drums, SN, t_stop, 0.5)
place(dry, brass(79, 0.3), t_stop, 0.5); place(wet, crash(), t_stop, 0.07)
gl = np.random.default_rng(21)                                  # les éclats du chrome
for k in range(15): place(wet, bell(mtof(gl.choice([96, 98, 100, 103, 105, 108])), 0.8), 19.2 + 9.4 * gl.random(), 0.035)

# ---------------------------------------------------------------- D · devant la vitrine : une boîte à musique (29,0 – 31,75)
MB = [(-1, 76), (0, 72), (1, 77), (2, 81), (3, 84), (4, 81), (5, 77), (6, 76), (7, 72)]
for i, m in MB:
    y = bell(mtof(m + 12), 1.1); place(dry, y, bt(13) + i * E8, 0.17); place(wet, y, bt(13) + i * E8, 0.13)
chord(dry, F9["p"][:4], bt(13), 1.6, 0.20, ep); place(dry, pad(F9["p"], BAR + 0.2, 650, 0.3), bt(13) - 0.1, 0.22); place(dry, sub(F9["b"], BAR, 0.2, 0.4), bt(13), 0.20)

# ---------------------------------------------------------------- E · TOUJOURS PLUS : la montée (31,75 – 35,5)
T_CUT = 35.5
for b, ch in ((14, Dm7), (15, G)):
    t0 = bt(b); last = 16 if b == 14 else int((T_CUT - t0) / S16)
    for s in range(0, last, 4): place(drums, K, t0 + s * S16, 0.8)
    for s in range(last): place(dry, bassn(ch["b"] + (12 if s % 4 == 2 else 0), S16 * 1.3), t0 + s * S16, 0.28 + 0.012 * s)
    place(dry, pad(ch["p"], last * S16 + 0.15, 1400 + 1500 * (b - 14), 0.2), t0, 0.36)
    for s in (0, 3, 6, 10, 12):
        if s < last: chord(dry, ch["p"][:4], t0 + s * S16, 0.5, 0.28, ep)
    if b == 14: arp_bar(t0, ch, 0.05, 24)
    else:
        for i in range(last): y = fm(mtof(ch["p"][ARP[i % 8]] + 24), 0.5, 1.0, 2.0, 5.0, 9.0); place(dry, y, t0 + i * S16, 0.07); place(wet, y, t0 + i * S16, 0.03)
roll(bt(15), T_CUT, 0.05, 0.40); riser(33.2, 2.3, 0.08)
# le plan large : un accord suspendu, puis ça repart
place(dry, orch(G["p"][:3], 0.9), T_CUT, 0.8); place(wet, crash(), T_CUT, 0.15); place(drums, K2, T_CUT, 0.9)
place(dry, held((43, 55, 62, 67, 71, 74), 0.78, 0.8), T_CUT, 0.16)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(16) - (4 - i) * S16, 0.34 + 0.05 * i)

# ---------------------------------------------------------------- F · le jour se lève sur la ville (36,25 – fin)
groove(16, C9); fanfare(bt(16), FANF[0]); place(dry, orch(C9["p"][:3], 0.5), bt(16), 0.36); arp_bar(bt(16), C9, 0.04, 24)
place(wet, choir((72, 76, 79), BAR + 0.25), bt(16) - 0.05, 0.18)
groove(17, F9, G); fanfare(bt(17), [(0, .75, 65), (.75, .25, 65), (1, 1, 69), (2, 1, 74), (3, .5, 71), (3.5, .5, 74)])
place(dry, orch(F9["p"][:3], 0.5), bt(17), 0.36); place(dry, orch(G["p"][:3], 0.5), bt(17, 2), 0.30)
place(wet, choir((72, 77, 81), 2 * BEAT + 0.2), bt(17) - 0.05, 0.18); place(wet, choir((74, 79, 83), 2 * BEAT + 0.2), bt(17, 2) - 0.05, 0.18)
roll(bt(17, 3), bt(18), 0.10, 0.36)
tf = bt(18)                                                     # l'accord final, tenu jusqu'à ce que la bande s'arrête
place(dry, orch(C9["p"][:3], 1.5), tf, 1.0); place(wet, orch(C9["p"][:3], 1.5), tf, 0.4); place(wet, crash(), tf, 0.18); place(drums, K2, tf, 1.0); place(drums, STOMP, tf, 0.7)
place(dry, held((36, 48, 55, 60, 64, 67, 72, 76), 6.0, 0.55), tf, 0.15); place(wet, choir((60, 64, 67, 72, 76), 6.0, 0.08), tf, 0.20); place(dry, sub(24, 6.0, 0.02, 1.0), tf, 0.40)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)):
    y = bell(mtof(m), 1.6); place(dry, y, tf + i * S16, 0.10); place(wet, y, tf + i * S16, 0.12)

# ---------------------------------------------------------------- bruitages
g1 = lp(rng.normal(0, 1, N), 0.35); g1 = g1 / g1.std(); g2 = lp(rng.normal(0, 1, N), 1.1); g2 = g2 / g2.std()
wn = rng.normal(0, 1, N)
wind = bp(wn, 180, 900) * (0.8 + 0.35 * g1).clip(0.2, 1.8) + bp(wn, 900, 2600) * 0.35 * (0.5 + 0.5 * g2).clip(0, 1.6)        # le vent sur les toits
wind *= env_t([(-3, 0.9), (6.4, 0.9), (8.6, 0.32), (18.3, 0.30), (19.2, 0.50), (28.8, 0.50), (29.0, 0.14), (31.9, 0.14), (32.2, 0.55), (35.4, 0.6), (35.6, 0.9), (60, 1.0)])
# le tramway : sonnette, moteur, roulement, étincelles au fil, freins
def ding(f=1397.0, dur=1.3): return fm(f, dur, 2.76, 1.3, 3.2, 5.0) + 0.5 * fm(f * 1.498, dur, 2.0, 0.6, 4.5, 6.0)
for t, g in ((8.45, 0.40), (8.82, 0.40), (17.75, 0.22)): place(sfx, ding(), t, g); place(sfxw, ding(), t, g * 0.5)
for t, g in ((35.95, 0.16), (36.3, 0.16)): place(sfxw, ding(), t, g)
ph = np.cumsum(np.interp(tb, [8.8, 9.6, 17.4, 18.2], [45, 104, 104, 40])) / SR
motor = lp(2 * (ph % 1) - 1, 420) * 0.6 + np.sin(2 * np.pi * 2 * ph) * 0.4 + lp(rng.normal(0, 1, N), 130, 4) * 5.0
motor *= env_t([(-3, 0), (8.8, 0), (9.5, 1.0), (17.3, 0.9), (18.3, 0), (60, 0)]) * (0.85 + 0.15 * np.sin(2 * np.pi * 3.1 * tb))
for t in np.arange(9.9, 17.6, 1.17):                             # les joints de rails
    for dt in (0.0, 0.16):
        d_ = np.arange(int(0.16 * SR)) / SR; place(sfx, lp(rng.normal(0, 1, len(d_)), 170, 4) * np.exp(-d_ * 26) * np.minimum(d_ / 0.01, 1) * 6, t + dt, 0.16)
sp = np.random.default_rng(31)
for t in (10.3, 11.9, 12.6, 14.4, 15.2, 16.7, 37.4, 38.3):       # étincelles au fil
    n_ = int(sp.uniform(0.10, 0.22) * SR); d_ = np.arange(n_) / SR
    z = bp(sp.normal(0, 1, n_), 1800, 7500) * (0.5 + 0.5 * np.sign(np.sin(2 * np.pi * 100 * d_))) * np.exp(-d_ * 14) * np.minimum(d_ / 0.006, 1)
    place(sfxw, z, t, 0.10 if t < 30 else 0.05)
n_ = int(0.9 * SR); d_ = np.arange(n_) / SR                      # les freins, à l'arrivée sur la place
sq = np.sin(2 * np.pi * np.cumsum(2100 - 500 * d_ / 0.9 + 25 * np.sin(2 * np.pi * 31 * d_)) / SR) * np.sin(np.pi * d_ / 0.9) ** 2
place(sfxw, sq, 17.35, 0.035)
# l'enseigne : le bourdonnement des tubes, les ampoules qui courent
phn = 100 * tb; buzz = hp(2 * (phn % 1) - 1, 900) * 0.5 + bp(np.sign(np.sin(2 * np.pi * phn)), 2200, 5200) * 0.5
buzz *= (0.75 + 0.25 * np.sign(np.sin(2 * np.pi * 7.0 * tb))) * env_t([(-3, 0), (31.95, 0), (32.05, 1.0), (35.4, 1.0), (35.55, 0.12), (41.5, 0.0), (60, 0)])
# devant la vitrine : la rumeur chaude de la boutique
shop = lp(rng.normal(0, 1, N), 300, 2) * env_t([(-3, 0), (28.95, 0), (29.1, 1), (31.9, 1), (32.05, 0), (60, 0)])

# ---------------------------------------------------------------- le mélange
mus = dry + reverb(wet) * 0.9 + drums + reverb(drums, 0.9, 6000) * 0.10
fxs = sfx + reverb(sfxw, 2.4, 5000) * 0.9
ref = np.percentile(np.abs(mus[: int((PRE + FILM) * SR)]), 99.9); mus = mus / ref * 0.8; fxs = fxs / ref * 0.8
vref = rms(mus[int((PRE + 9.3) * SR): int((PRE + 18.2) * SR)])
lvl = lambda x, a, b: rms(x[int((PRE + a) * SR): int((PRE + b) * SR)])
fxs = fxs + wind / lvl(wind, 0.5, 6.0) * vref * 0.34 + motor / lvl(motor, 10, 17) * vref * 0.30 \
    + buzz / lvl(buzz, 32.2, 35.2) * vref * 0.20 + shop / lvl(shop, 29.2, 31.8) * vref * 0.10
mix = np.tanh((mus + fxs) * 0.62)
for a, b, nm in ((0, 7, "toits"), (7, 9.25, "coup"), (9.25, 18.25, "avenue"), (18.25, 28.9, "place"), (29, 31.75, "vitrine"), (31.75, 35.5, "enseigne"), (35.5, 40.75, "jour"), (40.75, 42.5, "final")):
    print("%5.1f-%5.1f %-9s musique %6.1f dB  bruitages %6.1f dB  total %6.1f dB" % (a, b, nm, 20 * np.log10(lvl(mus, a, b)), 20 * np.log10(lvl(fxs, a, b)), 20 * np.log10(lvl(mix, a, b))))
tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n                # la bande s'arrête comme un vinyle
speed = (1 - uu) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * uu * D))
slow = lp(np.interp(np.clip(tc + np.cumsum(speed), 0, N - 2), np.arange(N), mix), 5000)
e = db(-3.0 * uu); tl = int(0.25 * SR); e[-tl:] *= np.linspace(1, 0, tl) ** 2
body = np.zeros(int((PRE + CUT + SNOW + NOSIG) * SR)); body[:tc] = mix[:tc]; body[tc:tc + n] = slow * e
sn = int((SNOW + 0.08) * SR); noise = bp(rng.normal(0, 1, sn), 300, 7000)
body[tc:tc + sn] += noise / rms(noise) * db(-25) * np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
_, _, jing = pork_intro.build_intro(64, 48, 25); jing = jing[: int(INTRO * SR)]
jing = np.tanh(jing / rms(jing[int(0.4 * SR): int(2.0 * SR)]) * lvl(mix, 9.3, 18.2) * db(-1.0))
out = np.zeros(int(round(TOTAL * SR))); out[: len(jing)] = jing
b0 = body[int(PRE * SR):]; b0[: int(0.3 * SR)] *= np.linspace(0, 1, int(0.3 * SR)); out[len(jing): len(jing) + len(b0)] += b0[: len(out) - len(jing)]
print("total", TOTAL, "peak", np.abs(out).max().round(3))
with wave.open("city_audio.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
