#!/bin/bash
# usage: ./test.sh out.png t1 t2 ...  -> planche des images aux temps donnés
CT="9.63 19.41 29.68 34.57 41.41 47.28"
out=$1; shift; rm -f /tmp/tf_*.rgb; i=0
for t in "$@"; do f=$(python3 -c "print(int(round($t*25)))"); ./tresor $f $((f+1)) $CT > /tmp/tf_$i.rgb 2>/dev/null; i=$((i+1)); done
python3 - "$out" "$@" <<'PY'
import sys, numpy as np
from PIL import Image, ImageDraw
out, ts = sys.argv[1], sys.argv[2:]
ims = [Image.fromarray(np.fromfile(f"/tmp/tf_{i}.rgb", np.uint8).reshape(384, 512, 3)) for i in range(len(ts))]
cols = min(3, len(ims)); rows = (len(ims) + cols - 1) // cols
S = Image.new("RGB", (cols * 512, rows * 384))
for i, (im, t) in enumerate(zip(ims, ts)):
    ImageDraw.Draw(im).text((6, 4), t, fill=(255, 255, 0)); S.paste(im, ((i % cols) * 512, (i // cols) * 384))
S.save(out)
PY
