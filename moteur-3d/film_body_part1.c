// ===================== PORKONIA, LE FILM : acte V, premiere partie =====================
// Rendu par trames (576 x 288, 50 trames par seconde) : la vraie structure d'une image video.
#define BEAT 0.5625f
#define PI 3.14159265f
enum { M_OCEAN, M_WOOD, M_PILE, M_POLE, M_LAMP, M_BULB, M_BANNER, M_KEG, M_HULL, M_RIB, M_PENNANT, M_SKIN, M_BLACK, M_EYEW, M_OUTFIT, M_HAT, M_GOLD,
       M_MUG, M_FOAM, M_SCARF, M_HOOF, M_BALLOON, M_LAND, M_HOUSE, M_STONE, M_LHOUSE, M_BOAT, M_BUB };
#define UN(dd, mm) { float _d = (dd); if (_d < d) { d = _d; *m = (mm); } }

static int SHOT, NOPIGS; static float T, QEND, DOORON, FOCUS, APERT, EXPO, FOV;
static v3 SUN, SUNC, MOON, SUBP, RO, CU, CV, CW;
static float PC = 1, PS = 0, GAP0 = 1, GAP1 = 0, BEAMS = 1;
S v3 subd(v3 d) { return V(PC * d.x - PS * d.y, PS * d.x + PC * d.y, d.z); }
S v3 subq(v3 p) { return subd(sub(p, SUBP)); }
S v3 subw(v3 q) { return add(SUBP, V(PC * q.x + PS * q.y, -PS * q.x + PC * q.y, q.z)); }
static void cam(v3 ro, v3 ta, float fov) { RO = ro; CW = nrm(sub(ta, ro)); CU = nrm(cross(V(0, 1, 0), CW));
    if (fabsf(CW.y) > 0.99999f) CU = nrm(cross(V(0, 0, 1), CW));       // (22 nov. 2026) caméra qui vise pile en haut ou en bas : sans ça, l'image s'écrase en une seule couleur
    CV = cross(CW, CU); FOV = fov; }
S float hashi(float x, float y) {
    unsigned n = (unsigned)(int)x * 374761393u + (unsigned)(int)y * 668265263u;
    n = (n ^ (n >> 13)) * 1274126177u; n ^= n >> 16;
    return (n & 0xffffff) * (1.0f / 16777216.0f);
}
static int refr(v3 i, v3 n, float eta, v3 *o) {
    float c = dot(n, i), k = 1 - eta * eta * (1 - c * c);
    if (k < 0) return 0;
    *o = sub(mul(i, eta), mul(n, eta * c + sqrtf(k)));
    return 1;
}
static const v3 PALO[6] = {{0.30f, 0.03f, 0.07f}, {0.05f, 0.08f, 0.20f}, {0.62f, 0.42f, 0.08f}, {0.06f, 0.18f, 0.10f}, {0.75f, 0.70f, 0.60f}, {0.10f, 0.10f, 0.11f}};
static const v3 PALB[5] = {{0.55f, 0.04f, 0.09f}, {0.95f, 0.62f, 0.10f}, {0.92f, 0.85f, 0.70f}, {0.05f, 0.45f, 0.50f}, {0.95f, 0.35f, 0.45f}};

// ---------------------------------------------------------------- the sea
static float wave(float x, float z, float *dx, float *dz) {
    static const float A[6] = {0.26f, 0.15f, 0.08f, 0.045f, 0.025f, 0.014f}, KX[6] = {0.07f, -0.16f, 0.33f, 0.55f, -1.0f, 1.6f},
                       KZ[6] = {0.10f, 0.20f, -0.25f, 0.75f, 0.8f, 1.9f}, W[6] = {1.09f, 1.58f, 2.0f, 3.0f, 3.5f, 4.9f};
    float h = 0; *dx = 0; *dz = 0;
    for (int i = 0; i < 6; i++) { float a = KX[i] * x + KZ[i] * z + W[i] * T + i * 1.7f, s = sinf(a), c = cosf(a); h += A[i] * s; *dx += A[i] * KX[i] * c; *dz += A[i] * KZ[i] * c; }
    return h;
}
static float ocean_hit(v3 ro, v3 rd, float tmax) {
    if (rd.y > -1e-4f) return -1;
    float t = ro.y > 0.7f ? (0.7f - ro.y) / rd.y : 0.0f, dx, dz;
    if (t > tmax) return -1;
    if (t > 260) { t = -ro.y / rd.y; return t < tmax ? t : -1; }
    for (int i = 0; i < 48; i++) {
        v3 p = add(ro, mul(rd, t)); float d = p.y - wave(p.x, p.z, &dx, &dz);
        if (d < 0.003f * (1 + t * 0.08f)) return t < tmax ? t : -1;
        t += d / (0.45f - rd.y); if (t > tmax) return -1;
    }
    return t < tmax ? t : -1;
}

// ---------------------------------------------------------------- the guests
static float biped(v3 p, float ph, float id, int *m) {      // a little upright pig, feet on y = 0, walking toward +z
    *m = M_SKIN;
    float bd = len(sub(p, V(0, 0.85f, 0))) - 1.2f; if (bd > 0.5f) return bd;
    float sg = p.x > 0 ? 1.0f : -1.0f, f = ph + (sg > 0 ? 0 : PI), sn = sinf(f), cs = cosf(f);
    v3 q = V(fabsf(p.x), p.y, p.z);
    float h1 = id, h2 = fract(id * 7.31f);
    int dancer = h1 < 0.12f, mug = h1 > 0.62f && sg > 0, wav = h1 > 0.5f && h1 <= 0.62f && sg > 0;
    float d = sdEll(sub(q, V(0, 1.22f, 0.02f)), V(0.33f, 0.29f, 0.29f));                                    // head
    d = smin(d, cylZ(sub(q, V(0, 1.16f, 0.30f)), 0.115f, 0.05f) - 0.03f, 0.05f);                           // snout
    { v3 e = sub(q, V(0.2f, 1.50f, -0.02f)); float c = 0.94f, s = 0.34f;                                    // ear
      d = smin(d, sdEll(V(c * e.x - s * e.y, s * e.x + c * e.y, e.z), V(0.09f, 0.13f, 0.045f)), 0.04f); }
    v3 hand = dancer ? V(0.30f, 1.20f + 0.05f * sn, 0.05f) : mug ? V(0.36f, 1.02f, 0.16f) : wav ? V(0.40f + 0.07f * sinf(T * 9 + id * 20), 1.30f, 0.04f) : V(0.31f, 0.56f, -0.15f * sn);
    d = fminf(d, len(sub(q, hand)) - 0.068f);
    d = fminf(d, len2(len2(q.y - 0.55f, q.z + 0.25f) - 0.06f, q.x) - 0.02f);                               // tail
    float o = smin(sdEll(sub(q, V(0, 0.70f, 0)), V(0.25f, 0.30f, 0.21f)), len(sub(q, V(0, 0.64f, 0.07f))) - 0.20f, 0.08f);
    o = smin(o, sdCap(q, V(0.24f, 0.88f, 0), hand, 0.058f), 0.04f);
    {   // legs: hip, knee, hoof
        v3 H = V(0.11f, 0.46f, 0), F = V(0.11f, 0.06f + 0.07f * fmaxf(cs, 0), 0.17f * sn);
        float dy = F.y - H.y, dz = F.z - H.z, L = sqrtf(dy * dy + dz * dz), k = sqrtf(fmaxf(0.0484f - L * L * 0.25f, 0));
        v3 K = V(0.11f, (H.y + F.y) * 0.5f + dz / L * k, (H.z + F.z) * 0.5f - dy / L * k);
        o = smin(o, fminf(sdCap(q, H, K, 0.078f), sdCap(q, K, F, 0.07f)), 0.04f);
        UN(sdEll(sub(q, add(F, V(0, -0.01f, 0.03f))), V(0.08f, 0.05f, 0.10f)), M_HOOF)
    }
    if (fract(id * 29.3f) < 0.3f) o = smin(o, fmaxf(len2(q.x, q.z) - mixf(0.36f, 0.22f, clampf((q.y - 0.22f) / 0.3f, 0, 1)), fabsf(q.y - 0.37f) - 0.15f), 0.03f);
    UN(o, M_OUTFIT)
    UN(len(sub(q, V(0.125f, 1.29f, 0.262f))) - 0.052f, M_EYEW)
    UN(len(sub(q, V(0.125f, 1.29f, 0.305f))) - 0.027f, M_BLACK)
    UN(len(sub(q, V(0.045f, 1.165f, 0.385f))) - 0.024f, M_BLACK)
    if (fract(id * 41.1f) < 0.35f) UN(len2(len2(q.x, q.z) - 0.2f, (q.y - 0.97f) * 1.3f) - 0.055f, M_SCARF)
    if (mug) { v3 g = sub(q, add(hand, V(0, 0.1f, 0.03f))); UN(cylY(g, 0.075f, 0.09f), M_MUG) UN(len(sub(g, V(0, 0.11f, 0))) - 0.07f, M_FOAM) }
    if (h2 < 0.22f) { float hy = q.y - 1.46f; UN(fmaxf(len2(q.x, q.z) - 0.17f * (1 - hy / 0.42f), fmaxf(-hy, hy - 0.42f)) * 0.85f, M_HAT) UN(len(sub(q, V(0, 1.89f, 0))) - 0.045f, M_FOAM) }
    else if (h2 < 0.40f) { UN(fminf(cylY(sub(q, V(0, 1.62f, 0)), 0.16f, 0.15f), cylY(sub(q, V(0, 1.48f, 0)), 0.25f, 0.012f)), M_HAT) }
    else if (h2 < 0.55f) { UN(sdEll(sub(q, V(0.03f, 1.52f, -0.02f)), V(0.27f, 0.075f, 0.26f)), M_HAT) }
    else if (h2 < 0.65f) { UN(box2(fabsf(len2(q.x, q.z) - 0.17f) - 0.02f, fabsf(q.y - 1.53f) - 0.05f), M_GOLD) }
    return d;
}
#define QS 1.25f
static float qpig(v3 p, float col, float diz, v3 *l, float *sc, float *zl) {     // the guest of column col, in the cell p falls in or the next one
    float zz = p.z - T - col * 0.31f, iz = roundf(zz / QS) + diz, id = hash2(iz, col * 3 + 1.7f);
    float zc = iz * QS + T + col * 0.31f, ph = 7.854f * T + id * 40;
    *sc = 0.9f + 0.25f * fract(id * 7.3f); *zl = zz - iz * QS;
    *l = V((p.x - col * 0.68f - (fract(id * 3.1f) - 0.5f) * 0.22f) / *sc, (p.y - 1.2f - (id < 0.12f ? 0.07f : 0.022f) * fabsf(sinf(ph))) / *sc, *zl / *sc);
    { float zr = iz * QS + col * 0.31f; if (zr > GAP0 && zr < GAP1) return -1; }
    return (zc > -62 && zc < QEND) ? id : -1;
}
static float qcell(v3 p, v3 *l, float *sc, float *zl) { return qpig(p, p.x > 0 ? 1.0f : -1.0f, 0, l, sc, zl); }
static float guest(v3 p, v3 *l) {       // which guest owns this point: id, and the point in his own frame
    if (fabsf(p.x - 1.55f) < 0.7f && fabsf(p.z - 7.6f) < 0.7f) { *l = V(-(p.x - 1.55f), p.y - 1.2f, -(p.z - 7.6f)); return 0.17f; }
    float sc, zl; float id = qcell(p, l, &sc, &zl); return id < 0 ? 0.5f : id;
}
static float pigat(v3 p, float col, float diz, int *m, float *zl0) {        // one guest and his balloon
    v3 l; float sc, zl, d = 1e9f, id = qpig(p, col, diz, &l, &sc, &zl); int mm; *zl0 = zl;
    if (id < 0) return d;
    d = biped(l, 7.854f * T + id * 40, id, m) * sc;
    if (fract(id * 53.7f) < 0.22f) {
        v3 c = V(col * 0.68f + 0.22f + 0.1f * sinf(T * 1.3f + id * 9), 1.2f + sc * 2.3f + 0.05f * sinf(T * 2 + id * 5), -0.12f + 0.08f * sinf(T * 1.1f + id * 3));
        float b = sdEll(V(p.x - c.x, p.y - c.y, zl - c.z), V(0.2f, 0.25f, 0.2f)); if (b < d) { d = b; *m = M_BALLOON; }
    }
    (void)mm; return d;
}
static float queue(v3 p, int *m) {
    float d = 1e9f; int mm; *m = M_SKIN;
    float d0 = fmaxf(fmaxf(p.y - 4.6f, fabsf(p.x) - 2.1f), fmaxf(-63.5f - p.z, p.z - 10.4f));
    if (d0 > 0.5f) return d0;
    { float b = biped(V(-(p.x - 1.55f), p.y - 1.2f, -(p.z - 7.6f)), 0, 0.17f, &mm); if (b < d) { d = b; *m = mm; } }      // the doorman
    if (NOPIGS) return d;
    // every guest lives in his own cell; near a border the neighbour is measured too, so that no ray steps into him
    float own = p.x > 0 ? 1.0f : -1.0f, g = 1e9f;
    for (int c = 0; c < 2; c++) {
        float col = c ? -own : own, zl, zl2;
        if (c && fabsf(p.x) >= 0.3f) { g = fminf(g, fabsf(p.x) + 0.06f); break; }
        float b = pigat(p, col, 0, &mm, &zl); if (b < d) { d = b; *m = mm; }
        if (fabsf(zl) > 0.3f) { b = pigat(p, col, zl > 0 ? 1.0f : -1.0f, &mm, &zl2); if (b < d) { d = b; *m = mm; } g = fminf(g, 1.0f); }
        else g = fminf(g, QS * 0.5f - fabsf(zl) + 0.15f);
    }
    return fminf(d, g);
}

