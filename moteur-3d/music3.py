"""Bande-son du film, version sans voix : jingle Channel Pork (mêmes notes, on le laisse sonner), lever du jour et plongée,
musique synthétisée calée sur les scènes (mesure = 2,25 s), bruitages, puis fin 'vinyle qui s'arrête' sur NO SIGNAL.
Les temps sont ceux du film 3D (0 = sortie de la narine) ; les tampons commencent PRE secondes avant."""
import sys, wave, numpy as np
from scipy.signal import butter, sosfilt, fftconvolve
sys.path.insert(0, "/home/claude/channelpork")
import pork_intro

SR = 48000
BAR = 2.25; BEAT = BAR / 4; S16 = BEAT / 4
INTRO, BODY, FREEZE, SNOW, NOSIG = 4.5, 76.5, 1.0, 0.45, 3.55
PRE = 2.6
CUT = BODY + FREEZE
TOTAL = INTRO + CUT + SNOW + NOSIG
N = int((PRE + BODY + 12) * SR)
rng = np.random.default_rng(7)
db = lambda x: 10 ** (x / 20)
rms = lambda x: np.sqrt(np.mean(x ** 2) + 1e-12)
mtof = lambda m: 440 * 2 ** ((m - 69) / 12)
lp = lambda x, fc, o=2: sosfilt(butter(o, fc, fs=SR, output="sos"), x)
hp = lambda x, fc, o=2: sosfilt(butter(o, fc, "highpass", fs=SR, output="sos"), x)
bp = lambda x, a, b: sosfilt(butter(2, [a, b], btype="band", fs=SR, output="sos"), x)
B = lambda: np.zeros(N)
dry, wet, drums, pd, pdr, pw, sfx, sfxw = B(), B(), B(), B(), B(), B(), B(), B()

def place(buf, y, t0, g=1.0):
    i = int(round((t0 + PRE) * SR)); n = min(len(y), len(buf) - i)
    if n > 0 and i >= 0: buf[i:i + n] += y[:n] * g

def ar(n, a, r):
    e = np.ones(n); na, nr = max(int(a * SR), 1), max(int(r * SR), 1)
    na, nr = min(na, n), min(nr, n)
    e[:na] = np.linspace(0, 1, na); e[-nr:] *= np.linspace(1, 0, nr); return e

def fm(f, dur, ratio=1.0, idx=2.2, dec=2.2, idec=6.0):
    d = np.arange(int(dur * SR)) / SR
    y = np.sin(2 * np.pi * f * d + idx * np.exp(-d * idec) * np.sin(2 * np.pi * f * ratio * d))
    return y * np.minimum(d / 0.004, 1) * np.exp(-d * dec) * ar(len(d), 0.001, 0.02)

def bell(f, dur): return fm(f, dur, 3.5, 1.6, 1.5, 2.5)

def saw(f, dur, det=(-0.08, 0.0, 0.08)):
    d = np.arange(int(dur * SR)) / SR; y = 0
    for c in det: y = y + 2 * ((f * 2 ** (c / 12) * d + rng.random()) % 1) - 1
    return y / len(det)

def pad(notes, dur, fc=1500, a=0.35):
    y = sum(saw(mtof(n), dur) for n in notes) / len(notes)
    return lp(y, fc) * ar(len(y), a, 0.5)

def brass(m, dur):
    f = mtof(m); n = int(dur * SR); d = np.arange(n) / SR
    vib = 1 + 0.004 * np.sin(2 * np.pi * 5.5 * d) * np.clip(d / 0.3, 0, 1)
    ph = np.cumsum(f * vib) / SR; y = 0
    for c in (-0.06, 0.05): y = y + 2 * ((ph * 2 ** (c / 12) + rng.random()) % 1) - 1
    a = np.clip(d / 0.06, 0, 1)
    y = lp(y, 900) * (1 - a * 0.7) + lp(y, 3200) * a * 0.7
    return y * ar(n, 0.03, 0.08) * 0.5

def bassn(m, dur):
    f = mtof(m); n = int(dur * SR); d = np.arange(n) / SR
    y = lp(2 * ((f * d) % 1) - 1, 500) * 0.6 + np.sin(2 * np.pi * f * d)
    return y * np.minimum(d / 0.005, 1) * np.exp(-d * 2.5) * ar(n, 0.002, 0.03)

