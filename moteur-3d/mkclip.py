"""film_body.c (après mk24.py) + le clip « Sous la neige de Douzi City » : clip_geo.c, clip_shade.c, et les aiguillages de la séquence 6."""
s = open('film_body.c').read()
def rep(a, b, n=1):
    global s
    assert s.count(a) == n, (s.count(a), a[:90])
    s = s.replace(a, b)
i = s.index("static float wmap0(v3 p, int *m) {"); s = s[:i] + open('clip_geo.c').read() + s[i:]
i = s.index("// ================================================================ le moteur commun"); s = s[:i] + open('clip_shade.c').read() + s[i:]
rep("    switch (SEQ) { case 1: return par_map(p, m);", "    switch (SEQ) { case 6: return clip_map(p, m); case 1: return par_map(p, m);")
rep("static v3 wfogc(v3 rd) { switch (SEQ) {", "static v3 wfogc(v3 rd) { switch (SEQ) { case 6: return clip_fogc(rd);")
rep("static float wfod(v3 ro, v3 rd, float t) { switch (SEQ) {", "static float wfod(v3 ro, v3 rd, float t) { switch (SEQ) { case 6: return clip_fod(ro, rd, t);")
rep("static v3 wsky(v3 ro, v3 rd) { switch (SEQ) {", "static v3 wsky(v3 ro, v3 rd) { switch (SEQ) { case 6: return clip_sky(ro, rd);")
rep("        if (SEQ == 1 && len2(p.x - WPX[k], p.z - WPZ[k]) > 0.85f) {", "        if (SEQ == 6 && clip_czmat(p, k, m, &cm)) { }\n        else if (SEQ == 1 && len2(p.x - WPX[k], p.z - WPZ[k]) > 0.85f) {")
rep("    default: switch (SEQ) { case 1: par_material(p, n, m, t, M, &snow, &up); break;", "    default: switch (SEQ) { case 6: clip_material(p, n, m, t, M, &snow, &up); break; case 1: par_material(p, n, m, t, M, &snow, &up); break;")
rep("    return L;\n}\nstatic float wtrace(", "    if (SEQ == 6) L = add(L, clip_light(p, n));\n    return L;\n}\nstatic float wtrace(")
rep("    if (SEQ == 3 && depth == 0) { col = add(col, tav_shafts(", "    if (SEQ == 6 && depth == 0) col = clip_air(ro, rd, t, col);\n    if (SEQ == 3 && depth == 0) { col = add(col, tav_shafts(")
rep("    SEQ = (int)(tb / 100.0f); float s = tb - 100.0f * SEQ; WS = s;", "    SEQ = (int)(tb / 100.0f); float s = tb - 100.0f * SEQ; int clipk = SEQ - 6; if (SEQ > 6) SEQ = 6; WS = s;")
rep("case 5: fin_setup(s); break;", "case 5: fin_setup(s); break; case 6: clip_setup(clipk, s); break;")
rep("        setup(fi / 50.0f + t0); int par = fi & 1;", "        setup(fi / 50.0f + t0); int par = getenv(\"CLIPPAR0\") ? 0 : (fi & 1);")
open('film_body.c', 'w').write(s)
