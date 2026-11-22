// Porkonia 3D: moteur de rendu par raymarching (champs de distance). Une scene = un fichier .c qui inclut ce fichier
// et definit map(), sky(), material(), scene_init() et frame_setup().
// Usage du binaire : ./scene F0 F1 c1 c2 ... > frames.rgb   (images 512x384 rgb24, 25 i/s ; c1.. = temps des coupes en s)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <omp.h>
#define W 512
#define H 384
#define FPS 25.0f
typedef struct { float x, y, z; } V;
#define VV(a,b,c) ((V){(a),(b),(c)})
static inline V add(V a, V b) { return VV(a.x + b.x, a.y + b.y, a.z + b.z); }
static inline V sub(V a, V b) { return VV(a.x - b.x, a.y - b.y, a.z - b.z); }
static inline V mul(V a, float s) { return VV(a.x * s, a.y * s, a.z * s); }
static inline V mulv(V a, V b) { return VV(a.x * b.x, a.y * b.y, a.z * b.z); }
static inline float dot(V a, V b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
static inline float len(V a) { return sqrtf(dot(a, a)); }
static inline V norm(V a) { return mul(a, 1.0f / (len(a) + 1e-9f)); }
static inline V cross(V a, V b) { return VV(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
static inline V mixv(V a, V b, float t) { return add(mul(a, 1 - t), mul(b, t)); }
static inline float clampf(float x, float a, float b) { return x < a ? a : (x > b ? b : x); }
static inline float mixf(float a, float b, float t) { return a + (b - a) * t; }
static inline float sstep(float a, float b, float x) { float t = clampf((x - a) / (b - a), 0, 1); return t * t * (3 - 2 * t); }
static inline float sq(float x) { return x * x; }
static inline float fract(float x) { return x - floorf(x); }
static inline float hash2(float x, float y) { return fract(sinf(x * 127.1f + y * 311.7f) * 43758.5453f); }

enum { M_PINK = 1, M_SNOUT, M_GOLD, M_BLACK, M_CHROME, M_RIBBON, M_LETTER, M_USER = 20 };
typedef struct { V alb, emi; int metal, rtrace, unlit; float f0, shin, rfl; } Mat;

static float T, CUT[16]; static int NCUT;
static V LDIR, SUNC, AMBU, AMBD, AMBC, RIMC, RIMD, FOGC, LETTER_EMI;
static float FOGD = 0.002f, MAXD = 300, EXPO = 1.0f, CONTRAST = 0.5f, METAL_LIT = 0.3f, METAL_ENV = 0.95f;
static int STEPS = 150;
static int PIG_NOSHADOW = 0;      // 1 : la peau des cochons couronnés ne reçoit pas d'ombre (le groin y faisait une tache dure, « gravée ») ; leurs ombres au sol restent

static float map(V p, int *m);
static V sky(V rd);
static void material(int m, V p, V n, Mat *o);
static void scene_init(void);
static void frame_setup(V *cam, V *tgt, float *focal);
#ifdef POSTFX
static V POSTFX(V ro, V rd, V c);                            // optionnel : effet ajoute par la scene (halo...)
#endif

static float sph(V p, V c, float r) { return len(sub(p, c)) - r; }
static float ell(V p, V c, V r) {
    V q = sub(p, c);
    float k0 = len(VV(q.x / r.x, q.y / r.y, q.z / r.z));
    float k1 = len(VV(q.x / (r.x * r.x), q.y / (r.y * r.y), q.z / (r.z * r.z)));
    return k1 < 1e-6f ? -fminf(r.x, fminf(r.y, r.z)) : k0 * (k0 - 1) / k1;
}
static float cap(V p, V a, V b, float r) {
    V pa = sub(p, a), ba = sub(b, a); float h = clampf(dot(pa, ba) / dot(ba, ba), 0, 1);
    return len(sub(pa, mul(ba, h))) - r;
}
static float smin(float a, float b, float k) { float h = fmaxf(k - fabsf(a - b), 0) / k; return fminf(a, b) - h * h * k * 0.25f; }
static float cylz(V q, float r, float hh) {
    float dx = sqrtf(q.x * q.x + q.y * q.y) - r, dz = fabsf(q.z) - hh;
    return fminf(fmaxf(dx, dz), 0) + sqrtf(sq(fmaxf(dx, 0)) + sq(fmaxf(dz, 0)));
}
static float box(V q, V b) {
    V d = VV(fabsf(q.x) - b.x, fabsf(q.y) - b.y, fabsf(q.z) - b.z);
    return fminf(fmaxf(d.x, fmaxf(d.y, d.z)), 0) + len(VV(fmaxf(d.x, 0), fmaxf(d.y, 0), fmaxf(d.z, 0)));
}

static float pig(V p, float ph, int bm, int *m) {
    float bnd = len(sub(p, VV(0, 1.0f, 0.2f))) - 1.75f;
    *m = bm;
    if (bnd > 0.5f) return bnd;
    p.x = fabsf(p.x);
    float s = sinf(ph);
    float d = ell(p, VV(0, 0.95f, 0), VV(0.62f, 0.6f, 0.95f));
    d = smin(d, sph(p, VV(0, 1.15f, 0.95f), 0.48f), 0.15f);
    d = smin(d, sph(p, VV(0.3f, 1.58f, 0.85f), 0.16f), 0.05f);
    d = fminf(d, cap(p, VV(0.33f, 0.55f, 0.5f + 0.15f * s), VV(0.33f, 0.13f, 0.5f + 0.4f * s), 0.13f));
    d = fminf(d, cap(p, VV(0.33f, 0.55f, -0.5f - 0.15f * s), VV(0.33f, 0.13f, -0.5f - 0.4f * s), 0.13f));
    d = fminf(d, sph(p, VV(0, 1.15f, -0.98f), 0.11f));
    float e = cylz(sub(p, VV(0, 1.08f, 1.45f)), 0.2f, 0.1f) - 0.02f;
    if (e < d) { d = e; *m = M_SNOUT; }
    e = sph(p, VV(0.2f, 1.33f, 1.34f), 0.07f);
    if (e < d) { d = e; *m = M_BLACK; }
    V q = sub(p, VV(0, 1.68f, 0.95f)); float rr = sqrtf(q.x * q.x + q.z * q.z);
    e = fmaxf(fabsf(rr - 0.24f) - 0.03f, fabsf(q.y) - 0.09f);
    e = fminf(e, len(sub(VV(q.x, q.y, fabsf(q.z)), VV(0.17f, 0.14f, 0.17f))) - 0.07f);
    if (e < d) { d = e; *m = (bm == M_GOLD) ? M_RIBBON : M_GOLD; }
    return d;
}

static void *slurp(const char *f, size_t n) {
    void *b = malloc(n); FILE *fp = fopen(f, "rb");
    if (!fp || fread(b, 1, n, fp) != n) { fprintf(stderr, "cannot read %s\n", f); exit(1); }
    fclose(fp); return b;
}
static V texel(const unsigned char *t, int w, int h, float u, float v) {
    int x = (int)clampf(u * w, 0, w - 1), y = (int)clampf(v * h, 0, h - 1);
    const unsigned char *c = t + (y * w + x) * 3;
    return VV(powf(c[0] / 255.0f, 2.2f), powf(c[1] / 255.0f, 2.2f), powf(c[2] / 255.0f, 2.2f));
}
static float *LSDF;
#define LW 1024
#define LH 512
static float lsdf(float x, float y) {
    float bx = fabsf(x) - 9.0f, by = fabsf(y - 4.5f) - 4.5f;
    float db = fminf(fmaxf(bx, by), 0) + sqrtf(sq(fmaxf(bx, 0)) + sq(fmaxf(by, 0)));
    if (db > 0.0f) return db + 0.25f;
    float u = (x + 9.0f) / 18.0f * LW - 0.5f, v = (9.0f - y) / 9.0f * LH - 0.5f;
    u = clampf(u, 0, LW - 1.001f); v = clampf(v, 0, LH - 1.001f);
    int iu = (int)u, iv = (int)v; float fu = u - iu, fv = v - iv;
    float *r = LSDF + iv * LW + iu;
    return mixf(mixf(r[0], r[1], fu), mixf(r[LW], r[LW + 1], fu), fv);
}
static float letters(V p, float y0, float depth) {
    float d2 = lsdf(p.x, p.y - y0), dz = fabsf(p.z) - depth;
    return fminf(fmaxf(d2, dz), 0) + sqrtf(sq(fmaxf(d2, 0)) + sq(fmaxf(dz, 0))) - 0.03f;
}

static int base_material(int m, Mat *o) {
    switch (m) {
    case M_PINK: o->alb = VV(0.95f, 0.47f, 0.55f); o->f0 = 0.06f; o->shin = 60; o->rfl = 1; return 1;
    case M_SNOUT: o->alb = VV(0.86f, 0.33f, 0.43f); return 1;
    case M_GOLD: o->alb = VV(1.0f, 0.72f, 0.26f); o->metal = 1; o->shin = 90; return 1;
    case M_LETTER: o->alb = VV(1.0f, 0.72f, 0.26f); o->metal = 1; o->shin = 90; o->emi = LETTER_EMI; return 1;
    case M_CHROME: o->alb = VV(0.92f, 0.92f, 0.96f); o->metal = 1; o->shin = 140; return 1;
    case M_BLACK: o->alb = VV(0.02f, 0.02f, 0.02f); o->shin = 120; return 1;
    case M_RIBBON: o->alb = VV(0.38f, 0.03f, 0.07f); return 1;
    }
    return 0;
}

static V normal(V p) {
    const float e = 0.004f; int m;
    V n = VV(0, 0, 0);
    n = add(n, mul(VV(1, -1, -1), map(add(p, VV(e, -e, -e)), &m)));
    n = add(n, mul(VV(-1, -1, 1), map(add(p, VV(-e, -e, e)), &m)));
    n = add(n, mul(VV(-1, 1, -1), map(add(p, VV(-e, e, -e)), &m)));
    n = add(n, mul(VV(1, 1, 1), map(add(p, VV(e, e, e)), &m)));
    return norm(n);
}
static float shadow(V ro) {
    float r = 1, t = 0.05f; int m;
    for (int i = 0; i < 40 && t < 40; i++) {
        float d = map(add(ro, mul(LDIR, t)), &m);
        if (d < 0.002f) return 0;
        r = fminf(r, 9.0f * d / t); t += clampf(d, 0.03f, 2.5f);
    }
    return clampf(r, 0, 1);
}

static V trace(V ro, V rd, int depth) {
    float t = 0.02f; int m = 0, hit = 0, steps = depth ? STEPS / 2 : STEPS;
    for (int i = 0; i < steps; i++) {
        float d = map(add(ro, mul(rd, t)), &m);
        if (d < 0.0012f * t + 0.0008f) { hit = 1; break; }
        t += d * 0.92f;
        if (t > MAXD) break;
    }
    if (!hit) return sky(rd);
    V p = add(ro, mul(rd, t)), n = normal(p);
    Mat o = { VV(0.5f, 0.5f, 0.5f), VV(0, 0, 0), 0, 0, 0, 0.04f, 40, 0 };
    if (!base_material(m, &o)) material(m, p, n, &o);
    float fog = 1 - expf(-t * FOGD);
    if (o.unlit) return mixv(o.emi, FOGC, fog * 0.5f);
    float sh = (PIG_NOSHADOW && (m == M_PINK || m == M_SNOUT || m == M_BLACK)) ? 1.0f : shadow(add(p, mul(n, 0.02f)));
    float nl = fmaxf(dot(n, LDIR), 0) * sh;
    V amb = add(add(mul(AMBU, 0.5f + 0.5f * n.y), mul(AMBD, 0.5f - 0.5f * n.y)), add(AMBC, mul(RIMC, fmaxf(dot(n, RIMD), 0))));
    V lit = add(mul(SUNC, nl), amb);
    float spec = powf(fmaxf(dot(n, norm(sub(LDIR, rd))), 0), o.shin) * sh;
    V rr = sub(rd, mul(n, 2 * dot(rd, n)));
    V col;
    if (o.metal) {
        V env = depth < 1 ? trace(add(p, mul(n, 0.02f)), rr, depth + 1) : sky(rr);
        col = add(mulv(o.alb, add(mul(lit, METAL_LIT), mul(env, METAL_ENV))), mul(mulv(o.alb, SUNC), spec * 1.6f));
    } else {
        col = mulv(o.alb, lit);
        if (o.rfl > 0) {
            float k = (o.f0 + (1 - o.f0) * powf(1 - fmaxf(-dot(n, rd), 0), 5)) * o.rfl;
            V env = (depth < 1 && o.rtrace) ? trace(add(p, mul(n, 0.02f)), rr, depth + 1) : sky(rr);
            col = add(mul(col, 1 - k), mul(env, k * (o.rtrace ? 1.0f : 0.8f)));
        }
        col = add(col, mul(SUNC, spec * 0.35f));
    }
    return mixv(add(col, o.emi), FOGC, fog);
}

static inline float tone(float c) { c = powf(1 - expf(-c * EXPO), 0.4545f); return mixf(c, c * c * (3 - 2 * c), CONTRAST); }

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s F0 F1 [cut1 cut2 ...] > frames.rgb\n", argv[0]); return 1; }
    int F0 = atoi(argv[1]), F1 = atoi(argv[2]);
    for (NCUT = 0; NCUT < argc - 3 && NCUT < 16; NCUT++) CUT[NCUT] = atof(argv[3 + NCUT]);
    scene_init();
    unsigned char *buf = malloc(W * H * 3);
    for (int f = F0; f < F1; f++) {
        T = f / FPS;
        V cam, tgt; float focal;
        frame_setup(&cam, &tgt, &focal);
        V fw = norm(sub(tgt, cam)), rt = norm(cross(VV(0, 1, 0), fw)), up = cross(fw, rt);
        #pragma omp parallel for schedule(dynamic, 4)
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                V acc = VV(0, 0, 0);
                for (int s = 0; s < 2; s++) {
                    float px = (x + 0.25f + 0.5f * s) / W * 2 - 1, py = 1 - (y + 0.25f + 0.5f * s) / H * 2;
                    V rdir = norm(add(mul(fw, focal), add(mul(rt, px * (float)W / H), mul(up, py))));
                    V c = trace(cam, rdir, 0);
#ifdef POSTFX
                    c = POSTFX(cam, rdir, c);
#endif
                    acc = add(acc, VV(tone(c.x), tone(c.y), tone(c.z)));
                }
                unsigned char *o = buf + (y * W + x) * 3;
                o[0] = (unsigned char)(127.5f * acc.x); o[1] = (unsigned char)(127.5f * acc.y); o[2] = (unsigned char)(127.5f * acc.z);
            }
        fwrite(buf, 1, W * H * 3, stdout);
        if (f % 25 == 0) fprintf(stderr, "%d\n", f);
    }
    return 0;
}
