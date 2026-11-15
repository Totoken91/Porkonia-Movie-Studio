#!/bin/sh
cd "$(dirname "$0")" && python3 mkpart2.py && python3 mkcity.py && python3 mk24.py && python3 - <<'P'
c = open('pork3d2.c').read(); s = open('film_body.c').read()
pre = c[:c.index("enum { FLOOR")]
tex = c[c.index("// ---------- textures"):c.index("static void emb(")].replace("static float *TX; static unsigned char *EM;", "static float *TX;")
prim = c[c.index("// ---------- primitives"):c.index("// ---------- the crowned pig")]
open('filmc.c', 'w').write(pre + tex + prim + s)
P
gcc -O3 -ffast-math -march=native -fopenmp -o ${OUT:-filmc} filmc.c -lm 2>&1 | grep -E "error|warning: (impl|unused variable)" | head -20
