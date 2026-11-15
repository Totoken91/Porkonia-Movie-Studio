"""film_body.c = moteur de la première partie + la fête (party_geo.c, party_shade.c) + retouches."""
s = open('film_body_part1.c').read()
def rep(a, b, n=1):
    global s
    assert s.count(a) == n, (s.count(a), a[:90])
    s = s.replace(a, b)
rep("M_BUB };", "M_BUB,\n       M_FLOOR, M_WALL, M_STAGE, M_DESK, M_DISC, M_SPK, M_SONAR, M_CABLE, M_BALL, M_PIPE, M_ROBE, M_HSKIN, M_CHOP, M_BONE, M_HPHONE, M_GLASS, M_FOAMY, M_BOTTLE, M_CAP, M_COUNTER, M_BOKEH, M_ENGINE, M_BEACON, M_WHALE };")
rep("static float PC = 1, PS = 0, GAP0 = 1, GAP1 = 0, BEAMS = 1;",
    "static float PC = 1, PS = 0, GAP0 = 1, GAP1 = 0, BEAMS = 1;\n"
    "static int PARTY, PMODE, NIGHT, NOPORT, WHALE, TRACK = 47;\n"
    "static float CLRX, CLRZ, CLRR, FOAMH, FOAMG, FOAMT, HOLEX = 1e9f, HOLEZ, MECH, KNOB, NOD, PLUG, SONARON, RINGS, WLIT = 0.55f, HOFF; static v3 WHP;")
# ---- the pig: dancing, and the two special guests
rep("    float bd = len(sub(p, V(0, 0.85f, 0))) - 1.2f; if (bd > 0.5f) return bd;\n    float sg = p.x > 0",
    "    float bd = len(sub(p, V(0, 0.85f, 0))) - 1.2f; if (bd > 0.5f) return bd;\n"
    "    int special = id > 0.97f, fat = id > 0.98f, mec = special && !fat; float cr = 0;\n"
    "    if (PARTY) { cr = (mec ? 0.05f * NOD : 0.07f) * (0.5f - 0.5f * cosf(2 * ph)); p.y += cr; }      // they all sink on the beat, feet on the floor\n"
    "    float sg = p.x > 0")
rep("    d = fminf(d, len(sub(q, hand)) - 0.068f);",
    "    if (PARTY) { float pv = fract(id * 17.3f); mug = !special && pv >= 0.4f && pv < 0.65f && sg > 0;\n"
    "        if (fat) hand = sg > 0 ? V(0.45f, 0.68f, 0.48f + 0.05f * sinf(4 * ph)) : mix3(V(0.37f, 1.36f + 0.10f * sinf(2 * ph), 0.12f), V(0.04f, 0.71f, 0.55f), KNOB);\n"
    "        else if (mec) hand = sg > 0 ? V(0.34f, 0.80f + 0.10f * sinf(T * 5) * (1 - NOD), 0.26f) : V(0.31f, 0.56f, 0);\n"
    "        else if (pv < 0.4f) hand = V(0.28f, 1.33f + 0.10f * sinf(2 * ph + (sg > 0 ? 0 : PI)), 0.06f);\n"
    "        else if (pv < 0.65f) hand = sg > 0 ? V(0.34f, 1.10f + 0.05f * sinf(2 * ph), 0.14f) : V(0.30f, 1.30f + 0.08f * sinf(2 * ph), 0.05f);\n"
    "        else if (pv < 0.85f) hand = V(0.10f + 0.10f * fabsf(sinf(ph)), 0.92f, 0.27f);\n"
    "        else hand = sg > 0 ? V(0.33f, 1.36f + 0.06f * sinf(4 * ph), 0.02f) : V(0.30f, 0.60f, 0.05f); }\n"
    "    d = fminf(d, len(sub(q, hand)) - 0.068f);")
rep("v3 H = V(0.11f, 0.46f, 0), F = V(0.11f, 0.06f + 0.07f * fmaxf(cs, 0), 0.17f * sn);",
    "v3 H = V(0.11f, 0.46f, 0), F = PARTY ? V(0.11f, 0.06f + cr, 0.015f) : V(0.11f, 0.06f + 0.07f * fmaxf(cs, 0), 0.17f * sn);")
