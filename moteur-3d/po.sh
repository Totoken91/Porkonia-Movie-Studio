#!/bin/sh
# ./po.sh sortie.png : l'affiche des Ranga Boyz (plan 50), image fixe portrait 1080 x 1440 (binaire filmclip_po)
cd "$(dirname "$0")" && CLIP="0 4 0 0 0 0 1 0" ./filmclip_po 100 102 5900 > /tmp/po.rgb 2>/dev/null && python3 -c "
import numpy as np
from PIL import Image
a = np.fromfile('/tmp/po.rgb', np.uint8).reshape(2, 720, 1080, 3); fr = np.zeros((1440, 1080, 3), np.uint8); fr[0::2] = a[0]; fr[1::2] = a[1]; Image.fromarray(fr).save('$1')"
