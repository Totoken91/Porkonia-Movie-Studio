"""Outils communs aux bandes-son des séquences de « 24 heures à Porkonia » : à exécuter (exec) après avoir fixé FILM (durée) et OFF (première barre de mesure)."""
import sys, wave, numpy as np
from scipy.signal import iirpeak, lfilter
src = open('music3.py').read()
exec(src[:src.index("# ---------------- prologue")])
INTRO = 2.6; CUT = FILM + 1.0; TOTAL = INTRO + CUT + SNOW + NOSIG
N = int((PRE + FILM + 12) * SR); B = lambda: np.zeros(N)
dry, wet, drums, sfx, sfxw = B(), B(), B(), B(), B()
tb = np.arange(N) / SR - PRE
env_t = lambda pts: np.interp(tb, [p[0] for p in pts], [p[1] for p in pts])
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

def ding(f=1397.0, dur=1.3): return fm(f, dur, 2.76, 1.3, 3.2, 5.0) + 0.5 * fm(f * 1.498, dur, 2.0, 0.6, 4.5, 6.0)
lvl = lambda x, a, b: rms(x[int((PRE + a) * SR): int((PRE + b) * SR)])
def finish(name, vwin, fx=(), sections=()):
    """Mélange, arrêt de bande façon vinyle, jingle de chaîne, fichier. vwin : la fenêtre (a, b) qui sert de référence de niveau ;
    fx : des bruitages continus (signal, (a, b) où mesurer son niveau, gain par rapport à la musique)."""
    mus = dry + reverb(wet) * 0.9 + drums + reverb(drums, 0.9, 6000) * 0.10
    fxs = sfx + reverb(sfxw, 2.4, 5000) * 0.9
    ref = np.percentile(np.abs(mus[: int((PRE + FILM) * SR)]), 99.9); mus = mus / ref * 0.8; fxs = fxs / ref * 0.8
    vref = lvl(mus, *vwin)
    for sig, win, g in fx: fxs = fxs + sig / lvl(sig, *win) * vref * g
    mix = np.tanh((mus + fxs) * 0.62)
    for a, b, nm in sections: print("%5.1f-%5.1f %-10s musique %6.1f dB  bruitages %6.1f dB  total %6.1f dB" % (a, b, nm, 20 * np.log10(lvl(mus, a, b) + 1e-9), 20 * np.log10(lvl(fxs, a, b) + 1e-9), 20 * np.log10(lvl(mix, a, b) + 1e-9)))
    tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n                # la bande s'arrête comme un vinyle
    speed = (1 - uu) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * uu * D))
    slow = lp(np.interp(np.clip(tc + np.cumsum(speed), 0, N - 2), np.arange(N), mix), 5000)
    e = db(-3.0 * uu); tl = int(0.25 * SR); e[-tl:] *= np.linspace(1, 0, tl) ** 2
    body = np.zeros(int((PRE + CUT + SNOW + NOSIG) * SR)); body[:tc] = mix[:tc]; body[tc:tc + n] = slow * e
    sn = int((SNOW + 0.08) * SR); noise = bp(rng.normal(0, 1, sn), 300, 7000)
    body[tc:tc + sn] += noise / rms(noise) * db(-25) * np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
    _, _, jing = pork_intro.build_intro(64, 48, 25); jing = jing[: int(INTRO * SR)]
    jing = np.tanh(jing / rms(jing[int(0.4 * SR): int(2.0 * SR)]) * lvl(mix, *vwin) * db(-1.0))
    out = np.zeros(int(round(TOTAL * SR))); out[: len(jing)] = jing
    b0 = body[int(PRE * SR):]; b0[: int(0.3 * SR)] *= np.linspace(0, 1, int(0.3 * SR)); out[len(jing): len(jing) + len(b0)] += b0[: len(out) - len(jing)]
    print("total", TOTAL, "peak", np.abs(out).max().round(3))
    with wave.open(name, "w") as w:
        w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
def windbed():
    g1 = lp(rng.normal(0, 1, N), 0.35); g1 = g1 / g1.std(); g2 = lp(rng.normal(0, 1, N), 1.1); g2 = g2 / g2.std(); wn = rng.normal(0, 1, N)
    return bp(wn, 180, 900) * (0.8 + 0.35 * g1).clip(0.2, 1.8) + bp(wn, 900, 2600) * 0.35 * (0.5 + 0.5 * g2).clip(0, 1.6)