def whistle(m, dur):                      # soft lead for the beer scene
    f = mtof(m); n = int(dur * SR); d = np.arange(n) / SR
    vib = 1 + 0.006 * np.sin(2 * np.pi * 5.2 * d) * np.clip(d / 0.25, 0, 1)
    ph = 2 * np.pi * np.cumsum(f * vib) / SR
    return (np.sin(ph) + 0.25 * np.sin(2 * ph) + 0.08 * np.sin(3 * ph)) * ar(n, 0.05, 0.12) * 0.6

def pwm(m, dur):                          # neon lead for the city
    f = mtof(m); n = int(dur * SR); d = np.arange(n) / SR
    vib = 1 + 0.005 * np.sin(2 * np.pi * 5.0 * d) * np.clip(d / 0.3, 0, 1)
    ph = (np.cumsum(f * vib) / SR) % 1
    y = np.where(ph < 0.5 + 0.3 * np.sin(2 * np.pi * 0.7 * d), 1.0, -1.0)
    return lp(y, 2400) * ar(n, 0.02, 0.15) * 0.35

def acid(m, dur):                         # squelchy 16th for the party
    f = mtof(m - 12); n = int(dur * SR); d = np.arange(n) / SR
    y = 2 * ((f * d) % 1) - 1; e = np.exp(-d * 28)
    return (lp(y, 500) * (1 - e) + bp(y, 700, 2600) * e * 2.2) * ar(n, 0.002, 0.02)

def orch(notes, dur=0.5):                 # 'orchestra hit' sampler stab
    y = 0
    for n in notes:
        for o in (-12, 0, 12): y = y + saw(mtof(n + o), dur, (-0.15, -0.05, 0.06, 0.14))
    d = np.arange(len(y)) / SR
    y = lp(y / (len(notes) * 3), 3600) * np.exp(-d * 7) + hp(rng.normal(0, 1, len(d)), 2000) * np.exp(-d * 60) * 0.25
    return y * ar(len(d), 0.002, 0.03)

def kick(f0=46, dec=8, sw=120):
    d = np.arange(int(0.42 * SR)) / SR; f = f0 + sw * np.exp(-d * 30)
    return np.sin(2 * np.pi * np.cumsum(f) / SR) * np.exp(-d * dec) * ar(len(d), 0.0005, 0.02)

def snare():
    d = np.arange(int(0.2 * SR)) / SR
    y = hp(rng.normal(0, 1, len(d)), 1200) * np.exp(-d * 16) * 0.7 + np.sin(2 * np.pi * 185 * d) * np.exp(-d * 24)
    g = int(0.2 * SR); gate = rng.normal(0, 1, g) * np.linspace(1, 0.6, g)
    w = fftconvolve(y, lp(gate, 5000))[: int(0.34 * SR)]
    w = w / np.abs(w).max(); w[-int(0.02 * SR):] *= np.linspace(1, 0, int(0.02 * SR))
    out = np.zeros(len(w)); out[:len(y)] += y; return out * 0.8 + w * 0.5

def clap():
    n = int(0.22 * SR); d = np.arange(n) / SR; y = np.zeros(n)
    for t0 in (0, 0.011, 0.022, 0.034):
        m = d >= t0; y[m] += rng.normal(0, 1, m.sum()) * np.exp(-(d[m] - t0) * (90 if t0 < 0.03 else 22))
    return bp(y, 900, 3800) * 0.6

def hat(op=False):
    d = np.arange(int((0.25 if op else 0.05) * SR)) / SR
    return hp(rng.normal(0, 1, len(d)), 7000) * np.exp(-d * (14 if op else 70))

def rim():
    d = np.arange(int(0.05 * SR)) / SR
    return (np.sin(2 * np.pi * 1750 * d) + 0.5 * np.sin(2 * np.pi * 820 * d)) * np.exp(-d * 90) + hp(rng.normal(0, 1, len(d)), 3000) * np.exp(-d * 200) * 0.3

def tom(f):
    d = np.arange(int(0.35 * SR)) / SR
    return np.sin(2 * np.pi * np.cumsum(f * (1 + 0.5 * np.exp(-d * 25))) / SR) * np.exp(-d * 9)

