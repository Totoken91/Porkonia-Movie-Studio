#!/bin/sh
# ./thd.sh nom "K s ts dur e th pat var" ... : une image fixe haute définition par plan (deux trames de 1152 x 576 tissées, puis 1152 x 864), dans /home/claude/clip/chope/nom_N.png
cd "$(dirname "$0")"; name=$1; shift; i=0
for a in "$@"; do set -- $a; K=$1; s=$2; st=$(date +%s.%N); f=$(python3 -c "print(2*int(round(($s+1)*25)))")
  CLIP="$3 $4 $5 $6 $7 $8" ./filmclip_hd $f $((f+2)) $((900+100*K)) > /tmp/thd.rgb 2>/dev/null; e=$(date +%s.%N)
  python3 -c "
import numpy as np
from PIL import Image
a = np.fromfile('/tmp/thd.rgb', np.uint8).reshape(2, 576, 1152, 3); fr = np.zeros((1152, 1152, 3), np.uint8); fr[0::2] = a[0]; fr[1::2] = a[1]
Image.fromarray(fr).resize((1152, 864), Image.LANCZOS).save('/home/claude/clip/chope/${name}_$i.png')"
  echo "plan $K : $(python3 -c "print(round($e-$st,1))") s"; i=$((i+1)); done
