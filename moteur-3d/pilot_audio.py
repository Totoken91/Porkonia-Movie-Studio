"""Plan pilote : voix 'animalese' (une syllabe synthétique par lettre), musique, bruitages, jingle, fin vinyle."""
import sys, json, wave, numpy as np
from scipy.signal import iirpeak, lfilter
src = open('music3.py').read()
exec(src[:src.index("# ---------------- prologue")])          # synthés et tampons de music3 (temps 0 = début du plan)
FILM = 22.0; INTRO = 2.6; CUT = FILM + 1.0; TOTAL = INTRO + CUT + SNOW + NOSIG
LINES = [(0.8, "Le 14 mars, à Port-Cochon, le Gras-Fond attend au bout de la jetée."), (5.6, "Huit cents invités montent à bord. Il y a de la place pour trente."),
         (10.6, "Personne ne compte."), (15.0, "À vingt-deux heures, il plonge."), (18.6, "La jetée reste allumée.")]
FORM = dict(a=(800, 1250), e=(480, 1750), i=(300, 2300), o=(450, 850), u=(320, 800), y=(300, 2300))
import unicodedata
vr = np.random.default_rng(12); vo = np.zeros(N); stamps = []
def syll(ch, f0):
    base = unicodedata.normalize('NFD', ch.lower())[0]
    n = int(0.085 * SR); d = np.arange(n) / SR
    f = f0 * (1 + 0.06 * np.sin(2 * np.pi * 9 * d)); ph = np.cumsum(f) / SR
    g = 2 * (ph % 1) - 1
    F1, F2 = FORM.get(base, (430 + 25 * (ord(base) % 7), 1300 + 90 * (ord(base) % 9)))
    y = 0
    for fc, q, a in ((F1, 6, 1.0), (F2, 9, 0.6)):
        b_, a_ = iirpeak(fc, q, fs=SR); y = y + lfilter(b_, a_, g) * a
    if base not in FORM: y = y * 0.7 + bp(vr.normal(0, 1, n), 2500, 6000) * np.exp(-d * 90) * 0.5
    return y * np.minimum(d / 0.006, 1) * np.exp(-d * 22)
for t0, txt in LINES:
    t = t0; chars = []
    for k, ch in enumerate(txt):
        chars.append(round(t, 3))
        if ch.isalnum():
            fall = 1 - 0.18 * (k / len(txt)) ** 2
            place(vo, syll(ch, 118 * fall * (1 + vr.uniform(-0.10, 0.12))), t, 1.0); t += 0.062
        elif ch == ' ': t += 0.045
        elif ch == ',': t += 0.20
        elif ch == '.': t += 0.34
        else: t += 0.03
    stamps.append(dict(t0=t0, t1=round(t, 3), text=txt, chars=chars))
json.dump(stamps, open('pilot_subs.json', 'w'), ensure_ascii=False)
print([(s['t0'], s['t1']) for s in stamps])
# ---- musique : crépuscule calme, la fête étouffée derrière la coque
CHS = [Am9, F9, C9, G, Am9, F9, C9, G, F9, C9]
for b, ch in enumerate(CHS):
    t0 = b * BAR
    place(dry, pad(ch["p"], BAR + 0.5, 1000, 0.6), t0, 0.42); place(dry, bassn(ch["b"], 1.8), t0, 0.32)
    for i in range(8):
        y = fm(mtof(ch["p"][[0, 2, 4, 3, 1, 2, 4, 2][i]] + 12), 0.9, 1.0, 1.6, 3.0, 6.0); place(dry, y, t0 + i * BEAT / 2, 0.09); place(wet, y, t0 + i * BEAT / 2, 0.11)
    near = np.clip(t0 / 13.0, 0, 1) * (t0 < 15.5)
    for bt in range(4):
        place(pdr, K2, t0 + bt * BEAT, 0.9 * near); place(pd, bassn(ch["b"], 0.24), t0 + (bt + 0.5) * BEAT, 0.5 * near)
    for bt in (1, 3): place(pdr, CL, t0 + bt * BEAT, 0.4 * near)
n_ = int(26 * SR); d_ = np.arange(n_) / SR                                        # la mer
place(sfx, lp(rng.normal(0, 1, n_), 500) * (0.55 + 0.45 * np.sin(2 * np.pi * 0.17 * d_)) * 1.6 + bp(rng.normal(0, 1, n_), 900, 3500) * (0.3 + 0.3 * np.sin(2 * np.pi * 0.11 * d_ + 1)) * 0.25, -2.5, 0.10)
for i in range(110):                                                              # les pas sur les planches
    tt = 14 * rng.random(); d = np.arange(int(0.05 * SR)) / SR
    place(sfx, lp(rng.normal(0, 1, len(d)), 700) * np.exp(-d * 70), tt, 0.10 * (1 - 0.5 * tt / 14))
y = brass(36, 2.6) + brass(43, 2.6) * 0.6; place(dry, y, 14.3, 0.5); place(wet, y, 14.3, 0.4)   # la corne du sous-marin
for i in range(120):                                                              # il s'enfonce
    tt = 16.5 + 5.0 * rng.random() ** 0.8; place(sfxw, blip(rng.uniform(200, 900), rng.uniform(0.05, 0.11)), tt, rng.uniform(0.04, 0.10))
place(sfx, whoosh(4.5, 1500, 200, 1.0), 16.5, 0.35)
tb = np.arange(N) / SR - PRE
party = lp(pd + pdr, 260, 4) * db(-4)
mus = dry + reverb(wet) * 0.9 + party
fxs = sfx + reverb(sfxw, 1.8, 5000) * 0.9
ref = np.percentile(np.abs(mus[: int((PRE + FILM) * SR)]), 99.9); mus = mus / ref * 0.8; fxs = fxs / ref * 0.8
vref = rms(mus[int((PRE + 2) * SR): int((PRE + 12) * SR)])
act = np.abs(vo) > 0.02 * np.abs(vo).max(); vo = vo / rms(vo[act]) * vref * db(6.5)
env = np.convolve((np.abs(vo) > vref * 0.1).astype(float), np.ones(int(0.3 * SR)) / int(0.3 * SR), mode="same")
mix = np.tanh(((mus + fxs) * db(-5.0 * np.clip(env * 2, 0, 1)) + vo) * 0.55)
tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n
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
with wave.open("pilot_audio.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