def crash():
    d = np.arange(int(2.5 * SR)) / SR
    return hp(rng.normal(0, 1, len(d)), 3500) * np.exp(-d * 1.8) * np.minimum(d / 0.003, 1)

def stomp():
    d = np.arange(int(0.2 * SR)) / SR
    return np.sin(2 * np.pi * np.cumsum(45 + 40 * np.exp(-d * 30)) / SR) * np.exp(-d * 18) + lp(rng.normal(0, 1, len(d)), 300) * np.exp(-d * 60) * 0.6

def ping():                               # the sonar, plugged into the speakers
    d = np.arange(int(1.6 * SR)) / SR
    return (np.sin(2 * np.pi * 1318.5 * d) + 0.3 * np.sin(2 * np.pi * 2637 * d)) * np.exp(-d * 3.2) * np.minimum(d / 0.003, 1)

def whoosh(dur, f0, f1, shape=2.0):
    n = int(dur * SR); x = rng.normal(0, 1, n); u = np.arange(n) / n
    fc = f0 * (f1 / f0) ** u; out = np.zeros(n)
    for bc in np.geomspace(min(f0, f1) * 0.7, max(f0, f1) * 1.4, 6):
        out += bp(x, bc / 1.35, bc * 1.35) * np.exp(-(np.log(fc / bc) / 0.35) ** 2)
    return out * np.sin(np.pi * u) ** shape

def blip(f0, dur=0.07):
    d = np.arange(int(dur * SR)) / SR
    return np.sin(2 * np.pi * np.cumsum(f0 * (1 + 1.5 * d / dur)) / SR) * np.sin(np.pi * d / dur) ** 2

def reverb(x, secs=2.2, damp=4500):
    L = int(secs * SR); ir = lp(rng.normal(0, 1, L) * np.exp(-np.arange(L) / SR * 6.9 / secs), damp)
    return fftconvolve(x, ir / np.sqrt(np.sum(ir ** 2)))[: len(x)]

def roll(t0, t1, g0, g1, buf=None, snd=None, step=S16):
    k = int(round((t1 - t0) / step))
    for i in range(k): place(drums if buf is None else buf, SN if snd is None else snd, t0 + i * step, g0 + (g1 - g0) * i / max(k - 1, 1))

def riser(t0, dur, g):
    n = int(dur * SR); d = np.arange(n) / SR
    place(wet, hp(rng.normal(0, 1, n), 2000) * (d / dur) ** 3, t0, g)

C9 = dict(b=36, p=[60, 64, 67, 71, 74]); Am9 = dict(b=33, p=[57, 60, 64, 67, 71])
F9 = dict(b=41, p=[60, 64, 65, 69, 72]); G = dict(b=43, p=[59, 62, 67, 69, 74])
Em7 = dict(b=40, p=[59, 62, 64, 67, 71]); Dm7 = dict(b=38, p=[57, 60, 62, 65, 69])
MAIN = [C9, Am9, F9, G]
BELLS = [[(0, 2, 79), (2, 1, 76), (3, 1, 74)], [(0, 3, 72), (3, 1, 76)], [(0, 2, 81), (2, 1, 77), (3, 1, 76)], [(0, 4, 74)]]
FANF = [[(0, .75, 67), (.75, .25, 67), (1, 1, 72), (2, 1, 76), (3, 1, 79)], [(0, 1.5, 76), (1.5, .5, 72), (2, 2, 69)],
        [(0, .75, 65), (.75, .25, 65), (1, 1, 69), (2, 1, 72), (3, 1, 77)], [(0, 1.5, 74), (1.5, .5, 71), (2, 2, 79)]]
K, K2, SN, CL, HH, HO, RIM, STOMP, PING = kick(), kick(42, 6.5, 150), snare(), clap(), hat(), hat(True), rim(), stomp(), ping()
ARP = [0, 1, 2, 3, 4, 3, 2, 1]

def arp_bar(t0, ch, g, octave=12):
    for i in range(16):
        y = fm(mtof(ch["p"][ARP[i % 8]] + octave), 0.5, 1.0, 2.0, 5.0, 9.0)
        place(dry, y, t0 + i * S16, g); place(wet, y, t0 + i * S16 + 3 * S16, g * 0.45); place(wet, y, t0 + i * S16, g * 0.25)