// ---------------------------------------------------------------- the Gras-Fond
static float subm(v3 p, int *m) {
    v3 q = subq(p); *m = M_HULL;
    float bl = len(sub(q, V(clampf(q.x, -25, 19), 3.0f, 0)));
    if (bl > 11.5f) return bl - 9.0f;
    float d = sdCap(q, V(-15, 0, 0), V(15, 0, 0), 4.2f);
    d = smin(d, sdEll(sub(q, V(-16, 0, 0)), V(10.5f, 3.6f, 3.6f)), 0.8f);
    d = smin(d, sdBox(sub(q, V(3.5f, 5.4f, 0)), V(2.8f, 1.9f, 0.7f)) - 0.6f, 0.7f);
    d = fminf(d, sdBox(sub(q, V(3.5f, 6.4f, 0)), V(0.9f, 0.1f, 3.0f)) - 0.06f);
    d = fminf(d, cylY(sub(q, V(4.6f, 9.0f, 0)), 0.12f, 1.4f));
    d = fminf(d, cylY(sub(q, V(2.6f, 8.6f, 0)), 0.2f, 1.0f));
    d = fminf(d, sdBox(sub(q, V(-23.5f, 0, 0)), V(1.5f, 4.4f, 0.14f)) - 0.06f);
    d = fminf(d, sdBox(sub(q, V(-23.5f, 0, 0)), V(1.5f, 0.14f, 4.4f)) - 0.06f);
    float ip = clampf(roundf(q.x / 4.0f), -3, 3), pxl = q.x - 4.0f * ip;
    UN(len2(len2(pxl, q.y - 0.6f) - 0.86f, fabsf(q.z) - 4.157f) - 0.12f, M_RIB)
    { float xb = clampf(roundf(q.x / 1.5f) * 1.5f, -13.5f, 15.0f); UN(len(V(q.x - xb, q.y - 3.55f, fabsf(q.z) - 2.5f)) - 0.12f, M_BULB) }       // a string of bulbs on each shoulder
    UN(cylY(sub(q, V(-6.0f, 4.25f, 0)), 0.75f, 0.12f) - 0.04f, M_RIB)                                                                          // the hatch
    UN(len2(len2(q.y, q.z) - 2.3f, q.x + 25.6f) - 0.16f, M_RIB)                                                                                 // the ring around the screw
    d = fminf(d, fmaxf(fabsf(q.x - 1.5f) - 15.5f, fmaxf(fabsf(q.z) - 1.15f, fabsf(q.y - 4.22f) - 0.07f)));                                      // the deck
    if (fabsf(q.z) < 1.0f && q.y > 2.5f) for (int k = 0; k < 2; k++) {          // bunting from the mast to the bow and to the stern
        v3 A = V(3.5f, 9.6f, 0), B = k ? V(-23.5f, 5.0f, 0) : V(19.0f, 4.7f, 0), ab = sub(B, A); float L = len(ab);
        float u = clampf(dot(sub(q, A), ab) / L, 0, L), i = roundf(u / 0.9f), ui = clampf(i * 0.9f, 0.45f, L - 0.45f);
        v3 c = add(A, mul(ab, ui / L)); c.y -= 1.3f * sinf(PI * ui / L);
        float dn = c.y - q.y, zl = q.z - 0.07f * sinf(T * 6 + i) * dn;
        UN(fmaxf(fmaxf(fabsf(q.x - c.x) - 0.27f * (1 - dn / 0.6f), -dn), fmaxf(dn - 0.6f, fabsf(zl) - 0.025f)) * 0.7f, M_PENNANT)
    }
    return d;
}

// ---------------------------------------------------------------- the pier
static float pier(v3 p, int *m) {
    float d = sdBox(sub(p, V(0, 1.05f, -27)), V(2.2f, 0.15f, 37.2f)); *m = M_WOOD;
    if (d > 6.5f) return d - 1.5f;
    float zc = clampf(roundf((p.z + 62) / 4), 0, 18) * 4 - 62, lx = fabsf(p.x) - 2.05f;
    UN(cylY(V(fabsf(p.x) - 1.9f, p.y + 0.6f, p.z - zc), 0.17f, 1.6f), M_PILE)
    float zl = clampf(roundf((p.z + 60) / 8), 0, 8) * 8 - 60;
    UN(cylY(V(lx, p.y - 3.1f, p.z - zl), 0.05f, 1.9f), M_POLE)
    UN(sdBox(V(lx, p.y - 5.1f, p.z - zl), V(0.13f, 0.17f, 0.13f)), M_LAMP)
    if (p.y > 3.9f && p.z > -60.5f && p.z < 4.5f) {                                // garlands along both sides
        float zb = roundf(p.z / 0.8f) * 0.8f, u = fract((zb + 60) / 8), yc = 4.95f - 0.5f * (1 - (2 * u - 1) * (2 * u - 1));
        UN(len(V(lx, p.y - yc, p.z - zb)) - 0.085f, M_BULB)
    }
    if (p.y > 4.2f && p.z > -14 && p.z < 10) {                                     // a ceiling of bulbs before the door
        float zs = clampf(roundf((p.z + 12) / 4), 0, 5) * 4 - 12, xb = clampf(roundf(p.x / 0.5f), -4, 4) * 0.5f;
        UN(len(V(p.x - xb, p.y - (5.0f - 0.42f * (1 - xb * xb / 4.2f)), p.z - zs)) - 0.085f, M_BULB)
    }
    UN(cylY(V(lx, p.y - 3.5f, p.z - 8.6f), 0.07f, 2.3f), M_POLE)
    UN(sdBox(sub(p, V(0, 5.45f, 8.6f)), V(2.25f, 0.40f, 0.035f)), M_BANNER)
    { float kz = clampf(roundf(p.z + 7.2f), 0, 2) - 7.2f; v3 k = V(p.x - 1.62f, p.y - 1.65f, p.z - kz);
      UN(cylY(k, 0.36f + 0.05f * (1 - k.y * k.y / 0.2f), 0.45f) * 0.8f, M_KEG)
      UN(cylY(V(p.x - 1.62f, p.y - 2.55f, p.z + 6.7f), 0.38f, 0.45f) * 0.9f, M_KEG) }
    return d;
}

