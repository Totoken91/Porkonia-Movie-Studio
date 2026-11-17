static cz_t CZC[24];       // les vingt-quatre allures de la foule du clip (17 nov. 2026) : les douze du film, puis douze variantes (coupe, couleurs, tenue)
// ================================================================ LE CLIP : « Sous la neige de Douzi City » (DJ Viteau), 268 s, 90,9 temps à la minute.
// Trois décors : 0 le Bal des Neiges (la salle), 1 Douzi City sous la neige (dehors), 2 l'espace des vitrines (damier, disque géant, tunnel, mur d'enceintes).
// Temps : 900 + 100 x numéro de plan. L'instant du morceau, la durée du plan, l'énergie, les couleurs, le motif et la variante viennent de la variable CLIP.
enum { W_CFLOOR = 400, W_CWALL, W_CSTAGE, W_CCONS, W_CVINYL, W_CMIX, W_CSPK, W_CNEON, W_CBALL, W_CBAR, W_CBARTOP, W_CSHELF, W_CSTOOL, W_CTABLE, W_CBEER, W_CGRILL, W_CSAUS, W_CMARB, W_CCHROME, W_CSPOT,
       W_CSNOW, W_CBLDG, W_CHALL, W_CLAMP, W_CLANT, W_CTRAIN, W_CVIAD, W_CSMAN, W_CSNOUT, W_CDISC, W_CRING, W_CCHECK, W_CSPKW, W_CPAD, W_CGOLDL, W_CLENS, W_CSTUDIO, W_CROOF, W_CGARL, W_CPROJ };
static int CPIGS = 1; static float CCR = 2.6f; static int CK, CSET, CHV, CPAT, CTH, CCPL, CCHEN, CGIANT, CTRON, CMBON, CNRING = 10, CVAR, CDJ = -1; static float DJQ, DJZ, DJT;
static float CTS, CD = 8, CE = 0.6f, CB, CBT, CBO, CDENS = 0.75f, CHEART, CSPOTK = 1, CSPOTL, CCHX, CDA, CGA, CTRX, CDAWN, CVU, CSCR, CGLY, CJ, CJA, CLITK = 0.22f, CBEAMK = 1, CHSUN = 1;
static v3 CMB[6], CSPH[3], CBALLC = {0, 5.0f, 0}, CSUND = {0, 0.1f, 1};
static const v3 CPALS[4][4] = {{{1.0f, 0.12f, 0.55f}, {0.10f, 0.75f, 1.0f}, {1.0f, 0.55f, 0.08f}, {0.25f, 1.0f, 0.30f}},        // la fête
                               {{0.30f, 0.50f, 1.0f}, {0.72f, 0.90f, 1.0f}, {0.55f, 0.32f, 1.0f}, {0.12f, 0.85f, 0.95f}},       // la neige
                               {{1.0f, 0.22f, 0.10f}, {1.0f, 0.60f, 0.10f}, {1.0f, 0.86f, 0.42f}, {1.0f, 0.18f, 0.52f}},        // la chaleur
                               {{0.80f, 0.05f, 0.16f}, {1.0f, 0.74f, 0.30f}, {1.0f, 0.92f, 0.78f}, {0.95f, 0.40f, 0.50f}}};     // Porkonia : bordeaux, or, crème, rose
#define CPAL CPALS[CTH]
#include "danseur.h"
S float c_rep(float x, float c) { return x - c * roundf(x / c); }
S float c_repc(float x, float c, float a, float b, float *k) { *k = clampf(roundf(x / c), a, b); return x - c * *k; }
S float c_cz(const cz_t *C, const rig_t *R, const pose_t *P, v3 l, int *m, float d) {       // un citoyen dans son repère : sa sphère d'abord, sa forme ensuite
    int mm; float sc = C->sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc, e = bd > 0.65f ? bd : fmaxf(cz_sdf(C, R, P, l, &mm) * sc, bd);
    if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } return d;
}

