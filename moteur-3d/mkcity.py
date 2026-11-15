"""film_body.c (sortie de mkpart2.py) + Douzi City (city_geo.c, city_shade.c) + filtre étoile sur les lumières."""
s = open('film_body.c').read()
def rep(a, b, n=1):
    global s
    assert s.count(a) == n, (s.count(a), a[:90])
    s = s.replace(a, b)
rep("M_BEACON, M_WHALE };", "M_BEACON, M_WHALE,\n       M_GROUND, M_FACADE, M_ROOF, M_CHIM, M_AWN, M_BLADE, M_BLOCK, M_LPOST, M_LBULB, M_WIRE, M_TRAM, M_CHROME, M_TITLE, M_PED, M_COL, M_DOME, M_MIN, M_SENAT, M_HALLES, M_NSIGN, M_STEEL, M_HAM, M_SAUC, M_CHAP, M_CLOTH, M_PATE, M_JAR, M_IVORY, M_HGLASS, M_HSIGN, M_STACK };")
rep("static int PARTY, PMODE, NIGHT, NOPORT, WHALE, TRACK = 47;", "static int PARTY, PMODE, NIGHT, NOPORT, WHALE, TRACK = 47;\nstatic float TRZ, TDIR = 1, SPARK, TITLEON, CPX = 1e9f, CPZ, DAWN, STAR; static int NPL; static float PLM[4][9];")
rep("static float pguest(v3 p, v3 *l);\n", "static float pguest(v3 p, v3 *l); static float cguest(v3 p, v3 *l);\n")
rep("    if (PARTY) return pguest(p, l);\n", "    if (PARTY) return pguest(p, l);\n    if (SHOT == 20) return cguest(p, l);\n")
rep("static float map(v3 p, int *m) {\n    if (SHOT == 10) return pmap(p, m);\n", "static float map(v3 p, int *m) {\n    if (SHOT == 10) return pmap(p, m);\n    if (SHOT == 20) return cmap(p, m);\n")
rep("static v3 shade(v3 ro, v3 rd, float *zout) {\n    if (SHOT == 10) return shade_party(ro, rd, zout);\n", "static v3 shade(v3 ro, v3 rd, float *zout) {\n    if (SHOT == 10) return shade_party(ro, rd, zout);\n    if (SHOT == 20) return shade_city(ro, rd, zout);\n")
rep("    if (tb >= 100) { setup2(tb - 100); return; }\n", "    CPX = 1e9f; TITLEON = 0; STAR = 0; DAWN = 0; SPARK = 0; TDIR = 1;\n    if (tb >= 200) { setup3(tb - 200); return; }\n    if (tb >= 100) { setup2(tb - 100); return; }\n")
# ---- a star filter on the lens: every strong light throws a cross (the old trick of the 90s)
rep("    boxblur(b1, bt, bw, bh, 2); memcpy(b2, b1, sizeof(float) * bw * bh * 3); boxblur(b2, bt, bw, bh, 7);\n",
    "    static float *s1, *s2; if (!s1) { s1 = malloc(sizeof(float) * bw * bh * 3); s2 = malloc(sizeof(float) * bw * bh * 3); }\n"
    "    if (STAR > 0) {\n"
    "        #pragma omp parallel for\n"
    "        for (int y = 0; y < bh; y++) for (int x = 0; x < bw; x++) for (int c = 0; c < 3; c++) { float ah = 0, av = 0;\n"
    "            for (int k = -18; k <= 18; k++) { float wk = 1 - fabsf((float)k) / 19.0f; int xx = x + k, yy = y + k;\n"
    "                if (xx >= 0 && xx < bw) ah += fmaxf(b1[(y * bw + xx) * 3 + c] - 5.0f, 0) * wk;\n"
    "                if (yy >= 0 && yy < bh) av += fmaxf(b1[(yy * bw + x) * 3 + c] - 5.0f, 0) * wk; }\n"
    "            s1[(y * bw + x) * 3 + c] = ah / 19.0f; s2[(y * bw + x) * 3 + c] = av / 19.0f; }\n"
    "    }\n"
    "    boxblur(b1, bt, bw, bh, 2); memcpy(b2, b1, sizeof(float) * bw * bh * 3); boxblur(b2, bt, bw, bh, 7);\n")
rep("            float v = fmaxf((HDR[(y * W + x) * 3 + c] + 0.55f * g1 + 0.40f * g2) * EXPO * vig, 0);",
    "            float g3 = STAR > 0 ? (s1[(ya * bw + xa) * 3 + c] + s2[(ya * bw + xa) * 3 + c]) * (1 - fx) * (1 - fy) + (s1[(ya * bw + xb) * 3 + c] + s2[(ya * bw + xb) * 3 + c]) * fx * (1 - fy)\n"
    "                + (s1[(yb * bw + xa) * 3 + c] + s2[(yb * bw + xa) * 3 + c]) * (1 - fx) * fy + (s1[(yb * bw + xb) * 3 + c] + s2[(yb * bw + xb) * 3 + c]) * fx * fy : 0;\n"
    "            float v = fmaxf((HDR[(y * W + x) * 3 + c] + 0.55f * g1 + 0.40f * g2 + 1.3f * STAR * g3) * EXPO * vig, 0);")
i = s.index("static float map(v3 p, int *m) {"); s = s[:i] + '#define CZ_BND0 0.30f\n' + open('citizen.h').read() + open('city_geo.c').read() + s[i:]
i = s.index("static v3 shade(v3 ro, v3 rd, float *zout) {"); s = s[:i] + open('city_shade.c').read() + s[i:]
open('film_body.c', 'w').write(s)