// ---------------------------------------------------------------- Port-Cochon: the hill, the houses, the church, the lighthouse, the boats
static float land_h(float x, float z) {
    float r = fmaxf((-66 - z) / 100.0f, 0);
    return 1.6f + 30 * powf(fminf(r, 1.1f), 1.4f) + (3 * sinf(x * 0.03f + 1) + 2 * sinf(x * 0.07f + z * 0.05f)) * fminf(r, 1) + 45 * sstep(1.0f, 1.9f, r) * (0.65f + 0.35f * sinf(x * 0.017f + 2));
}
static int house(float px, float pz, float *c, float *h) {    // c: cx, cz, wx, wz, hh, rh, by, swap
    float ix = roundf(px / 10), iz = roundf((pz + 77) / 10); *h = hash2(ix * 1.3f + 7, iz * 1.7f + 3);
    c[0] = ix * 10 + (fract(*h * 11.3f) - 0.5f) * 1.2f; c[1] = iz * 10 - 77 + (fract(*h * 23.7f) - 0.5f) * 1.2f;
    if (*h > 0.86f || (fabsf(c[0] + 30) < 13 && fabsf(c[1] + 118) < 15)) return 0;
    c[2] = 2.4f + 1.0f * fract(*h * 5.1f); c[3] = 2.4f + 1.0f * fract(*h * 9.7f); c[4] = 3.4f + 3.4f * fract(*h * 17.9f); c[5] = 1.5f + 1.2f * fract(*h * 31.1f);
    c[6] = land_h(c[0], c[1]) - 0.3f; c[7] = fract(*h * 41.3f) < 0.5f;
    return 1;
}
static __thread int INSH;           // set while a shadow ray is marching: the boats' masts are too thin and too far to count
static const float BOATS[4][3] = {{-9, -40, 0.4f}, {9.5f, -30, 2.1f}, {-13, -56, 1.2f}, {14, -52, -0.6f}};
static float town(v3 p, int *m) {
    float d = 1e9f; *m = M_LAND;
    if (!INSH && p.z < -25 && fabsf(p.x) < 22 && p.y < 4.5f) for (int i = 0; i < 4; i++) {            // boats
        float dx, dz, by = 0.8f * wave(BOATS[i][0], BOATS[i][1], &dx, &dz) + 0.2f, c = cosf(BOATS[i][2]), s = sinf(BOATS[i][2]);
        v3 q = V(p.x - BOATS[i][0], p.y - by, p.z - BOATS[i][1]); q = V(c * q.x + s * q.z, q.y, -s * q.x + c * q.z);
        if (len(q) > 4.5f) { d = fminf(d, len(q) - 3.5f); continue; }
        float hd = fmaxf(fmaxf(sdEll(q, V(2.4f, 0.85f, 0.95f)), q.y - 0.22f), -sdEll(sub(q, V(0, 0.28f, 0)), V(2.2f, 0.8f, 0.78f)));
        UN(hd, M_BOAT) UN(cylY(sub(q, V(0.3f, 1.6f, 0)), 0.04f, 1.5f), M_POLE) UN(len(sub(q, V(0.3f, 3.15f, 0))) - 0.1f, M_LAMP)
    }
    {   // the lighthouse on its breakwater
        v3 q = sub(p, V(-45, 0, -30));
        float bd = fminf(len(V(q.x, 0, q.z)) - 9, fmaxf(fabsf(q.x) - 2.5f, fabsf(q.z + 19) - 17.5f));
        if (bd < 3 && p.y < 26) {
            float r = len2(q.x, q.z);
            UN(fminf(sdEll(sub(q, V(0, -0.5f, 0)), V(7, 3, 7)), sdBox(sub(q, V(0, 0.6f, -19)), V(2.2f, 1.0f, 17))), M_STONE)
            UN(fmaxf(r - mixf(2.2f, 1.4f, clampf((q.y - 2) / 18, 0, 1)), fabsf(q.y - 11) - 9) * 0.95f, M_LHOUSE)
            UN(fminf(cylY(sub(q, V(0, 20.3f, 0)), 2.0f, 0.15f), fmaxf(r - 1.25f * (1 - (q.y - 22.4f) / 1.6f), fmaxf(22.4f - q.y, q.y - 24)) * 0.8f), M_LHOUSE)
            UN(cylY(sub(q, V(0, 21.4f, 0)), 1.0f, 1.0f), M_LAMP)
        } else d = fminf(d, fmaxf(bd, p.y - 25) - 1);
    }
    if (p.z > -62) return fminf(d, p.z + 66);
    UN(fmaxf((p.y - land_h(p.x, p.z)) * 0.55f, p.z + 66), M_LAND)
    if (p.y < 9) { float lx = p.x - clampf(roundf(p.x / 12), -8, 8) * 12;                     // lamps along the quay
        UN(cylY(V(lx, p.y - 3.6f, p.z + 67.2f), 0.07f, 2.0f), M_POLE) UN(len(V(lx, p.y - 5.75f, p.z + 67.2f)) - 0.22f, M_LAMP) }
    {   // the church
        v3 q = V(p.x + 30, p.y - land_h(-30, -118) + 1, p.z + 118);
        if (len(q) < 40) {
            float ch = sdBox(sub(q, V(0, 4, -3)), V(4.5f, 4.5f, 8));
            ch = fminf(ch, fmaxf(fmaxf((fabsf(q.x) * 3.2f + (q.y - 8.5f) * 4.8f - 15.36f) / 5.77f, 8.4f - q.y), fabsf(q.z + 3) - 8.3f));
            ch = fminf(ch, sdBox(sub(q, V(0, 9, 7.5f)), V(2.6f, 9.5f, 2.6f)));
            ch = fminf(ch, fmaxf((len2(q.x, q.z - 7.5f) - 3.3f * (28.5f - q.y) / 10) * 0.94f, 18.4f - q.y));
            UN(ch, M_STONE)
        }
    }
    if (p.z < -70 && p.z > -172 && fabsf(p.x) < 125 && p.y < 62) {
        float c[8], h;
        if (house(p.x, p.z, c, &h)) {
            v3 q = V(p.x - c[0], p.y - c[6], p.z - c[1]); float wx = c[2], wz = c[3];
            if (c[7] > 0.5f) { float t_ = q.x; q.x = q.z; q.z = t_; t_ = wx; wx = wz; wz = t_; }
            float hd = sdBox(sub(q, V(0, c[4] * 0.5f - 1.5f, 0)), V(wx, c[4] * 0.5f + 1.5f, wz)), ow = wx + 0.25f;
            hd = fminf(hd, fmaxf(fmaxf((fabsf(q.x) * c[5] + (q.y - c[4]) * ow - c[5] * ow) / sqrtf(c[5] * c[5] + ow * ow), c[4] - 0.05f - q.y), fabsf(q.z) - wz - 0.25f));
            UN(hd, M_HOUSE)
        }
        d = fminf(d, fminf(5 - fabsf(p.x - roundf(p.x / 10) * 10), 5 - fabsf(p.z + 77 - roundf((p.z + 77) / 10) * 10)) + 0.7f);
    }
    return d;
}
static float bubbles(v3 p) {            // rising from the hull once it is under
    v3 q = sub(p, SUBP);
    float bd = fmaxf(fmaxf(fabsf(q.x + 5) - 28, fabsf(q.z) - 7), fmaxf(-q.y - 3, p.y + 0.3f)); if (bd > 1.0f) return bd;
    const float c = 2.2f; float ix = floorf(p.x / c), iz = floorf(p.z / c);
    float y = p.y - T * (1.6f + 2.2f * hashi(ix, iz)), iy = floorf(y / c), h = hashi(ix * 3 + iy * 7, iz * 5 - iy * 11);
    v3 l = V(p.x - (ix + 0.5f) * c, y - (iy + 0.5f) * c, p.z - (iz + 0.5f) * c);
    float guard = fminf(fminf(c * 0.5f - fabsf(l.x), c * 0.5f - fabsf(l.y)), c * 0.5f - fabsf(l.z)) + 0.1f;
    if (h < 0.45f) return guard;
    float r = 0.055f + 0.65f * (h - 0.45f) * (h - 0.45f);
    v3 o = V((fract(h * 13.7f) - 0.5f) * 0.9f + 0.06f * sinf(y * 2.5f + h * 40), (fract(h * 7.3f) - 0.5f) * 0.9f, (fract(h * 23.1f) - 0.5f) * 0.9f);
    return fminf(len(sub(l, o)) - r, guard);
}
static float map(v3 p, int *m) {
    int mm; float d = subm(p, m), b;
    if (SHOT == 5) { b = bubbles(p); if (b < d) { d = b; *m = M_BUB; } return d; }
    b = pier(p, &mm); if (b < d) { d = b; *m = mm; }
    b = queue(p, &mm); if (b < d) { d = b; *m = mm; }
    b = town(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}
static v3 normal(v3 p, float t) {
    int m; float e = 0.0015f * (1 + t * 0.04f);
    float a = map(V(p.x + e, p.y - e, p.z - e), &m), b = map(V(p.x - e, p.y - e, p.z + e), &m), c = map(V(p.x - e, p.y + e, p.z - e), &m), d = map(V(p.x + e, p.y + e, p.z + e), &m);
    return nrm(V(a - b - c + d, -a - b + c + d, -a + b - c + d));
}
static float trace(v3 ro, v3 rd, int *m, float tmax, int steps) {
    float t = 0.03f;
    for (int i = 0; i < steps && t < tmax; i++) { float d = map(add(ro, mul(rd, t)), m); if (d < 0.0008f * (1 + t * 0.6f)) return t; t += d; }
    return -1;
}
static float shadow(v3 ro, v3 rd) {
    float res = 1, t = 0.06f; int m; INSH = 1;
    for (int i = 0; i < 36 && t < 75; i++) { float d = map(add(ro, mul(rd, t)), &m); if (d < 0.004f) { INSH = 0; return 0; } if (d < 0.12f) res = fminf(res, mixf(fminf(40 * d / t, 1), 1, sstep(0.05f, 0.12f, d))); t += clampf(d, 0.06f, 6.0f); }
    INSH = 0; return clampf(res, 0, 1);
}
static float ambocc(v3 p, v3 n) {
    float o = 0, sc = 1; int m;
    for (int i = 1; i <= 4; i++) { float h = 0.05f + 0.14f * i, d = map(add(p, mul(n, h)), &m); o += (h - fminf(d, h)) * sc; sc *= 0.7f; }
    return clampf(1 - 1.6f * o, 0, 1);
}

// ---------------------------------------------------------------- the sky at dusk
static v3 skygrad(v3 rd) {
    float y = fmaxf(rd.y, 0), hl = sqrtf(rd.x * rd.x + rd.z * rd.z) + 1e-5f, sl = sqrtf(SUN.x * SUN.x + SUN.z * SUN.z);
    float k = 0.5f + 0.5f * (rd.x * SUN.x + rd.z * SUN.z) / (hl * sl); k *= k;
    v3 col = mix3(V(0.035f, 0.07f, 0.22f), mix3(V(0.40f, 0.25f, 0.42f), V(1.6f, 0.62f, 0.22f), k), powf(1 - y, 5));
    col = add(col, mul(V(1.4f, 0.9f, 0.35f), k * k * expf(-y * 14)));
    float sd = fmaxf(dot(rd, SUN), 0);
    return add(col, mul(SUNC, 0.22f * powf(sd, 12) + 0.5f * powf(sd, 90)));
}
static v3 psky(v3 rd) {
    v3 col = skygrad(rd);
    float sd = dot(rd, SUN);
    if (sd > 0.9985f) col = mix3(col, V(26, 15, 5), sstep(0.9985f, 0.9990f, sd));
    float md = dot(rd, MOON);
    if (md > 0.995f) { float cr = fbm(rd.x * 40 + 3, rd.y * 40 + rd.z * 25);
        col = mix3(col, mul(V(1.5f, 1.4f, 1.15f), 0.75f + 0.5f * cr), sstep(0.9982f, 0.9987f, md)); col = add(col, mul(V(0.25f, 0.27f, 0.35f), sstep(0.995f, 1.0f, md) * 0.5f)); }
    if (rd.y > 0.08f) {     // first stars, away from the sun
        float q = rd.y + 0.3f, sx = rd.x / q * 90, sz = rd.z / q * 90, h = hash2(floorf(sx), floorf(sz));
        if (h > 0.986f) col = add(col, mul(V(1, 1, 1), sstep(0.08f, 0.5f, rd.y) * (1 - 0.7f * fmaxf(sd, 0)) * 60 * (h - 0.986f) * sstep(0.3f, 0.1f, len2(fract(sx) - 0.5f, fract(sz) - 0.5f))));
    }
    if (rd.y > 0.012f) for (int l = 0; l < 2; l++) {    // two decks of cloud, lit from underneath by the sun that has gone
        float s = l ? 2.6f : 1.2f, k = s / (rd.y + 0.09f), u = rd.x * k + T * 0.012f + l * 7, v = rd.z * k + l * 3;
        float dn = fbm(u, v), c = sstep(0.50f, 0.80f, dn) * sstep(0.012f, 0.14f, rd.y);
        if (c > 0.003f) { float d2 = fbm(u + SUN.x * 0.35f, v + SUN.z * 0.35f), ed = clampf((dn - d2) * 4 + 0.3f, 0, 1), side = 0.5f + 0.5f * fmaxf(sd, -0.4f);
            v3 cc = mix3(V(0.14f, 0.09f, 0.17f), V(2.1f, 0.75f, 0.28f), ed * side * side); col = mix3(col, cc, c * 0.9f); }
    }
    return col;
}
static v3 skyamb(v3 n) {    // what the dusk sky sends on a surface
    float sl = sqrtf(SUN.x * SUN.x + SUN.z * SUN.z), up = 0.5f + 0.5f * n.y, sd = (n.x * SUN.x + n.z * SUN.z) / sl;
    v3 a = add(mul(V(0.07f, 0.09f, 0.20f), up), mul(V(0.05f, 0.035f, 0.03f), 1 - up));
    a = add(a, mul(V(0.75f, 0.30f, 0.10f), fmaxf(sd, 0) * fmaxf(sd, 0) * (0.35f + 0.65f * (1 - fabsf(n.y)))));
    return add(a, mul(V(0.10f, 0.06f, 0.12f), fmaxf(-sd, 0)));
}
static v3 locallight(v3 p, v3 n, v3 rdir, float gloss, v3 *spec) {      // lamps, garlands, the open door
    v3 L = V(0, 0, 0); *spec = V(0, 0, 0);
    if (p.z < -66 || p.z > 17 || fabsf(p.x) > 14 || p.y > 9) return L;
    float zi = floorf((p.z + 60) / 8);
    for (int k = 0; k <= 1; k++) for (int s = -1; s <= 1; s += 2) {
        v3 lv = sub(V(2.05f * s, 5.0f, -60 + 8 * clampf(zi + k, 0, 8)), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2), I = 4.4f / (1 + d2);
        L = add(L, mul(V(1.0f, 0.66f, 0.28f), I * fmaxf(dot(n, lv) * il, 0)));
        *spec = add(*spec, mul(V(1.0f, 0.66f, 0.28f), I * powf(fmaxf(dot(rdir, lv) * il, 0), gloss)));
    }
    if (p.z > -61 && p.z < 5) for (int s = -1; s <= 1; s += 2) { float dx = 2.05f * s - p.x, dy = 4.6f - p.y, dl = sqrtf(dx * dx + dy * dy) + 0.3f;
        L = add(L, mul(V(1.0f, 0.5f, 0.25f), 0.8f / dl * fmaxf((n.x * dx + n.y * dy) / dl, 0))); }
    if (p.z > -15 && p.z < 10.5f) L = add(L, mul(V(1.0f, 0.55f, 0.3f), 0.40f * fmaxf(n.y, 0) * sstep(5.0f, 1.2f, p.y) * sstep(2.6f, 1.6f, fabsf(p.x))));
    { v3 lv = sub(V(SUBP.x, SUBP.y + 2.0f, SUBP.z - 4.3f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
      L = add(L, mul(V(1.0f, 0.62f, 0.25f), DOORON * 10.0f / (1 + d2) * sstep(0.0f, 0.5f, lv.z * il) * fmaxf(dot(n, lv) * il, 0))); }
    return L;
}

// ---------------------------------------------------------------- materials
typedef struct { v3 alb, emis; float spec, gloss, sss, env; } mat_t;
static v3 portcol(float ip) { float h = hash2(ip * 3.7f + 1.3f, floorf(T / BEAT) * 1.9f); static const v3 P[3] = {{1.0f, 0.10f, 0.50f}, {0.10f, 0.75f, 1.0f}, {1.0f, 0.58f, 0.08f}};
    return mul(P[(int)(h * 2.999f)], 1.0f + 2.8f * expf(-fract(T / BEAT) * 4.0f)); }
static void material(v3 p, v3 n, int m, float t, mat_t *M) {
    M->alb = V(0.5f, 0.5f, 0.5f); M->emis = V(0, 0, 0); M->spec = 0.15f; M->gloss = 30; M->sss = 0; M->env = 0;
    switch (m) {
    case M_WOOD: { float pl = p.z * 2.4f, h = hash2(floorf(pl), floorf(p.x * 0.5f + 7));
        M->alb = mul(mix3(V(0.20f, 0.12f, 0.07f), V(0.33f, 0.21f, 0.12f), h), 0.5f + 0.5f * sstep(0.0f, 0.06f, 0.5f - fabsf(fract(pl) - 0.5f))); M->spec = 0.4f; M->gloss = 50; break; }
    case M_PILE: M->alb = mul(V(0.10f, 0.07f, 0.05f), 0.5f + 0.5f * sstep(0.0f, 1.2f, p.y)); M->spec = 0.5f; break;
    case M_POLE: M->alb = V(0.03f, 0.035f, 0.04f); M->spec = 0.6f; M->gloss = 80; break;
    case M_LAMP: M->emis = V(9.0f, 5.6f, 2.2f);
        if (p.x < -40 && p.y > 15) { v3 w = nrm(sub(RO, V(-45, 21.4f, -30))); float a = T * 0.9f, f = fmaxf(fabsf(w.x * cosf(a) + w.z * sinf(a)), 0);
            M->emis = mul(V(10, 9, 6.5f), 1 + 40 * powf(f, 300)); }
        break;
    case M_BULB: { float i = roundf(p.z / 0.8f) + roundf(p.x / 0.5f) * 7; v3 qb = subq(p);
        if (fabsf(fabsf(qb.z) - 2.5f) < 0.4f && fabsf(qb.y - 3.55f) < 0.4f) i = roundf(qb.x / 1.5f) * 3 + (qb.z > 0 ? 1 : 0);
        float h = hash2(i, 1.3f);
        v3 c = h < 0.3f ? V(1.0f, 0.75f, 0.35f) : h < 0.55f ? V(1.0f, 0.18f, 0.12f) : h < 0.8f ? V(1.0f, 0.55f, 0.1f) : V(0.25f, 0.6f, 1.0f);
        M->emis = mul(c, 6.0f * (0.75f + 0.25f * sinf(T * 5 + i * 2.4f))); break; }
    case M_BANNER: { float sf = 3.9f / (TR[T_NUIT].w - 80.0f), cap = 67 * sf, uu = p.z < 8.6f ? p.x : -p.x;
        float d = text2d(T_NUIT, uu / sf + TR[T_NUIT].w * 0.5f, 134.0f - (p.y - (5.45f - cap * 0.5f)) / sf) * sf, px = fmaxf(t * 0.004f, 0.012f);
        M->alb = mix3(V(0.36f, 0.04f, 0.09f), V(0.95f, 0.85f, 0.6f), sstep(px, -px, d));
        M->alb = mix3(M->alb, V(0.85f, 0.62f, 0.22f), sstep(0.07f, 0.04f, fminf(2.25f - fabsf(p.x), 0.40f - fabsf(p.y - 5.45f)))); break; }
    case M_KEG: M->alb = fabsf(fract(p.y * 2.2f + 0.1f) - 0.5f) > 0.42f ? V(0.25f, 0.25f, 0.27f) : mix3(V(0.30f, 0.17f, 0.08f), V(0.42f, 0.26f, 0.12f), hash2(floorf(atan2f(p.z + 6.2f, p.x - 1.62f) * 5), 2)); M->spec = 0.3f; break;
    case M_HULL: { v3 q = subq(p);
        float ip = clampf(roundf(q.x / 4.0f), -3, 3), pxl = q.x - 4.0f * ip, rr = len2(pxl, q.y - 0.6f);
        M->alb = q.y < -1.0f ? V(0.32f, 0.035f, 0.075f) : V(0.66f, 0.63f, 0.56f);
        if (q.y > 4.1f && fabsf(q.z) < 1.2f && fabsf(q.x - 1.5f) < 15.6f) M->alb = mul(V(0.30f, 0.19f, 0.11f), 0.6f + 0.4f * sstep(0.0f, 0.08f, 0.5f - fabsf(fract(q.z * 3.0f) - 0.5f)));
        M->alb = mix3(M->alb, V(0.85f, 0.62f, 0.22f), sstep(0.26f, 0.2f, fabsf(q.y + 1.0f)));
        M->alb = mul(M->alb, (1 - 0.3f * sstep(0.07f, 0.02f, fabsf(fract(q.x / 4.0f + 0.5f) - 0.5f) * 4.0f)) * (SHOT == 5 ? 1.0f : 0.65f + 0.35f * sstep(0.2f, 1.6f, p.y)));
        M->spec = 0.7f; M->gloss = 60; M->env = 0.2f;
        if (q.x > 17 && len(V(q.x - 18.69f, q.y - 1.2f, fabsf(q.z) - 1.6f)) < 0.5f) { M->emis = V(9, 8, 5.5f); M->alb = V(0.02f, 0.02f, 0.02f); }
        if (fabsf(q.z) > 3.3f && fabsf(q.y - 0.6f) < 1.2f && rr < 0.76f) {      // a porthole: the light of the party, and the guests dancing against it
            float hsh = hash2(ip * 5.3f + (q.z > 0 ? 3.1f : 0.7f), 2.9f), sil = 0;
            for (int k = 0; k < 2; k++) { if (k == 1 && hsh < 0.4f) break;
                float ph = T / BEAT * PI + k * 1.3f + hsh * 6, bx = (k ? 0.30f : -0.20f) + 0.10f * sinf(ph * 0.5f + hsh * 9), by = -0.42f - 0.10f * k + 0.10f * fabsf(sinf(ph));
                float dx = pxl - bx, dy = q.y - 0.6f - by;
                float dd = fminf(fminf(len2(dx, dy) - 0.25f, len2(fabsf(dx) - 0.18f, dy - 0.25f) - 0.09f), len2(dx * 0.75f, dy + 0.52f) - 0.27f);
                dd = fminf(dd, sdCap(V(dx, dy, 0), V(0.2f, -0.3f, 0), V(0.42f, 0.12f + 0.12f * sinf(ph), 0), 0.055f));
                sil = fmaxf(sil, sstep(0.025f, -0.01f, dd)); }
            M->emis = mul(portcol(ip + (q.z > 0 ? 9 : 0)), (0.8f + 0.8f * sstep(0.76f, 0.1f, rr)) * (1 - 0.9f * sil)); M->alb = V(0.02f, 0.02f, 0.02f); }
        else if (q.z < 0 && q.y > 1.3f && q.y < 3.8f && fabsf(q.x + 11) < 6.5f) {
            float sf = 1.15f / 67.0f, d = text2d(T_GF, (q.x + 11) / sf + TR[T_GF].w * 0.5f, 134.0f - (q.y - 1.9f) / sf) * sf, px = fmaxf(t * 0.003f, 0.01f);
            M->alb = mix3(M->alb, V(0.32f, 0.035f, 0.075f), sstep(px, -px, d));
        } else if (q.z < 0 && fabsf(q.x) < 1.25f && q.y > 0.7f && q.y < 3.4f) {
            if (fabsf(q.x) < 1.05f && q.y < 3.2f) { M->emis = mul(V(7.0f, 4.2f, 1.6f), DOORON); M->alb = mul(M->alb, 1 - 0.85f * DOORON); }
            else M->alb = V(0.85f, 0.62f, 0.22f);
        }
        break; }
    case M_RIB: case M_GOLD: M->alb = V(1.0f, 0.72f, 0.28f); M->env = 1; M->spec = 1.0f; M->gloss = 90; break;
    case M_PENNANT: { int k = (int)(fabsf(roundf((subq(p).x - 3.5f) / 0.87f))) % 3; M->alb = k == 0 ? V(0.45f, 0.04f, 0.09f) : k == 1 ? V(0.9f, 0.62f, 0.14f) : V(0.9f, 0.86f, 0.75f); M->sss = 2; break; }
    case M_SKIN: { v3 l; guest(p, &l); float ax = fabsf(l.x); M->alb = V(0.93f, 0.53f, 0.54f); M->sss = 1; M->spec = 0.22f; M->gloss = 18;
        if (l.y > 0.9f) {
            M->alb = mix3(M->alb, V(1.0f, 0.30f, 0.36f), 0.6f * sstep(0.12f, 0.04f, len(V(ax - 0.215f, l.y - 1.16f, l.z - 0.19f))));        // cheeks
            if (l.z > 0.335f) M->alb = V(0.95f, 0.49f, 0.51f);                                                                          // the flat of the snout
            if (l.z > 0.08f && l.z < 0.3f && ax < 0.125f && fabsf(l.y - (0.955f + 2.0f * l.x * l.x)) < 0.014f) M->alb = V(0.33f, 0.05f, 0.08f);     // a smile
            if (l.y > 1.42f && l.z > 0.0f && ax > 0.12f) M->alb = mix3(M->alb, V(0.98f, 0.40f, 0.45f), sstep(0.0f, 0.03f, l.z - 0.005f) * 0.7f);   // inside of the ears
        }
        break; }
    case M_BLACK: M->alb = V(0.02f, 0.02f, 0.02f); M->spec = 0.9f; M->gloss = 120; break;
    case M_EYEW: M->alb = V(0.95f, 0.95f, 0.92f); M->spec = 0.6f; M->gloss = 90; break;
    case M_HOOF: M->alb = V(0.10f, 0.06f, 0.05f); M->spec = 0.4f; break;
    case M_FOAM: M->alb = V(0.95f, 0.93f, 0.88f); break;
    case M_MUG: M->alb = V(0.95f, 0.55f, 0.08f); M->emis = V(0.35f, 0.16f, 0.02f); M->spec = 0.9f; M->gloss = 80; break;
    case M_OUTFIT: { v3 l; float id = guest(p, &l); int k = (int)(fract(id * 13.7f) * 5.999f); M->alb = PALO[k];
        if (l.z > 0.05f && fabsf(l.x) < 0.065f && l.y > 0.5f && l.y < 0.97f) M->alb = V(0.9f, 0.88f, 0.82f);
        if (l.z > 0.05f && fabsf(l.x) < 0.12f && fabsf(l.y - 0.93f) < 0.04f) M->alb = PALB[(k + 1) % 5];
        if (l.y < 0.47f && fract(id * 29.3f) < 0.3f) M->alb = mix3(M->alb, PALB[(k + 2) % 5], 0.6f);
        M->spec = 0.1f; break; }
    case M_HAT: { v3 l; float id = guest(p, &l), h2 = fract(id * 7.31f); int k = (int)(fract(id * 19.9f) * 4.999f);
        if (h2 < 0.22f) M->alb = fract(l.y * 9) < 0.5f ? PALB[k] : V(0.92f, 0.88f, 0.8f);
        else if (h2 < 0.40f) M->alb = (l.y > 1.50f && l.y < 1.56f) ? V(0.45f, 0.04f, 0.09f) : V(0.025f, 0.025f, 0.03f);
        else M->alb = PALO[k % 4];
        M->spec = 0.25f; break; }
    case M_SCARF: { v3 l; float id = guest(p, &l); M->alb = PALB[(int)(fract(id * 23.3f) * 4.999f)]; break; }
    case M_BALLOON: { v3 l; float id = guest(p, &l); M->alb = PALB[(int)(fract(id * 31.7f) * 4.999f)]; M->sss = 2; M->spec = 1.2f; M->gloss = 120; M->env = 0.35f; break; }
    case M_LAND:
        if (p.z > -66.4f && p.y < 1.7f) { M->alb = mul(V(0.30f, 0.28f, 0.25f), 0.7f + 0.3f * hash2(floorf(p.x * 0.8f + floorf(p.y * 1.6f) * 0.5f), floorf(p.y * 1.6f))); M->alb = mul(M->alb, 0.4f + 0.6f * sstep(0.2f, 1.2f, p.y)); }
        else if (p.z > -71) M->alb = V(0.22f, 0.21f, 0.20f);
        else M->alb = mul(mix3(V(0.07f, 0.11f, 0.05f), V(0.14f, 0.15f, 0.07f), fbm(p.x * 0.1f, p.z * 0.1f)), 1.0f);
        M->spec = 0.05f; break;
    case M_HOUSE: { float c[8], h; house(p.x, p.z, c, &h);
        v3 q = V(p.x - c[0], p.y - c[6], p.z - c[1]); float nx = n.x, nz = n.z;
        if (c[7] > 0.5f) { float t_ = q.x; q.x = q.z; q.z = t_; t_ = nx; nx = nz; nz = t_; }
        if (n.y > 0.3f || q.y > c[4] + 0.02f) { M->alb = fract(h * 3.3f) < 0.7f ? V(0.42f, 0.16f, 0.09f) : V(0.13f, 0.14f, 0.17f); M->alb = mul(M->alb, 0.8f + 0.2f * sstep(0.2f, 0.3f, fract(q.y * 3))); }
        else { int k = (int)(fract(h * 7.7f) * 3.999f); static const v3 W[4] = {{0.78f, 0.70f, 0.55f}, {0.80f, 0.58f, 0.36f}, {0.82f, 0.62f, 0.56f}, {0.80f, 0.78f, 0.72f}};
            M->alb = W[k];
            float u = fabsf(nx) > 0.5f ? q.z : q.x, fu = fract(u / 2.1f + 0.5f), fv = fract((q.y - 0.5f) / 2.7f), wi = floorf(u / 2.1f + 0.5f) + 7 * floorf((q.y - 0.5f) / 2.7f);
            if (fv > 0.25f && fv < 0.75f && q.y < c[4] - 0.3f && q.y > 0.5f) {
                if (fu > 0.32f && fu < 0.68f) { float hw = hash2(wi + h * 50, h * 9); if (hw < 0.55f) M->emis = mul(V(3.4f, 1.9f, 0.7f), 0.5f + hw * 1.6f); M->alb = V(0.03f, 0.04f, 0.06f); M->spec = 0.8f; }
                else if (fu > 0.22f && fu < 0.78f) M->alb = k % 2 ? V(0.30f, 0.05f, 0.08f) : V(0.08f, 0.22f, 0.20f);
            }
        }
        break; }
    case M_STONE: M->alb = mul(V(0.36f, 0.34f, 0.31f), 0.75f + 0.5f * fbm(p.x * 0.7f + p.y, p.z * 0.7f - p.y)); break;
    case M_LHOUSE: M->alb = p.y > 20 ? V(0.10f, 0.10f, 0.12f) : fract(p.y / 4.5f) < 0.5f ? V(0.86f, 0.85f, 0.80f) : V(0.60f, 0.07f, 0.07f); M->spec = 0.3f; break;
    case M_BOAT: { int k = p.x < -11 ? 0 : p.x < 0 ? 1 : p.x < 12 ? 2 : 3; static const v3 B[4] = {{0.75f, 0.72f, 0.65f}, {0.05f, 0.35f, 0.38f}, {0.45f, 0.05f, 0.09f}, {0.70f, 0.50f, 0.10f}};
        M->alb = n.y > 0.6f ? V(0.28f, 0.18f, 0.10f) : B[k]; M->spec = 0.5f; break; }
    default: break;
    }
}

// ---------------------------------------------------------------- shading above the water
static float LBT; static int DBG;
static float linebeam(v3 ro, v3 rd, float tmax, v3 o, v3 b, float w0, float sp, float fall, float smax) {     // glow of a cone of light seen from the side
    v3 w = sub(ro, o); float bb = dot(rd, b), d1 = dot(rd, w), d2 = dot(b, w), den = 1 - bb * bb; if (den < 1e-4f) return 0;
    float t = (bb * d2 - d1) / den, s = (d2 - bb * d1) / den;
    if (t < 0) { t = 0; s = d2; }          // the closest approach is behind the eye: what is left is the glow around the eye itself
    LBT = t;
    if (t > tmax || s < 0.3f || s > smax) return 0;
    v3 c = sub(add(ro, mul(rd, t)), add(o, mul(b, s))); float wd = w0 + sp * s;
    return expf(-dot(c, c) / (wd * wd)) * expf(-s * fall) / (0.4f + sqrtf(den)) * sstep(0.3f, 1.5f, s) * sstep(smax, smax * 0.75f, s);
}
static v3 beam(v3 ro, v3 rd, float tmax) {
    v3 acc = V(0, 0, 0); float a = T * 0.9f;
    for (int k = 0; k < 2; k++) { v3 b = nrm(V(cosf(a + k * PI), 0.05f, sinf(a + k * PI)));
        acc = add(acc, mul(V(1.0f, 0.92f, 0.7f), 0.30f * linebeam(ro, rd, tmax, V(-45, 21.4f, -30), b, 0.5f, 0.045f, 0.012f, 400))); }
    if (BEAMS > 0) { v3 o = subw(V(3.5f, 8.3f, 0)); if (o.y > 0.5f) for (int k = 0; k < 2; k++) {
        float th = 0.55f + 0.3f * sinf(T * 0.8f + k * 2.3f), ph = (k ? -0.6f : 0.75f) * T + k * 2.1f;
        v3 b = V(cosf(ph) * sinf(th), cosf(th), sinf(ph) * sinf(th));
        acc = add(acc, mul(k ? V(1.0f, 0.30f, 0.55f) : V(1.0f, 0.78f, 0.35f), BEAMS * 0.45f * linebeam(ro, rd, tmax, o, b, 0.35f, 0.03f, 0.008f, 260))); } }
    return acc;
}
static int ell_hit(v3 ro, v3 rd, v3 c, v3 r, float *t0, float *t1) {
    v3 o = V((ro.x - c.x) / r.x, (ro.y - c.y) / r.y, (ro.z - c.z) / r.z), d = V(rd.x / r.x, rd.y / r.y, rd.z / r.z);
    float a = dot(d, d), b = dot(o, d), h = b * b - a * (dot(o, o) - 1); if (h < 0) return 0;
    h = sqrtf(h); *t0 = (-b - h) / a; *t1 = (-b + h) / a; return 1;
}
static float cloud(v3 ro, v3 rd, float tmax, v3 c, v3 r, float rise, float jit) {      // optical depth of a cloud of spray, or of bubbles
    float t0, t1; if (!ell_hit(ro, rd, c, r, &t0, &t1)) return 0;
    t0 = fmaxf(t0, 0); t1 = fminf(t1, tmax); if (t1 <= t0) return 0;
    float dt = (t1 - t0) / 8, acc = 0;
    for (int i = 0; i < 8; i++) { v3 p = add(ro, mul(rd, t0 + (i + jit) * dt));
        v3 e = V((p.x - c.x) / r.x, (p.y - c.y) / r.y, (p.z - c.z) / r.z); float fall = 1 - dot(e, e); if (fall <= 0) continue;
        float n = fbm(p.x * 0.55f + p.z * 0.4f, (p.y - T * rise) * 0.5f + p.z * 0.3f);
        acc += fall * sstep(0.38f, 0.8f, n + 0.25f * fall) * dt; }
    return acc;
}
static v3 shade_solid(v3 ro, v3 rd, float t, int m, int depth) {
    v3 p = add(ro, mul(rd, t)), n = normal(p, t), ps = add(p, mul(n, 0.02f));
    mat_t M; material(p, n, m, t, &M);
    float ndl = dot(n, SUN), w = M.sss > 0 ? 0.4f : 0.0f, dif = fmaxf((ndl + w) / (1 + w), 0);
    float sh = dif > 0 ? (depth == 0 ? shadow(ps, SUN) : 0.6f) : 0, ao = depth == 0 ? ambocc(p, n) : 0.8f; if (DBG == 1) sh = 1; if (DBG == 2) ao = 1;
    v3 rdir = refl(rd, n), spec;
    v3 L = add(add(mul(SUNC, dif * sh), mul(skyamb(n), ao)), mul(locallight(p, n, rdir, M.gloss, &spec), 0.35f + 0.65f * ao));
    float f5 = powf(1 - fmaxf(-dot(rd, n), 0), 5);
    if (m >= M_SKIN && m <= M_BALLOON && depth == 0) L = add(L, mul(V(0.95f, 0.62f, 0.50f), 0.30f * fmaxf(-dot(rd, n), 0) * (0.5f + 0.5f * ao)));      // a little fill from the camera, for the faces
    v3 col;
    if (M.env > 0.9f) col = add(mulv(M.alb, add(mul(skygrad(V(rdir.x, fabsf(rdir.y), rdir.z)), 0.9f), mul(L, 0.35f))), M.emis);
    else {
        col = add(mulv(M.alb, L), M.emis);
        if (M.env > 0) col = add(col, mul(skygrad(V(rdir.x, fabsf(rdir.y), rdir.z)), M.env * (0.2f + 0.8f * f5) * (0.4f + 0.6f * ao)));
    }
    col = add(col, mul(add(mul(SUNC, powf(fmaxf(dot(rdir, SUN), 0), M.gloss) * sh), spec), M.spec));
    col = add(col, mul(mulv(M.alb, skyamb(mul(rd, -0.3f))), 0.0f));
    col = add(col, mul(mulv(M.alb, skygrad(V(n.x, fabsf(n.y) * 0.3f, n.z))), 0.45f * f5 * ao));                 // rim from the bright horizon
    if (M.sss > 0) {        // the low sun through an ear, a balloon, a pennant
        float back = powf(fmaxf(dot(rd, SUN), 0), 2) * fmaxf(0.35f - ndl * 0.65f, 0) * (depth == 0 ? 1.0f : 0.6f);
        col = add(col, mul(mulv(M.sss > 1.5f ? M.alb : V(1.0f, 0.22f, 0.16f), SUNC), back * (M.sss > 1.5f ? 1.0f : 0.55f)));
    }
    float fg = 1 - expf(-t / 420.0f);
    return mix3(col, skygrad(V(rd.x, fmaxf(rd.y, 0.02f), rd.z)), fg);
}
static float foam(v3 p) {
    int m; float ds = subm(V(p.x, 0.25f, p.z), &m), nz = fbm(p.x * 2.3f + T * 0.5f, p.z * 2.3f - T * 0.3f);
    float fo = sstep(1.2f, 0.1f, ds) * sstep(0.25f, 0.6f, nz + 0.25f) * (SUBP.y > -3.5f);
    if (fabsf(p.x) < 3 && p.z > -64 && p.z < 10.5f) { float zc = roundf((p.z + 62) / 4) * 4 - 62; fo = fmaxf(fo, sstep(0.75f, 0.2f, len2(fabsf(p.x) - 1.9f, p.z - zc)) * sstep(0.3f, 0.6f, nz + 0.2f)); }
    if (p.z > -66 && p.z < -64) fo = fmaxf(fo, sstep(1.6f, 0.2f, p.z + 66) * sstep(0.35f, 0.6f, nz + 0.1f));
    if (SHOT == 4) {        // the wake, then the boil where it goes under
        float ax = SUBP.x, az = SUBP.z - 14, L2 = ax * ax + az * az + 1e-3f, u = clampf((p.x * ax + (p.z - 14) * az) / L2, 0, 1), dw = len2(p.x - ax * u, p.z - 14 - az * u);
        fo = fmaxf(fo, sstep(4.5f, 1.0f, dw) * sstep(0.45f, 0.7f, nz + 0.15f * u) * 0.8f);
        float ks = clampf((0.4f - SUBP.y) / 9, 0, 1), er = len2((p.x - SUBP.x + 3) / 24.0f, (p.z - SUBP.z) / 6.5f);
        fo = fmaxf(fo, sstep(1.05f, 0.5f, er) * sstep(0.03f, 0.4f, ks) * (1 - 0.55f * sstep(0.85f, 1.0f, ks)) * sstep(0.15f, 0.5f, nz + 0.3f * fbm(p.x * 5 + T * 3, p.z * 5)));
    }
    return clampf(fo, 0, 1) * 0.92f;
}
static v3 shade_ocean(v3 ro, v3 rd, float t) {
    v3 p = add(ro, mul(rd, t)); float dx, dz; wave(p.x, p.z, &dx, &dz);
    float fd = 1 / (1 + t * 0.012f);
    float rx = (fbm(p.x * 1.9f + T * 0.7f, p.z * 1.9f) - 0.47f) * 0.6f * fd + (vnoise(p.x * 7 + T, p.z * 7) - 0.5f) * 0.22f * fd * fd;
    float rz = (fbm(p.x * 1.9f + 31, p.z * 1.9f - T * 0.6f) - 0.47f) * 0.6f * fd + (vnoise(p.x * 7 + 9, p.z * 7 - T) - 0.5f) * 0.22f * fd * fd;
    v3 n = nrm(V(-dx - rx, 1, -dz - rz));
    float f = 0.02f + 0.98f * powf(1 - fmaxf(-dot(rd, n), 0), 5);
    v3 rdir = refl(rd, n); if (rdir.y < 0.03f) { rdir.y = 0.03f; rdir = nrm(rdir); }
    v3 ps = add(p, V(0, 0.05f, 0)); int mr; float tr = trace(ps, rdir, &mr, 140.0f, 56);
    v3 rc = tr > 0 ? shade_solid(ps, rdir, tr, mr, 1) : psky(rdir);
    rc = add(rc, beam(ps, rdir, tr > 0 ? tr : 1e9f));
    float sl = sqrtf(SUN.x * SUN.x + SUN.z * SUN.z), hx = SUN.x / sl, hz = SUN.z / sl;
    v3 deep = add(V(0.006f, 0.030f, 0.045f), mul(V(0.05f, 0.22f, 0.20f), 1.5f * powf(fmaxf(rd.x * hx + rd.z * hz, 0), 3) * fmaxf(-dx * hx - dz * hz + 0.15f, 0)));
    v3 spc, ll = locallight(p, n, rdir, 200, &spc);
    v3 col = add(mix3(deep, rc, f), add(mulv(V(0.02f, 0.05f, 0.06f), ll), mul(spc, 0.5f)));
    float fo = t < 200 ? foam(p) : 0;
    if (fo > 0) col = mix3(col, mulv(V(0.85f, 0.88f, 0.9f), add(add(mul(SUNC, fmaxf(dot(n, SUN), 0) + 0.30f), mul(skyamb(V(0, 1, 0)), 2.2f)), mul(ll, 0.7f))), fo);
    return mix3(col, skygrad(V(rd.x, 0.02f, rd.z)), 1 - expf(-t / 420.0f));
}

// ---------------------------------------------------------------- under the water
static v3 wfog(float y) { float k = expf(fminf(y, 0) * 0.075f); return V(0.004f + 0.048f * k, 0.014f + 0.45f * k, 0.032f + 0.52f * k); }
static float caus(float x, float z) {
    float a = sinf(x * 0.9f + T * 0.8f + 1.3f * sinf(z * 0.7f - T * 0.5f)), b = sinf(z * 1.1f - T * 0.7f + 1.1f * sinf(x * 0.8f + T * 0.6f));
    float c = clampf(0.5f + 0.5f * a * b + 0.25f * sinf((x + z) * 1.7f + T), 0, 1); return c * c * c * 2.2f;
}
static float msnow(v3 ro, v3 rd, float tmax) {     // specks adrift
    const float c = 1.4f; float acc = 0, t = 0.3f;
    v3 o = add(ro, V(T * 0.12f, T * 0.3f, 0)); tmax = fminf(tmax, 14.0f);
    for (int i = 0; i < 22 && t < tmax; i++) {
        v3 q = add(o, mul(rd, t)); float ix = floorf(q.x / c), iy = floorf(q.y / c), iz = floorf(q.z / c), h = hash2(ix + iy * 57.3f, iz + iy * 13.7f);
        v3 oc = sub(V((ix + 0.2f + 0.6f * fract(h * 17.3f)) * c, (iy + 0.2f + 0.6f * fract(h * 39.1f)) * c, (iz + 0.2f + 0.6f * fract(h * 71.7f)) * c), o);
        float tc = dot(oc, rd);
        if (tc > 0.25f && tc < tmax) { float d2 = dot(oc, oc) - tc * tc, R = 0.012f + 0.014f * h + tc * 0.0022f; acc += sstep(R * R, R * R * 0.2f, d2) * (1 - tc / 14.0f) * 0.7f; }
        float tx = rd.x > 1e-5f ? ((ix + 1) * c - q.x) / rd.x : rd.x < -1e-5f ? (ix * c - q.x) / rd.x : 1e9f;
        float ty = rd.y > 1e-5f ? ((iy + 1) * c - q.y) / rd.y : rd.y < -1e-5f ? (iy * c - q.y) / rd.y : 1e9f;
        float tz = rd.z > 1e-5f ? ((iz + 1) * c - q.z) / rd.z : rd.z < -1e-5f ? (iz * c - q.z) / rd.z : 1e9f;
        t += fminf(tx, fminf(ty, tz)) + 0.01f;
    }
    return fminf(acc, 1);
}
static v3 shade_deep(v3 ro, v3 rd, float *zout) {
    int m; float t = trace(ro, rd, &m, 110.0f, 160), tsurf = rd.y > 1e-3f ? -ro.y / rd.y : 1e9f, tt;
    v3 LW = nrm(V(SUN.x * 0.74f, 0.67f, SUN.z * 0.74f)), col;
    const float FD = 0.030f;
    if (t > 0 && t < tsurf) {
        tt = t; v3 p = add(ro, mul(rd, t)), n = normal(p, t);
        float ndv = fmaxf(-dot(rd, n), 0);
        if (m == M_BUB) {       // a bubble is a little mirror: bright where it shows the surface, dark underneath, a bright rim
            float rim = powf(1 - ndv, 2), up = sstep(-0.35f, 0.75f, n.y);
            col = add(mix3(mul(wfog(p.y - 8), 0.7f), mul(V(0.70f, 0.95f, 1.0f), 0.5f + 1.6f * expf(p.y * 0.03f)), up), mul(V(0.8f, 1.0f, 1.0f), 0.9f * rim * expf(p.y * 0.02f)));
        } else {
            mat_t M; material(p, n, m, t, &M);
            float s = -p.y / LW.y, cz = caus(p.x + LW.x * s, p.z + LW.z * s), sunw = fmaxf(dot(n, LW), 0) * expf(p.y * 0.042f);
            v3 L = add(mul(wfog(p.y), 1.2f * (0.55f + 0.45f * n.y)), mul(V(0.55f, 1.0f, 0.95f), 3.0f * sunw * (0.30f + 1.2f * cz)));
            v3 q = subq(p); float ip = clampf(roundf(q.x / 4.0f), -3, 3), rr = len2(q.x - 4 * ip, q.y - 0.6f);
            if (fabsf(q.z) > 2.5f) L = add(L, mul(portcol(ip + (q.z > 0 ? 9 : 0)), 0.55f * expf(-rr * 0.7f)));
            { float xb = clampf(roundf(q.x / 1.5f) * 1.5f, -13.5f, 15.0f), db = len(V(q.x - xb, q.y - 3.55f, fabsf(q.z) - 2.5f)); L = add(L, mul(V(1.0f, 0.6f, 0.25f), 0.5f / (0.3f + db * db * 3))); }
            col = add(mulv(M.alb, L), M.emis);
            if (M.env > 0.9f) col = add(col, mulv(M.alb, mul(wfog(p.y + 8 * refl(rd, n).y), 1.4f)));
            col = add(col, mul(wfog(p.y + 12), 0.5f * powf(1 - ndv, 3)));                       // the water lights the edges
        }
    } else if (tsurf < 110) {       // the surface from below: a window on the sky, a mirror beyond
        tt = tsurf; v3 p = add(ro, mul(rd, tt)); float dx, dz; wave(p.x, p.z, &dx, &dz);
        float rx = (fbm(p.x * 1.4f + T * 0.7f, p.z * 1.4f) - 0.47f) * 0.9f, rz = (fbm(p.x * 1.4f + 31, p.z * 1.4f - T * 0.6f) - 0.47f) * 0.9f;
        v3 n = nrm(V(dx + rx, -1, dz + rz)), rr;
        if (refr(rd, n, 1.33f, &rr) && rr.y > 0.01f) col = mul(psky(rr), 0.9f); else col = mul(wfog(-7), 0.5f + 0.5f * fbm(p.x * 0.6f, p.z * 0.6f + T * 0.3f));
    } else { tt = 110; col = wfog(ro.y + rd.y * 22); }
    float fg = 1 - expf(-tt * FD);
    col = mix3(mulv(col, V(expf(-tt * 0.04f), expf(-tt * 0.010f), expf(-tt * 0.007f))), wfog(ro.y + rd.y * fminf(tt, 30) * 0.6f), fg);
    float jit = hash2(rd.x * 913.7f + T, rd.y * 517.3f);
    {   // shafts of light from the surface
        float acc = 0, dt = fminf(tt, 60.0f) / 16;
        for (int i = 0; i < 16; i++) { v3 q = add(ro, mul(rd, (i + jit) * dt)); if (q.y > -0.2f) continue; float s = -q.y / LW.y;
            acc += caus(q.x + LW.x * s, q.z + LW.z * s) * expf(q.y * 0.075f) * expf(-(i + jit) * dt * 0.025f); }
        col = add(col, mul(V(0.35f, 0.8f, 0.75f), acc * dt * 0.024f));
    }
    {   // what the boat throws into the water: the light of every porthole, the two lamps at the bow, and the cloud of bubbles it drags down
        v3 o = subq(ro), d = subd(rd), acc = V(0, 0, 0);
        for (int sd = -1; sd <= 1; sd += 2) {
            if ((sd > 0) != (o.z > 0) && fabsf(o.z) > 4.2f && tt > 0 && t > 0 && t < tsurf && fabsf(o.z) - fabsf(d.z) * tt > 0) continue;      // the far side is behind the hull
            for (int ip = -3; ip <= 3; ip++) { float g = linebeam(o, d, tt, V(4.0f * ip, 0.6f, sd * 4.1f), V(0, 0, sd), 0.62f, 0.16f, 0.16f, 16.0f);
                if (g > 0.001f) acc = add(acc, mul(portcol(ip + (sd > 0 ? 9 : 0)), g * 0.34f * expf(-LBT * FD))); }
            { v3 b = nrm(V(1, -0.10f, sd * 0.13f)); float g = linebeam(o, d, tt, V(18.8f, 1.2f, sd * 1.6f), b, 0.5f, 0.10f, 0.045f, 60.0f);
              acc = add(acc, mul(V(1.0f, 0.93f, 0.72f), g * 1.1f * expf(-fmaxf(LBT, 0) * FD))); }
        }
        col = add(col, acc);
        float sdive = fmaxf(T - 30, 0), od = cloud(ro, rd, tt, add(SUBP, V(-4, 8, 0)), V(29, 13, 8.5f), 3.2f, jit) * (0.012f + 0.10f * expf(-sdive * 0.16f));
        col = mix3(col, mul(V(0.62f, 0.95f, 1.0f), 0.22f + 1.5f * expf(SUBP.y * 0.045f)), 1 - expf(-od));
    }
    col = add(col, mul(V(0.6f, 0.85f, 0.85f), msnow(ro, rd, tt) * 0.5f * (0.25f + expf(ro.y * 0.05f))));
    *zout = tt; return col;
}
static v3 shade(v3 ro, v3 rd, float *zout) {
    if (SHOT == 5) return shade_deep(ro, rd, zout);
    int m; float ts = trace(ro, rd, &m, 760.0f, 190), to = ocean_hit(ro, rd, ts > 0 ? ts : 1e9f), t; v3 col;
    if (to > 0) { t = to; col = shade_ocean(ro, rd, to); }
    else if (ts > 0) { t = ts; col = shade_solid(ro, rd, ts, m, 0); }
    else { t = 1e4f; col = psky(rd); }
    col = add(col, beam(ro, rd, t));
    if (SHOT == 4) { float ks = clampf((0.4f - SUBP.y) / 10, 0, 1), amp = sstep(0.02f, 0.25f, ks) * (1 - sstep(0.7f, 1.0f, ks));
        if (amp > 0.01f) { float od = cloud(ro, rd, t, V(SUBP.x - 2, 0.6f + 1.5f * amp, SUBP.z), V(27, 2.5f + 3.5f * amp, 7.5f), 1.6f, hash2(rd.x * 811.3f + T, rd.y * 433.7f)) * amp * 0.32f;
            v3 sc = add(mul(SUNC, 0.22f + 1.3f * powf(fmaxf(dot(rd, SUN), 0), 5)), mul(skyamb(V(0, 1, 0)), 2.5f));
            col = mix3(col, sc, 1 - expf(-od)); } }
    *zout = t; return col;
}

// ---------------------------------------------------------------- the shots
static void setup(float tb) {
    T = tb; APERT = 0; FOCUS = 10; EXPO = 1.0f; NOPIGS = 0; DOORON = 1; QEND = 9.4f; SUBP = V(0, 0.4f, 14); PC = 1; PS = 0; GAP0 = 1; GAP1 = 0; BEAMS = 1;
    SUN = nrm(V(-0.8f, 0.07f, 1)); SUNC = V(3.2f, 1.25f, 0.42f); MOON = nrm(V(0.48f, 0.21f, -0.86f));
    if (tb < 8) { SHOT = 1; float u = ease(tb / 8) * 0.5f + tb / 16; cam(mix3(V(-58, 21, 52), V(-45, 12, 40), u), mix3(V(4, 4.5f, -38), V(2, 3.5f, -34), u), 1.65f); }
    else if (tb < 15) { SHOT = 2; float s = tb - 8; GAP0 = -60.5f; GAP1 = -56.5f;
        cam(V(-0.14f + 0.04f * s, 2.30f + 0.02f * s, -50 + s), V(0.10f, 2.26f, -70 + s), 2.1f); FOCUS = 3.3f; APERT = 0.035f; }
    else if (tb < 21) { SHOT = 3; float s = (tb - 15) / 6; cam(V(-3.6f + 0.6f * s, 8.9f - 0.7f * s, 13.2f), V(0.3f, 1.6f, 5.2f + 1.2f * s), 1.3f); FOCUS = 8.5f; APERT = 0.012f; EXPO = 0.92f; }
    else if (tb < 30) { SHOT = 4; float s = tb - 21, u = s / 9, k = ease((s - 1.5f) / 7.0f), a = 0.15f * sstep(0.8f, 4.5f, s);
        SUBP = V(15 * u * u, 0.4f - 11.5f * k, 14 + 3 * u * u); PC = cosf(a); PS = sinf(a); DOORON = 0; NOPIGS = 1;
        cam(V(30 - 3 * u, 2.4f + 2.4f * u, -22), V(-3 + 7 * u, 2.2f - 1.2f * u, 14), 1.4f); }
    else { SHOT = 5; float s = tb - 30, a = 0.15f - 0.07f * sstep(0, 18, s); SUBP = V(2 + 1.2f * s, -9.5f - 2.0f * s, 14); PC = cosf(a); PS = sinf(a); DOORON = 0; NOPIGS = 1; EXPO = 1.2f;
        if (tb < 39.375f) cam(V(-11 + 0.95f * s, -21.5f - 1.15f * s, -5 + 0.2f * s), add(SUBP, V(-2, 1, 0)), 1.3f);
        else { float u = tb - 39.375f; cam(subw(V(18.5f - 0.30f * u, 1.9f + 0.10f * u, -10.5f + 0.15f * u)), subw(V(3.0f - 0.2f * u, 0.7f, -4.2f)), 1.45f); } }       // alongside, at the height of the portholes
}

// ---------------------------------------------------------------- the picture: adaptive antialiasing, depth of field, bloom, film curve
#define W 576
#define HF 288
static float *HDR, *ZB;
static const float JX[4] = {0.125f, -0.375f, 0.375f, -0.125f}, JY[4] = {0.375f, 0.125f, -0.125f, -0.375f};
static v3 sample(int x, int j, int par, int s, float *z) {
    float px = (2 * (x + 0.5f + JX[s & 3]) - W) / W * (4.0f / 3.0f), py = (576 - 2 * (2 * j + par + 0.5f + JY[s & 3] * 2)) / 576.0f;
    return shade(RO, nrm(add(add(mul(CU, px), mul(CV, py)), mul(CW, FOV))), z);
}
S float lum(const float *c) { return 0.3f * c[0] + 0.6f * c[1] + 0.1f * c[2]; }
static void boxblur(float *a, float *tmp, int w, int h, int r) {
    for (int pass = 0; pass < 2; pass++) {
        #pragma omp parallel for
        for (int y = 0; y < h; y++) for (int c = 0; c < 3; c++) { float s = 0; int n = 0;
            for (int x = -r; x < w; x++) { if (x + r < w) { s += a[(y * w + x + r) * 3 + c]; n++; } if (x - r - 1 >= 0) { s -= a[(y * w + x - r - 1) * 3 + c]; n--; } if (x >= 0) tmp[(y * w + x) * 3 + c] = s / n; } }
        #pragma omp parallel for
        for (int x = 0; x < w; x++) for (int c = 0; c < 3; c++) { float s = 0; int n = 0;
            for (int y = -r; y < h; y++) { if (y + r < h) { s += tmp[((y + r) * w + x) * 3 + c]; n++; } if (y - r - 1 >= 0) { s -= tmp[((y - r - 1) * w + x) * 3 + c]; n--; } if (y >= 0) a[(y * w + x) * 3 + c] = s / n; } }
    }
}
static void post(int fi, unsigned char *out) {
    static float *tmp, *b1, *b2, *bt; static double *sat;
    if (!tmp) { tmp = malloc(sizeof(float) * W * HF * 3); b1 = malloc(sizeof(float) * (W / 4) * (HF / 2) * 3); b2 = malloc(sizeof(float) * (W / 4) * (HF / 2) * 3); bt = malloc(sizeof(float) * (W / 4) * (HF / 2) * 3); sat = malloc(sizeof(double) * (W + 1) * (HF + 1) * 3); }
    if (APERT > 0) {        // depth of field: box whose size follows the circle of confusion
        for (int i = 0; i < (W + 1) * 3; i++) sat[i] = 0;
        for (int y = 0; y < HF; y++) { double r[3] = {0, 0, 0}; for (int c = 0; c < 3; c++) sat[((y + 1) * (W + 1)) * 3 + c] = 0;
            for (int x = 0; x < W; x++) for (int c = 0; c < 3; c++) { r[c] += HDR[(y * W + x) * 3 + c]; sat[((y + 1) * (W + 1) + x + 1) * 3 + c] = sat[(y * (W + 1) + x + 1) * 3 + c] + r[c]; } }
        #pragma omp parallel for
        for (int y = 0; y < HF; y++) for (int x = 0; x < W; x++) {
            float r = fminf(APERT * fabsf(1 / ZB[y * W + x] - 1 / FOCUS) * W, 13.0f);
            if (r < 0.7f) { for (int c = 0; c < 3; c++) tmp[(y * W + x) * 3 + c] = HDR[(y * W + x) * 3 + c]; continue; }
            int rx = (int)(r + 0.5f), ry = (int)(r * 0.667f + 0.5f), x0 = x - rx < 0 ? 0 : x - rx, x1 = x + rx + 1 > W ? W : x + rx + 1, y0 = y - ry < 0 ? 0 : y - ry, y1 = y + ry + 1 > HF ? HF : y + ry + 1;
            double inv = 1.0 / ((x1 - x0) * (y1 - y0));
            for (int c = 0; c < 3; c++) tmp[(y * W + x) * 3 + c] = (float)((sat[(y1 * (W + 1) + x1) * 3 + c] - sat[(y0 * (W + 1) + x1) * 3 + c] - sat[(y1 * (W + 1) + x0) * 3 + c] + sat[(y0 * (W + 1) + x0) * 3 + c]) * inv);
        }
        memcpy(HDR, tmp, sizeof(float) * W * HF * 3);
    }
    int bw = W / 4, bh = HF / 2;        // bloom: what is brighter than white bleeds
    for (int y = 0; y < bh; y++) for (int x = 0; x < bw; x++) for (int c = 0; c < 3; c++) { float s = 0;
        for (int dy = 0; dy < 2; dy++) for (int dx = 0; dx < 4; dx++) { const float *q = HDR + ((y * 2 + dy) * W + x * 4 + dx) * 3; float l = lum(q); s += q[c] * fmaxf(l - 1.1f, 0) / (l + 1e-3f); }
        b1[(y * bw + x) * 3 + c] = fminf(s / 8, 40.0f); }
    boxblur(b1, bt, bw, bh, 2); memcpy(b2, b1, sizeof(float) * bw * bh * 3); boxblur(b2, bt, bw, bh, 7);
    #pragma omp parallel for
    for (int y = 0; y < HF; y++) for (int x = 0; x < W; x++) {
        float fx = (x + 0.5f) / 4 - 0.5f, fy = (y + 0.5f) / 2 - 0.5f; int ix = (int)floorf(fx), iy = (int)floorf(fy); fx -= ix; fy -= iy;
        int xa = ix < 0 ? 0 : ix, xb = ix + 1 > bw - 1 ? bw - 1 : ix + 1, ya = iy < 0 ? 0 : iy, yb = iy + 1 > bh - 1 ? bh - 1 : iy + 1;
        float vx = (2 * (x + 0.5f) - W) / W, vy = (2 * (y + 0.5f) - HF) / HF, vig = 1 - 0.22f * (vx * vx + vy * vy * 0.6f);
        float d = (hash2(x + fi * 0.37f, y + fi * 0.11f) - 0.5f) / 255.0f;
        for (int c = 0; c < 3; c++) {
            float g1 = (b1[(ya * bw + xa) * 3 + c] * (1 - fx) + b1[(ya * bw + xb) * 3 + c] * fx) * (1 - fy) + (b1[(yb * bw + xa) * 3 + c] * (1 - fx) + b1[(yb * bw + xb) * 3 + c] * fx) * fy;
            float g2 = (b2[(ya * bw + xa) * 3 + c] * (1 - fx) + b2[(ya * bw + xb) * 3 + c] * fx) * (1 - fy) + (b2[(yb * bw + xa) * 3 + c] * (1 - fx) + b2[(yb * bw + xb) * 3 + c] * fx) * fy;
            float v = fmaxf((HDR[(y * W + x) * 3 + c] + 0.55f * g1 + 0.40f * g2) * EXPO * vig, 0);
            v = clampf((v * (2.51f * v + 0.03f)) / (v * (2.43f * v + 0.59f) + 0.14f), 0, 1);
            out[(y * W + x) * 3 + c] = (unsigned char)(clampf(powf(v, 0.4545f) + d, 0, 1) * 255);
        }
    }
}
#include <unistd.h>
static int badf(float v) { unsigned u; memcpy(&u, &v, 4); return (u & 0x7f800000u) == 0x7f800000u; }       // NaN ou infini (test sur les bits : -ffast-math efface isfinite)
static FILE *open_data(const char *argv0, const char *name) {       // (22 nov. 2026) txt.bin : dossier courant, puis $PORK_DIR, puis le dossier du binaire
    FILE *f = fopen(name, "rb"); if (f) return f;
    char path[4096]; const char *e = getenv("PORK_DIR");
    if (e) { snprintf(path, sizeof path, "%s/%s", e, name); if ((f = fopen(path, "rb"))) return f; }
    ssize_t n = readlink("/proc/self/exe", path, sizeof path - 1);
    if (n <= 0) { snprintf(path, sizeof path, "%s", argv0); n = (ssize_t)strlen(path); } else path[n] = 0;
    char *sl = strrchr(path, '/'); if (sl && (size_t)(sl - path) + strlen(name) + 2 < sizeof path) { strcpy(sl + 1, name); f = fopen(path, "rb"); }
    return f;
}
int main(int argc, char **argv) {       // film F0 F1 [T0] > fields.rgb       (field f is at time f / 50 + T0; even fields carry the even lines)
    if (getenv("DBG")) DBG = atoi(getenv("DBG"));
    if (argc < 3) { fprintf(stderr, "usage : %s F0 F1 [T0] [noaa] > trames.rgb   (trames F0 à F1-1, 576 x 288 rgb24, 50 par seconde ; T0 choisit la séquence, voir README)\n", argv[0]); return 2; }
    char *e1, *e2, *e3 = 0; long lf0 = strtol(argv[1], &e1, 10), lf1 = strtol(argv[2], &e2, 10); double dt0 = argc > 3 ? strtod(argv[3], &e3) : 0;
    if (*e1 || *e2 || (argc > 3 && *e3) || dt0 != dt0 || dt0 > 1e6 || dt0 < -1e6 || lf0 < -1000000 || lf1 > 100000000) { fprintf(stderr, "%s : arguments illisibles (F0 F1 entiers, T0 nombre)\n", argv[0]); return 2; }
    int f0 = (int)lf0, f1 = (int)lf1; float t0 = (float)dt0; int noaa = argc > 4, zstat = getenv("ZSTAT") != 0;
    TX = malloc(sizeof(float) * TXW * TXH);
    FILE *f = open_data(argv[0], "txt.bin"); if (!f || fread(TX, sizeof(float), TXW * TXH, f) != TXW * TXH) { fprintf(stderr, "txt.bin introuvable ou incomplet : lancer python3 tex.py dans le dossier du moteur (ou PORK_DIR=...)\n"); return 1; } fclose(f);
    HDR = malloc(sizeof(float) * W * HF * 3); ZB = malloc(sizeof(float) * W * HF);
    unsigned char *out = malloc(W * HF * 3), *mask = malloc(W * HF);
    for (int fi = f0; fi < f1; fi++) {
        setup(fi / 50.0f + t0); int par = fi & 1;
        #pragma omp parallel for schedule(dynamic, 2)
        for (int j = 0; j < HF; j++) for (int x = 0; x < W; x++) { v3 c = sample(x, j, par, fi, ZB + j * W + x); float *q = HDR + (j * W + x) * 3; q[0] = c.x; q[1] = c.y; q[2] = c.z; }
        if (!noaa) {        // more rays where neighbours disagree
            for (int j = 0; j < HF; j++) for (int x = 0; x < W; x++) { float l = lum(HDR + (j * W + x) * 3), mx = 0;
                if (x > 0) mx = fmaxf(mx, fabsf(l - lum(HDR + (j * W + x - 1) * 3))); if (x < W - 1) mx = fmaxf(mx, fabsf(l - lum(HDR + (j * W + x + 1) * 3)));
                if (j > 0) mx = fmaxf(mx, fabsf(l - lum(HDR + ((j - 1) * W + x) * 3))); if (j < HF - 1) mx = fmaxf(mx, fabsf(l - lum(HDR + ((j + 1) * W + x) * 3)));
                mask[j * W + x] = mx > 0.10f + 0.22f * l; }
            #pragma omp parallel for schedule(dynamic, 2)
            for (int j = 0; j < HF; j++) for (int x = 0; x < W; x++) if (mask[j * W + x]) { float z; float *q = HDR + (j * W + x) * 3;
                v3 a = sample(x, j, par, fi + 1, &z), b = sample(x, j, par, fi + 2, &z);
                q[0] = (fminf(q[0], 60) + fminf(a.x, 60) + fminf(b.x, 60)) / 3; q[1] = (fminf(q[1], 60) + fminf(a.y, 60) + fminf(b.y, 60)) / 3; q[2] = (fminf(q[2], 60) + fminf(a.z, 60) + fminf(b.z, 60)) / 3; }
        }
        if (zstat) { int nz = 0, sk = 0; for (int i = 0; i < W * HF; i++) { if (ZB[i] < 0.12f) nz++; if (ZB[i] > 9e3f) sk++; }       // pour qc.py : caméra dans un objet, ciel
            fprintf(stderr, "ZSTAT %d %.4f %.4f\n", fi, nz / (float)(W * HF), sk / (float)(W * HF)); }
        { long nb = 0; for (int i = 0; i < W * HF * 3; i++) if (badf(HDR[i])) { HDR[i] = 0; nb++; }
            if (nb) fprintf(stderr, "trame %d : %ld valeurs non finies remises à 0\n", fi, nb); }
        post(fi, out);
        fwrite(out, 1, W * HF * 3, stdout);
        if (fi % 25 == 0) { fprintf(stderr, "%d\n", fi); fflush(stderr); }
    }
    return 0;
}
