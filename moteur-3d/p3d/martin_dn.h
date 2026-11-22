// Adaptateur : le Danseur (Martin Plaisir) du moteur du film, branché dans p3d.h. danseur.h, danseur_mat.h et ml_dance sont repris tels quels.
// (v3, V(), S, PI, len2, sdEll, sdCap, box2, cylY, cylZ... sont maintenant dans p3d.h, avec le cochon standard)
#define UN(dd, mm) { float _d = (dd); if (_d < d) { d = _d; *m = (mm); } }
S float ease(float x) { x = clampf(x, 0, 1); return x * x * (3 - 2 * x); }
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
