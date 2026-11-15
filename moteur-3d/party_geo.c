// ================================================================ ACTE V, deuxième partie : la fête à bord
// The inside of the Gras-Fond: x along the hull (the stage at +x), y up, the dance floor at y = -1.6.
#define CS 1.1f
#define FLY -1.6f
static float pcell(v3 p, v3 *l, float *sc, float *wall) {       // the dancer standing on the tile under p
    float ix = roundf(p.x / CS), iz = roundf(p.z / CS - 0.5f) + 0.5f, cx = ix * CS, cz = iz * CS;
    *wall = 0.55f - fmaxf(fabsf(p.x - cx), fabsf(p.z - cz));
    float id = hash2(ix * 1.7f + 3.1f, iz * 2.3f + 11.7f);
    if (ix < -12.5f || ix > 8.5f || fabsf(iz) > 2.6f || id > 0.93f || (fabsf(cx - HOLEX) < 0.1f && fabsf(cz - HOLEZ) < 0.1f) || len2(cx - CLRX, cz - CLRZ) < CLRR) return -1;
    *sc = 0.82f + 0.14f * fract(id * 7.3f);
    float a = (fract(id * 13.7f) - 0.5f) * 1.2f + 0.25f * sinf(PI * 0.5f * T / BEAT + id * 20), c = cosf(a), s = sinf(a);
    float dx = p.x - cx - (fract(id * 5.3f) - 0.5f) * 0.1f, dz = p.z - cz - (fract(id * 9.1f) - 0.5f) * 0.1f;
    *l = V((-dx * s + dz * c) / *sc, (p.y - FLY) / *sc, (dx * c + dz * s) / *sc);
    return id;
}
#define FATX 12.25f
#define FATS 1.45f
S v3 fatl(v3 p) { return V(p.z / FATS, (p.y + 1.0f) / FATS, (FATX - p.x) / FATS); }                         // Fatbass, facing the room
S v3 mecl(v3 p) { float dx = p.x - 10.95f, dz = p.z + 1.25f; return V(-dx * 0.66f + dz * 0.75f, p.y + 1.0f, dx * 0.75f + dz * 0.66f); }      // the mechanic, facing him
S v3 robl(v3 p) { return V(-(p.z - HOLEZ), p.y - FLY, -(p.x - HOLEX)); }                                    // the man in the bathrobe, facing the back of the room
static float pguest(v3 p, v3 *l) {
    if (p.x > 10.6f) {
        if (MECH > 0 && len2(p.x - 10.95f, p.z + 1.25f) < 0.8f) { *l = mecl(p); return 0.975f; }
        *l = fatl(p); return 0.985f;
    }
    float sc, wall, id = pcell(p, l, &sc, &wall); return id < 0 ? 0.5f : id;
}
static float pcrowd(v3 p, int *m) {
    float bx = fmaxf(fmaxf(p.x - 9.4f, -13.8f - p.x), fmaxf(fabsf(p.z) - 3.32f, p.y - FLY - 1.95f)); *m = M_SKIN;
    if (bx > 0.3f) return bx;
    v3 l; float sc, wall, id = pcell(p, &l, &sc, &wall), d = wall + 0.10f;
    if (id >= 0) { float b = biped(l, PI * T / BEAT + (id - 0.5f) * 0.8f, id, m) * sc; if (b < d) return b; }
    return d;
}
static float robeman(v3 p, int *m) {        // nobody knew him: a tall man in a white bathrobe, a pork chop in each hand
    *m = M_HSKIN;
    float bd = len(sub(p, V(0, 1.0f, 0.1f))) - 1.4f; if (bd > 0.5f) return bd;
    float sg = p.x > 0 ? 1.0f : -1.0f, ext = 0.5f + 0.5f * sinf(T * 2.4f + (sg > 0 ? 0 : PI));
    v3 q = V(fabsf(p.x), p.y, p.z), sh = V(0.23f, 1.48f, 0), hand = V(0.30f + 0.20f * ext, 1.22f + 0.22f * ext, 0.26f + 0.24f * ext);
    float d = len(sub(q, V(0, 1.74f, 0.01f))) - 0.155f;
    d = fminf(d, sdCap(q, V(0, 1.50f, 0), V(0, 1.62f, 0), 0.06f));
    d = fminf(d, len(sub(q, hand)) - 0.055f);
    d = fminf(d, sdCap(q, V(0.10f, 0.44f, 0), V(0.11f, 0.10f, 0), 0.055f));
    float r = fmaxf(len2(q.x * 0.88f, q.z) - mixf(0.31f, 0.20f, clampf((q.y - 0.45f) / 1.0f, 0, 1)), fabsf(q.y - 0.98f) - 0.53f) - 0.03f;
    r = smin(r, sdCap(q, sh, mix3(sh, hand, 0.82f), 0.085f), 0.06f);
    r = fminf(r, len2(len2(q.x * 0.88f, q.z) - 0.265f, q.y - 1.03f) - 0.03f);                               // the belt
    r = fminf(r, sdEll(sub(q, V(0.11f, 0.04f, 0.05f)), V(0.07f, 0.04f, 0.13f)));                             // slippers
    UN(r, M_ROBE)
    { v3 g = sub(q, add(hand, V(0, 0.10f, 0.03f))); UN(sdEll(g, V(0.15f, 0.11f, 0.035f)), M_CHOP) UN(sdCap(g, V(0.08f, -0.05f, 0), V(0.21f, -0.14f, 0), 0.022f), M_BONE) }
    return d;
}
static float proom(v3 p, int *m) {
    float r = len2(p.y, p.z), d = 3.9f - r; *m = M_WALL;
    UN(fminf(15.0f - p.x, p.x + 15.0f), M_WALL)
    UN(p.y - FLY, M_FLOOR)
    { float xi = roundf(p.x / 3) * 3; UN(fmaxf(3.68f - r, fabsf(p.x - xi) - 0.11f), M_RIB) }               // the frames of the hull
    UN(len2(p.y - 3.05f, fabsf(p.z) - 1.5f) - 0.13f, M_PIPE)
    UN(len(sub(p, V(2.0f, 2.7f, 0))) - 0.42f, M_BALL)                                                       // the mirror ball
    UN(fmaxf(len2(p.x - 2.0f, p.z) - 0.03f, 3.1f - p.y), M_PIPE)
    if (FOAMH > 0) UN(fmaxf((p.y - FLY - FOAMH - 0.07f * (sinf(p.x * 2.1f + T * 0.5f) * sinf(p.z * 1.7f - T * 0.3f) + 0.5f * sinf(p.x * 4.3f + p.z * 3.1f + T))) * 0.7f, p.x - 10.55f), M_FOAMY)
    if (p.x > 8.5f) {       // the stage: the desk and its two turntables, the speakers, the sonar
        UN(sdBox(sub(p, V(12.9f, -1.3f, 0)), V(2.3f, 0.3f, 3.6f)), M_STAGE)
        UN(sdBox(sub(p, V(11.5f, -0.55f, 0)), V(0.32f, 0.45f, 1.25f)) - 0.02f, M_DESK)
        UN(cylY(V(p.x - 11.5f, p.y + 0.055f, fabsf(p.z) - 0.72f), 0.33f, 0.022f), M_DISC)
        UN(cylY(V(p.x - 11.45f, p.y + 0.02f, p.z), 0.075f, 0.06f), M_GOLD)                                 // the big knob
        UN(sdBox(V(p.x - 12.5f, p.y - 0.1f, fabsf(p.z) - 2.75f), V(0.45f, 1.1f, 0.5f)) - 0.03f, M_SPK)
        UN(sdBox(sub(p, V(11.4f, -0.4f, 1.85f)), V(0.28f, 0.6f, 0.33f)) - 0.02f, M_SONAR)
        if (PLUG > 0) {     // the cable from the speakers to the sonar
            v3 A = V(12.3f, 0.95f, 2.3f), B = V(11.45f, 0.24f, 1.85f), a = A;
            for (int k = 1; k <= 8; k++) { float u = k / 8.0f; v3 b = mix3(A, B, u); b.y -= 0.75f * sinf(PI * u); b.x -= 0.25f * sinf(PI * u); UN(sdCap(p, a, b, 0.028f), M_CABLE) a = b; }
        }
    } else d = fminf(d, 8.9f - p.x + 0.4f);
    return d;
}
static const float CA6[6] = {0.921f, 0.1235f, -0.7975f, -0.921f, -0.1235f, 0.7975f}, SA6[6] = {0.389f, 0.992f, 0.603f, -0.389f, -0.992f, -0.603f};
static const float CA4[4] = {-0.5885f, -0.0292f, 0.5403f, 0.8776f}, SA4[4] = {-0.8085f, -0.9996f, -0.8415f, -0.4794f};
static float barset(v3 p, int *m) {         // the bar, very close: a mug, then a row of bottles; the room is a blur of lights behind
    float d = sdBox(sub(p, V(0, -0.5f, 1.0f)), V(9, 0.5f, 2.2f)); *m = M_COUNTER;
    if (p.z > 9.5f) {
        float cx = roundf(p.x / 2.4f), cy = roundf(p.y / 2.0f), h = hash2(cx * 3.3f + 1, cy * 7.7f + 2);
        UN(len(sub(p, V(cx * 2.4f + (h - 0.5f) * 1.2f, cy * 2.0f + (fract(h * 9.3f) - 0.5f) * 0.7f, 11 + 3 * fract(h * 5.1f)))) - 0.26f, M_BOKEH)
        d = fminf(d, fminf(1.2f - fabsf(p.x - cx * 2.4f), 1.0f - fabsf(p.y - cy * 2.0f)) + 0.3f);
    } else d = fminf(d, 10.2f - p.z);
    if (PMODE == 1) {
        float g = FOAMG, gl = fmaxf(len2(p.x, p.z) - 0.42f, fabsf(p.y - 0.68f) - 0.68f) - 0.02f;
        gl = fminf(gl, fmaxf(len2(len2(p.x - 0.52f, p.y - 0.7f) - 0.27f, p.z) - 0.065f, 0.40f - p.x));
        { v3 q = sub(p, V(2.0f, 0, 1.7f)); gl = fminf(gl, fmaxf(len2(q.x, q.z) - 0.42f, fabsf(q.y - 0.68f) - 0.68f) - 0.02f);
          UN(sdEll(sub(q, V(0, 1.38f, 0)), V(0.43f, 0.12f, 0.43f)), M_FOAMY) }
        UN(gl, M_GLASS)
        float f = sdEll(sub(p, V(0, 1.36f + 0.22f * g, 0)), V(0.43f + 0.08f * g, 0.10f + 0.30f * g, 0.43f + 0.08f * g));
        for (int k = 0; k < 6; k++) f = smin(f, len(sub(p, V(0.36f * CA6[k], 1.40f + 0.20f * g + 0.05f * sinf(k * 1.9f + T * 1.3f), 0.36f * SA6[k]))) - (0.13f + 0.12f * g * (0.6f + 0.4f * sinf(k * 2.3f))), 0.10f);
        for (int k = 0; k < 4; k++) { float u = clampf((g - 0.30f - 0.13f * k) * 2.0f, 0, 1); if (u <= 0) continue;
            v3 top = V(0.45f * CA4[k], 1.36f, 0.45f * SA4[k]), bot = V(0.47f * CA4[k], 1.36f - 1.27f * u, 0.47f * SA4[k]);
            f = smin(f, fminf(sdCap(p, top, bot, 0.055f), len(sub(p, bot)) - 0.10f), 0.07f); }
        { float g2 = sstep(0.5f, 1.0f, g); if (g2 > 0) f = smin(f, sdEll(sub(p, V(0.05f, 0.0f, -0.25f)), V(0.45f + 0.8f * g2, 0.035f + 0.05f * g2, 0.4f + 0.55f * g2)), 0.10f); }
        UN(f, M_FOAMY)
    } else for (int k = -2; k <= 2; k++) {
        v3 q = V(p.x - k * 0.62f - FOAMG * 0.014f * sinf(T * 47 + k * 2.1f), p.y, p.z - FOAMG * 0.014f * cosf(T * 53 + k * 1.3f)); float r = len2(q.x, q.z);
        float b = fmaxf(r - 0.20f, fabsf(q.y - 0.48f) - 0.48f) - 0.015f;
        b = smin(b, fmaxf(r - mixf(0.20f, 0.065f, clampf((q.y - 0.96f) / 0.32f, 0, 1)), fabsf(q.y - 1.2f) - 0.26f), 0.03f);
        UN(b, M_BOTTLE) UN(cylY(V(q.x, q.y - 1.48f, q.z), 0.085f, 0.035f), M_CAP)
    }
    return d;
}
static float engine(v3 p, int *m) {         // the engine room, under its red lamp
    float d = fminf(fminf(3.0f - fabsf(p.z), 3.3f - p.y), fminf(7.0f - p.x, p.x + 6.0f)); *m = M_ENGINE;
    UN(p.y, M_FLOOR)
    UN(sdBox(sub(p, V(2.2f, 0.95f, 0)), V(2.0f, 0.75f, 0.85f)) - 0.05f, M_ENGINE)
    { float xi = clampf(roundf((p.x - 0.85f) / 0.9f), 0, 3) * 0.9f + 0.85f; UN(cylY(V(p.x - xi, p.y - 2.0f, p.z), 0.3f, 0.32f) - 0.02f, M_ENGINE) }
    UN(len2(len2(p.y - 1.0f, p.z) - 0.75f, p.x - 4.5f) - 0.12f, M_PIPE)
    UN(len2(p.y - 2.75f, fabsf(p.z) - 2.5f) - 0.14f, M_PIPE)
    UN(len2(p.y - 0.55f, p.z + 2.6f) - 0.2f, M_PIPE)
    UN(len2(p.x - 5.6f, p.z - 2.2f) - 0.16f, M_PIPE)
    UN(len(sub(p, V(0.2f, 2.95f, -1.9f))) - 0.16f, M_BEACON)
    UN((p.y - FOAMH * (1 + 0.5f * sstep(5.0f, -4.0f, p.x)) - 0.10f * (sinf(p.x * 1.7f + T * 0.6f) * sinf(p.z * 1.9f - T * 0.4f) + 0.5f * sinf(p.x * 3.7f - p.z * 3.3f + T * 0.9f))) * 0.7f, M_FOAMY)
    return d;
}
static float pmap(v3 p, int *m) {
    if (PMODE == 1 || PMODE == 2) return barset(p, m);
    if (PMODE == 3) return engine(p, m);
    int mm; float d = proom(p, m), b;
    b = pcrowd(p, &mm); if (b < d) { d = b; *m = mm; }
    if (p.x > 9.5f) {
        b = biped(fatl(p), PI * T / BEAT, 0.985f, &mm) * FATS; if (b < d) { d = b; *m = mm; }
        if (MECH > 0) { b = biped(mecl(p), PI * T / BEAT, 0.975f, &mm); if (b < d) { d = b; *m = mm; } }
    } else d = fminf(d, 10.2f - p.x);
    if (HOLEX < 1e8f) { b = robeman(robl(p), &mm); if (b < d) { d = b; *m = mm; } }
    return d;
}
static float whale(v3 p, int *m) {          // far off, in the dark
    v3 q = sub(p, WHP); *m = M_WHALE;
    float bd = len(q) - 16; if (bd > 2) return bd;
    q.y -= 0.9f * sinf(q.x * 0.16f - T * 0.9f) * sstep(-2.0f, 13.0f, q.x);
    float d = sdEll(sub(q, V(-3.5f, 0, 0)), V(8.0f, 2.5f, 2.3f));
    d = smin(d, sdEll(sub(q, V(5.5f, 0.2f, 0)), V(6.5f, 1.3f, 1.1f)), 1.2f);
    d = smin(d, sdEll(V(q.x - 12.0f, q.y - 0.2f, fabsf(q.z) - 1.6f), V(1.3f, 0.18f, 2.0f)), 0.5f);
    d = smin(d, sdEll(V(q.x + 4.5f, q.y + 1.6f, fabsf(q.z) - 2.6f), V(1.6f, 0.2f, 2.2f)), 0.4f);
    return d * 0.8f;
}