rep("    if (fract(id * 29.3f) < 0.3f) o = smin(", "    if (fract(id * 29.3f) < 0.3f && !special) o = smin(")
rep("    if (fract(id * 41.1f) < 0.35f) UN(", "    if (fract(id * 41.1f) < 0.35f && !special) UN(")
rep("    if (h2 < 0.22f) { float hy = q.y - 1.46f;",
    "    if (fat) {      // Fatbass: an officer's cap, dark glasses, headphones\n"
    "        UN(sdEll(sub(q, V(0, 1.53f, 0)), V(0.25f, 0.07f, 0.24f)), M_FOAM) UN(sdEll(sub(q, V(0, 1.49f, 0.20f)), V(0.17f, 0.02f, 0.12f)), M_BLACK) UN(len(sub(q, V(0, 1.545f, 0.225f))) - 0.035f, M_GOLD)\n"
    "        UN(sdBox(sub(q, V(0, 1.295f, 0.285f)), V(0.20f, 0.045f, 0.03f)) - 0.012f, M_BLACK)\n"
    "        UN(fmaxf(len2(len2(q.x, q.y - 1.25f) - 0.375f, q.z + 0.02f) - 0.03f, 1.25f - q.y), M_HPHONE) UN(sdEll(sub(q, V(0.36f, 1.22f, -0.02f)), V(0.07f, 0.12f, 0.12f)), M_HPHONE)\n"
    "    } else if (mec) { UN(sdEll(sub(q, V(0, 1.52f, 0)), V(0.27f, 0.10f, 0.27f)), M_HAT) UN(sdEll(sub(q, V(0, 1.47f, 0.18f)), V(0.16f, 0.02f, 0.13f)), M_HAT) }\n"
    "    else if (h2 < 0.22f) { float hy = q.y - 1.46f;")
rep("static float guest(v3 p, v3 *l) {       // which guest owns this point: id, and the point in his own frame\n",
    "static float pguest(v3 p, v3 *l);\nstatic float guest(v3 p, v3 *l) {       // which guest owns this point: id, and the point in his own frame\n    if (PARTY) return pguest(p, l);\n")
# ---- materials
rep("int k = (int)(fract(id * 13.7f) * 5.999f); M->alb = PALO[k];", "int k = (int)(fract(id * 13.7f) * 5.999f); M->alb = PALO[k];\n        if (id > 0.98f) M->alb = V(0.03f, 0.05f, 0.14f); else if (id > 0.97f) M->alb = V(0.08f, 0.20f, 0.40f);")
rep("        if (h2 < 0.22f) M->alb = fract(l.y * 9) < 0.5f ? PALB[k] : V(0.92f, 0.88f, 0.8f);", "        if (id > 0.97f) M->alb = V(0.95f, 0.70f, 0.06f);\n        else if (h2 < 0.22f) M->alb = fract(l.y * 9) < 0.5f ? PALB[k] : V(0.92f, 0.88f, 0.8f);")
rep("if (hw < 0.55f) M->emis", "if (hw < WLIT) M->emis")
i = s.index("    default: break;\n    }\n}")
s = s[:i] + "    case M_WHALE: M->alb = p.y < WHP.y - 0.9f ? V(0.55f, 0.60f, 0.62f) : V(0.24f, 0.30f, 0.36f); M->spec = 0.3f; break;\n" + s[i:]
# ---- the night coast
rep("*h = hash2(ix * 1.3f + 7, iz * 1.7f + 3);", "*h = hash2(ix * 1.3f + 7 + HOFF, iz * 1.7f + 3);")
rep("    if (!INSH && p.z < -25 && fabsf(p.x) < 22", "    if (!INSH && !NOPORT && p.z < -25 && fabsf(p.x) < 22")
rep("    {   // the lighthouse on its breakwater", "    if (!NOPORT) {   // the lighthouse on its breakwater")
rep("static v3 skygrad(v3 rd) {\n", "static v3 skygrad(v3 rd) {\n    if (NIGHT) { float y = fmaxf(rd.y, 0), md = fmaxf(dot(rd, MOON), 0); return add(mix3(V(0.004f, 0.007f, 0.022f), V(0.02f, 0.035f, 0.075f), powf(1 - y, 6)), mul(V(0.10f, 0.12f, 0.18f), 0.25f * powf(md, 20) + 0.5f * powf(md, 300))); }\n")
rep("    if (sd > 0.9985f) col = mix3(col, V(26, 15, 5)", "    if (sd > 0.9985f && !NIGHT) col = mix3(col, V(26, 15, 5)")
rep("* 60 * (h - 0.986f)", "* (NIGHT ? 150 : 60) * (h - 0.986f)")
rep("v3 cc = mix3(V(0.14f, 0.09f, 0.17f), V(2.1f, 0.75f, 0.28f), ed * side * side);", "v3 cc = NIGHT ? mix3(V(0.012f, 0.016f, 0.03f), V(0.16f, 0.19f, 0.27f), ed * side * side) : mix3(V(0.14f, 0.09f, 0.17f), V(2.1f, 0.75f, 0.28f), ed * side * side);")
rep("static v3 skyamb(v3 n) {    // what the dusk sky sends on a surface\n", "static v3 skyamb(v3 n) {    // what the dusk sky sends on a surface\n    if (NIGHT) return mul(V(0.012f, 0.018f, 0.04f), 0.6f + 0.4f * n.y);\n")
rep("    v3 L = V(0, 0, 0); *spec = V(0, 0, 0);\n", "    v3 L = V(0, 0, 0); *spec = V(0, 0, 0);\n    if (NOPORT) return L;\n")
rep("    v3 acc = V(0, 0, 0); float a = T * 0.9f;\n", "    v3 acc = V(0, 0, 0); float a = T * 0.9f;\n    if (NOPORT) return acc;\n")
rep("    if (fabsf(p.x) < 3 && p.z > -64 && p.z < 10.5f) {", "    if (!NOPORT && fabsf(p.x) < 3 && p.z > -64 && p.z < 10.5f) {")
rep("static float LBT; static int DBG;", "static __thread float LBT; static int DBG;")
# ---- dispatch
rep("static float map(v3 p, int *m) {\n    int mm; float d = subm(p, m), b;\n    if (SHOT == 5) { b = bubbles(p); if (b < d) { d = b; *m = M_BUB; } return d; }",
    "static float map(v3 p, int *m) {\n    if (SHOT == 10) return pmap(p, m);\n    if (NOPORT) return town(p, m);\n    int mm; float d = subm(p, m), b;\n"
    "    if (SHOT == 5) { b = bubbles(p); if (b < d) { d = b; *m = M_BUB; } if (WHALE) { b = whale(p, &mm); if (b < d) { d = b; *m = mm; } } return d; }")
