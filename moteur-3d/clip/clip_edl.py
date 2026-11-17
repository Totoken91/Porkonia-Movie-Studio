"""Le découpage du clip « Sous la neige de Douzi City » : une liste de plans calée sur les temps du morceau.
Un temps = 0,6602 s ; le temps 0 est à 0,191 s ; une mesure = 4 temps. Chaque plan : sa durée en temps, son numéro de plan 3D (K),
et ses réglages : e énergie, th palette (0 fête, 1 neige, 2 chaleur, 3 Porkonia), pat motif des dalles, var variante, hz images par seconde
(50, 25 ou 12,5), fx effets de régie, tr transition d'entrée, D durée du mouvement de caméra (par défaut celle du plan), s0 décalage dans le mouvement.
  python3 clip_edl.py        -> le tableau des plans, les totaux
"""
import math
P, PH, DUR, FPS = 0.6602, 0.191, 268.224, 50
NBT = (DUR - PH) / P                      # 405,98 temps
E, _d = [], dict(e=0.6, th=0, pat=0, var=0, hz=50, fx=[], tr="cut")
def sec(**kw): _d.update(kw)
def S(beats, K, **kw):
    d = dict(_d); d["fx"] = list(_d["fx"]); d.update(kw); d["beats"], d["K"] = beats, K; E.append(d)