# ---------------- prologue : the day rises behind the medal, then the dive through the nostril ----------------
y = pad([36, 48, 55, 60, 64, 67, 71], 3.2, 900, 1.4); place(dry, y, -2.6, 1.15); place(wet, y, -2.6, 0.5)
n_ = int(3.2 * SR); d_ = np.arange(n_) / SR
place(dry, np.sin(2 * np.pi * 65.41 * d_) * np.clip(d_ / 1.8, 0, 1) ** 1.5 * np.clip((3.2 - d_) / 0.9, 0, 1), -2.4, 0.5)
place(sfx, whoosh(1.0, 250, 2600, 1.5), -1.15, 0.75); place(sfx, whoosh(1.1, 2600, 220, 1.5), -0.25, 0.7)
for nn in (84, 91):
    y = bell(mtof(nn), 2.5); place(dry, y, 0.0, 0.10); place(wet, y, 0.0, 0.14)
# ---------------- A : dawn (bars 0-3) ----------------
for b in range(4):
    ch, t0 = MAIN[b], b * BAR
    place(dry, pad(ch["p"], BAR + 0.4, 900 + 250 * b, 0.8 if b == 0 else 0.35), t0, 0.5)
    if b >= 1: arp_bar(t0, ch, 0.10 + 0.06 * b)
    for bt, du, n in BELLS[b]:
        y = bell(mtof(n), du * BEAT + 1.2); place(dry, y, t0 + bt * BEAT, 0.2); place(wet, y, t0 + bt * BEAT, 0.22)
    if b >= 2: place(dry, bassn(36, BAR) * ar(int(BAR * SR), 0.3, 0.3), t0, 0.3)
riser(3 * BAR, BAR, 0.12); roll(3 * BAR + 2 * BEAT, 4 * BAR, 0.06, 0.4)
# ---------------- B : the monument (bars 4-7) ----------------
place(drums, crash(), 4 * BAR, 0.2)
y = orch(C9["p"]); place(dry, y, 4 * BAR, 0.5); place(wet, y, 4 * BAR, 0.3)
for b in range(4, 8):
    ch, t0 = MAIN[b % 4], b * BAR
    place(dry, pad(ch["p"], BAR + 0.4, 1700), t0, 0.42); arp_bar(t0, ch, 0.26)
    for bt in (0, 1.5, 2, 3, 3.5): place(dry, bassn(ch["b"], 0.5), t0 + bt * BEAT, 0.5)
    for bt in (0, 1.75, 2): place(drums, K, t0 + bt * BEAT, 0.95)
    for bt in (1, 3): place(drums, SN, t0 + bt * BEAT, 0.55)
    for i in range(8): place(drums, HO if i == 7 else HH, t0 + i * BEAT / 2, 0.10 if i % 2 else 0.07)
    for bt, du, n in BELLS[b % 4]:
        y = fm(mtof(n - 12), du * BEAT + 0.8, 1.0, 2.6, 2.0, 5.0); place(dry, y, t0 + bt * BEAT, 0.3); place(wet, y, t0 + bt * BEAT, 0.16)
    for n in (ch["p"][0] + 12, ch["p"][2] + 12, ch["p"][4] + 12): place(dry, brass(n, 0.28), t0, 0.10)
# ---------------- liquid metal (bars 8-9) ----------------
t0 = 8 * BAR
place(dry, pad(F9["p"], BAR + 0.4, 1300), t0, 0.45); place(dry, pad(G["p"], BAR + 0.6, 1900), t0 + BAR, 0.5)
place(dry, bassn(41, BAR), t0, 0.45); place(dry, bassn(43, BAR), t0 + BAR, 0.5); place(drums, K, t0, 0.9)
SCALE = [60, 62, 64, 65, 67, 69, 71, 72, 74, 76, 77, 79, 81, 83, 84, 86, 88, 89, 91, 93, 95, 96]
for i in range(32):
    place(drums, HH, t0 + i * S16, 0.04 + 0.002 * i)
    y = fm(mtof(SCALE[min(int(i * len(SCALE) / 32), len(SCALE) - 1)]), 0.4, 1.0, 2.2, 6.0, 9.0)
    place(dry, y, t0 + i * S16, 0.09 + 0.004 * i); place(wet, y, t0 + i * S16, 0.12)