rep("static v3 shade_deep(v3 ro, v3 rd, float *zout) {", "static v3 sonar_rings(v3 ro, v3 rd, float tt);\nstatic v3 shade_deep(v3 ro, v3 rd, float *zout) {")
rep("    col = add(col, mul(V(0.6f, 0.85f, 0.85f), msnow(ro, rd, tt)", "    if (RINGS > 0) col = add(col, sonar_rings(ro, rd, tt));\n    col = add(col, mul(V(0.6f, 0.85f, 0.85f), msnow(ro, rd, tt)")
rep("static v3 shade(v3 ro, v3 rd, float *zout) {\n", "static v3 shade(v3 ro, v3 rd, float *zout) {\n    if (SHOT == 10) return shade_party(ro, rd, zout);\n    if (SHOT == 99) { *zout = 1; return V(0, 0, 0); }\n")
rep("    SUN = nrm(V(-0.8f, 0.07f, 1)); SUNC = V(3.2f, 1.25f, 0.42f); MOON = nrm(V(0.48f, 0.21f, -0.86f));\n",
    "    SUN = nrm(V(-0.8f, 0.07f, 1)); SUNC = V(3.2f, 1.25f, 0.42f); MOON = nrm(V(0.48f, 0.21f, -0.86f));\n"
    "    PARTY = 0; PMODE = 0; NIGHT = 0; NOPORT = 0; WHALE = 0; FOAMH = 0; FOAMG = 0; FOAMT = 0; CLRR = 0; HOLEX = 1e9f; MECH = 0; KNOB = 0; NOD = 0; PLUG = 0; SONARON = 0; RINGS = 0; WLIT = 0.55f; HOFF = 0;\n"
    "    if (tb >= 100) { setup2(tb - 100); return; }\n")
i = s.index("static float map(v3 p, int *m) {"); s = s[:i] + open('party_geo.c').read() + s[i:]
i = s.index("static v3 shade(v3 ro, v3 rd, float *zout) {"); s = s[:i] + open('party_shade.c').read() + s[i:]
open('film_body.c', 'w').write(s)
