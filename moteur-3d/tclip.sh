#!/bin/sh
# ./tclip.sh nom "K s ts dur e th pat var" ... : une trame par plan (K : numéro de plan, s : instant dans le plan), planches 2x2 dans /home/claude/clip/
cd "$(dirname "$0")"; name=$1; shift; i=0
for a in "$@"; do set -- $a; K=$1; s=$2; st=$(date +%s.%N); f=$(python3 -c "print(int(round(($s+1)*50)))")
  CLIP="$3 $4 $5 $6 $7 $8" ./filmclip $f $((f+1)) $((900+100*K)) ${NOAA} > /tmp/tclip_$i.rgb 2>/dev/null; e=$(date +%s.%N); echo "plan $K s=$s : $(python3 -c "print(round($e-$st,2))") s"; i=$((i+1)); done
python3 - "$name" $i <<'Q'
import sys, numpy as np
from PIL import Image
name, n = sys.argv[1], int(sys.argv[2])
ims = [Image.fromarray(np.fromfile(f'/tmp/tclip_{i}.rgb', dtype=np.uint8).reshape(288, 576, 3)).resize((768, 576), Image.LANCZOS) for i in range(n)]
for i in range(0, n, 4):
    sh = Image.new('RGB', (1536, 1152))
    for k, im in enumerate(ims[i:i + 4]): sh.paste(im, ((k % 2) * 768, (k // 2) * 576))
    sh.save(f'/home/claude/clip/{name}_{i // 4}.png')
Q