# DÉCOUPAGE N° 2 (16 nov. 2026, retours de Kenny) : 62 plans au lieu de 144, on a le temps de regarder ; les deux refrains et le sommet final sont dansés par
# les Ranga Boyz (la chorégraphie de 32 temps tient exactement dans les 8 mesures d'un refrain) ; aucun plan n'est repris plus de deux fois.
# ============ INTRO A, mesures 0-3 : des nappes, pas de basse. Douzi City sous la neige.
sec(e=0.25, th=1, hz=50, fx=["bars"])
S(8, 20, tr="fade")                               # la ville, de haut ; on descend vers le Bal
S(8, 23, var=0)                                   # le Bal, de l'autre trottoir : la grue monte le long de la tour
# ============ INTRO B, mesures 4-7 : la basse entre doucement. « Sous la neige, on garde la chaleur »
sec(e=0.4, th=1, fx=["bars"])
S(6, 21)                                          # la rue, les porcs de marche qui la remontent
S(5, 23, var=1, th=2)                             # on longe une fenêtre : derrière, on danse
S(5, 28)                                          # le bonhomme de neige
# ============ mesure 8 : suspens
S(4, 13, e=0.3, th=1, fx=["bars", "zoomin"])      # la boule à facettes
# ============ mesures 9-11 : le rythme démarre
sec(e=0.75, th=0, fx=["punch"])
S(6, 1, tr="flash", fx=["punch", "rgb"])          # DJ Viteau aux platines (son titrage reste sur lui)
S(6, 0, th=2)                                     # la salle, en entier
# ============ COUPLET 1, mesures 12-23 : deux lignes par image
sec(e=0.6, th=2, hz=50, fx=[])
S(8, 22, th=1)                                    # Dehors la ville est toute blanche / Les lumières glissent sur les toits
S(8, 5)                                           # Mais dans la salle tout se mélange / Et moi je reste près de toi
S(8, 1, var=1)                                    # Un peu de musique dans l'hiver
S(8, 7, th=1)                                     # Un peu de buée sur les carreaux
S(8, 8)                                           # Les autres parlent, les autres rient
S(8, 10)                                          # la chope sur la table
# ============ REFRAIN 1, mesures 24-31 : la piste s'ouvre, les Ranga Boyz (une phrase de la chorégraphie par deux mesures)
sec(e=0.9, th=0, hz=50, pat=0, fx=["punch"])
S(8, 42, var=0, tr="flash")                       # phrase 1, en V, à l'unisson : de face, au ras des dalles
S(8, 42, var=1, pat=1)                            # phrase 2, en ligne, la vague en canon : à la grue
S(4, 42, var=3)                                   # phrase 3 : serré sur Kevin
S(4, 42, var=4, pat=3)                            # le tour sur soi, vu d'en haut
S(4, 42, var=2, pat=2)                            # LE RANGA STYLE : travelling de côté
S(4, 42, var=0, D=5.28, s0=2.64, fx=["punch", "rgb"])      # la pose finale
# ============ mesure 32 : arrêt. La chenille passe.
S(4, 12, e=0.4, th=2, fx=[])
# ============ mesures 33-35 : le gimmick
sec(e=0.9, th=3, fx=["punch"])
S(6, 33)                                          # le porc d'or
S(6, 38)                                          # la chope géante
# ============ COUPLET 2, mesures 36-47
sec(e=0.7, th=2, hz=50, fx=[])
S(8, 4)                                           # La basse tourne dans la salle : au ras de la piste
S(8, 8, var=4)                                    # Quelqu'un commande une autre tournée : il trinque
S(8, 9)                                           # Et les gobelins jouent près du bar
S(8, 11)                                          # le cuisinier
S(8, 16)                                          # la salle, de côté
S(8, 19)                                          # les billes, au ras du sol
# ============ REFRAIN 2, mesures 48-55 : les Ranga Boyz dans la rue, sous les guirlandes
sec(e=0.96, th=1, hz=50, pat=0, fx=["punch"])
# DÉCOUPAGE N° 4 (16 nov., 20 h 45) : moins de Ranga Boyz, les plans retirés reviennent, un Martin Plaisir de plus au sommet final
S(8, 27, th=0, tr="flash")                        # les fenêtres de la ville battent comme un vumètre
S(8, 3, pat=2, th=2)                              # la piste, d'en haut
S(4, 29, var=2)                                   # phrase 3 : travelling de côté, en contre-plongée
S(4, 29, var=1, D=5.28, s0=2.64)                  # le tour sur soi, de haut
S(4, 29, var=3, fx=["punch", "rgb"])              # LE RANGA STYLE : serré sur Kevin
S(4, 29, var=0, D=5.28, s0=2.64, fx=["punch", "rgb"])      # la pose finale
# ============ BREAKDOWN, mesures 56-59 : seuls sous la poursuite
sec(e=0.2, th=1, pat=1, hz=50, fx=["bars"])       # DÉCOUPAGE N° 3 (16 nov. au soir) : Martin Plaisir entre dans le clip
S(8, 45, var=0)                                   # Martin Plaisir, seul sous la poursuite, la salle presque vide : une main remonte vers le ciel
S(8, 45, var=1)                                   # on tourne autour de lui : les bras en serpent
# ============ SOMMET 1, mesures 60-67 : l'espace des vitrines
sec(e=1.0, th=0, pat=1, hz=50, fx=["punch", "rgb"])
S(8, 30, tr="flash")                              # le disque et son anneau
S(8, 31)                                          # le tunnel de néons
S(4, 35, fx=["punch", "kal6"])                    # la ronde, vue du ciel
S(4, 37)                                          # la boule, comme une planète
S(8, 36)                                          # la foule sans fin
# ============ PONT, mesures 68-71
sec(e=0.7, th=1, hz=50, fx=["bars"])
S(4, 25)                                          # la ville, de très haut
S(4, 24, var=0)                                   # le dernier train sur le viaduc
S(8, 46)                                          # Martin Plaisir dans la rue, sous les guirlandes ; les porcs de marche passent
# ============ MONTÉE, mesures 72-73
S(8, 32, e=0.8, th=0, fx=["zoomin"])              # le mur d'enceintes
# ============ SOMMET 2, mesures 74-79
sec(e=1.0, th=2, pat=4, hz=50, fx=["punch", "rgb"])
S(8, 2, tr="flash")                               # la salle, de l'estrade
S(8, 18)                                          # DJ Viteau, vu de la piste
S(8, 34, th=3)                                    # DJ VITEAU, en lettres d'or
# ============ REFRAIN DÉPOUILLÉ, mesures 80-83 : la voix, sans la basse
sec(e=0.3, th=1, pat=1, hz=50, fx=["bars"])
S(8, 45, var=2)                                   # la piste s'écarte pour Martin Plaisir : la grue descend sur lui
S(8, 45, var=3)                                   # on tourne autour de lui
# ============ DERNIER COUPLET, mesures 84-87 : la basse revient, plus fort que jamais
sec(e=0.9, th=2, pat=2, hz=50, fx=["punch"])
S(8, 45, var=4)                                   # la basse revient : il frappe des hanches, il tourne sur lui-même
S(8, 45, var=5, fx=["punch", "rgb"])              # il marche vers nous, descend, remonte, nous vise : le clin d'œil
# ============ BREAKDOWN 2, mesures 88-91 : « On avait dit ça il y a trois heures »
sec(e=0.3, th=1, hz=25, fx=["bars"])
S(8, 23, var=3)                                   # sur le toit : les projecteurs dans la neige
S(8, 17, pat=1)                                   # eux deux, la salle presque vide
# ============ SOMMET FINAL, mesures 92-99 : les Ranga Boyz dans la rue, une dernière fois (pat = 4 : la chorégraphie repart de son début)
sec(e=1.0, th=3, pat=4, hz=50, fx=["punch", "rgb"])
S(4, 23, var=2, th=1, tr="flash")                 # dehors, l'affiche
S(4, 26, e=0.9, fx=["punch"])                     # le matin se lève sur la ville
S(4, 20, var=1, e=0.8, fx=[])                     # la ville, de haut ; il fait jour
S(8, 46, var=1)                                   # Martin Plaisir dans la rue : les hanches, les bras, un tour sur lui-même
S(4, 46, var=2)                                   # son regard, son clin d'œil
S(4, 29, var=3)                                   # les Ranga Boyz : LE RANGA STYLE
S(4, 29, var=0, D=5.28, s0=2.64)                  # la pose finale
# ============ l'accord final
S(NBT - 400, 26, th=3, e=0.5, s0=6.0, D=8.0, fx=["bars"], tr="fade")