riser(t0 + BAR, BAR, 0.14); roll(21.375, 22.5, 0.12, 0.5)
# ---------------- C : the parade (bars 10-13) ----------------
y = orch(C9["p"]); place(dry, y, 10 * BAR, 0.55); place(wet, y, 10 * BAR, 0.3)
for b in range(10, 14):
    ch, t0 = MAIN[(b - 10) % 4], b * BAR
    place(dry, pad(ch["p"], BAR + 0.4, 1500), t0, 0.33)
    for bt in range(4):
        place(drums, K, t0 + bt * BEAT, 0.9); place(sfx, STOMP, t0 + bt * BEAT, 0.5)
        place(dry, bassn(ch["b"] + (7 if bt % 2 else 0), 0.45), t0 + bt * BEAT, 0.5)
        place(drums, SN, t0 + bt * BEAT, 0.5 if bt % 2 else 0.2); place(drums, SN, t0 + (bt + 0.5) * BEAT, 0.15); place(drums, SN, t0 + (bt + 0.75) * BEAT, 0.2)
    for i in range(8): place(drums, HH, t0 + i * BEAT / 2, 0.06)
    for bt, du, n in FANF[(b - 10) % 4]:
        for o, g in ((0, 0.34), (12, 0.15)):
            y = brass(n + o, du * BEAT * 0.95); place(dry, y, t0 + bt * BEAT, g); place(wet, y, t0 + bt * BEAT, g * 0.5)
        place(dry, bell(mtof(n + 12), du * BEAT + 0.5), t0 + bt * BEAT, 0.07)
    if b in (10, 12): place(drums, crash(), t0, 0.18)
t0 = 13 * BAR
roll(t0 + 3 * BEAT, t0 + BAR, 0.10, 0.4, step=S16 / 2); roll(t0 + 2 * BEAT, t0 + BAR, 0.06, 0.4, snd=tom(62), step=S16 / 2)
place(dry, pad([48, 55, 62, 67, 71, 74], BAR + 0.3, 2200, 1.2), t0, 0.5)          # swell on the Cube
# ---------------- D : the beer (bars 14-16) ----------------
BEERCH = [dict(b=41, p=[57, 60, 64, 65, 69]), Em7, Dm7]
LEAD = [[(0, 2, 69), (2, 1, 72), (3, 1, 76)], [(0, 2, 74), (2, 2, 71)], [(0, 1, 72), (1, 1, 69), (2, 2, 65)]]
for k, b in enumerate(range(14, 17)):
    ch, t0 = BEERCH[k], b * BAR
    for bt in (0, 1.5, 3):
        for n in ch["p"]:
            y = fm(mtof(n), 1.3, 1.0, 1.1, 2.4, 4.0); place(dry, y, t0 + bt * BEAT, 0.085); place(wet, y, t0 + bt * BEAT, 0.045)
    place(dry, bassn(ch["b"], 0.9), t0, 0.5); place(dry, bassn(ch["b"], 0.5), t0 + 2.5 * BEAT, 0.38)
    place(drums, K, t0, 0.6)
    for bt in (1, 3): place(drums, RIM, t0 + bt * BEAT, 0.3); place(wet, RIM, t0 + bt * BEAT, 0.1)
    for i in range(8): place(drums, HH, t0 + i * BEAT / 2, 0.035)
    for bt, du, n in LEAD[k]:
        y = whistle(n, du * BEAT * 0.95); place(dry, y, t0 + bt * BEAT, 0.2); place(wet, y, t0 + bt * BEAT, 0.2)
for i in range(10):                                                               # run up before the dive
    y = bell(mtof(SCALE[6 + i]), 0.8); place(dry, y, 37.35 + i * 0.085, 0.09); place(wet, y, 37.35 + i * 0.085, 0.1)