S float dj_bling(v3 p, int *m, float d) {       // le bling de DJ Viteau : des verres fumés dans ses cercles d'or, une chaîne en or, sa médaille
    int k = CDJ; float sc = WCZ[k].sc; v3 l = w_local(p, k); if ((len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc > 0.3f) return d;
    const rig_t *R = &WRG[k]; v3 q = cz_loc(sub(l, R->ot), R->tx, R->ty, R->tz), h = mul(cz_loc(sub(q, R->nk), R->hx, R->hy, R->hz), 1 / R->HS);
    UN(fmaxf(len2(fabsf(h.x) - 0.160f, h.y - 0.318f) - 0.097f, fabsf(h.z - 0.326f) - 0.007f) * R->HS * sc, W_CLENS)
    UN(fmaxf(len2(q.x, q.y - DJQ) - 0.062f, fabsf(q.z - DJZ - 0.014f) - 0.011f) * sc, W_PGOLD)                                                   // la médaille, sur le ventre
    UN((sdCap(V(fabsf(q.x), q.y, q.z), V(0.035f, DJQ + 0.055f, DJZ + 0.010f), V(0.15f, DJT, DJZ - 0.05f), 0.012f) - 0.005f * sinf(q.y * 150.0f)) * sc * 0.75f, W_PGOLD)         // la chaîne : elle remonte sous le menton
    return d;
}
// ---------------------------------------------------------------- décor 0 : le Bal des Neiges. La salle : 18 m sur 22, 6,5 m sous plafond ; l'estrade au fond (+z).
#define BAX 9.0f
#define BAZ0 -10.0f
#define BAZ1 12.0f
#define BAH 6.5f
#define STY 0.5f
static __thread float CFIX, CFIZ;       // la case du danseur touché
S int bal_fan(v3 p, v3 *l) {       // la piste : un danseur par case de 1,4 m ; -1 : personne. (17 nov. 2026, « tous alignés bizarre ») la grille est tournée de 12 degrés par rapport à la salle,
    // chaque colonne est décalée en profondeur à sa façon et chacun s'écarte de sa case : plus de rangs ni de files à l'œil. Seule une partie de la foule saute, chacun à son moment, sans à-coup.
    const float c0 = 0.9781f, s0 = 0.2079f; float qx = p.x * c0 - p.z * s0, qz = p.x * s0 + p.z * c0;
    float ix = clampf(floorf(qx / 1.4f), -3, 2), oz = 0.95f * (hash2(ix * 2.3f + 0.7f, 7.3f) - 0.5f), iz = clampf(floorf((qz + 0.2f - oz) / 1.4f), -4, 3), cx = (ix + 0.5f) * 1.4f, cz = (iz + 0.5f) * 1.4f - 0.2f + oz, h = hash2(ix * 3.7f + 1.3f, iz * 1.9f + 4.1f);
    int k = (int)(fract(h * 11.3f) * 11.999f); if (k == 5) k = 0; if (k == 11) k = 4;       // pas d'enfants au bal à trois heures du matin
    float sc = CZA[k].sc, yw = 2.1f * (fract(h * 29.0f) - 0.5f) - 0.21f, c = cosf(yw), sn = sinf(yw), dx = qx - cx - 0.46f * (fract(h * 5.1f) - 0.5f), dz = qz - cz - 0.30f * (fract(h * 9.7f) - 0.5f);
    float hs = sinf(PI * CB + 2.2f * fract(h * 7.0f)), hop = fract(h * 17.0f) < 0.55f ? CJA * (0.5f + 0.5f * fract(h * 13.0f)) * hs * hs : 0;
    *l = V((dx * c - dz * sn) / sc, (p.y - hop) / sc, (dx * sn + dz * c) / sc); CFIX = ix; CFIZ = iz;
    { float wx = cx * c0 + cz * s0, wz = -cx * s0 + cz * c0; if (fabsf(wx) > 3.95f || wz < -5.25f || wz > 4.85f) return -1; }       // la foule ne déborde pas de l'emprise d'avant : les caméras posées au bord de la piste restent dégagées
    if (fract(h * 3.7f) > CDENS || (CCPL && len2(cx * c0 + cz * s0, -cx * s0 + cz * c0 - 0.5f) < CCR)) return -1;
    return k + 12 * (fract(h * 23.0f) < 0.68f);       // les deux tiers : des jeunes
}
static float bal_map(v3 p, int *m) {
    int mm; float d = p.y, b, ax = fabsf(p.x); *m = W_CFLOOR;
    UN(fminf(fminf(BAX - ax, BAH - p.y), fminf(p.z - BAZ0, BAZ1 - p.z)), W_CWALL)
    if (p.z > 6.5f) {       // l'estrade : la console, deux platines, les enceintes, l'enseigne
        UN(sdBox(V(p.x, p.y - STY * 0.5f, p.z - 10.0f), V(5.5f, STY * 0.5f, 2.0f)), W_CSTAGE)
        UN(sdBox(V(p.x, p.y - STY - 0.31f, p.z - 8.75f), V(1.45f, 0.29f, 0.40f)) - 0.02f, W_CCONS)
        { float sx = ax - 0.38f, ty = p.y - STY - 0.645f;
            UN(fminf(cylY(V(sx, ty, p.z - 9.0f), 0.275f, 0.018f), cylY(V(sx, ty - 0.026f, p.z - 9.0f), 0.25f, 0.010f)), W_CVINYL)
            UN(fminf(sdCap(V(sx, ty, p.z - 9.0f), V(0.27f, 0.06f, -0.20f), V(0.11f, 0.05f, 0.05f), 0.011f), cylY(V(sx - 0.27f, ty - 0.03f, p.z - 8.80f), 0.028f, 0.035f)), W_CCHROME)
            UN(fminf(sdBox(V(p.x, ty - 0.015f, p.z - 8.92f), V(0.085f, 0.03f, 0.22f)), sdBox(V(p.x, ty - 0.01f, p.z - 8.52f), V(1.25f, 0.025f, 0.12f))), W_CMIX) }
        UN(sdBox(V(ax - 3.7f, p.y - STY - 1.3f, p.z - 10.7f), V(0.62f, 1.3f, 0.5f)) - 0.02f, W_CSPK)
        UN(sdBox(V(p.x, p.y - 4.35f, p.z - 11.9f), V(3.5f, 0.66f, 0.06f)), W_CNEON)
    } else d = fminf(d, 7.0f - p.z);
    UN(len(sub(p, CBALLC)) - 0.55f, W_CBALL) UN(cylY(V(p.x, p.y - 6.0f, p.z), 0.025f, 0.5f), W_CCHROME)       // la boule à facettes
    if (p.y > 5.2f) { float zl = fabsf(p.z - 1.0f) - 4.0f, xl = p.x - 2.4f * clampf(roundf(p.x / 2.4f), -3, 3);      // deux ponts de lumière, sept projecteurs chacun
        UN(cylX(V(p.x, p.y - 6.05f, zl), 0.06f, 8.6f), W_CCHROME) UN(cylY(V(xl, p.y - 5.80f, zl), 0.13f, 0.17f), W_CSPOT) }
    else d = fminf(d, 5.6f - p.y);
    if (p.x > 5.0f && p.z < 3.5f) { float k, zl = c_repc(p.z + 2.0f, 1.8f, -2, 2, &k);       // le bar : le comptoir, les étagères, cinq tabourets
        UN(sdBox(V(p.x - 6.9f, p.y - 0.53f, p.z + 2.0f), V(0.42f, 0.53f, 4.5f)), W_CBAR)
        UN(sdBox(V(p.x - 6.85f, p.y - 1.09f, p.z + 2.0f), V(0.55f, 0.035f, 4.6f)) - 0.01f, W_CBARTOP)
        UN(sdBox(V(p.x - 8.85f, p.y - 2.7f, p.z + 2.0f), V(0.15f, 1.35f, 4.3f)), W_CSHELF)
        UN(fminf(cylY(V(p.x - 6.0f, p.y - 0.74f, zl), 0.21f, 0.04f) - 0.02f, cylY(V(p.x - 6.0f, p.y - 0.36f, zl), 0.035f, 0.36f)), W_CSTOOL)
    } else d = fminf(d, fmaxf(5.75f - p.x, p.z - 2.65f));
    if (p.x < -5.3f && p.z < -2.3f) { float k, zl = c_repc(p.z + 3.8f, 3.6f, -1, 0, &k); v3 q = V(p.x + 6.3f, p.y, zl);       // deux tables de banquet ; sur la première, un verre
        UN(fminf(fminf(cylY(V(q.x, q.y - 0.74f, q.z), 0.55f, 0.02f), cylY(V(q.x, q.y - 0.37f, q.z), 0.045f, 0.36f)), cylY(V(q.x, q.y - 0.02f, q.z), 0.28f, 0.02f)), W_CTABLE)
    } else d = fminf(d, fmaxf(p.x + 5.75f, p.z + 3.25f));
    if (p.x < -5.7f && p.z > 5.3f) { v3 q = V(p.x + 7.0f, p.y, p.z - 6.6f); float k, xl = c_repc(q.x, 0.26f, -2, 2, &k);       // le grill : la braise, cinq saucisses
        UN(fminf(sdBox(V(q.x, q.y - 0.62f, q.z), V(0.78f, 0.17f, 0.42f)) - 0.02f, cylY(V(fabsf(q.x) - 0.66f, q.y - 0.22f, fabsf(q.z) - 0.32f), 0.03f, 0.23f)), W_CGRILL)
        UN(sdCap(V(xl, q.y - 0.855f, q.z), V(0, 0, -0.24f), V(0, 0, 0.24f), 0.048f), W_CSAUS)
    } else d = fminf(d, fmaxf(p.x + 6.2f, 6.15f - p.z));
    if (CMBON) { float e = len(V(p.x - 4.4f, p.y, p.z + 7.4f)) - 1.7f; if (e > 0.3f) d = fminf(d, e); else for (int i = 0; i < 6; i++) UN(len(sub(p, CMB[i])) - 0.05f, W_CMARB) }       // les billes des gobelins
    if (!NOCZ && CDENS > 0) {       // les danseurs de la piste
        float bz = fmaxf(fmaxf(ax - 4.2f, p.y - 2.3f), fmaxf(-5.8f - p.z, p.z - 5.4f));
        if (bz < 0) { v3 l; int k = bal_fan(p, &l);
            if (k >= 0) d = c_cz(&CZC[k % 24], &CRR[k], &CRP[k], l, m, d);
            { float ix = clampf(floorf(p.x / 1.4f), -3, 2), iz = clampf(floorf((p.z + 0.2f) / 1.4f), -4, 3), lx = p.x - (ix + 0.5f) * 1.4f, lz = p.z + 0.2f - (iz + 0.5f) * 1.4f;
                d = fminf(d, fminf((ix < -2.5f && lx < 0) || (ix > 1.5f && lx > 0) ? 1e9f : 0.7f - fabsf(lx), (iz < -3.5f && lz < 0) || (iz > 2.5f && lz > 0) ? 1e9f : 0.7f - fabsf(lz)) + 0.3f); } }
        else d = fminf(d, bz + 0.3f); }
    if (CCHEN) { float zc = -7.7f, bd = fmaxf(fabsf(p.z - zc) - 0.75f, p.y - 1.25f);       // la chenille : huit porcs de marche, le meneur chromé devant
        if (bd > 0.4f) d = fminf(d, bd);
        else { float k, lx = c_repc(p.x - CCHX, 1.6f, -7, 0, &k), ph = PI * CB + 0.5f * k, bob = 0.05f * fabsf(sinf(ph));
            b = p4_sdf(V(-(p.z - zc) / 0.5f, (p.y - bob) / 0.5f, lx / 0.5f), ph, 1, k > -0.5f ? W_PCHROME : (fract(k * 0.37f + 0.1f) < 0.25f ? W_PGOLD : W_PPINK), W_PEYE, W_PGOLD, &mm) * 0.5f; if (b < d) { d = b; *m = mm; }
            d = fminf(d, ((k < -6.5f && lx < 0) || (k > -0.5f && lx > 0) ? 1e9f : 0.8f - fabsf(lx)) + 0.06f); } }
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    if (DNON) d = dn_map(p, m, d);       // Martin Louria
    if (CDJ >= 0 && !NOCZ) d = dj_bling(p, m, d);
    return d;
}

// ---------------------------------------------------------------- décor 1 : Douzi City sous la neige. La rue file vers +z ; le Bal est à droite (x > 6,5) ; au fond, le viaduc.
S void cl_spot(int i, v3 *b0, v3 *bd) {       // les quatre projecteurs du toit du Bal : où est la lentille, où elle vise
    *bd = nrm(V(-0.5f + 0.6f * sinf(CB * 0.4f + i * 1.9f), 1, 0.7f * cosf(CB * 0.31f + i * 2.3f))); *b0 = add(V(12.0f + 4.0f * (i & 1), 9.25f, -3.0f + 5.0f * i), mul(*bd, 0.55f)); }
S float vil_h(float kx, float kz) { float h = hash2(kx * 7.3f + 1.7f, kz * 3.9f + 5.1f); return 6.0f + 12.0f * h * h; }
static float vil_map(v3 p, int *m) {
    int mm; float d = p.y, b, ax = fabsf(p.x); *m = W_CSNOW;
    { float kx = roundf(p.x / 12.0f), kz = roundf(p.z / 11.0f), lx = p.x - 12.0f * kx, lz = p.z - 11.0f * kz;       // un immeuble par case, la rue au milieu
        if (kx != 0 && fabsf(kx) < 5.5f && !((kx == 1 || kx == 2) && fabsf(kz) < 1.5f)) { float h = vil_h(kx, kz);
            float wx = 4.6f + 0.8f * hash2(kx, kz + 9.0f), wz = 4.2f + 0.9f * hash2(kx + 5.0f, kz);
            UN(sdBox(V(lx, p.y - h * 0.5f, lz), V(wx, h * 0.5f, wz)), W_CBLDG)
            UN(fmaxf(fmaxf(fabsf(lx) * 0.6f + (p.y - h) * 0.8f - (wx + 0.45f) * 0.6f, fabsf(lz) - wz - 0.35f), h - 0.05f - p.y), W_CROOF)       // un toit à deux pentes, sous la neige
            UN(sdBox(V(lx - wx * (hash2(kx + 2.0f, kz + 3.0f) > 0.5f ? 0.45f : -0.45f), p.y - h - 2.0f, lz - 1.0f), V(0.42f, 2.0f, 0.42f)), W_CBLDG) }       // sa cheminée
        d = fminf(d, fminf(6.0f - fabsf(lx), 5.5f - fabsf(lz)) + 0.4f); }
    if (p.x > 3.5f && fabsf(p.z) < 18.0f) {       // le Bal des Neiges : l'aile et son toit d'ardoise, la tour d'angle et son dôme, les corniches, la marquise sur ses deux colonnes
        float e = fminf(sdBox(V(p.x - 18.0f, p.y - 3.5f, p.z - 4.0f), V(11.5f, 3.5f, 12.0f)), sdBox(V(p.x - 11.0f, p.y - 6.0f, p.z + 12.0f), V(4.5f, 6.0f, 4.0f)));
        e = fminf(e, fminf(sdBox(V(p.x - 18.0f, p.y - 7.0f, p.z - 4.0f), V(11.8f, 0.16f, 12.3f)), sdBox(V(p.x - 18.0f, p.y - 7.85f, p.z - 4.0f), V(10.9f, 0.85f, 11.4f))));
        e = fminf(e, fminf(sdBox(V(p.x - 11.0f, p.y - 12.0f, p.z + 12.0f), V(4.8f, 0.16f, 4.3f)), fmaxf(len(V(p.x - 11.0f, p.y - 12.0f, p.z + 12.0f)) - 3.7f, 12.0f - p.y)));
        e = fminf(e, fminf(sdBox(V(p.x - 5.7f, p.y - 3.35f, p.z + 12.0f), V(0.9f, 0.10f, 2.3f)), cylY(V(p.x - 4.95f, p.y - 1.65f, fabsf(p.z + 12.0f) - 2.1f), 0.13f, 1.65f)));
        UN(e, W_CHALL)
        { v3 q = V(p.x - 11.0f, p.y - 15.66f, p.z + 12.0f); float eb = len(V(q.x, q.y - 1.2f, q.z)) - 2.7f;       // tout en haut du dôme : un porc d'or, qui tourne
            if (eb > 0.4f) d = fminf(d, eb);
            else { float c = cosf(CGA * 0.5f), sn = sinf(CGA * 0.5f); b = p4_sdf(V((q.x * c - q.z * sn) / 1.3f, q.y / 1.3f, (q.x * sn + q.z * c) / 1.3f), 0, 0, W_PGOLD, W_PEYE, W_PGOLD, &mm) * 1.3f; if (b < d) { d = b; *m = mm; } } }
        { float eb = fmaxf(fmaxf(8.5f - p.y, p.y - 10.6f), fmaxf(10.6f - p.x, p.x - 17.4f));       // sur le toit : quatre projecteurs de poursuite, chacun sur son pied, qui suivent leur faisceau
            if (eb > 0.3f) d = fminf(d, eb);
            else for (int i = 0; i < 4; i++) { v3 b0, bd, c0 = V(12.0f + 4.0f * (i & 1), 9.25f, -3.0f + 5.0f * i); cl_spot(i, &b0, &bd);
                UN(fminf(sdCap(p, sub(c0, mul(bd, 0.35f)), add(c0, mul(bd, 0.42f)), 0.33f), fminf(sdBox(V(p.x - c0.x, p.y - 8.83f, p.z - c0.z), V(0.42f, 0.13f, 0.30f)), sdBox(V(p.x - c0.x, p.y - 9.05f, fabsf(p.z - c0.z) - 0.40f), V(0.07f, 0.32f, 0.04f)))), W_CPROJ) } }
    } else d = fminf(d, fmaxf(3.5f - p.x, fabsf(p.z) - 18.0f) + 0.3f);
    d = fminf(d, (p.y - 0.30f * sstep(5.7f, 6.6f, ax)) * 0.9f);       // la neige poussée au pied des maisons
    { float gb = fmaxf(fmaxf(ax - 5.3f, p.y - 4.9f), 3.85f - p.y);       // des guirlandes d'ampoules en travers de la rue, d'un réverbère à l'autre et entre deux
        if (gb < 0.3f) { float zg = c_rep(p.z - 5.5f, 5.5f), xk = roundf(p.x / 0.65f) * 0.65f; UN(len(V(p.x - xk, p.y - (4.72f - 0.72f * (1 - (xk / 5.2f) * (xk / 5.2f))), zg)) - 0.085f, W_CGARL) d = fminf(d, 0.33f - fabsf(p.x - xk) + 0.1f); }
        else d = fminf(d, gb); }
    { float zt = c_rep(p.z, 11.0f); v3 q = V((ax - 5.95f) / 0.42f, p.y / 0.42f, zt / 0.42f); float e = (len(V(q.x, q.y - 4.5f, q.z)) - 5.6f) * 0.42f;       // des sapins sur les trottoirs
        if (p.x > 0 && fabsf(p.z) < 17.0f) e = fmaxf(e, 0.5f);       // pas devant le Bal
        if (e > 0.3f) d = fminf(d, e);
        else { UN(cylY(V(q.x, q.y - 0.8f, q.z), 0.28f, 0.8f) * 0.42f, W_TRUNK)
            UN(fminf(fminf(cone(V(q.x, q.y - 1.5f, q.z), 2.3f, 4.2f), cone(V(q.x, q.y - 3.9f, q.z), 1.8f, 3.7f)), cone(V(q.x, q.y - 6.2f, q.z), 1.25f, 3.4f)) * 0.42f * 0.9f, W_FIR) } }
    { float sx = ax - 1.5f, pb = fmaxf(fabsf(sx) - 0.55f, p.y - 1.0f);       // les porcs de marche, sur deux files, qui remontent la rue au pas
        if (!CPIGS) { }
        else if (p.z < 56.0f && pb < 0.25f) { float zs = p.z - CTS * 1.3f, k2 = roundf(zs / 5.0f), l2 = zs - 5.0f * k2;
            b = p4_sdf(V(sx / 0.5f, p.y / 0.5f, l2 / 0.5f), CTS * 6.0f + k2 * 1.7f, 1, W_PPINK, W_PEYE, W_PGOLD, &mm) * 0.5f; if (b < d) { d = b; *m = mm; }
            d = fminf(d, 2.5f - fabsf(l2) + 0.35f); }
        else d = fminf(d, fmaxf(pb, 0.2f)); }
    { float zl = c_rep(p.z - 5.5f, 11.0f);       // les réverbères
        UN(cylY(V(ax - 5.2f, p.y - 2.2f, zl), 0.07f, 2.2f), W_CLAMP) UN(len(V(ax - 5.2f, p.y - 4.6f, zl)) - 0.25f, W_CLANT) }
    { v3 q = V((p.x + 3.2f - (p.z + 1.5f)) * 0.7071f, p.y, (p.x + 3.2f + p.z + 1.5f) * 0.7071f); float e = len(V(q.x, q.y - 1.2f, q.z)) - 1.5f;       // un bonhomme de neige : un porc, bien sûr
        if (e > 0.3f) d = fminf(d, e);
        else { UN(fminf(fminf(len(V(q.x, q.y - 0.55f, q.z)) - 0.62f, len(V(q.x, q.y - 1.32f, q.z)) - 0.43f), fminf(len(V(q.x, q.y - 1.92f, q.z)) - 0.33f, len(V(fabsf(q.x) - 0.22f, q.y - 2.20f, q.z)) - 0.10f)), W_CSMAN)
            UN(fminf(cylZ(V(q.x, q.y - 1.88f, q.z + 0.33f), 0.115f, 0.05f), len2(len2(q.x, q.z) - 0.31f, q.y - 1.62f) - 0.075f), W_CSNOUT)
            UN(fmaxf(fabsf(len2(q.x, q.z) - 0.15f) - 0.02f, fabsf(q.y - 2.27f) - 0.06f), W_PGOLD) } }
    if (p.z > 58.0f && p.z < 74.0f) { float zq = p.z - 66.0f, xl = c_rep(p.x, 16.0f);       // le viaduc ; dessus, le dernier train
        UN(fminf(fmaxf(sdBox(V(p.x, p.y - 4.2f, zq), V(400.0f, 4.2f, 1.6f)), 6.3f - len2(xl, p.y - 1.2f)), sdBox(V(p.x, p.y - 8.55f, zq), V(400.0f, 0.2f, 1.9f))), W_CVIAD)
        if (CTRON) { float k, xo = c_repc(p.x - CTRX, 10.2f, -5, 0, &k); UN(sdBox(V(xo, p.y - 10.2f, zq), V(4.75f, 1.25f, 1.15f)) - 0.2f, W_CTRAIN) }
    } else d = fminf(d, p.z <= 58.0f ? 64.0f - p.z : p.z - 68.0f);
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    if (DNON) d = dn_map(p, m, d);       // Martin Louria
    return d;
}

// ---------------------------------------------------------------- décor 2 : l'espace des vitrines. Un damier sans fin ; dessus, selon CHV :
// 0 le disque géant et sa ronde ; 1 le tunnel de néons ; 2 la foule sans fin ; 3 le mur d'enceintes ; 4 les lettres d'or.
S int hyp_fan(v3 p, v3 *l) {
    if (CHV == 0) { float sa = 6.2831853f / CNRING, a = atan2f(p.z, p.x) - CDA, k = roundf(a / sa), ak = k * sa + CDA, c = cosf(ak), sn = sinf(ak), dx = p.x - 4.3f * c, dz = p.z - 4.3f * sn;
        int ki = (((int)k % CNRING) + CNRING) % CNRING, f = ki % 12; if (f == 5) f = 10; if (f == 11) f = 4; float sc = CZA[f].sc;
        *l = V((dx * sn - dz * c) / sc, (p.y - 0.13f - CJ * (0.6f + 0.4f * (ki & 1))) / sc, (dx * c + dz * sn) / sc); return f + 12 * (ki & 1); }
    if (CHV == 2) { float ix = floorf(p.x / 2.2f), iz = floorf(p.z / 2.2f), h = hash2(ix * 3.7f + 1.3f, iz * 1.9f + 4.1f); int k = (int)(fract(h * 11.3f) * 11.999f); if (k == 5) k = 0; if (k == 11) k = 4;
        float sc = CZA[k].sc, dx = p.x - (ix + 0.5f) * 2.2f - 0.5f * (fract(h * 5.1f) - 0.5f), dz = p.z - (iz + 0.5f) * 2.2f - 0.5f * (fract(h * 9.7f) - 0.5f);
        *l = V(-dx / sc, (p.y - CJ * (0.5f + 0.5f * fract(h * 13.0f))) / sc, -dz / sc); if (fract(h * 3.7f) > 0.9f) return -1; return k + 12 * (fract(h * 23.0f) < 0.5f); }
    { float k, lx = c_repc(p.x, 1.7f, -2, 2, &k); int f = (int)k + 2; f = f == 0 ? 1 : f == 1 ? 8 : f == 2 ? 4 : f == 3 ? 2 : 9; float sc = CZA[f].sc;       // devant le mur : cinq danseurs, de face
        *l = V(-lx / sc, (p.y - CJ) / sc, -(p.z - 3.4f) / sc); return f + 12 * ((int)k & 1); }
}
static float hyp_map(v3 p, int *m) {
    int mm; float d = p.y, b; *m = W_CCHECK;
    if (CHV == 1) {       // le tunnel : un anneau de néon tous les 3 m ; dans l'axe, tous les 18 m, un porc de marche d'or ou de chrome qui tourne
        float zl = c_rep(p.z, 3.0f), r = len2(p.x, p.y); d = len2(r - 4.2f, zl) - 0.09f; *m = W_CRING;
        { float zc = 18.0f * roundf((p.z - 9.0f) / 18.0f) + 9.0f; v3 q = V(p.x, p.y, p.z - zc); float e = len(q) - 2.1f;
            if (e > 0.4f) d = fminf(d, e);
            else { float a = CGA + zc * 0.3f, c = cosf(a), sn = sinf(a); b = p4_sdf(V(q.x * c - q.z * sn, q.y + 0.9f, q.x * sn + q.z * c), 0, 0, ((int)roundf(zc / 18.0f) & 1) ? W_PGOLD : W_PCHROME, W_PEYE, W_PGOLD, &mm); if (b < d) { d = b; *m = mm; } } }
        return d; }
    if (CHV == 0) { float r = len2(p.x, p.z);
        UN(cylY(V(p.x, p.y - 0.06f, p.z), 6.0f, 0.06f) - 0.01f, W_CDISC)
        if (CGIANT == 2) UN(cylY(V(p.x, p.y - 0.42f, p.z), 1.7f, 0.30f) - 0.02f, W_CPAD)       // le socle de la chope géante
        else if (CGIANT) { float e = len(V(p.x, p.y - 2.1f, p.z)) - 3.0f;       // au centre, sur un socle : le porc de marche, en or, qui tourne comme un trophée
            if (e > 0.5f) d = fminf(d, e);
            else { float c = cosf(CGA), sn = sinf(CGA); b = p4_sdf(V((p.x * c - p.z * sn) / 1.5f, (p.y - 0.74f) / 1.5f, (p.x * sn + p.z * c) / 1.5f), 0, 0, W_PGOLD, W_PEYE, W_PGOLD, &mm) * 1.5f; if (b < d) { d = b; *m = mm; }
                UN(cylY(V(p.x, p.y - 0.42f, p.z), 1.7f, 0.30f) - 0.02f, W_CPAD) } }
        else UN(len(sub(p, CBALLC)) - 1.2f, W_CBALL)
        if (!NOCZ) { float bz = fmaxf(fabsf(r - 4.3f) - 1.0f, p.y - 2.5f);
            if (bz < 0) { v3 l; int k = hyp_fan(p, &l); float sa = 6.2831853f / CNRING, a = atan2f(p.z, p.x) - CDA; d = c_cz(&CZC[k % 24], &CRR[k], &CRP[k], l, m, d);
                d = fminf(d, r * sinf(fmaxf(sa * 0.5f - fabsf(a - roundf(a / sa) * sa), 0)) + 0.3f); }
            else d = fminf(d, bz + 0.3f); }
        for (int k = 0; k < 3; k++) UN(len(sub(p, CSPH[k])) - (0.7f + 0.2f * k), W_SPH)
    }
    if (CHV == 2 && !NOCZ) {
        if (p.y < 2.5f) { v3 l; int k = hyp_fan(p, &l); if (k >= 0) d = c_cz(&CZC[k % 24], &CRR[k], &CRP[k], l, m, d);
            d = fminf(d, fminf(1.1f - fabsf(p.x - (floorf(p.x / 2.2f) + 0.5f) * 2.2f), 1.1f - fabsf(p.z - (floorf(p.z / 2.2f) + 0.5f) * 2.2f)) + 0.35f); }
        else d = fminf(d, p.y - 2.5f + 0.4f); }
    if (CHV == 3) { UN(8.0f - p.z, W_CSPKW)
        if (!NOCZ) { float bz = fmaxf(fmaxf(fabsf(p.x) - 4.25f, p.y - 2.5f), fabsf(p.z - 3.4f) - 0.85f);
            if (bz < 0) { v3 l; float k; int f = hyp_fan(p, &l); float lx = c_repc(p.x, 1.7f, -2, 2, &k); d = c_cz(&CZC[f % 24], &CRR[f], &CRP[f], l, m, d);
                d = fminf(d, ((k < -1.5f && lx < 0) || (k > 1.5f && lx > 0) ? 1e9f : 0.85f - fabsf(lx)) + 0.3f); }
            else d = fminf(d, bz + 0.3f); } }
    if (CHV == 5 && CK == 41 && p.z < -1.9f && p.y < 1.0f) { float k, lx = c_repc(p.x, 1.25f, -2, 2, &k); int mm;       // cinq porcs de marche, au pas, devant les habitants
        float b = p4_sdf(V(-(p.z + 2.9f) / 0.5f, p.y / 0.5f, lx / 0.5f), CTS * 5.0f + k * 1.3f, 1, W_PPINK, W_PEYE, W_PGOLD, &mm) * 0.5f; if (b < d) { d = b; *m = mm; return d; } }
    if (CHV == 5 && DNON) { *m = W_CSTUDIO; return dn_map(p, m, d); }
    if (CHV == 5) { *m = W_CSTUDIO; if (WN) { int mm; float b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; } } return d; }       // le studio : un plateau laqué, et ceux qu'on y pose
    if (CHV == 4) {       // DJ VITEAU, en lettres d'or, debout sur le damier
        float s = 13.0f / (TR[T_VITEAU].w - 80.0f), bb = fmaxf(fmaxf(fabsf(p.x) - 7.2f, fabsf(p.z) - 0.9f), p.y - 2.6f);
        if (bb > 0.5f) d = fminf(d, bb);
        else { float d2 = text2d(T_VITEAU, p.x / s + TR[T_VITEAU].w * 0.5f, TR[T_VITEAU].base - (p.y - 0.02f) / s) * s; UN(fmaxf(d2, fabsf(p.z) - 0.55f) * 0.9f - 0.03f, W_CGOLDL) }
        for (int k = 0; k < 3; k++) UN(len(sub(p, CSPH[k])) - (0.7f + 0.2f * k), W_SPH)
    }
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    if (DNON) d = dn_map(p, m, d);       // Martin Louria
    return d;
}
static float clip_map(v3 p, int *m) { return CSET == 0 ? bal_map(p, m) : CSET == 1 ? vil_map(p, m) : hyp_map(p, m); }
