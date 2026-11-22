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
// Les raccourcis de calcul (« loin de cet objet, saute ») passent tous par P3D_EARLY : ./scene check les coupe (NOBOUND = 1)
// et compare l'image obtenue à l'image normale. Un raccourci trop optimiste fait disparaître un morceau d'objet : le contrôle le trouve.
// Règle : la valeur rendue par un raccourci ne doit jamais dépasser la vraie distance à l'objet.
static int NOBOUND = 0;
static int P3D_GROUND = 0;     // 1 : y = 0 est un sol infini. Un rayon qui descend sans rien toucher (pas épuisés en rasant le sol au loin) le touche quand même, calculé exactement
static __thread float P3D_HITT;       // distance du premier objet touché par le rayon de la caméra (1e9 : le ciel), pour POSTFX (faisceaux, particules)
#define P3D_EARLY(b, thr) do { if (!NOBOUND && (b) > (thr)) return (b); } while (0)

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

// ---- le cochon couronné : c'est le MODÈLE STANDARD n° 2 du moteur du film (../porc4.h), inclus tel quel, jamais recopié ni retouché.
// Groin fondu dans la tête, deux narines rondes toutes simples, couronne d'or à cinq pointes, queue en tire-bouchon, une seule matière pour le corps.
// Repère : pattes sur y = 0, regarde vers +z, environ 2,6 de long. ph = phase des pattes. bm : matière du corps (M_PINK, M_GOLD, M_CHROME).
// Noms du moteur du film, pour inclure ses fichiers tels quels (porc4.h, danseur.h) :
typedef V v3;
#define V(a, b, c) VV((a), (b), (c))
#define S static inline
#ifndef PI
#define PI 3.14159265f
#endif
S float len2(float a, float b) { return sqrtf(a * a + b * b); }
S v3 nrm(v3 a) { return norm(a); }
S v3 mix3(v3 a, v3 b, float t) { return mixv(a, b, t); }
S float sdEll(v3 p, v3 r) {
    float k0 = len(V(p.x / r.x, p.y / r.y, p.z / r.z));
    float k1 = len(V(p.x / (r.x * r.x), p.y / (r.y * r.y), p.z / (r.z * r.z)));
    return k0 * (k0 - 1.0f) / (k1 + 1e-6f);
}
S float sdCap(v3 p, v3 a, v3 b, float r) { return cap(p, a, b, r); }
S float box2(float dx, float dy) { return fminf(fmaxf(dx, dy), 0) + len2(fmaxf(dx, 0), fmaxf(dy, 0)); }
S float sdBox(v3 p, v3 b) { return box(p, b); }
S float cylY(v3 p, float r, float h) { return box2(len2(p.x, p.z) - r, fabsf(p.y) - h); }
S float cylZ(v3 p, float r, float h) { return box2(len2(p.x, p.y) - r, fabsf(p.z) - h); }
#include "../porc4.h"
static float pig(V p, float ph, int bm, int *m) {
    return p4_sdf(p, ph, 1.0f, bm, M_BLACK, M_GOLD, m);
}