# ---------------- E + F : the party (bars 17-23, muffled until the cut at 45 s) ----------------
PCH = [F9, C9, G, Am9, F9, C9, G]
for k, b in enumerate(range(17, 24)):
    ch, t0 = PCH[k], b * BAR
    for bt in range(4):
        place(pdr, K2, t0 + bt * BEAT, 1.0)
        place(pd, bassn(ch["b"], 0.24), t0 + (bt + 0.5) * BEAT, 0.6); place(pd, bassn(ch["b"] + 12, 0.12), t0 + (bt + 0.75) * BEAT, 0.3)
        place(pdr, HO, t0 + (bt + 0.5) * BEAT, 0.09)
    for bt in (1, 3): place(pdr, CL, t0 + bt * BEAT, 0.55); place(pw, CL, t0 + bt * BEAT, 0.12)
    for i in range(16):
        place(pdr, HH, t0 + i * S16, 0.035)
        place(pd, acid(ch["p"][[0, 2, 4, 2, 1, 3, 4, 3][i % 8]] + (12 if i % 4 < 2 else 0), 0.135), t0 + i * S16, 0.13)
    for bt in (0, 1.5, 2.75):
        y = orch(ch["p"], 0.3); place(pd, y, t0 + bt * BEAT, 0.22); place(pw, y, t0 + bt * BEAT, 0.1)
    place(pw, PING, t0, 0.3); place(pd, PING, t0, 0.07)
roll(23 * BAR + 3 * BEAT, 24 * BAR, 0.2, 0.5, buf=pdr, snd=tom(110), step=S16)
# ---------------- G : Douzi City (bars 24-27) ----------------
CCH = [Am9, F9, dict(b=40, p=[60, 64, 67, 71, 76]), G]
MEL = [[(0, 1.5, 76), (1.5, 0.5, 74), (2, 2, 72)], [(0, 3, 69), (3, 1, 72)], [(0, 1.5, 79), (1.5, 0.5, 76), (2, 2, 74)], [(0, 4, 74)]]
for k, b in enumerate(range(24, 28)):
    ch, t0 = CCH[k], b * BAR
    place(dry, pad(ch["p"], BAR + 0.5, 1200 + 250 * k, 0.5), t0, 0.5)
    place(dry, bassn(ch["b"], 1.6), t0, 0.5); place(dry, bassn(ch["b"], 0.5), t0 + 2.5 * BEAT, 0.3)
    place(drums, K, t0, 0.7); place(drums, K, t0 + 2 * BEAT, 0.6)
    for bt in (1, 3): place(drums, RIM, t0 + bt * BEAT, 0.35); place(wet, RIM, t0 + bt * BEAT, 0.3)
    for i in range(8): place(drums, HH, t0 + i * BEAT / 2, 0.04)
    for bt, du, n in MEL[k]:
        y = pwm(n, du * BEAT * 0.97); place(dry, y, t0 + bt * BEAT, 0.2); place(wet, y, t0 + bt * BEAT, 0.18); place(wet, y, t0 + (bt + 0.75) * BEAT, 0.08)
    if k >= 2: arp_bar(t0, ch, 0.10 + 0.08 * (k - 2))
    for j in range(7): place(wet, bell(mtof(int(rng.choice([84, 88, 91, 95, 96]))), 1.5), t0 + rng.random() * BAR, 0.05)   # snow
riser(27 * BAR, BAR, 0.15); roll(27 * BAR + 2 * BEAT, 28 * BAR, 0.06, 0.45)
# ---------------- H : finale (bars 28-33) ----------------
for b in (28, 30): place(drums, crash(), b * BAR, 0.24)
y = orch(C9["p"]); place(dry, y, 28 * BAR, 0.6); place(wet, y, 28 * BAR, 0.35)
for k, b in enumerate(range(28, 32)):
    ch, t0 = MAIN[k], b * BAR
    place(dry, pad(ch["p"] + [ch["p"][0] - 12], BAR + 0.5, 2000), t0, 0.5)
    place(dry, bassn(ch["b"], 1.2), t0, 0.55); place(dry, bassn(ch["b"], 0.5), t0 + 2.5 * BEAT, 0.4)
    place(drums, K, t0, 1.0); place(drums, K, t0 + 2.5 * BEAT, 0.8); place(drums, SN, t0 + 2 * BEAT, 0.6)
    for i in range(8): place(drums, HH, t0 + i * BEAT / 2, 0.06)
    arp_bar(t0, ch, 0.2)
    for bt, du, n in BELLS[k]:
        y = brass(n - 12, du * BEAT * 0.97); place(dry, y, t0 + bt * BEAT, 0.3); place(wet, y, t0 + bt * BEAT, 0.15)
        place(dry, brass(n, du * BEAT * 0.97), t0 + bt * BEAT, 0.14)
        y = bell(mtof(n + 12), du * BEAT + 1.0); place(dry, y, t0 + bt * BEAT, 0.12); place(wet, y, t0 + bt * BEAT, 0.12)
