"""Acte V, deuxième partie : « Porc 47 » en plein son dans la coque, voix 'animalese' du narrateur (sans bruit de frappe),
filtrages selon le lieu (bar, compartiment moteur, sous l'eau, côte nord), sonar, baleine, fin vinyle."""
import sys, json, wave, unicodedata, numpy as np
from scipy.signal import iirpeak, lfilter
src = open('music3.py').read()
exec(src[:src.index("# ---------------- prologue")])          # synthés et tampons communs (temps 0 = début du film)
FILM = 80.5; INTRO = 2.6; CUT = FILM + 1.0; TOTAL = INTRO + CUT + SNOW + NOSIG
N = int((PRE + FILM + 12) * SR); B = lambda: np.zeros(N)
dry, wet, sfx, sfxw = B(), B(), B(), B()
OFF = 0.125 - 4 * BEAT                                         # les temps tombent sur ceux de l'image (dalles, danseurs, hublots)
LINES = [(1.2, "Au bout de vingt minutes de set, un homme en peignoir, que personne ne connaissait,"),
         (8.4, "a commencé à distribuer des côtes de porc à mains nues."),
         (12.6, "Il n'a jamais été identifié."),
         (15.4, "Il figure aujourd'hui sur trois timbres."),
         (19.3, "Vers une heure, la mousse a commencé à monter dans les verres toute seule."),
         (24.9, "Puis dans les bouteilles fermées."),
         (28.9, "Puis dans le compartiment moteur."),
         (33.5, "Un invité a dit : « c'est la vibe ». Ça a suffi."),
         (39.0, "Pour avoir plus de basses, Fatbass avait branché ses enceintes sur le sonar."),
         (46.2, "Cette nuit-là, sur la côte nord, plusieurs habitants se sont réveillés"),
         (51.9, "en pleurant, sans savoir pourquoi."),
         (58.0, "Un mécanicien est venu lui demander de baisser. Fatbass a monté le son."),
         (63.3, "Le mécanicien a hoché la tête, et a noté dans le journal de bord :"),
         (69.6, "Ce sont ses derniers mots écrits."),
         (72.9, "Il est aujourd'hui maire de Lardombre.")]

# ---------------------------------------------------------------- la voix (la même que dans la première partie)
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
    env = 0.5 - 0.5 * np.cos(np.pi * np.minimum(d / 0.014, 1))
    env = env * np.exp(-d * 20) * (0.5 + 0.5 * np.cos(np.pi * np.clip((d - 0.060) / 0.035, 0, 1)))
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
vo = lp(vo, 4200, 4)
json.dump(stamps, open('acte5b_subs.json', 'w'), ensure_ascii=False)
print([(s['t0'], s['t1']) for s in stamps])

# ---------------------------------------------------------------- « Porc 47 »
tb = np.arange(N) / SR - PRE
env_t = lambda pts: np.interp(tb, [p[0] for p in pts], [p[1] for p in pts])
def pingf(f, dur=1.3):
    d = np.arange(int(dur * SR)) / SR
    return (np.sin(2 * np.pi * f * d) + 0.3 * np.sin(2 * np.pi * 2 * f * d)) * np.exp(-d * 3.6) * np.minimum(d / 0.003, 1)
CHS = [Am9, F9, C9, G]
ACID = [0, 0, 12, 0, 7, 0, 12, 10, 0, 0, 12, 0, 3, 7, 12, 0]
LEAD = [[(0, 88), (1.5, 91), (3, 93)], [(0, 93), (1.5, 89), (3, 88)], [(0, 91), (1.5, 88), (3, 84)], [(0, 86), (1.5, 91), (2.5, 95), (3, 93)]]
kick, low, mid, top, lead = B(), B(), B(), B(), B()
T_SONAR, T_KNOB = 43.1, 61.3
for b in range(38):
    t0 = OFF + b * BAR; ch = CHS[b % 4]
    for bt in range(4):
        place(kick, K2, t0 + bt * BEAT, 1.0); place(low, bassn(ch["b"], 0.26), t0 + (bt + 0.5) * BEAT, 0.55)
        place(top, HO, t0 + (bt + 0.5) * BEAT, 0.10)
    for bt in (1, 3): place(mid, CL, t0 + bt * BEAT, 0.5)
    for i in range(16):
        place(mid, acid(ch["b"] + 24 + ACID[i], S16 * 0.9), t0 + i * S16, 0.20); place(top, HH, t0 + i * S16, 0.035 if i % 4 else 0.06)
    place(mid, orch(ch["p"][:3], 0.5), t0, 0.22)
    if b % 4 == 3:
        for i in range(4): place(mid, SN, t0 + 3 * BEAT + i * S16, 0.18 + 0.06 * i)
    if t0 + 0.01 >= T_SONAR:                                    # une fois branché, c'est le sonar qui tient la mélodie
        for bt, m in LEAD[b % 4]: y = pingf(mtof(m)); place(lead, y, t0 + bt * BEAT, 0.30); place(wet, y, t0 + bt * BEAT, 0.16)
        place(low, np.sin(2 * np.pi * mtof(ch["b"] - 12) * np.arange(int(BAR * SR)) / SR) * ar(int(BAR * SR), 0.01, 0.1), t0, 0.22)