// ---- foules et objets répétés sur une grille (plan xz) : une seule case évaluée, donc aussi rapide qu'une boucle écrite à la main,
// mais sans artefact. fn(l, cx, cz, ix, iz, m) : distance au contenu de la case (ix, iz) de centre (cx, 0, cz), l = p moins ce centre,
// 1e9 si la case est vide. rmax : distance horizontale maximale entre le centre d'une case et son contenu.
//  - le contenu doit tenir dans sa case (rmax <= cs / 2) : sinon ce qui dépasse dans la case voisine disparaît. grid_check() le vérifie
//    au démarrage et arrête le programme avec un message, au lieu de laisser passer un objet coupé ;
//  - le pas vers la case voisine est exact : distance au bord + (cs / 2 - rmax), jamais plus (l'ancien « + 0,45 » à la main pouvait
//    faire entrer un rayon dans le cochon d'à côté) ;
//  - la zone occupée par la foule se borne avec grid_zone_margin(rmax) : le bord de la zone recule de rmax (un cochon du premier rang
//    qui regarde vers l'intérieur dépasse de sa case : c'est son groin qui disparaissait).
typedef float (*p3d_cell_fn)(V l, float cx, float cz, float ix, float iz, int *m);
static void grid_check(const char *what, float cs, float rmax) {
    if (rmax > cs * 0.5f) { fprintf(stderr, "p3d : %s ne tient pas dans sa case (rmax %.2f > moitie de case %.2f) : agrandir la case ou reduire l'objet\n", what, rmax, cs * 0.5f); exit(3); }
}
static float grid_xz(V p, float cs, float rmax, p3d_cell_fn fn, int *m) {
    float ix = floorf(p.x / cs), iz = floorf(p.z / cs), cx = (ix + 0.5f) * cs, cz = (iz + 0.5f) * cs, d;
    if (NOBOUND) {                                                     // contrôle seulement : les 3 x 3 cases, sans pas de bord
        int mm; d = 1e9f;
        for (int i = -1; i <= 1; i++) for (int j = -1; j <= 1; j++) {
            float jx = ix + i, jz = iz + j, kx = (jx + 0.5f) * cs, kz = (jz + 0.5f) * cs;
            float e = fn(VV(p.x - kx, p.y, p.z - kz), kx, kz, jx, jz, &mm); if (e < d) { d = e; *m = mm; } }
        return d;
    }
    float lx = p.x - cx, lz = p.z - cz;
    float step = fminf(cs * 0.5f - fabsf(lx), cs * 0.5f - fabsf(lz)) + (cs * 0.5f - rmax) + 0.002f;
    d = fn(VV(lx, p.y, lz), cx, cz, ix, iz, m);
    return d < step ? d : step;
}
static inline float grid_zone_margin(float rmax) { return rmax; }

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
    float u = (x + 9.0f) / 18.0f * LW - 0.5f, v = (9.0f - y) / 9.0f * LH - 0.5f;
    u = clampf(u, 0, LW - 1.001f); v = clampf(v, 0, LH - 1.001f);
    int iu = (int)u, iv = (int)v; float fu = u - iu, fv = v - iv;
    float *r = LSDF + iv * LW + iu;
    float val = mixf(mixf(r[0], r[1], fu), mixf(r[LW], r[LW + 1], fu), fv);
    // hors du cadre : borne sûre. Les lettres sont dans le cadre (distance >= db), et à au moins val - db du point du cadre le plus proche.
    // (l'ancien « db + 0,25 » coupait les lettres posées sur le bas du cadre ; « db » seul faisait un mur fantôme sur le bord)
    return db > 0.0f ? fmaxf(db, val - db) : val;
}
static float letters(V p, float y0, float depth) {
    float d2 = lsdf(p.x, p.y - y0), dz = fabsf(p.z) - depth;
    return fminf(fmaxf(d2, dz), 0) + sqrtf(sq(fmaxf(d2, 0)) + sq(fmaxf(dz, 0))) - 0.03f;
}