t0 = 32 * BAR; HOLD = 15.0
roll(t0 - 2 * BEAT, t0, 0.05, 0.35, snd=tom(65), step=S16 / 2)
place(dry, pad([36, 48, 55, 60, 64, 67, 71, 74], HOLD, 2200), t0, 0.8)
place(dry, bassn(36, 4.0), t0, 0.7); place(drums, K, t0, 1.0); place(drums, crash(), t0, 0.28); place(drums, SN, t0, 0.5)
y = orch(C9["p"]); place(dry, y, t0, 0.6); place(wet, y, t0, 0.35)
for o, g in ((72, 0.22), (79, 0.18), (84, 0.12)):
    y = brass(o, 6.5); place(dry, y, t0, g); place(wet, y, t0, g * 0.6)
for i, n in enumerate([72, 76, 79, 83, 86, 88, 91, 95, 98, 96, 91, 88]):
    y = bell(mtof(n), 3.0); place(dry, y, t0 + 0.4 + i * BEAT * 0.5, 0.16); place(wet, y, t0 + 0.4 + i * BEAT * 0.5, 0.2)
for i in range(3): place(wet, bell(mtof([84, 91, 98][i]), 3.5), t0 + 4.5 + i * 0.9, 0.14)

# ---------------- sound effects ----------------
for tc in (9.0, 69.3):                                                           # through the O
    place(sfx, whoosh(0.9, 250, 2600, 1.5), tc - 0.9, 0.5); place(sfx, whoosh(1.1, 2600, 220, 1.5), tc - 0.1, 0.5)
for tc in (58.4, 60.28, 61.92): place(sfx, whoosh(0.9, 400, 1800), tc - 0.5, 0.3)   # under the neon gantries
d = np.arange(int(0.35 * SR)) / SR                                               # the blob lands
place(sfx, np.sin(2 * np.pi * np.cumsum(70 + 160 * np.exp(-d * 14)) / SR) * np.exp(-d * 7), 18.42, 0.7)
place(sfxw, whoosh(2.4, 500, 4200, 1.0), 19.0, 0.12)                              # the metal flows into a pig
for i in range(12):
    y = bell(mtof(72 + 2 * i), 1.0); place(sfxw, y, 19.1 + i * 0.18, 0.07)
for bt in range(39, 41): place(sfx, STOMP, bt * BEAT, 0.5)                        # it marks time before the parade
n = int(1.3 * SR); d = np.arange(n) / SR; x = rng.normal(0, 1, n)                 # the dive
place(sfx, (lp(x, 6000) * np.exp(-d * 9) + lp(x, 500) * (1 - np.exp(-d * 9)) * 1.5) * np.exp(-d * 3.5), 38.2, 0.5)
for i in range(150):                                                             # bubbles
    tb = 38.3 + 6.7 * rng.random() ** 1.6
    place(sfxw, blip(rng.uniform(280, 1300), rng.uniform(0.04, 0.09)), tb, rng.uniform(0.04, 0.11))
n = int(5.0 * SR); d = np.arange(n) / SR                                         # the submarine hums
place(sfx, (np.sin(2 * np.pi * 48 * d) + 0.5 * np.sin(2 * np.pi * 96 * d)) * np.clip(d / 4.5, 0, 1) ** 2 * ar(n, 0.01, 0.05), 40.0, 0.16)
n = int(9.0 * SR); d = np.arange(n) / SR                                         # wind over the city
place(sfx, bp(rng.normal(0, 1, n), 250, 900) * (0.6 + 0.4 * np.sin(2 * np.pi * 0.23 * d)) * ar(n, 1.0, 0.6), 54.0, 0.035)