club = kick + low + mid + top + lead
club *= env_t([(-3, 1), (T_KNOB - 0.1, 1), (T_KNOB + 0.6, 1.45), (99, 1.45)])                      # Fatbass monte le son
club = np.tanh(club * 0.9) / 0.9
# ce qu'on en entend selon l'endroit
ROOM = [(-3, 15.0), (33.0, 45.5), (57.5, 66.5), (72.5, 99)]                                         # dans la salle : plein son
def gate(spans, fade=0.12): return np.clip(np.max([np.minimum((tb - a) / fade, (b - tb) / fade) for a, b in spans], axis=0), 0, 1)
g_room = gate(ROOM); g_doc = gate([(15.0, 18.8), (66.5, 72.5)]); g_bar = gate([(18.8, 28.5)]); g_eng = gate([(28.5, 33.0)]); g_sea = gate([(45.5, 51.0)]); g_coast = gate([(51.0, 57.5)])
mus = club * g_room + lp(club, 700, 4) * db(-7) * g_doc + lp(club, 2300) * db(-4) * g_bar + lp(club, 280, 4) * db(-2) * g_eng \
    + lp(club, 230, 4) * db(-1) * g_sea + lp(club, 140, 4) * db(-9) * g_coast

# ---------------------------------------------------------------- bruitages et ambiances
hub = np.zeros(N)                                                # la foule : des voix sans mots
for k in range(16):
    f0 = rng.uniform(260, 700); x = bp(rng.normal(0, 1, N), f0, f0 * 2.2)
    hub += x * np.clip(lp(rng.normal(0, 1, N), rng.uniform(2.5, 4.5)) * 40, 0, 1)
hub = lp(hub, 1900); hub = hub / (np.abs(hub).max() + 1e-9) * (g_room + 0.35 * g_bar)
fizz = bp(rng.normal(0, 1, N), 3500, 9000) * (0.7 + 0.3 * lp(rng.normal(0, 1, N), 6) * 30).clip(0.3, 1.3)       # la mousse qui monte
fizz *= env_t([(-3, 0), (18.8, 0), (19.5, 0.25), (24.3, 1.0), (24.6, 0.5), (28.3, 1.0), (28.6, 0.6), (32.8, 1.3), (33.1, 0.35), (38.4, 0.5), (38.6, 0), (99, 0)])
for i in range(46):                                              # et ses bulles
    tt = 19.5 + 13.3 * rng.random(); place(sfxw, blip(rng.uniform(300, 900), rng.uniform(0.07, 0.13)), tt, rng.uniform(0.03, 0.07))
for tt in (15.55, 16.25, 16.95):                                 # trois timbres posés l'un après l'autre
    d = np.arange(int(0.25 * SR)) / SR; place(sfx, lp(rng.normal(0, 1, len(d)), 160, 4) * np.exp(-d * 22) * np.minimum(d / 0.008, 1) * 6, tt, 0.5)
n_ = int(4.8 * SR); d_ = np.arange(n_) / SR                      # compartiment moteur : un grondement sous la lampe rouge
place(sfx, lp(rng.normal(0, 1, n_), 90, 4) * 5 * (0.75 + 0.25 * np.sin(2 * np.pi * 0.8 * d_)) * ar(n_, 0.2, 0.3), 28.4, 0.22)
place(sfx, STOMP, T_SONAR - 0.02, 0.9)                           # la prise
riser(T_SONAR - 1.6, 1.6, 0.10)
n_ = int(6.0 * SR); d_ = np.arange(n_) / SR                      # sous l'eau : le grondement, les ondes, la baleine
place(sfx, lp(rng.normal(0, 1, n_), 110, 4) * 4 * ar(n_, 0.2, 0.3), 45.4, 0.16)
for k in range(10): place(sfxw, PING, 45.5 + 0.1875 + k * BEAT, 0.32)
nw = int(3.4 * SR); dw = np.arange(nw) / SR
fw = 150 * (1 + 0.45 * np.sin(np.pi * dw / 3.4) ** 2) * (1 + 0.012 * np.sin(2 * np.pi * 5 * dw))
whale = (np.sin(2 * np.pi * np.cumsum(fw) / SR) + 0.3 * np.sin(4 * np.pi * np.cumsum(fw) / SR)) * np.sin(np.pi * dw / 3.4) ** 1.5
place(sfxw, whale, 47.0, 0.55)
n_ = int(7.0 * SR); d_ = np.arange(n_) / SR                      # la côte nord : la mer, une nappe triste, les ondes qui arrivent
sea = lp(rng.normal(0, 1, n_), 500) * (0.55 + 0.45 * np.sin(2 * np.pi * 0.17 * d_)) * 1.6 + bp(rng.normal(0, 1, n_), 900, 3500) * (0.3 + 0.3 * np.sin(2 * np.pi * 0.11 * d_ + 1)) * 0.25
place(sfx, sea * ar(n_, 0.3, 0.4), 50.8, 0.13)
place(dry, pad([57, 60, 64, 71], 3.6, 900, 1.0), 51.0, 0.5); place(dry, pad([53, 57, 60, 64], 3.4, 900, 1.0), 54.2, 0.5)
place(wet, pad([69, 72, 76], 6.4, 1400, 1.5), 51.0, 0.22)
for k in range(6): place(sfxw, PING, 51.2 + 0.0625 + k * 2 * BEAT, 0.10)
place(wet, whale * 0.6, 53.6, 0.28)
riser(T_KNOB - 1.2, 1.2, 0.12); place(dry, crash(), T_KNOB, 0.10)

