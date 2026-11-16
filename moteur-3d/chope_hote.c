// ================================================================ LA CHOPE STANDARD (chope.h) DANS LE MOTEUR DU FILM
// De quoi poser jusqu'à six chopes de Douzi Ambrée dans une séquence « w24 » : chacune à sa place, à sa taille, tournée et penchée comme on veut.
// Dans le réglage d'une séquence (xxx_setup) :
//     ch_put(position, échelle, lacet, inclinaison, fill, foam, cold, lace, drip, sous-bock);
//   position : le point où le verre touche son support (le sous-bock descend 0,004 x échelle plus bas) ; échelle : 1 = taille réelle (16,5 cm),
//   1,5 quand un citoyen la tient ou qu'elle est posée près d'eux (sinon elle paraît minuscule à côté de leurs têtes), 10 pour une image de studio,
//   24 pour un monument ; lacet : 0 = l'anse vers +x, la marque face à -z ; inclinaison : pour trinquer.
//   Pour la mettre dans une main : I->pos = main - (I->ex * 0,090 + I->ey * 0,083) x échelle (le milieu de l'anse).
// Trois crochets, déjà en place dans ce moteur (ils ne coûtent rien tant qu'aucune chope n'est posée) :
//   la distance : wmap() ajoute ch_mapd(p, m, d) à la scène ; la couleur : if (m == W_CHOPE) col = ch_shade(ro, rd, p, depth) ;
//   la lumière : L = add(L, ch_glow(p, n)) (la lueur d'ambre que la bière jette à son pied). NCH est remis à zéro par setupw à chaque image.
enum { W_CHOPE = 480 };
static v3 wshade(v3 ro, v3 rd, int depth, float *zout); static v3 wlight(v3 p, v3 n, float ao, float sun);
typedef struct { float fill, foam, cold, lace, drip, t; int coaster; } chope_t;
#define NCHI 6
typedef struct { v3 pos, ex, ey, ez; float sc; chope_t c; } chi_t;
static chi_t CHI[NCHI]; static int NCH; static __thread int NOCH; static __thread const chi_t *CHCUR;
S v3 chi_ld(const chi_t *I, v3 d) { return V(dot(d, I->ex), dot(d, I->ey), dot(d, I->ez)); }
S v3 chi_l(const chi_t *I, v3 p) { return mul(chi_ld(I, sub(p, I->pos)), 1 / I->sc); }
S v3 chi_wd(const chi_t *I, v3 d) { return add(add(mul(I->ex, d.x), mul(I->ey, d.y)), mul(I->ez, d.z)); }
S v3 chi_w(const chi_t *I, v3 l) { return add(I->pos, mul(chi_wd(I, l), I->sc)); }
static v3 ch_env(v3 lo, v3 ld) { float z; NOCH++; v3 c = wshade(chi_w(CHCUR, lo), chi_wd(CHCUR, ld), 1, &z); NOCH--; return c; }
static v3 ch_light(v3 lp, v3 ln) { v3 n = chi_wd(CHCUR, ln); return wlight(chi_w(CHCUR, lp), n, 1.0f, fmaxf(dot(n, WSUN), 0) * (WSUNC.x > 0.01f)); }
static v3 ch_spec(v3 lp, v3 lr) { v3 p = chi_w(CHCUR, lp), r = chi_wd(CHCUR, lr), s = mul(WSUNC, 3.0f * powf(fmaxf(dot(r, WSUN), 0), 400.0f));
    for (int i = 0; i < WNL; i++) { v3 lv = sub(V(WLT[i][0], WLT[i][1], WLT[i][2]), p); float d2 = dot(lv, lv); s = add(s, mul(V(WLT[i][3], WLT[i][4], WLT[i][5]), WLT[i][6] / (1 + WLT[i][7] * d2) * 2.5f * powf(fmaxf(dot(r, lv) / sqrtf(d2), 0), 180.0f))); }
    return s; }
