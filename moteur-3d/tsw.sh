#!/bin/sh
# T0=300 ./tsw.sh name t1 t2 ... : one field of a 24h sequence at each time, 2x2 sheets
cd "$(dirname "$0")"; name=$1; shift
for t in "$@"; do s=$(date +%s.%N); f=$(python3 -c "print(int(round($t*50)))"); ${BIN:-./filmc} $f $((f+1)) ${T0:-300} > tc_$t.rgb 2>/dev/null; e=$(date +%s.%N); echo "t=$t $(python3 -c "print(round($e-$s,2))")s"; done
python3 - "$name" "$@" <<'P'
import sys, numpy as np
from PIL import Image
name = sys.argv[1]; ts = sys.argv[2:]
ims = [Image.fromarray(np.fromfile(f'tc_{t}.rgb', dtype=np.uint8).reshape(288, 576, 3)).resize((768, 576), Image.LANCZOS) for t in ts]
for i in range(0, len(ims), 4):
    sh = Image.new('RGB', (1536, 1152))
    for k, im in enumerate(ims[i:i + 4]): sh.paste(im, ((k % 2) * 768, (k // 2) * 576))
    sh.save(f'{name}_{i // 4}.png')
P