# ---------------------------------------------------------------- le mélange
pads = dry + reverb(wet) * 0.9
fxs = sfx + reverb(sfxw, 2.4, 5000) * 0.9
ref = np.percentile(np.abs(mus[: int((PRE + FILM) * SR)]), 99.9)
mus = mus / ref * 0.8; pads = pads / ref * 0.8; fxs = fxs / ref * 0.8
vref = rms(mus[int((PRE + 1) * SR): int((PRE + 14) * SR)])
fxs = fxs + hub * vref * 0.22 + fizz / rms(fizz[int((PRE + 20) * SR): int((PRE + 32) * SR)]) * vref * 0.20
act = np.abs(vo) > 0.02 * np.abs(vo).max(); vo = vo / rms(vo[act]) * vref * db(7.5)
env = np.convolve((np.abs(vo) > vref * 0.1).astype(float), np.ones(int(0.3 * SR)) / int(0.3 * SR), mode="same")
mix = np.tanh(((mus + pads + fxs) * db(-6.5 * np.clip(env * 2, 0, 1)) + vo) * 0.55)
for a, b in ((0, 15), (15, 18.8), (18.8, 28.5), (28.5, 33), (33, 45.5), (45.5, 51), (51, 57.5), (57.5, 66.5), (66.5, 72.5), (72.5, 80.5)):
    print("%5.1f-%5.1f  niveau %.1f dB" % (a, b, 20 * np.log10(rms(mix[int((PRE + a) * SR): int((PRE + b) * SR)]))))
tc = int((CUT + PRE) * SR); D = 3.0; n = int(D * SR); uu = np.arange(n) / n                # la bande s'arrête comme un vinyle
speed = (1 - uu) ** 1.5 * (1 + 0.012 * np.sin(2 * np.pi * 1.3 * uu * D))
slow = lp(np.interp(np.clip(tc + np.cumsum(speed), 0, N - 2), np.arange(N), mix), 5000)
e = db(-3.0 * uu); tl = int(0.25 * SR); e[-tl:] *= np.linspace(1, 0, tl) ** 2
body = np.zeros(int((PRE + CUT + SNOW + NOSIG) * SR)); body[:tc] = mix[:tc]; body[tc:tc + n] = slow * e
sn = int((SNOW + 0.08) * SR); noise = bp(rng.normal(0, 1, sn), 300, 7000)
body[tc:tc + sn] += noise / rms(noise) * db(-25) * np.minimum(np.arange(sn) / (0.004 * SR), 1) * np.clip((sn - np.arange(sn)) / (0.08 * SR), 0, 1)
_, _, jing = pork_intro.build_intro(64, 48, 25); jing = jing[: int(INTRO * SR)]
jing = np.tanh(jing / rms(jing[int(0.4 * SR): int(2.0 * SR)]) * vref * db(1.0) * 0.55)
out = np.zeros(int(round(TOTAL * SR))); out[: len(jing)] = jing
b0 = body[int(PRE * SR):]; b0[: int(0.3 * SR)] *= np.linspace(0, 1, int(0.3 * SR)); out[len(jing): len(jing) + len(b0)] += b0[: len(out) - len(jing)]
print("total", TOTAL, "peak", np.abs(out).max().round(3), "voix/musique dB", round(20 * np.log10(rms(vo[act]) / vref), 1))
with wave.open("acte5b_audio.wav", "w") as w:
    w.setnchannels(2); w.setsampwidth(2); w.setframerate(SR); w.writeframes((np.stack([out, out], 1) * 32767).astype(np.int16).tobytes())
