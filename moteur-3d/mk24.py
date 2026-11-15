"""film_body.c (après mkcity.py) + les séquences de jour de « 24 heures à Porkonia » (w24_geo.c, w24_shade.c)."""
s = open('film_body.c').read()
def rep(a, b, n=1):
    global s
    assert s.count(a) == n, (s.count(a), a[:90])
    s = s.replace(a, b)
rep("    if (SHOT == 20) return cmap(p, m);\n", "    if (SHOT == 20) return cmap(p, m);\n    if (SHOT == 21) return wmap(p, m);\n")
rep("    if (SHOT == 20) return shade_city(ro, rd, zout);\n", "    if (SHOT == 20) return shade_city(ro, rd, zout);\n    if (SHOT == 21) return shade_w(ro, rd, zout);\n")
rep("    if (tb >= 200) { setup3(tb - 200); return; }\n", "    if (tb >= 300) { setupw(tb - 300); return; }\n    if (tb >= 200) { setup3(tb - 200); return; }\n")
i = s.index("static float map(v3 p, int *m) {"); s = s[:i] + open('w24_geo.c').read() + s[i:]
i = s.index("static v3 shade(v3 ro, v3 rd, float *zout) {"); s = s[:i] + open('w24_shade.c').read() + s[i:]
open('film_body.c', 'w').write(s)
