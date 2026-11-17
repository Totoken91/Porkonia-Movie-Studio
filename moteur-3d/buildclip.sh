#!/bin/sh
# Le moteur du film + le clip : binaire filmclip (ne touche pas à filmc).
cd "$(dirname "$0")" && python3 mkpart2.py && python3 mkcity.py && python3 mk24.py && python3 mkclip.py && python3 - <<'Q'
c = open('pork3d2.c').read(); s = open('film_body.c').read()
pre = c[:c.index("enum { FLOOR")]
tex = c[c.index("// ---------- textures"):c.index("static void emb(")].replace("static float *TX; static unsigned char *EM;", "static float *TX;")
prim = c[c.index("// ---------- primitives"):c.index("// ---------- the crowned pig")]
open('filmclip.c', 'w').write(pre + tex + prim + s)
Q
gcc -O3 -ffast-math -march=native -fopenmp -o ${OUT:-filmclip} filmclip.c -lm 2>&1 | grep -E "error|warning: (impl|unused variable)" | head -30
if [ -n "$HD" ]; then sed -e 's/^#define W 576$/#define W 1152/' -e 's/^#define HF 288$/#define HF 576/' -e 's|py = (576 - 2 \* (2 \* j + par + 0.5f + JY\[s \& 3\] \* 2)) / 576.0f;|py = (2 * HF - 2 * (2 * j + par + 0.5f + JY[s \& 3] * 2)) / (2.0f * HF);|' filmclip.c > filmclip_hd.c && gcc -O3 -ffast-math -march=native -fopenmp -o filmclip_hd filmclip_hd.c -lm 2>&1 | grep -E "error" | head; grep -c "2 \* HF - 2" filmclip_hd.c; fi
