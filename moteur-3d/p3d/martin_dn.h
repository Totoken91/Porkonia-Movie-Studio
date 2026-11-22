// Adaptateur : le Danseur (Martin Plaisir) du moteur du film, branché dans p3d.h. danseur.h, danseur_mat.h et ml_dance sont repris tels quels.
typedef V v3;
#define V(a, b, c) VV((a), (b), (c))
#define S static inline
#define PI 3.14159265f
#define UN(dd, mm) { float _d = (dd); if (_d < d) { d = _d; *m = (mm); } }
S float len2(float a, float b) { return sqrtf(a * a + b * b); }
S v3 nrm(v3 a) { return norm(a); }
S v3 mix3(v3 a, v3 b, float t) { return mixv(a, b, t); }
S float ease(float x) { x = clampf(x, 0, 1); return x * x * (3 - 2 * x); }
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
S v3 cz_loc(v3 d, v3 ax, v3 ay, v3 az) { return V(dot(d, ax), dot(d, ay), dot(d, az)); }
S v3 cz_rot(v3 v, float yaw, float pitch, float roll) {
    float c = cosf(roll), s = sinf(roll); v = V(v.x * c - v.y * s, v.x * s + v.y * c, v.z);
    c = cosf(pitch); s = sinf(pitch); v = V(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
    c = cosf(yaw); s = sinf(yaw); return V(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}
#include "../danseur.h"
typedef struct { v3 alb, emis; float spec, gloss, sss, env; } mat_t;
#include "../danseur_mat.h"
#pragma push_macro("W")
#pragma push_macro("H")
#undef W
#undef H
#include "ml_dance.h"
#include "ml_sexy.h"
#pragma pop_macro("H")
#pragma pop_macro("W")
static void dn_to_mat(V p, int m, Mat *o) {       // matières du Danseur -> matières de p3d.h
    mat_t M = {{0.5f, 0.5f, 0.5f}, {0, 0, 0}, 0.2f, 20, 0, 0}; dn_material(p, m, &M);
    o->alb = M.alb; o->emi = M.emis; o->shin = fmaxf(M.gloss, 4);
    if (m == D_GOLD) { o->metal = 1; o->shin = 90; return; }
    o->f0 = 0.03f * M.spec; o->rfl = fminf(M.spec, 1.0f) * 0.55f;
}