def build():
    """Les plans, avec leurs trames : k0 première trame (au 1/50 s du morceau), nf nombre de trames, mult pas de rendu, n images à rendre."""
    out, b = [], 0.0
    for i, d in enumerate(E):
        t0 = 0.0 if i == 0 else PH + b * P; b += d["beats"]; t1 = DUR if i == len(E) - 1 else PH + b * P
        k0, k1 = int(round(t0 * FPS)), int(round(t1 * FPS)); mult = {50: 1, 25: 2, 12.5: 4}[d["hz"]]
        sh = dict(d); sh.update(i=i, t0=k0 / FPS, dur=(k1 - k0) / FPS, k0=k0, nf=k1 - k0, mult=mult, n=-(-(k1 - k0) // mult), beat0=b - d["beats"])
        sh["D"] = d.get("D", sh["dur"]); sh["s0"] = d.get("s0", 0.0); out.append(sh)
    assert abs(b - NBT) < 1e-6, (b, NBT)
    return out
SHOTS = build()
def shot_at(k):
    for sh in SHOTS:
        if sh["k0"] <= k < sh["k0"] + sh["nf"]: return sh
    return SHOTS[-1]
if __name__ == "__main__":
    tot = sum(s["n"] for s in SHOTS); print(len(SHOTS), "plans ;", sum(s["nf"] for s in SHOTS), "trames à l'écran ;", tot, "images à rendre")
    for s in SHOTS: print(f'{s["i"]:3d}  m{s["beat0"] / 4:6.2f}  {s["t0"]:7.2f} s  {s["dur"]:5.2f} s  plan {s["K"]:2d}  e={s["e"]:.2f} th={s["th"]} var={s["var"]} {s["hz"]:>4} Hz  {" ".join(s["fx"])}  {s["tr"] if s["tr"] != "cut" else ""}')
