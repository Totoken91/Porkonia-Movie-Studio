// PORKONIA EN IMAGES DE SYNTHESE - moteur de raymarching SDF ecrit pour l'occasion.
// Usage: pork3d2 F0 F1 FPS AA [W H] > frames.rgb      (temps du film = image / FPS) ; "cam" en 5e argument: trace la camera
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include "tex.h"

typedef struct { float x, y, z; } v3;
#define S static inline
S v3 V(float x, float y, float z) { v3 r = {x, y, z}; return r; }
S v3 add(v3 a, v3 b) { return V(a.x + b.x, a.y + b.y, a.z + b.z); }
S v3 sub(v3 a, v3 b) { return V(a.x - b.x, a.y - b.y, a.z - b.z); }
S v3 mul(v3 a, float s) { return V(a.x * s, a.y * s, a.z * s); }
S v3 mulv(v3 a, v3 b) { return V(a.x * b.x, a.y * b.y, a.z * b.z); }
S float dot(v3 a, v3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
S v3 cross(v3 a, v3 b) { return V(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
S float len(v3 a) { return sqrtf(dot(a, a)); }
S float len2(float a, float b) { return sqrtf(a * a + b * b); }
S v3 nrm(v3 a) { return mul(a, 1.0f / (len(a) + 1e-9f)); }
S v3 mix3(v3 a, v3 b, float t) { return add(mul(a, 1 - t), mul(b, t)); }
S v3 refl(v3 i, v3 n) { return sub(i, mul(n, 2 * dot(n, i))); }
S float clampf(float x, float a, float b) { return x < a ? a : x > b ? b : x; }
S float mixf(float a, float b, float t) { return a + (b - a) * t; }
S float sstep(float a, float b, float x) { float t = clampf((x - a) / (b - a), 0, 1); return t * t * (3 - 2 * t); }
S float ease(float x) { x = clampf(x, 0, 1); return x * x * (3 - 2 * x); }
S float smin(float a, float b, float k) { float h = fmaxf(k - fabsf(a - b), 0) / k; return fminf(a, b) - h * h * k * 0.25f; }
S float fract(float x) { return x - floorf(x); }
#ifndef HASH_STABLE
S float hash2(float x, float y) { return fract(sinf(x * 127.1f + y * 311.7f) * 43758.5453f); }
#else
// (22 nov. 2026) Le hasard du décor (fenêtres allumées, tenues de la foule, flocons...) passe par sin(...) * 43758 : la moindre différence d'arrondi
// change le résultat. Inlinée, la fonction était compilée différemment selon l'endroit (FMA ou pas, sinus vectorisé à 4 ulp ou pas) : toucher une
// ligne ailleurs dans le moteur allumait ou éteignait des fenêtres de Douzi City. Hors ligne, sans contraction ni vectorisation, elle rend
// toujours la même valeur pour les mêmes x, y, quel que soit le reste du code. Pas par défaut : elle change un peu le hasard des films déjà rendus
// (quelques fenêtres, flocons, tenues). STABLE=1 ./buildc.sh (ou -DHASH_STABLE) pour une nouvelle production, à garder jusqu'au bout.
__attribute__((noinline, optimize("fp-contract=off", "no-tree-vectorize"))) static float hash2(float x, float y) { return fract(sinf(fmaf(x, 127.1f, y * 311.7f)) * 43758.5453f); }
#endif
S float vnoise(float x, float y) {
    float ix = floorf(x), iy = floorf(y), fx = x - ix, fy = y - iy;
    fx = fx * fx * (3 - 2 * fx); fy = fy * fy * (3 - 2 * fy);
    float a = hash2(ix, iy), b = hash2(ix + 1, iy), c = hash2(ix, iy + 1), d = hash2(ix + 1, iy + 1);
    return a + (b - a) * fx + (c - a) * fy + (a - b - c + d) * fx * fy;
}
S float fbm(float x, float y) {
    float s = 0, a = 0.5f;
    for (int i = 0; i < 4; i++) { s += a * vnoise(x, y); x = x * 2.03f + 17.1f; y = y * 2.03f + 5.3f; a *= 0.5f; }
    return s;
}

enum { FLOOR, GOLD, CHROME, PINK, BLACK, MARBLE, FOAM, SAUS, GRASS, BEER, LETT, FLAG, POLE, MEAT, STONE,
       HULL, PORT, DARK, TILE, WALL, BALL, RIB, BOOTH, SONAR, SPK, BLDG, SIGN, POST, SHADES, NMAT };
enum { W_MON, W_PARADE, W_MUG, W_DEEP, W_PARTY, W_CITY };
#define BEAT 0.5625f
#define PI 3.14159265f

// ---------- per-frame globals
static int WORLD; static float T;
static v3 SUN, SUNCOL, SKYTOP, SKYHOR, CLOUD, CAMP;
static float SUNSIZE, FOGD, TMAXD, PIGAMP; static int RETRO, FLOORON;
static v3 SPH[5], BLOBC, BLOBR; static float SPR, SPSK, MORPH, CHPH; static int CHMODE;

// ---------- textures
static float *TX; static unsigned char *EM;
static float text2d(int id, float u, float v) {     // signed distance in atlas px, valid outside the block too
    const trect *r = &TR[id];
    float du = fmaxf(fmaxf(-u, u - (r->w - 1)), 0), dv = fmaxf(fmaxf(-v, v - (r->h - 1)), 0);
    float cu = clampf(u, 0, r->w - 1.001f), cv = clampf(v, 0, r->h - 1.001f);
    int ix = (int)cu, iy = (int)cv; float fx = cu - ix, fy = cv - iy;
    const float *q = TX + (r->y0 + iy) * TXW + ix;
    float s = (q[0] * (1 - fx) + q[1] * fx) * (1 - fy) + (q[TXW] * (1 - fx) + q[TXW + 1] * fx) * fy;
    float L = len2(du, dv);
    return L > 0 ? fmaxf(L, s - L) : s;
}
static void emb(float u, float v, float *rgba) {    // emblem, linear colour + alpha
    u = clampf(u, 0, 1) * 254.99f; v = clampf(v, 0, 1) * 254.99f;
    int ix = (int)u, iy = (int)v; float fx = u - ix, fy = v - iy;
    const unsigned char *q = EM + (iy * 256 + ix) * 4;
    for (int c = 0; c < 4; c++) {
        float a = (q[c] * (1 - fx) + q[4 + c] * fx) * (1 - fy) + (q[1024 + c] * (1 - fx) + q[1028 + c] * fx) * fy;
        a /= 255.0f; rgba[c] = c < 3 ? a * a : a;
    }
}

// ---------- primitives
S float sdEll(v3 p, v3 r) {
    float k0 = len(V(p.x / r.x, p.y / r.y, p.z / r.z));
    float k1 = len(V(p.x / (r.x * r.x), p.y / (r.y * r.y), p.z / (r.z * r.z)));
    return k0 * (k0 - 1.0f) / (k1 + 1e-6f);
}
S float sdCap(v3 p, v3 a, v3 b, float r) {
    v3 pa = sub(p, a), ba = sub(b, a);
    float h = clampf(dot(pa, ba) / dot(ba, ba), 0, 1);
    return len(sub(pa, mul(ba, h))) - r;
}
S float box2(float dx, float dy) { return fminf(fmaxf(dx, dy), 0) + len2(fmaxf(dx, 0), fmaxf(dy, 0)); }
S float sdBox(v3 p, v3 b) {
    float dx = fabsf(p.x) - b.x, dy = fabsf(p.y) - b.y, dz = fabsf(p.z) - b.z;
    return fminf(fmaxf(dx, fmaxf(dy, dz)), 0) + len(V(fmaxf(dx, 0), fmaxf(dy, 0), fmaxf(dz, 0)));
}
S float cylY(v3 p, float r, float h) { return box2(len2(p.x, p.z) - r, fabsf(p.y) - h); }
S float cylZ(v3 p, float r, float h) { return box2(len2(p.x, p.y) - r, fabsf(p.z) - h); }
S float cylX(v3 p, float r, float h) { return box2(len2(p.y, p.z) - r, fabsf(p.x) - h); }

// ---------- the crowned pig (faces +z, feet on y=0, about 2.6 long)
static float pig(v3 p, float ph, int skin, int *m) {
    *m = skin;
    float bd = len(sub(p, V(0, 0.95f, 0.25f))) - 1.8f;
    if (bd > 0.9f) return bd;
    float sg = p.x > 0 ? 1.0f : -1.0f;
    p.x = fabsf(p.x);
    float d = sdEll(sub(p, V(0, 0.9f, 0)), V(0.62f, 0.6f, 0.95f));
    d = smin(d, len(sub(p, V(0, 1.05f, 0.95f))) - 0.48f, 0.15f);
    d = smin(d, cylZ(sub(p, V(0, 0.98f, 1.42f)), 0.19f, 0.11f) - 0.035f, 0.06f);
    d = fmaxf(d, -(len(sub(p, V(0.075f, 0.99f, 1.585f))) - 0.045f));
    v3 q = sub(p, V(0.31f, 1.5f, 0.8f));
    float c = 0.8776f, s = 0.4794f;
    d = smin(d, sdEll(V(c * q.x + s * q.y, -s * q.x + c * q.y, q.z), V(0.13f, 0.23f, 0.06f)), 0.06f);
    float sw = 0.2f * PIGAMP * sinf(ph) * sg;
    d = smin(d, sdCap(p, V(0.32f, 0.6f, 0.55f), V(0.32f, 0.14f, 0.55f + sw), 0.14f), 0.08f);
    d = smin(d, sdCap(p, V(0.32f, 0.6f, -0.55f), V(0.32f, 0.14f, -0.55f - sw), 0.14f), 0.08f);
    q = sub(p, V(0, 1.12f, -0.99f));
    float tr = len2(q.y, q.z) - 0.12f;
    d = fminf(d, len2(tr, q.x) - 0.035f);
    float e = len(sub(p, V(0.2f, 1.22f, 1.34f))) - 0.055f;
    if (e < d) { d = e; *m = BLACK; }
    q = sub(p, V(0, 1.53f, 0.9f));
    if (len(q) < 0.7f) {    // crown
        float r = len2(q.x, q.z);
        float cr = box2(fabsf(r - 0.24f) - 0.02f, fabsf(q.y) - 0.07f) - 0.008f;
        float sec = 6.2831853f / 5, a = atan2f(q.z, q.x) - 1.5707963f;
        a -= sec * roundf(a / sec);
        float lx = r * cosf(a) - 0.24f, lz = r * sinf(a), ly = q.y - 0.07f;
        float sp = fmaxf(len2(lx, lz) - 0.075f * (1 - ly / 0.2f), fmaxf(-ly, ly - 0.19f)) * 0.8f;
        cr = fminf(cr, fminf(sp, len(V(lx, ly - 0.2f, lz)) - 0.035f));
        if (cr < d) { d = cr; *m = GOLD; }
    }
    return d;
}

// ================= WORLD : the plain (letters PORKONIA, monument, liquid chrome) =================
#define ZL (-40.0f)
#define CAPW 12.0f
#define TS (CAPW / 140.0f)
#define OY ((235.0f - TITLE_OYROW) * TS)
#define PEDH 3.6f
#define PZ (-8.5f)
static float letters(v3 p) {
    float x0 = -TITLE_OX * TS, x1 = (1145 - TITLE_OX) * TS;
    float bx = fmaxf(fmaxf(x0 - p.x, p.x - x1), 0), by = fmaxf(p.y - (CAPW + 4), 0), bz = fmaxf(fabsf(p.z - ZL) - 1.6f, 0);
    float bd = sqrtf(bx * bx + by * by + bz * bz);
    if (bd > 1.5f) return bd * 0.9f;
    float d2 = text2d(T_TITLE, p.x / TS + TITLE_OX, 235.0f - p.y / TS) * TS;
    float dz = fabsf(p.z - ZL) - 1.2f, r = 0.14f;
    return (fminf(fmaxf(d2 + r, dz + r), 0) + len2(fmaxf(d2 + r, 0), fmaxf(dz + r, 0)) - r) * 0.85f;
}
static float hills(v3 p) {
    if (p.y > 15) return p.y - 14;
    float m = fmaxf(sstep(60, 120, fabsf(p.x)), sstep(90, 150, p.z));
    float h = m * (6.5f + 3.5f * sinf(p.x * 0.06f + 1) * sinf(p.z * 0.045f) + 2.5f * sinf(p.z * 0.1f + p.x * 0.03f)) - 0.6f;
    return (p.y - h) * 0.55f;
}
static float monument(v3 p, int *m) {
    *m = MARBLE;
    float bd = len(sub(p, V(0, 4.5f, 0))) - 6.6f;
    if (bd > 1.0f) return bd;
    float d = cylY(sub(p, V(0, 0.35f, 0)), 3.4f, 0.32f) - 0.03f;
    d = fminf(d, cylY(sub(p, V(0, 1.3f, 0)), 2.8f, 0.57f) - 0.03f);
    d = fminf(d, cylY(sub(p, V(0, 2.75f, 0)), 2.3f, 0.82f) - 0.03f);
    int mm; const float k = 2.4f;
    float pg = pig(V(-p.x / k, (p.y - PEDH) / k, -p.z / k), 0, GOLD, &mm) * k;
    if (pg < d) { d = pg; *m = mm == BLACK ? BLACK : GOLD; }
    return d;
}
static float chromeobj(v3 p, int *m) {
    *m = CHROME;
    if (CHMODE == 1) {
        float d = 1e9f;
        for (int i = 0; i < 5; i++) d = smin(d, len(sub(p, SPH[i])) - SPR, SPSK);
        return d;
    }
    float db = 0;
    if (CHMODE == 2 || CHMODE == 3) {
        float w = 0.05f * sinf(p.x * 4 + T * 5) * sinf(p.y * 5 - T * 4) * sinf(p.z * 4 + T * 3);
        db = sdEll(sub(p, BLOBC), BLOBR) + w;
        if (CHMODE == 2) return db * 0.8f;
    }
    int mm;
    float dp = pig(V(-p.x, p.y - 0.06f * PIGAMP * fabsf(sinf(CHPH)), -(p.z - PZ)), CHPH, CHROME, &mm);
    if (CHMODE == 3) {
        float rip = 0.28f * MORPH * (1 - MORPH) * sinf(p.y * 9 - T * 12) * 0.25f;
        *m = MORPH > 0.93f ? mm : CHROME;
        return (db * (1 - MORPH) + dp * MORPH + rip) * 0.6f;
    }
    *m = mm; return dp;
}
static float mon_map(v3 p, int *m) {
    float d = monument(p, m); int mm;
    float c = chromeobj(p, &mm); if (c < d) { d = c; *m = mm; }
    float l = letters(p); if (l < d) { d = l; *m = LETT; }
    float h = hills(p); if (h < d) { d = h; *m = GRASS; }
    return d;
}

// ================= WORLD : the great parade, flags, the Cube de Porc =================
#define PSX 3.4f
#define PSZ 4.6f
#define PV 2.4f
#define CUBEC V(-78.0f, 86.0f, -440.0f)
static float flagpos(v3 p, v3 *l, float *ph) {
    const float cx = 6.8f, cz = 13.8f;
    float ix = roundf((p.x + 1.7f) / cx), iz = roundf(p.z / cz);
    *l = V(p.x - (ix * cx - 1.7f), p.y, p.z - iz * cz);
    *ph = hash2(ix, iz) * 6.28f;
    return fminf(cx * 0.5f - fabsf(l->x), cz * 0.5f - fabsf(l->z)) + 0.4f;
}
S float flagwave(float fx, float fy, float ph) {
    return 0.26f * powf(clampf(fx / 2.4f, 0, 1), 0.8f) * sinf(2.6f * fx - 5.5f * T + 0.7f * fy + ph) + 0.06f * sinf(5.0f * fx - 9.0f * T + ph);
}
static float flags(v3 p, int *m) {
    *m = POLE;
    if (p.y > 9.5f) return p.y - 8.6f;
    float dc = len2(p.x - CAMP.x, p.z - CAMP.z);
    if (dc > 125) return (dc - 120) * 0.9f;
    v3 l; float ph; float guard = flagpos(p, &l, &ph);
    float d = cylY(V(l.x, l.y - 3.75f, l.z), 0.075f, 3.75f);
    d = fminf(d, len(sub(l, V(0, 7.62f, 0))) - 0.17f);
    float fx = l.x - 0.07f, fy = l.y - 6.45f;
    float df = fmaxf(fmaxf(fabsf(l.z - flagwave(fx, fy, ph)) - 0.03f, fmaxf(-fx, fx - 2.4f)), fabsf(fy) - 0.85f) * 0.6f;
    if (df < d) { d = df; *m = FLAG; }
    return fminf(d, guard);
}
static float parade(v3 p, int *m, int ex) {
    float d; *m = PINK;
    if (p.y > 2.6f) d = p.y - 2.0f;
    else {
        float zz = p.z - PZ + (T - 22.5f) * PV;
        float ix = roundf(p.x / PSX), iz = roundf(zz / PSZ);
        float h = hash2(ix, iz), ph = PI * T / BEAT;
        v3 l = V(p.x - ix * PSX, p.y - 0.06f * fabsf(sinf(ph)), zz - iz * PSZ);
        int skin = (ix == 0 && iz == 0) ? CHROME : h < 0.06f ? GOLD : h < 0.2f ? CHROME : PINK;
        d = pig(V(-l.x, l.y, -l.z), ph, skin, m);
        if (!ex) d = fminf(d, fminf(PSX * 0.5f - fabsf(l.x), PSZ * 0.5f - fabsf(l.z)) + 0.35f);
    }
    int mm; float f = flags(p, &mm);
    if (f < d) { d = f; *m = mm; }
    v3 q = sub(p, CUBEC);
    float c = sdBox(q, V(80, 80, 80)) - 0.5f;
    if (c < d) { d = c; *m = MEAT; }
    c = sdBox(V(q.x, p.y - 3.0f, q.z), V(100, 3, 100));
    if (c < d) { d = c; *m = STONE; }
    return d;
}

// ================= WORLD : the Douzi Ambree mug, the Seven Sausages =================
static float glass(v3 p) {
    float r = len2(p.x, p.z), a = atan2f(p.z, p.x);
    float body = (box2(r - 1.2f - 0.05f * cosf(a * 10), fabsf(p.y - 1.55f) - 1.42f) - 0.06f) * 0.8f;
    v3 q = sub(p, V(1.6f, 1.6f, 0)); q.y *= 0.72f;
    float h = len2(len2(q.x, q.y) - 0.72f, q.z) - 0.17f;
    return smin(body, h, 0.12f);
}
static float mug(v3 p, int *m, int nog) {
    float d = 1e9f; *m = FOAM;
    float bd = len(sub(p, V(0, 1.8f, 0))) - 4.6f;
    if (bd > 0.9f) return bd;
    float bump = 0.05f * sinf(p.x * 7 + 1) * sinf(p.y * 8) * sinf(p.z * 7 + 2);
    float f = cylY(sub(p, V(0, 3.22f, 0)), 1.1f, 0.14f) - 0.16f;
    f = smin(f, sdEll(sub(p, V(0, 3.42f, 0)), V(1.05f, 0.42f, 1.05f)), 0.15f);
    f = smin(f, sdEll(sub(p, V(-0.55f, 2.62f, -1.12f)), V(0.24f, 0.62f, 0.16f)), 0.2f);
    d = (f + bump) * 0.8f;
    if (!(nog & 1)) { float g = glass(p); if (g < d) { d = g; *m = BEER; } }
    float c = 0.9553f, s = 0.2955f;
    v3 q = V(p.x, c * (p.y - 1.9f) - s * p.z, s * (p.y - 1.9f) + c * p.z);
    float sec = 6.2831853f / 7, a = atan2f(q.z, q.x) - T * 0.7f;
    a -= sec * roundf(a / sec);
    float r = len2(q.x, q.z);
    v3 l = V(r * cosf(a) - 3.3f, q.y, r * sinf(a));
    float sa = sdCap(l, V(0.06f, 0, -0.55f), V(0.06f, 0, 0.55f), 0.19f);
    if (sa < d) { d = sa; *m = SAUS; }
    return d;
}

static float deep_map(v3 p, int *m); static float party_map(v3 p, int *m); static float city_map(v3 p, int *m);
static float map(v3 p, int *m, int nog) {
    if (WORLD == W_MON) return mon_map(p, m);
    if (WORLD == W_DEEP) return deep_map(p, m);
    if (WORLD == W_PARTY) return party_map(p, m);
    if (WORLD == W_CITY) return city_map(p, m);
    if (WORLD == W_PARADE) return parade(p, m, nog & 2);
    return mug(p, m, nog);
}
static v3 normal(v3 p, int nog, float e) {
    int m;
    float a = map(V(p.x + e, p.y - e, p.z - e), &m, nog), b = map(V(p.x - e, p.y - e, p.z + e), &m, nog);
    float c = map(V(p.x - e, p.y + e, p.z - e), &m, nog), d = map(V(p.x + e, p.y + e, p.z + e), &m, nog);
    return nrm(V(a - b - c + d, -a - b + c + d, -a + b - c + d));
}
static v3 glassN(v3 p) {
    float e = 0.004f;
    float a = glass(V(p.x + e, p.y - e, p.z - e)), b = glass(V(p.x - e, p.y - e, p.z + e));
    float c = glass(V(p.x - e, p.y + e, p.z - e)), d = glass(V(p.x + e, p.y + e, p.z + e));
    return nrm(V(a - b - c + d, -a - b + c + d, -a + b - c + d));
}

// ---------- sky
static v3 sky(v3 rd) {
    float y = fmaxf(rd.y, 0);
    v3 col = mix3(SKYHOR, SKYTOP, powf(y, 0.45f));
    float sd = dot(rd, SUN);
    col = add(col, mul(SUNCOL, 0.25f * powf(fmaxf(sd, 0), 8) + 0.5f * powf(fmaxf(sd, 0), 64)));
    if (rd.y > 0.005f) {
        float k = 1.0f / (rd.y + 0.12f);
        float c = fbm(rd.x * k * 1.3f + T * 0.02f, rd.z * k * 1.3f + 3.0f);
        c = sstep(0.48f, 0.78f, c) * sstep(0.0f, 0.12f, rd.y);
        col = mix3(col, add(CLOUD, mul(SUNCOL, 0.25f * powf(fmaxf(sd, 0), 4))), c * 0.85f);
    }
    float ang = acosf(clampf(sd, -1, 1));
    if (ang < SUNSIZE) {
        float disc = sstep(SUNSIZE, SUNSIZE * 0.96f, ang);
        if (RETRO) {    // banded sun
            float v = (rd.y - SUN.y) / SUNSIZE;
            float band = v < 0.25f ? sstep(0.0f, 0.12f, fract((0.25f - v) * 5.5f + T * 0.25f) - 0.18f - 0.35f * (0.25f - v)) : 1.0f;
            v3 sc = mix3(V(3.2f, 0.5f, 0.35f), V(3.4f, 2.4f, 0.6f), clampf(v * 0.5f + 0.5f, 0, 1));
            col = mix3(col, sc, disc * band);
        } else col = mix3(col, mul(SUNCOL, 6.0f), disc);
    }
    return col;
}

// ---------- tracing
static float trace(v3 ro, v3 rd, int *m, int nog, int steps) {
    float tfl = (FLOORON && rd.y < -1e-4f) ? -ro.y / rd.y : 1e9f;
    float tmax = fminf(TMAXD, tfl), t = 0.02f;
    for (int i = 0; i < steps && t < tmax; i++) {
        float d = map(add(ro, mul(rd, t)), m, nog);
        if (d < 0.0006f * (1 + t)) return t;
        t += d;
    }
    if (tfl < TMAXD) { *m = FLOOR; return tfl; }
    return -1;
}
static float shadow(v3 ro, v3 rd, int nog) {
    float res = 1, t = 0.05f; int m;
    for (int i = 0; i < 40 && t < 70; i++) {
        float d = map(add(ro, mul(rd, t)), &m, nog);
        if (d < 0.002f) return 0;
        if (d < 0.3f) res = fminf(res, 10 * d / t);
        t += clampf(d, 0.03f, 5.0f);
    }
    return clampf(res, 0, 1);
}
static float ambocc(v3 p, v3 n, int nog) {
    float o = 0, sc = 1; int m;
    for (int i = 1; i <= 4; i++) {
        float h = 0.06f + 0.16f * i;
        v3 q = add(p, mul(n, h));
        float d = map(q, &m, nog | 2); if (FLOORON) d = fminf(d, q.y);
        o += (h - d) * sc; sc *= 0.75f;
    }
    return clampf(1 - 1.4f * o, 0, 1);
}
S float checker(float x, float z, float w) {
    float ax = 2 * (fabsf(fract((x - 0.5f * w) * 0.5f) - 0.5f) - fabsf(fract((x + 0.5f * w) * 0.5f) - 0.5f)) / w;
    float az = 2 * (fabsf(fract((z - 0.5f * w) * 0.5f) - 0.5f) - fabsf(fract((z + 0.5f * w) * 0.5f) - 0.5f)) / w;
    return 0.5f - 0.5f * ax * az;
}
S float checkat(v3 p, v3 rd, float t) {
    float w = fmaxf(t * 0.0035f / fmaxf(-rd.y, 0.02f), 0.002f) / 2.0f;
    return w > 1.0f ? 0.5f : checker(p.x / 2.0f, p.z / 2.0f, w);
}
static int refr(v3 i, v3 n, float eta, v3 *o) {
    float c = dot(n, i), k = 1 - eta * eta * (1 - c * c);
    if (k < 0) return 0;
    *o = sub(mul(i, eta), mul(n, eta * c + sqrtf(k)));
    return 1;
}

#include "night.h"

static v3 shade(v3 ro, v3 rd, int depth, int nog) {
    if (WORLD == W_DEEP) return shade_deep(ro, rd, depth);
    if (WORLD == W_PARTY) return shade_party(ro, rd, depth);
    if (WORLD == W_CITY) return shade_city(ro, rd, depth);
    int m;
    float t = trace(ro, rd, &m, nog, depth ? 90 : 170);
    if (t < 0) return sky(rd);
    v3 p = add(ro, mul(rd, t));
    v3 n = m == FLOOR ? V(0, 1, 0) : normal(p, nog, m == LETT ? 0.05f : 0.0015f * (1 + t * 0.05f));
    if (m == FLAG && dot(n, rd) > 0) n = mul(n, -1);
    v3 ps = add(p, mul(n, 0.02f));
    v3 alb = V(1, 1, 1), F0 = V(0.04f, 0.04f, 0.04f); int metal = 0, noref = 0; float gloss = 60, spk = 0.6f, ambk = 1;
    switch (m) {
    case FLOOR: alb = mix3(V(0.30f, 0.03f, 0.07f), V(0.90f, 0.84f, 0.72f), checkat(p, rd, t)); F0 = V(0.10f, 0.10f, 0.10f); gloss = 200; spk = 1.2f; break;
    case GOLD: metal = 1; F0 = V(1.0f, 0.74f, 0.30f); break;
    case CHROME: metal = 1; F0 = V(0.92f, 0.93f, 0.96f); break;
    case LETT: metal = 1; F0 = fabsf(n.z) > 0.6f ? V(0.95f, 0.93f, 0.92f) : V(1.0f, 0.70f, 0.28f); break;
    case PINK: alb = V(0.95f, 0.47f, 0.52f); F0 = V(0.05f, 0.05f, 0.05f); gloss = 40; spk = 0.9f; break;
    case BLACK: alb = V(0.02f, 0.02f, 0.02f); F0 = V(0.08f, 0.08f, 0.08f); gloss = 120; spk = 2; break;
    case MARBLE: { float v = fbm(p.x * 1.5f + 3 * fbm(p.x * 0.7f, p.z * 0.7f + p.y), p.y * 3 + p.z * 1.5f);
        alb = mix3(V(0.92f, 0.9f, 0.86f), V(0.45f, 0.36f, 0.34f), sstep(0.55f, 0.75f, v)); F0 = V(0.07f, 0.07f, 0.07f); break; }
    case FOAM: alb = V(1.0f, 0.96f, 0.86f); spk = 0.1f; ambk = 1.5f; break;
    case SAUS: alb = V(0.62f, 0.17f, 0.10f); F0 = V(0.06f, 0.06f, 0.06f); gloss = 80; spk = 1.5f; break;
    case GRASS: { float v = fbm(p.x * 0.15f, p.z * 0.15f); alb = mix3(V(0.16f, 0.42f, 0.10f), V(0.42f, 0.58f, 0.16f), v); spk = 0; noref = 1; break; }
    case POLE: alb = p.y > 7.42f ? V(0.95f, 0.70f, 0.25f) : V(0.80f, 0.80f, 0.82f); spk = 0.8f; noref = 1; break;
    case FLAG: {
        v3 l; float ph; flagpos(p, &l, &ph);
        float fx = l.x - 0.07f, fy = l.y - 6.45f, sf = 0.27f / 140.0f;
        float d = text2d(T_TITLE, (fx - 1.2f) / sf + 572.5f, 235.0f - (fy + 0.135f) / sf) * sf;
        float px = fmaxf(t * 0.0025f, 0.004f);
        float bord = fminf(fminf(fx, 2.4f - fx), 0.85f - fabsf(fy));
        alb = mix3(V(0.34f, 0.035f, 0.08f), V(0.80f, 0.58f, 0.22f), sstep(0.10f + px, 0.10f - px, bord));
        alb = mix3(alb, V(0.95f, 0.92f, 0.84f), sstep(px, -px, d));
        spk = 0.15f; noref = 1; ambk = 1.3f; break; }
    case MEAT: {    // pork belly: rind, thick fat, streaky meat
        v3 q = sub(p, CUBEC);
        float u = fabsf(n.x) > 0.5f ? q.z : q.x, h = (q.y + 80) / 160;
        float hh = h + (fbm(u * 0.01f, 3.3f) - 0.5f) * 0.10f;
        float seam = fmaxf(fmaxf(sstep(0.035f, 0.0f, fabsf(hh - 0.22f - 0.03f * sinf(u * 0.02f))), sstep(0.03f, 0.0f, fabsf(hh - 0.45f))),
                           sstep(0.04f, 0.0f, fabsf(hh - 0.66f + 0.03f * sinf(u * 0.015f + 2))));
        float marb = sstep(0.55f, 0.75f, fbm(u * 0.05f, q.y * 0.12f)) * 0.5f;
        v3 meat = mix3(V(0.72f, 0.16f, 0.17f), V(0.50f, 0.09f, 0.11f), fbm(u * 0.02f + 5, q.y * 0.02f));
        alb = mix3(meat, V(0.98f, 0.90f, 0.80f), fmaxf(seam, marb));
        alb = mix3(alb, V(0.98f, 0.91f, 0.80f), sstep(0.80f, 0.83f, hh));
        alb = mix3(alb, V(0.70f, 0.38f, 0.10f), sstep(0.925f, 0.94f, hh));
        if (n.y > 0.5f) alb = V(0.70f, 0.38f, 0.10f);
        F0 = V(0.05f, 0.05f, 0.05f); gloss = 25; spk = 0.5f; ambk = 1.2f; noref = 1; break; }
    case STONE: alb = V(0.62f, 0.60f, 0.58f); noref = 1; break;
    default: break;
    }
    float ndv = fmaxf(dot(mul(rd, -1), n), 0), f5 = powf(1 - ndv, 5);
    v3 fr = add(F0, mul(sub(V(1, 1, 1), F0), f5));
    v3 rdir = refl(rd, n);
    float ndl = fmaxf(dot(n, SUN), 0);
    if (m == FLAG) ndl = 0.35f + 0.65f * fabsf(dot(n, SUN));
    float sh = ndl > 0 ? shadow(ps, SUN, nog) : 0;
    if (WORLD == W_MUG && !nog && sh < 1 && ndl > 0) sh = 0.5f * sh + 0.5f * shadow(ps, SUN, 1) * 0.8f;
    float ao = ambocc(p, n, nog);
    v3 col;
    if (m == BEER) {
        v3 rc = depth < 2 ? shade(ps, rdir, depth + 1, nog) : sky(rdir);
        v3 rr, tc;
        if (refr(rd, n, 1 / 1.33f, &rr)) {
            v3 pin = sub(p, mul(n, 0.03f)); float ti = 0;
            for (int i = 0; i < 70; i++) { float d = -glass(add(pin, mul(rr, ti))); if (d < 0.003f) break; ti += fmaxf(d, 0.012f); }
            v3 pe = add(pin, mul(rr, ti)), n2 = glassN(pe), ro2;
            if (!refr(rr, mul(n2, -1), 1.33f, &ro2)) ro2 = refl(rr, mul(n2, -1));
            tc = depth < 2 ? shade(add(pe, mul(n2, 0.04f)), ro2, depth + 1, 1) : sky(ro2);
            tc = mulv(tc, V(expf(-0.10f * ti), expf(-0.42f * ti), expf(-1.25f * ti)));
            tc = add(tc, mul(V(0.9f, 0.45f, 0.06f), 0.10f * (1 - expf(-0.5f * ti)) * (0.4f + ndl * sh)));
        } else tc = rc;
        float f = 0.04f + 0.96f * f5;
        col = add(mul(tc, 1 - f), mul(rc, f));
        col = add(col, mul(SUNCOL, 2.5f * powf(fmaxf(dot(rdir, SUN), 0), 300) * sh));
    } else {
        v3 rc;
        if (!noref && depth < 2 && (metal || depth == 0)) rc = shade(ps, rdir, depth + 1, nog);
        else rc = mul(sky(V(rdir.x, fabsf(rdir.y), rdir.z)), rdir.y < 0 ? 0.35f : 1.0f);
        if (metal) {
            col = mul(mulv(F0, rc), 0.35f + 0.65f * ao);
            col = add(col, mul(mulv(F0, SUNCOL), 0.06f * ndl * sh));
        } else {
            v3 amb = mul(mix3(mul(SKYHOR, 0.22f), mul(add(SKYTOP, SKYHOR), 0.33f), 0.5f + 0.5f * n.y), ambk);
            v3 dif = mulv(alb, add(mul(SUNCOL, ndl * sh), mul(amb, ao)));
            col = add(mulv(dif, sub(V(1, 1, 1), fr)), mulv(rc, mul(fr, 0.4f + 0.6f * ao)));
            col = add(col, mul(SUNCOL, spk * powf(fmaxf(dot(rdir, SUN), 0), gloss) * sh));
        }
    }
    float fog = 1 - expf(-(t / FOGD) * (t / FOGD));
    return mix3(col, SKYHOR, fog);
}

// ---------- cameras
static v3 RO, CU, CV, CW; static float FOV;
static void cam(v3 ro, v3 ta, float roll, float fov) {
    RO = ro; CAMP = ro; CW = nrm(sub(ta, ro));
    v3 up = V(sinf(roll), cosf(roll), 0);
    CU = nrm(cross(up, CW)); CV = cross(CW, CU); FOV = fov;
}
static float spl(const float *k, int n, int st, int c, float t) {    // Catmull-Rom through keys (row = time, values...)
    int i = 0; while (i < n - 2 && t > k[(i + 1) * st]) i++;
    float t0 = k[i * st], t1 = k[(i + 1) * st], h = t1 - t0, s = clampf((t - t0) / h, 0, 1);
    float p0 = k[i * st + c], p1 = k[(i + 1) * st + c];
    float m0 = i > 0 ? (p1 - k[(i - 1) * st + c]) / (t1 - k[(i - 1) * st]) : (p1 - p0) / h;
    float m1 = i < n - 2 ? (k[(i + 2) * st + c] - p0) / (k[(i + 2) * st] - t0) : (p1 - p0) / h;
    float s2 = s * s, s3 = s2 * s;
    return (2 * s3 - 3 * s2 + 1) * p0 + (s3 - 2 * s2 + s) * h * m0 + (-2 * s3 + 3 * s2) * p1 + (s3 - s2) * h * m1;
}
static void camk(const float *k, int n, float t) {
    float o[9]; for (int c = 1; c < 9; c++) o[c] = spl(k, n, 9, c, t);
    cam(V(o[1], o[2], o[3]), V(o[4], o[5], o[6]), o[7], o[8]);
}
// t, pos, target, roll, fov
static const float KAB[] = {
     0.0f,  13.0f, 1.3f, -104,   -6, 6.5f, -40,   0.00f, 1.25f,
     3.0f,   9.0f, 1.1f,  -91,   -4, 6.4f, -40,   0.05f, 1.25f,
     6.0f,   3.6f, 2.2f,  -72,   -1, 6.2f, -30,  -0.04f, 1.25f,
     8.0f,   0.5f, 5.2f,  -52,    0, 6.0f,   0,   0.00f, 1.25f,
     9.0f,   0.0f, 6.09f, -40,    0, 5.9f,   0,   0.00f, 1.25f,
    10.0f,   0.6f, 6.4f,  -25,    0, 5.6f,   0,  -0.05f, 1.30f,
    11.0f,   4.5f, 6.2f, -14.5f,  0, 5.4f,   0,  -0.10f, 1.35f,
    12.0f,  11.0f, 5.8f,  -8.5f,  0, 5.3f,   0,  -0.06f, 1.35f,
    13.0f,  14.0f, 5.6f,   0.5f,  0, 5.3f,   0,   0.00f, 1.35f,
    14.0f,  10.0f, 5.4f,  10.0f,  0, 5.3f,   0,   0.00f, 1.35f,
    15.0f,   0.0f, 5.2f,  13.5f,  0, 5.2f,   0,   0.00f, 1.35f,
    16.0f, -10.0f, 5.0f,   9.5f,  0, 5.0f,   0,   0.00f, 1.35f,
    17.0f, -13.0f, 4.4f,  -1.0f,  0, 4.2f,  -2,   0.00f, 1.35f,
    18.0f,  -9.0f, 3.2f, -10.5f,  0, 2.6f,  -5,   0.00f, 1.35f,
    19.2f,  -3.0f, 1.7f, -14.6f,  0, 1.3f,  PZ,   0.00f, 1.45f,
    21.0f,   1.6f, 1.0f, -15.2f,  0, 1.05f, PZ,   0.00f, 1.60f,
    22.5f,   1.9f, 0.95f,-15.4f,  0, 1.05f, PZ,   0.00f, 1.70f };
// t, pos, yaw, pitch, fov   (yaw 0 = looking +z, pi = looking -z)
static const float KC[] = {
    22.5f, 1.9f, 0.95f, -15.4f, -0.2687f, 0.014f, 1.70f,
    24.0f, 2.2f, 1.3f,  -16.5f, -0.22f,   0.00f,  1.60f,
    26.0f, 2.5f, 5.5f,  -20.0f,  0.9f,   -0.30f,  1.45f,
    28.0f, 2.4f, 13,    -23.0f,  2.3f,   -0.42f,  1.35f,
    29.5f, 1.9f, 19,    -23.5f,  3.08f,  -0.24f,  1.30f,
    31.5f, 1.7f, 25,    -22.0f,  3.1416f,-0.13f,  1.30f };
static const float KD[] = {
    31.5f,  5.7f, 3.4f, -8.8f,   0, 1.9f, 0,   0, 1.40f,
    33.5f, -0.5f, 2.7f, -8.4f,   0, 1.9f, 0,   0, 1.40f,
    35.3f, -6.0f, 2.4f, -4.0f,   0, 2.0f, 0,   0, 1.40f,
    36.6f, -5.4f, 4.6f,  1.8f,   0, 2.6f, 0,   0, 1.40f,
    37.5f, -2.8f, 6.9f,  1.4f,   0, 3.2f, 0,   0, 1.35f,
    38.25f,-0.2f, 4.2f,  0.2f,   0, 3.0f, 0,   0, 1.30f };
static const float KH[] = {
    63.0f,  0, 4.22f,  -16,     0,     5.9f, 0,   0, 1.5f,
    66.0f,  0, 4.9f,   -24,     0,     5.9f, 0,   0, 1.5f,
    68.3f,  0, 5.8f,   -34,     0,     6.0f, 0,   0, 1.5f,
    69.3f,  0, 6.09f,  -40,     0,     6.0f, 0,   0, 1.5f,
    71.0f,  0, 6.3f,   -58,    -2.5f,  6.2f, 0,   0, 1.5f,
    73.0f,  0, 6.6f,   -82,    -9.5f,  6.4f, 0,   0, 1.5f,
    75.0f,  0, 6.9f,   -95,   -13.5f,  6.5f, 0,   0, 1.5f,
    76.5f,  0, 7.0f,   -98,   -14.2f,  6.5f, 0,   0, 1.5f };
#define NK(a, st) ((int)(sizeof(a) / sizeof(float) / st))

static void orbit_spheres(float k) {      // k = 0 orbiting ... 1 merged in front of the monument
    v3 mc = V(0, 1.6f, PZ);
    for (int i = 0; i < 5; i++) {
        float a = i * 6.2831853f / 5 + T * 0.45f;
        v3 o = V(7.5f * cosf(a), 5.2f + 1.2f * sinf(T * 1.1f + i * 1.9f), 7.5f * sinf(a));
        v3 r = sub(o, mc); float an = 5.0f * k, c = cosf(an), s = sinf(an);
        r = V(c * r.x - s * r.z, r.y, s * r.x + c * r.z);
        SPH[i] = add(mc, mul(r, powf(1 - k, 1.3f)));
    }
    SPR = mixf(0.5f, 0.855f, k * k) * (1 + 0.05f * expf(-fract(T / (2 * BEAT)) * 2 * BEAT * 7) * (1 - k));
    SPSK = 0.02f + 1.1f * k; CHMODE = 1;
}
static void daysky(float e) {
    SUN = nrm(V(0.55f, 0.10f + 0.34f * e, -0.62f));
    SUNCOL = mix3(V(2.6f, 1.5f, 0.8f), V(2.6f, 2.2f, 1.7f), e);
    SKYTOP = mix3(V(0.10f, 0.20f, 0.60f), V(0.13f, 0.33f, 0.82f), e);
    SKYHOR = mix3(V(1.0f, 0.56f, 0.36f), V(0.86f, 0.80f, 0.72f), e);
    CLOUD = mix3(V(1.0f, 0.75f, 0.7f), V(1.0f, 0.96f, 0.93f), e);
}
static void setup(float tb) {
    T = tb; RETRO = 0; SUNSIZE = 0.035f; FOGD = 260; TMAXD = 320; PIGAMP = 1; CHMODE = 0; CHPH = 0; FLOORON = 1;
    if (tb < 22.5f) {            // A + B : one continuous shot
        WORLD = W_MON; daysky(clampf(tb / 22.5f, 0, 1));
        if (tb < 18) orbit_spheres(ease((tb - 15.3f) / 2.7f));
        else if (tb < 19) {      // the blob drops and squashes
            float u = tb - 18, y = 1.6f - 0.88f * ease(u / 0.45f), q = expf(-u * 4) * sinf(u * 22) * 0.16f * (u > 0.4f);
            float ry = 0.72f * (1 + q) + (0.855f - 0.72f) * (1 - ease(u / 0.45f)), r = sqrtf(0.625f / ry);
            BLOBR = V(r, ry, r); BLOBC = V(0, fmaxf(y, ry), PZ); CHMODE = 2;
        } else if (tb < 21.3f) { BLOBR = V(0.932f, 0.72f, 0.932f); BLOBC = V(0, 0.72f, PZ); MORPH = ease((tb - 19) / 2.3f); CHMODE = 3; PIGAMP = 0; }
        else { CHMODE = 4; PIGAMP = sstep(21.375f, 21.9f, tb); CHPH = PI * tb / BEAT; }
        camk(KAB, NK(KAB, 9), tb);
    } else if (tb < 31.5f) {     // C : the parade
        WORLD = W_PARADE; daysky(1); FOGD = 640; TMAXD = 760;
        float e = ease((tb - 22.5f) / 7.5f), ph = mixf(2.416f, 0.95f, e);     // the day goes by: the sun swings round during the shot
        SUN = nrm(V(sinf(ph) * 0.83f, mixf(0.44f, 0.72f, e), cosf(ph) * 0.83f));
        float o[7]; for (int c = 1; c < 7; c++) o[c] = spl(KC, NK(KC, 7), 7, c, tb);
        v3 p = V(o[1], o[2], o[3]);
        cam(p, add(p, V(sinf(o[4]) * cosf(o[5]) * 10, sinf(o[5]) * 10, cosf(o[4]) * cosf(o[5]) * 10)), 0, o[6]);
    } else if (tb < 38.25f) {    // D : the beer
        WORLD = W_MUG;
        SUN = nrm(V(0.55f, 0.62f, -0.45f)); SUNCOL = V(2.7f, 2.5f, 2.2f);
        SKYTOP = V(0.10f, 0.30f, 0.85f); SKYHOR = V(0.72f, 0.84f, 0.95f); CLOUD = V(1.0f, 0.97f, 0.95f);
        camk(KD, NK(KD, 9), tb);
    } else if (tb < 45) { WORLD = W_DEEP; setup_deep(tb); }
    else if (tb < 54) { WORLD = W_PARTY; setup_party(tb); }
    else if (tb < 63) { WORLD = W_CITY; setup_city(tb); }
    else {                       // H : sunrise behind the monument, back out through the O
        WORLD = W_MON; orbit_spheres(0);
        SUN = nrm(V(0, 0.105f, 1)); SUNCOL = V(2.4f, 0.9f, 0.45f); SUNSIZE = 0.21f; RETRO = 1;
        SKYTOP = V(0.10f, 0.06f, 0.30f); SKYHOR = V(0.95f, 0.30f, 0.22f); CLOUD = V(0.75f, 0.25f, 0.4f);
        camk(KH, NK(KH, 9), tb);
        if (tb < 63.05f) { v3 p = RO; cam(p, add(p, mul(SUN, 10)), 0, 1.5f); }
    }
}

int main(int argc, char **argv) {
    int f0 = atoi(argv[1]), f1 = atoi(argv[2]); float fps = atof(argv[3]); int aa = atoi(argv[4]);
    int camdump = argc > 5 && !strcmp(argv[5], "cam");
    int W = argc > 6 ? atoi(argv[5]) : 768, H = argc > 6 ? atoi(argv[6]) : 576;
    TX = malloc(sizeof(float) * TXW * TXH); EM = malloc(256 * 256 * 4);
    FILE *f = fopen("txt.bin", "rb"); if (!f || fread(TX, sizeof(float), TXW * TXH, f) != TXW * TXH) { fprintf(stderr, "txt.bin?\n"); return 1; } fclose(f);
    f = fopen("emb.bin", "rb"); if (!f || fread(EM, 1, 256 * 256 * 4, f) != 256 * 256 * 4) { fprintf(stderr, "emb.bin?\n"); return 1; } fclose(f);
    unsigned char *buf = malloc(W * H * 3);
    static const float jx[4] = {0.125f, -0.375f, 0.375f, -0.125f}, jy[4] = {0.375f, 0.125f, -0.125f, -0.375f};
    for (int fr = f0; fr < f1; fr++) {
        setup(fr / fps);
        if (camdump) { printf("%.3f %.3f %.3f %.3f\n", fr / fps, RO.x, RO.y, RO.z); continue; }
        #pragma omp parallel for schedule(dynamic, 4)
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                v3 c = V(0, 0, 0);
                for (int s = 0; s < aa; s++) {
                    float ox = aa > 1 ? jx[s % 4] : 0, oy = aa > 1 ? jy[s % 4] : 0;
                    float px = (2 * (x + 0.5f + ox) - W) / H, py = (H - 2 * (y + 0.5f + oy)) / H;
                    v3 rd = nrm(add(add(mul(CU, px), mul(CV, py)), mul(CW, FOV)));
                    c = add(c, shade(RO, rd, 0, 0));
                }
                c = mul(c, 1.0f / aa);
                float d = (hash2(x + fr * 0.37f, y) - 0.5f) / 255.0f;
                float r = powf(1 - expf(-fmaxf(c.x, 0) * 0.95f), 0.4545f) + d, g = powf(1 - expf(-fmaxf(c.y, 0) * 0.95f), 0.4545f) + d, b = powf(1 - expf(-fmaxf(c.z, 0) * 0.95f), 0.4545f) + d;
                unsigned char *o = buf + (y * W + x) * 3;
                o[0] = (unsigned char)(clampf(r, 0, 1) * 255); o[1] = (unsigned char)(clampf(g, 0, 1) * 255); o[2] = (unsigned char)(clampf(b, 0, 1) * 255);
            }
        fwrite(buf, 1, W * H * 3, stdout);
        if (fr % 25 == 0) fprintf(stderr, "%d\n", fr);
    }
    return 0;
}