static float ch_etch(float a, float y) {       // DOUZI AMBRÉE, dépoli sur le bandeau, face à -z
    if (y < 0.134f || y > 0.160f) return 0; float sf = 0.0105f / 67.0f, da = a + 1.5707963f; if (da > 3.14159265f) da -= 6.2831853f;
    return sstep(0.7f, -0.7f, text2d(T_AMBREE, da * 0.0475f / sf + TR[T_AMBREE].w * 0.5f, TR[T_AMBREE].base - (y - 0.1415f) / sf));
}
#define CH_ENV(o, d) ch_env(o, d)
#define CH_LIGHT(p, n) ch_light(p, n)
#define CH_SPEC(p, r) ch_spec(p, r)
#define CHOPE_ETCH(a, y) ch_etch(a, y)
#define CHOPE_T_DEFINED
#include "chope.h"
static void ch_put(v3 pos, float sc, float yaw, float tilt, float fill, float foam, float cold, float lace, float drip, int coaster) {
    if (NCH >= NCHI) return; chi_t *I = &CHI[NCH++]; float c = cosf(yaw), s = sinf(yaw), ct = cosf(tilt), st = sinf(tilt); v3 ex0 = V(c, 0, -s);
    I->ex = add(mul(ex0, ct), V(0, st, 0)); I->ey = add(mul(ex0, -st), V(0, ct, 0)); I->ez = V(s, 0, c); I->pos = pos; I->sc = sc; I->c = (chope_t){fill, foam, cold, lace, drip, T, coaster};
}
__attribute__((noinline)) static float ch_mapd(v3 p, int *m, float d) {
    if (NOCH) return d;
    for (int k = 0; k < NCH; k++) { const chi_t *I = &CHI[k]; float bd = len(sub(p, chi_w(I, CH_C))) - 0.125f * I->sc;
        if (bd > 0.03f * I->sc) { d = fminf(d, bd); continue; }
        float e = ch_hull(&I->c, chi_l(I, p)) * I->sc * 0.9f; if (e < d) { d = e; *m = W_CHOPE; } }
    return d;
}
__attribute__((noinline)) static v3 ch_shade(v3 ro, v3 rd, v3 p, int depth) {
    int kb = 0; float db = 1e9f; for (int k = 0; k < NCH; k++) { float e = len(sub(p, chi_w(&CHI[k], CH_C))) / CHI[k].sc; if (e < db) { db = e; kb = k; } }
    const chi_t *I = &CHI[kb], *sv = CHCUR; v3 lo = chi_l(I, ro), ld = chi_ld(I, rd), lp = chi_l(I, p), c; CHCUR = I;
    if (depth > 0) { int md = ch_med(&I->c, add(lp, mul(ld, 0.004f))); v3 n = nrm(V(lp.x, 0.2f, lp.z));       // dans un reflet : la mousse, l'ambre, sans optique
        c = md == CHM_FOAM ? ch_foamshade(&I->c, lp, n) : md == CHM_COASTER ? mulv(V(0.6f, 0.4f, 0.3f), ch_light(lp, V(0, 1, 0))) : add(mul(mulv(CH_BEERC, ch_light(lp, n)), lp.y < ch_yb(&I->c) ? 0.7f : 0.12f), ch_spec(lp, refl(ld, n))); }
    else c = chope_trace(&I->c, lo, ld, fmaxf(dot(sub(lp, lo), ld) - 0.004f, 0));
    CHCUR = sv; return c;
}
__attribute__((noinline)) static v3 ch_glow(v3 p, v3 n) {       // au pied d'une chope pleine : la lumière ressort en ambre, à l'opposé de la source
    v3 L = V(0, 0, 0);
    for (int k = 0; k < NCH; k++) { const chi_t *I = &CHI[k]; float sc = I->sc, dy = p.y - I->pos.y; if (fabsf(dy) > 0.03f * sc || n.y < 0.5f || I->c.fill < 0.05f) continue;
        v3 dl = WSUNC.x > 0.01f ? WSUN : V(0.35f, 1, -0.6f); float l2 = len2(dl.x, dl.z) + 1e-4f, ux = -dl.x / l2, uz = -dl.z / l2, ox = p.x - I->pos.x - ux * 0.105f * sc, oz = p.z - I->pos.z - uz * 0.105f * sc,
              al = (ox * ux + oz * uz) / (0.055f * sc), ac = (ox * uz - oz * ux) / (0.026f * sc), g = expf(-(al * al + ac * ac));
        L = add(L, mul(mulv(V(1.0f, 0.50f, 0.09f), add(WSUNC, mul(WAMBU, 1.5f))), 2.2f * g * I->c.fill)); }
    return L;
}