# ---------------- mix ----------------
tb = np.arange(N) / SR - PRE
party = pd + pdr + reverb(pw, 1.6, 5000) * 0.8
u = np.clip((tb - 38.25) / 6.75, 0, 1)
muff = (lp(party, 240, 4) * (1 - u) + lp(party, 850, 4) * u) * db(-8 + 6 * u)
party = np.where(tb < 45.0, muff, party) * (tb < 54.0)
tail = np.zeros(N); i54 = int((54.0 + PRE) * SR); seg = party[i54 - int(0.4 * SR): i54]   # the party rings on in the night
place(tail, reverb(np.concatenate([seg, np.zeros(int(3 * SR))]), 2.8, 3000), 54.0 - 0.4, 0.5)
mus = dry + reverb(wet) * 0.9 + drums + reverb(drums, 0.9, 6000) * 0.08 + party * 1.25 + tail * (tb >= 54.0)
fx = sfx + reverb(sfxw, 1.8, 5000) * 0.9
mus *= np.clip((tb + PRE) / 0.05, 0, 1)
ref = np.percentile(np.abs(mus), 99.9)
mus = mus / ref * 0.8; fx = fx / ref * 0.8

vref = rms(mus[int((9 + PRE) * SR): int((18 + PRE) * SR)])
mix = np.tanh((mus + fx) * 0.55)                 # no voice in this version, so nothing ducks

# record slowing to a stop from the tape cut, never a plain fade
tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n
speed = (1 - uu) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * uu * D))
pos = np.clip(tc + np.cumsum(speed), 0, N - 2)
slow = lp(np.interp(pos, np.arange(N), mix), 5000)
e = db(-3.0 * uu); tl = int(0.25 * SR); e[-tl:] *= np.linspace(1, 0, tl) ** 2
body = np.zeros(int((PRE + CUT + SNOW + NOSIG) * SR))
body[:tc] = mix[:tc]; body[tc:tc + n] = slow * e
sn = int((SNOW + 0.08) * SR)
noise = bp(rng.normal(0, 1, sn), 300, 7000)
ne = np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
body[tc:tc + sn] += noise / rms(noise) * db(-25) * ne
hs = tc + int(SNOW * SR); hiss = lp(rng.normal(0, 1, len(body) - hs), 4000)
body[hs:] += hiss / rms(hiss) * db(-52) * np.clip((len(body) - hs - np.arange(len(body) - hs)) / (1.2 * SR), 0, 1)

# the validated jingle (same notes, same timing, same tape spin-up and sync buzz), left to ring instead of being cut at 2.6 s
nj = int(7.0 * SR); tj = np.arange(nj) / SR; arng = np.random.default_rng(4)
j = pork_intro.jingle(nj, arng)
sp = 1 - 0.05 * np.clip(1 - (tj - 0.35) / 0.2, 0, 1) * (tj > 0.3)
j = np.interp(np.clip(np.cumsum(sp) - sp[0], 0, nj - 2), np.arange(nj), j)
saw_ = (tj * 50) % 1 - 0.5
buzz = pork_intro._bandpass(np.sign(saw_) * 0.5 + saw_, 250, 3500) * 0.22 * np.clip(1 - (tj - 0.30) / 0.36, 0, 1) * (tj >= 0.30)
hiss = arng.normal(0, 1, nj) * (0.16 * (tj < 0.30) + 0.07 * np.clip(1 - (tj - 0.30) / 0.4, 0, 1) * (tj >= 0.30))
jing = j + buzz + hiss
jing = np.tanh(jing / rms(jing[int(0.4 * SR): int(2.0 * SR)]) * vref * db(2.0) * 0.55)
out = np.zeros(int(round(TOTAL * SR)))
out[: len(jing)] = jing
i0 = int(round((INTRO - PRE) * SR)); out[i0: i0 + len(body)] += body[: len(out) - i0]
out = np.clip(out, -1, 1)
print("total", round(TOTAL, 2), "s  peak", round(np.abs(out).max(), 3))
names = ["prologue", "A aube", "B monument", "métal", "C parade", "D bière", "E fond", "F fête", "G ville", "H final"]
edges = [-2.4, 0, 9, 18, 22.5, 31.5, 38.25, 45, 54, 63, 76.5]
for nm, a, b in zip(names, edges[:-1], edges[1:]):
    sl = slice(int((a + PRE) * SR), int((b + PRE) * SR))
    print(f"{nm:11s} musique {20 * np.log10(rms(mus[sl])):6.1f} dB  bruitages {20 * np.log10(rms(fx[sl]) + 1e-9):6.1f} dB")
with wave.open("final_audio.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR)
    w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
