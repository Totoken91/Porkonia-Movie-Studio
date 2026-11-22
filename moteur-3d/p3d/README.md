# p3d : le moteur des clips 3D courts (Canal Pork)

Le moteur léger du skill « porkonia-3d » : une scène = un fichier `.c` qui inclut `p3d.h` et écrit `map()`, `sky()`, `material()`, `scene_init()` et `frame_setup()`. Images 512 x 384, 25 i/s, rendu sur le processeur (OpenMP). Pour les citoyens, les foules et les intérieurs, c'est le moteur du film (dossier parent) qu'il faut.

| Fichier | Rôle |
|---|---|
| `p3d.h` | Le moteur (maths, primitives, cochon couronné, lettres géantes, textures, ombres, reflets, boucle de rendu) |
| `pigspin.c` | Le cochon des titrages Canal Pork (96 vues qui tournent, `pig.rgba`) |
| `martin_dn.h` | L'adaptateur qui fait tourner **Martin Plaisir** (le Danseur du moteur du film : `../danseur.h`, `../danseur_mat.h`, tels quels) dans p3d |
| `ml_dance.h` | Sa danse souple de 32 temps : **copie** de `ml_dance` dans `../clip_shade.c` (garder les deux pareilles) |
| `ml_sexy.h` | Sa danse « archi sexy » devant l'anneau (26 temps, 22 nov. 2026) |
| `tresor-national/` | Le clip « Le Cockring Homologué Martin Plaisir, trésor national » (22 nov. 2026) |

## Garde-fou contre les objets coupés (22 nov. 2026)

Le groin des cochons disparaissait quand la caméra avançait. Cause : un raccourci de calcul trop optimiste. Le pas autorisé au bord d'une case de foule était « distance au bord + 0,45 », tapé à la main ; un rayon venu de la case vide d'à côté sautait 0,45 de trop dans la case du cochon et passait par-dessus l'avant du groin, collé au bord. Le contrôle l'a confirmé : le bug remis exprès, il signale 61 images, objet « groin ».

Ce qui rend ce genre d'artefact impossible à laisser passer :
- `grid_xz(p, cs, rmax, fn, m)` : la foule sur une grille, une seule case évaluée (aussi rapide qu'avant), avec un pas de bord exact : distance au bord + (cs / 2 - rmax), jamais plus. `rmax` se mesure (le cochon couronné : 1,583 x l'échelle) ; `grid_check()` arrête le programme si l'objet ne tient pas dans sa case.
- `P3D_EARLY(b, seuil)` : tout raccourci « loin d'ici, saute » passe par cette macro. Règle : la valeur rendue ne doit jamais dépasser la vraie distance.
- `./scene check F0 F1 PAS [coupes]` (une dizaine de secondes, en 160 x 120, à lancer avant chaque rendu ; il ne ralentit pas le rendu) : chaque image est calculée deux fois, avec les pas exacts du rendu et sans aucun raccourci, à pas prudents. Il signale un rayon qui rate ou qui touche un autre objet que la référence, en taches d'au moins deux pixels, et dit quel objet et où. Il ignore ce qui n'est pas un trou : bord frôlé à moins d'un pixel, horizon lointain.
- Autres défauts trouvés par le contrôle et corrigés dans le moteur : les lettres géantes (« + 0,25 » qui coupait le bas des lettres, puis un mur fantôme sur le bord du cadre : borne sûre maintenant), le sol rasant au loin (`P3D_GROUND = 1` : un rayon qui descend touche le plan y = 0 calculé exactement), les bords rasés quand les pas sont épuisés (touchés s'ils sont à moins d'un pixel).

