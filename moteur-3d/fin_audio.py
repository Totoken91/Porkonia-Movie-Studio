"""Le lendemain, 06 h 00 : le jour se lève (sans voix). La nuit finit sur une nappe et des clochettes ; six heures sonnent au beffroi
pendant que les cuivres montent ; la marche du matin revient quand la caméra s'élève vers les lettres ; un coup d'orchestre quand on passe
par le O (29,25 s) ; le thème en entier, soleil dans le O ; puis le générique, plus léger, et l'accord final (47,25 s). Fin vinyle sur NO SIGNAL."""
FILM = 50.0; OFF = 0.0
exec(open('seqaudio.py').read())
# ---- la nuit finit (0 – 9)
for b, ch in enumerate((Am9, F9, Dm7, Em7)):
    place(dry, pad(ch["p"], BAR + 0.5, 900 + 250 * b, 0.9), bt(b) - 0.2, 0.22); place(wet, pad([n + 12 for n in ch["p"][:3]], BAR + 0.5, 2000, 1.0), bt(b) - 0.2, 0.12)
    place(dry, sub(ch["b"], BAR + 0.3), bt(b) - 0.1, 0.20)
    for i in range(4): y = bell(mtof(ch["p"][[0, 2, 4, 2][i]] + 24), 1.6); place(dry, y, bt(b, i), 0.03 + 0.008 * b); place(wet, y, bt(b, i), 0.045 + 0.01 * b)
# ---- le jour vient (9 – 18) : les cuivres montent, six heures sonnent, le premier rayon (15,4)
for i, (ch, top) in enumerate(((Dm7, 62), (F9, 65), (G, 67), (G, 71))):
    b = 4 + i; notes = (ch["b"] + 12, ch["b"] + 19, top, top + 4 if i else top + 3, top + 7)
    place(dry, held(notes, BAR + 0.1, 0.85), bt(b), 0.09 + 0.03 * i); place(wet, choir([n + 12 for n in notes[2:]], BAR + 0.2), bt(b) - 0.05, 0.09 + 0.03 * i)
    place(dry, sub(ch["b"], BAR, 0.3, 0.4), bt(b), 0.26)
    place(dry, brass(top + 12, BEAT * 1.9), bt(b), 0.16 + 0.04 * i); place(dry, brass(top + 14, BEAT * 1.9), bt(b, 2), 0.16 + 0.04 * i)
for k in range(6): y = ding(294.0, 2.6) + 0.5 * ding(147.0, 2.6); place(sfx, y, 12.6 + 1.02 * k, 0.24); place(sfxw, y, 12.6 + 1.02 * k, 0.30)
for i, m in enumerate((84, 88, 91, 96, 100, 103, 108, 112)): y = bell(mtof(m), 1.4); place(dry, y, 15.4 + i * 0.07, 0.07); place(wet, y, 15.4 + i * 0.07, 0.10)
roll(bt(7), bt(8), 0.05, 0.36)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(8) - (4 - i) * S16, 0.32 + 0.05 * i)
# ---- on s'élève vers les lettres (18 – 27) : la marche du matin
place(dry, orch(C9["p"][:3], 0.9), bt(8), 0.7); place(wet, crash(), bt(8), 0.13)
for i in range(4):
    b = 8 + i; ch = MAIN[i]; groove(b, ch, lvl=0.95); fanfare(bt(b), FANF[i], 0.9); arp_bar(bt(b), ch, 0.035, 24)
    place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.30); place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.14)
# ---- par le O (27 – 29,25) : la montée, puis le coup
groove(12, C9, lvl=0.9, last=8); roll(bt(12), bt(12, 2), 0.06, 0.2); roll(bt(12, 2), bt(13), 0.2, 0.55); riser(bt(12), BAR, 0.12)
tf = bt(13)
place(dry, orch(C9["p"][:3], 1.4), tf, 1.0); place(wet, orch(C9["p"][:3], 1.4), tf, 0.4); place(wet, crash(), tf, 0.18); place(drums, K2, tf, 1.0); place(drums, STOMP, tf, 0.8)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)): y = bell(mtof(m), 1.6); place(dry, y, tf + i * S16, 0.10); place(wet, y, tf + i * S16, 0.12)
# ---- le soleil dans le O (29,25 – 36) : le thème en entier
for i in range(3):
    b = 13 + i; ch = MAIN[i]; groove(b, ch, lvl=1.05); lead(bt(b), BELLS[i], 0.85); fanfare(bt(b), FANF[i], 0.9)
    place(wet, choir([n + 12 for n in ch["p"][1:4]], BAR + 0.25), bt(b) - 0.05, 0.19); place(dry, orch(ch["p"][:3], 0.5), bt(b), 0.32)
for i, f in enumerate((210, 170, 135, 105)): place(drums, tom(f), bt(16) - (4 - i) * S16, 0.32 + 0.05 * i)
# ---- le générique (36 – 47,25) : plus léger
for i in range(5):
    b = 16 + i; ch = MAIN[(i + 3) % 4] if i < 4 else G; groove(b, ch, lvl=0.72); lead(bt(b), BELLS[(i + 3) % 4], 0.6); arp_bar(bt(b), ch, 0.04, 24)
    place(wet, pad([n + 12 for n in ch["p"][1:4]], BAR + 0.2, 2400, 0.2), bt(b), 0.10)
roll(bt(20, 3), bt(21), 0.10, 0.38)
te = bt(21)
place(dry, orch(C9["p"][:3], 1.5), te, 1.0); place(wet, orch(C9["p"][:3], 1.5), te, 0.4); place(wet, crash(), te, 0.18); place(drums, K2, te, 1.0); place(drums, STOMP, te, 0.7)
place(dry, held((36, 48, 55, 60, 64, 67, 72, 76), 6.0, 0.55), te, 0.15); place(wet, choir((60, 64, 67, 72, 76), 6.0, 0.08), te, 0.20); place(dry, sub(24, 6.0, 0.02, 1.0), te, 0.40)
for i, m in enumerate((84, 86, 88, 91, 93, 96, 98, 100, 103, 105, 108, 112)): y = bell(mtof(m), 1.6); place(dry, y, te + i * S16, 0.10); place(wet, y, te + i * S16, 0.12)
fanfare(te + BEAT, [(0, .75, 67), (.75, .25, 67), (1, 2, 72)], 0.8)
# ---- bruitages : le vent du plateau, le drapeau
wind = windbed() * env_t([(-3, 0.8), (8.0, 0.8), (9.0, 0.5), (17.5, 0.45), (27.0, 0.5), (29.0, 1.1), (30.0, 0.6), (36, 0.5), (70, 0.5)])
finish("fin_audio.wav", (18.5, 26.5), fx=[(wind, (0.5, 8.0), 0.45)],
       sections=[(0, 9, "la nuit finit"), (9, 18, "le jour"), (18, 27, "vers les lettres"), (27, 29.25, "par le O"), (29.25, 36, "le soleil"), (36, 47.25, "générique"), (47.25, 51.0, "final")])
