import sys, numpy as np
from PIL import Image
# obf.py src name t1 t2 ... : single fields (no weave), each 1152x864, 1x2 sheets -> name_k.png ; optional crop "t:x0,y0,x1,y1" in field px (576x288)
src, name = sys.argv[1], sys.argv[2]
W, H = 576, 288
mm = np.memmap(src, dtype=np.uint8, mode='r'); n = mm.size // (W * H * 3); mm = mm.reshape(n, H, W, 3)
ims = []
for a in sys.argv[3:]:
    t, _, c = a.partition(':'); f = min(int(round(float(t) * 50)), n - 1); im = Image.fromarray(np.array(mm[f]))
    if c: x0, y0, x1, y1 = [int(v) for v in c.split(',')]; im = im.crop((x0, y0, x1, y1)); w = x1 - x0; h = (y1 - y0) * 2; s = min(1152 / w, 864 / h); im = im.resize((int(w * s), int(h * s)), Image.LANCZOS)
    else: im = im.resize((1152, 864), Image.LANCZOS)
    ims.append(im)
for i, im in enumerate(ims): im.save(f'{name}_{i}.png')