Ajouts au moteur le 22 nov. 2026 :
- Le cochon couronné de p3d **est** le cochon standard n° 2 (`../porc4.h`), inclus tel quel : groin fondu dans la tête, deux narines rondes toutes simples, couronne d'or à cinq pointes, une seule matière pour tout le corps (un cochon d'or a donc un groin d'or). L'ancien cochon fait main de p3d (groin rose collé, sans narines) est supprimé. p3d.h fournit pour ça les noms du moteur du film (`v3`, `V()`, `sdEll`, `cylZ`...), ce qui permet aussi d'inclure `danseur.h` tel quel.
- La couronne du cochon standard est remodelée (`p4_crown` dans `../porc4.h`, 22 nov. 2026, Kenny : « un vrai modèle », pas des triangles accrochés à un cylindre) : d'une seule pièce, la paroi s'évase et son bord monte lui-même en cinq pointes aux creux arrondis, perles soudées aux pointes, bourrelet roulé à la base, moulure sur le bandeau, cinq cabochons sertis. Son or (`M_CROWN`) ne reflète que le ciel : en miroir parfait, le damier du sol y faisait des zébrures. Le moteur du film compile avec (vérifié).
- `P3D_HITT` : la distance du premier objet touché, pour les effets POSTFX (faisceaux, particules qui passent derrière les objets).
- `POSTFX` : une scène peut définir `#define POSTFX ma_fonction` avant `#include "p3d.h"` pour ajouter un effet à chaque rayon (le halo doré et les étincelles en étoile des pierres du clip).
- `PIG_NOSHADOW = 1` : la peau des cochons couronnés ne reçoit plus d'ombre. En gros plan, le groin faisait une tache sombre à bord dur sur le visage (on aurait dit qu'il était gravé). Leurs ombres au sol restent.

Martin Plaisir dans p3d : `DNN = 0; DNLOOK = DN_MARTIN;` à chaque image dans `frame_setup`, une pose (`dn_rest`, `ml_dance`, `ml_sexy`, ou la marche `mp_walk` de `tresor.c`), puis `dn_place(&P, position, lacet, échelle)` ; dans `map`, `d = dn_map(p, m, d);` ; dans `material`, `if (m >= D_SKIN) dn_to_mat(p, m, o);`. Il fait 1,9 dans son repère : échelle 2,3 à côté des cochons couronnés. `ml_dance.h` nomme des tableaux `W` et `H`, d'où le `push_macro` dans l'adaptateur.

## Le clip « trésor national »

```sh
cd tresor-national
python3 gen_tex.py                                    # lettres TRÉSOR NATIONAL, plaque de la fiche technique
gcc -O3 -ffast-math -march=native -fopenmp -I.. tresor.c -o tresor -lm
./test.sh planche.png 12 27 38 50                     # images fixes aux temps donnés
./tresor 0 1355 9.63 19.41 29.68 34.57 41.41 47.28 > frames.rgb      # 20 à 25 min sur 2 cœurs
python3 build.py audio && python3 build.py video      # intro, titrages, légendes, logo, Porko Effect v3, NO SIGNAL
```

Il faut les outils de la chaîne dans `/home/claude/channelpork` (skill channel-pork-video) et la musique de Kenny (`ba260513-muizik.mp3`, prise à partir de 1:30), qui n'est pas dans ce dépôt. Les coupes sont calées sur le tempo du morceau (122 BPM).

Ce qu'il y a dedans :
- La rotonde du Musée national de Porkonia, la nuit : 20 colonnes, damier de marbre brillant, tapis rouge, ville et lune.
- L'anneau : jonc d'or poli au profil biseauté, perlage sur les arêtes, tresse ciselée, 16 pierres taillées à facettes serties à griffes (rubis et diamants), couronne de Porkonia au sommet, étincelles en étoile.
- Les cochons couronnés en larmes : la larme sort du coin de l'œil, glisse sur la joue (à la surface de la tête), puis tombe.
- Ciel de crépuscule étoilé (Voie lactée et bandes de poussière, étoiles de toutes couleurs, comète à deux queues, étoiles filantes, nuages roses bas) ; douches de lumière volumétriques et flaques au sol, poussières d'or, projecteurs de couleur qui balaient le ciel puis se croisent au-dessus de l'anneau ; bloom au montage ; lettres TRÉSOR NATIONAL en émail bordeaux, biseau d'or et ampoules de music-hall.
- Martin Plaisir remonte le tapis en roulant des hanches (`mp_walk`), puis danse `ml_sexy` devant l'anneau, de dos ; l'anneau lévite, TRÉSOR NATIONAL descend du ciel, feu d'artifice.
- Habillage : deux titrages seulement (Kenny : « le titrage incessant est insupportable ») ; les stats sont des légendes discrètes accrochées à l'objet (point d'or, trait fin, texte court) ; pas d'incident de bande sur la fiche technique.