static int base_material(int m, Mat *o) {
    switch (m) {
    case M_PINK: o->alb = VV(0.95f, 0.47f, 0.52f); o->f0 = 0.05f; o->shin = 40; o->rfl = 1; return 1;       // matière officielle du cochon standard
    case M_SNOUT: o->alb = VV(0.86f, 0.33f, 0.43f); return 1;
    case M_GOLD: o->alb = VV(1.0f, 0.74f, 0.30f); o->metal = 1; o->shin = 90; return 1;
    case M_LETTER: o->alb = VV(1.0f, 0.72f, 0.26f); o->metal = 1; o->shin = 90; o->emi = LETTER_EMI; return 1;
    case M_CHROME: o->alb = VV(0.92f, 0.93f, 0.96f); o->metal = 1; o->shin = 140; return 1;
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
    float t = 0.02f, d = 1e9f; int m = 0, hit = 0, steps = depth ? STEPS / 2 : STEPS, i;
    for (i = 0; i < steps; i++) {
        d = map(add(ro, mul(rd, t)), &m);
        if (d < 0.0012f * t + 0.0008f) { hit = 1; break; }
        t += d * 0.92f;
        if (t > MAXD) break;
    }
    if (!hit && i == steps && t < MAXD && d < 0.004f * t) hit = 1;       // pas épuisés à moins d'un pixel d'une surface : c'est un bord rasé, on le touche
    if (!hit && P3D_GROUND && rd.y < -1e-4f) { float tg = -ro.y / rd.y; if (tg > 0 && tg < MAXD) { t = tg; hit = 1; map(add(ro, mul(rd, t)), &m); } }
    if (depth == 0) P3D_HITT = hit ? t : 1e9f;
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


// ---- ./scene check F0 F1 PAS [coupes...] : chaque PAS image, rend la profondeur en 160 x 120 avec et sans raccourcis (NOBOUND)
// et compte les pixels qui diffèrent : un objet qui manque ou qui est coupé. Code de sortie 1 s'il y en a.
static __thread int P3D_MHIT; static __thread float P3D_NEAR;       // plus courte approche d'une surface avant l'arrêt du rayon (en largeur de pixel ~ 0,004)
static float p3d_march(V ro, V rd, int careful) {
    // passe normale : exactement les pas du rendu (c'est là que se cachent les sauts de trop) ; passe de référence : sans raccourci, à pas prudents
    float t = 0.02f, k = careful ? 0.5f : 0.92f, d = 1e9f, near = 1e9f; P3D_NEAR = 1e9f; int m, n = careful ? STEPS * 12 : STEPS, i;
    for (i = 0; i < n; i++) {
        d = map(add(ro, mul(rd, t)), &m);
        if (d < 0.0012f * t + 0.0008f) { P3D_MHIT = m; return t; }
        near = fminf(near, d / t); P3D_NEAR = near;
        t += d * k; if (t > MAXD) break;
    }
    if (!careful && i == n && t < MAXD && d < 0.004f * t) { P3D_MHIT = m; return t; }       // comme dans trace()
    if (P3D_GROUND && rd.y < -1e-4f) { float tg = -ro.y / rd.y; if (tg > 0 && tg < MAXD) { map(add(ro, mul(rd, tg)), &m); P3D_MHIT = m; return tg; } }       // comme dans trace()
    P3D_MHIT = near < 0.004f ? -2 : -1;       // -2 : raté, mais passé à moins d'un pixel d'une surface (un bord frôlé, pas un trou)
    return -1;
}
static int p3d_check(int argc, char **argv) {
    if (argc < 5) { fprintf(stderr, "usage: %s check F0 F1 PAS [coupes...]\n", argv[0]); return 2; }
    int F0 = atoi(argv[2]), F1 = atoi(argv[3]), st = atoi(argv[4]), bad = 0; const int CW = 160, CH = 120;
    for (NCUT = 0; NCUT < argc - 5 && NCUT < 16; NCUT++) CUT[NCUT] = atof(argv[5 + NCUT]);
    scene_init();
    static float d0[160 * 120], d1[160 * 120], nr0[160 * 120]; static int m0[160 * 120], m1[160 * 120];
    for (int f = F0; f < F1; f += st) {
        T = f / FPS; V cam, tgt; float focal; frame_setup(&cam, &tgt, &focal);
        V fw = norm(sub(tgt, cam)), rt = norm(cross(VV(0, 1, 0), fw)), up = cross(fw, rt);
        for (int pass = 0; pass < 2; pass++) {
            NOBOUND = pass; float *D = pass ? d1 : d0;
            #pragma omp parallel for schedule(dynamic, 4)
            for (int y = 0; y < CH; y++) for (int x = 0; x < CW; x++) {
                float px = (x + 0.5f) / CW * 2 - 1, py = 1 - (y + 0.5f) / CH * 2;
                D[y * CW + x] = p3d_march(cam, norm(add(mul(fw, focal), add(mul(rt, px * (float)W / H), mul(up, py)))), pass);
                (pass ? m1 : m0)[y * CW + x] = P3D_MHIT; if (!pass) nr0[y * CW + x] = P3D_NEAR;
            }
        }
        NOBOUND = 0;
        static unsigned char df[160 * 120]; int n = 0, ex = 0, ey = 0;
        for (int i = 0; i < CW * CH; i++) { float a = d0[i], b = d1[i];
            // un objet coupé : le rayon rate là où la référence touche, ou bien il touche autre chose (le groin coupé laisse voir la tête derrière)
            df[i] = ((a < 0) != (b < 0) && !(a < 0 && m0[i] == -2)) || (a > 0 && b > 0 && m0[i] != m1[i] && fabsf(a - b) > 0.02f * b + 0.02f);
            if (b > 0 && (a < 0 || a > b) && nr0[i] < 0.004f) df[i] = 0;       // le rayon a frôlé l'objet à moins d'un pixel : un bord, pas un trou
            if (fmaxf(a, b) > 0.4f * MAXD) df[i] = 0; }       // l'horizon lointain (sol rasant, dans le brouillard) : le rendu y manque de pas, ce n'est pas un objet coupé
        for (int i = 0; i < CW * CH; i++) {       // un pixel compte s'il a un voisin qui diffère aussi : un vrai trou fait une tache
            if (!df[i]) continue; int x = i % CW, y = i / CW;
            int nb = (x > 0 && df[i - 1]) || (x < CW - 1 && df[i + 1]) || (y > 0 && df[i - CW]) || (y < CH - 1 && df[i + CW]);
            if (nb) { if (!n) { ex = x; ey = y; } n++; }
        }
        if (n) {       // on dit quel objet manque (vu par le calcul sans raccourci) et où il est
            bad++; float px = (ex + 0.5f) / CW * 2 - 1, py = 1 - (ey + 0.5f) / CH * 2; int i = ey * CW + ex, mm = -1;
            V rd = norm(add(mul(fw, focal), add(mul(rt, px * (float)W / H), mul(up, py)))); float tt = d1[i] > 0 ? d1[i] : d0[i];
            V hp = add(cam, mul(rd, tt)); NOBOUND = 1; map(hp, &mm); NOBOUND = 0;
            printf("image %d (%.2f s) : %d pixels differents ; ex. (%d, %d) : avec raccourcis %.2f, sans %.2f, objet %d en (%.2f, %.2f, %.2f)\n", f, T, n, ex, ey, d0[i], d1[i], mm, hp.x, hp.y, hp.z); fflush(stdout); }
    }
    printf(bad ? "CONTROLE : %d images avec un objet coupe ou manquant\n" : "CONTROLE : aucun artefact de raccourci\n", bad);
    return bad ? 1 : 0;
}

int main(int argc, char **argv) {
    if (argc < 3) { fprintf(stderr, "usage: %s F0 F1 [cut1 cut2 ...] > frames.rgb\n", argv[0]); return 1; }
    if (!strcmp(argv[1], "check")) return p3d_check(argc, argv);
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
