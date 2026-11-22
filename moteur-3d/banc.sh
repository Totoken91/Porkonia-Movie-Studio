#!/bin/sh
# Le banc d'essai (plan 51 du clip, 22 nov. 2026) : douze scènes-pièges au studio, en images fixes HD, assemblées en planche.
#   ./banc.sh nom [soleil] [épreuves...]     soleil = azimut par huitièmes de tour (0 à 7) ; épreuves 0 à 11 (toutes par défaut)
#   BIN=./filmclip_hd (par défaut) ; WDBG=64 pour voir les normales, 32 l'occlusion
# 0 visage soleil rasant · 1 visage contre-jour · 2 bras au ciel · 3 deux citoyens collés · 4 foule des 24 allures · 5 vue de dessus
# 6 pieds (baskets, sabots) · 7 main et chope · 8 Martin Plaisir · 9 marcheurs de profil · 10 chope contre-jour · 11 caméra à 50 cm · 12 six gestes (bras et corps)
cd "$(dirname "$0")"; name=$1; sun=${2:-0}; shift; [ $# -gt 0 ] && shift; list=${*:-"0 1 2 3 4 5 6 7 8 9 10 11 12"}
mkdir -p banc; for v in $list; do s=$(date +%s.%N)
  CLIP="0 4 0.5 0 $sun $v 1 0" ${BIN:-./filmclip_hd} 100 102 6001 > banc/_$v.rgb 2>/dev/null
  echo "épreuve $v : $(python3 -c "print(round($(date +%s.%N)-$s,1))") s"; done
python3 - "$name" $list <<'P'
import sys, numpy as np
from PIL import Image, ImageDraw
name, vs = sys.argv[1], sys.argv[2:]
NOMS = ["visage rasant", "visage contre-jour", "bras au ciel", "deux collés", "foule 24", "vue de dessus", "pieds", "main + chope", "Martin", "marcheurs", "chope contre-jour", "camera 50 cm", "six gestes"]
ims = []
for v in vs:
    a = np.fromfile(f"banc/_{v}.rgb", np.uint8).reshape(2, 576, 1152, 3); fr = np.zeros((1152, 1152, 3), np.uint8); fr[0::2] = a[0]; fr[1::2] = a[1]
    im = Image.fromarray(fr).resize((960, 720), Image.LANCZOS); d = ImageDraw.Draw(im); d.rectangle((0, 0, 260, 26), fill=(0, 0, 0)); d.text((6, 6), f"{v} {NOMS[int(v)]}", fill=(255, 220, 0)); ims.append(im)
for i in range(0, len(ims), 4):
    sh = Image.new("RGB", (1920, 1440))
    for k, im in enumerate(ims[i:i + 4]): sh.paste(im, ((k % 2) * 960, (k // 2) * 720))
    sh.save(f"banc/{name}_{i // 4}.png"); print(f"banc/{name}_{i // 4}.png")
P
