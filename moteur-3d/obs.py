import sys, numpy as np
from PIL import Image
# obs.py src name t1 t2 ... : woven frames (two fields) from a field file, 2x2 sheets
src, name = sys.argv[1], sys.argv[2]; ts = [float(x) for x in sys.argv[3:]]
W, H = 576, 288
mm = np.memmap(src, dtype=np.uint8, mode='r'); n = mm.size // (W * H * 3); mm = mm[:n * W * H * 3].reshape(n, H, W, 3)
ims = []
for t in ts:
    f = min(int(round(t * 50)) // 2 * 2, n - 2)
    fr = np.zeros((576, W, 3), np.uint8); fr[0::2] = mm[f]; fr[1::2] = mm[f + 1]
    ims.append(Image.fromarray(fr).resize((768, 576), Image.LANCZOS))
for i in range(0, len(ims), 4):
    sh = Image.new('RGB', (1536, 1152))
    for k, im in enumerate(ims[i:i + 4]): sh.paste(im, ((k % 2) * 768, (k // 2) * 576))
    sh.save(f'{name}_{i // 4}.png')
print(n, 'fields')
