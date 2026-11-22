// Canal Pork : « Le Cockring Homologué Martin Plaisir, trésor national ». Rotonde du Musée national de Porkonia, la nuit.
// Coupes : CUT[0] macro de l'objet, CUT[1] les visiteurs en larmes, CUT[2] la fiche technique, CUT[3] Martin Plaisir
// remonte le tapis (de dos), CUT[4] cérémonie : les lettres TRÉSOR NATIONAL descendent, CUT[5] l'objet lévite, feu d'artifice.
#define POSTFX halo

#include "p3d.h"
#include "martin_dn.h"
enum { N_FLOOR = M_USER, N_LET, N_MARBLE, N_VELVET, N_RING, N_RUBY, N_DIAM, N_TEAR, N_PANEL, N_SATIN, N_FLARE, N_BOOT, N_MSKIN };
static unsigned char *PLQ;
static V MOON;
static float RY, ROT, GLOW, LYC, FW0, RSC = 1;
static int CROWD, MART;
#define NBM 10
#define NPART 240
static V PP[NPART], PCOL[NPART]; static float PSZ[NPART], PIN[NPART];       // les poussières d'or, placées une fois par image
static V GW[16], GN[16]; static float GTW[16];      /* 2 x NGEM */          // les pierres qui scintillent, une fois par image
static int NB; static V BO[NBM], BA[NBM], BC[NBM]; static float BL[NBM], BR[NBM], BS[NBM];      // projecteurs : origine, axe, couleur, longueur, rayon au bout, force
static void beam(V o, V target, V col, float r, float strength) {
    if (NB >= NBM) return; V a = sub(target, o); float l = len(a);
    BO[NB] = o; BA[NB] = mul(a, 1 / l); BL[NB] = l; BC[NB] = col; BR[NB] = r; BS[NB] = strength; NB++;
}

#define RR 0.38f
#define Rr 0.065f
#define PANC VV(0, 1.22f, -1.62f)
#define TILT 0.62f

// ---------------------------------------------------------------- l'objet : jonc d'or biseauté, perlage, volutes ciselées, 16 pierres taillées serties à griffes, couronne
#define RBW 0.065f      // demi-largeur radiale du jonc
#define RBZ 0.052f      // demi-épaisseur (faces avant et arrière)
#define NGEM 8
static V ring_local(V p) {
    V q = sub(p, VV(0, RY, 0)); float c = cosf(ROT), s = sinf(ROT);
    return VV(c * q.x - s * q.z, q.y, s * q.x + c * q.z);
}
static __thread V GL;   // point dans le repère de la dernière pierre touchée (pour ses facettes)
static float gem_r(int k) { return (k & 1) ? 0.034f : 0.049f; }       // rubis (pair) gros, diamants (impair) plus petits
static float ring_q(V q, int *m) {      // q : repère de l'anneau, à l'échelle 1
    float rad = sqrtf(q.x * q.x + q.y * q.y), ang = atan2f(q.y, q.x), dr = rad - RR;
    float d = box(VV(dr, q.z, 0), VV(RBW - 0.016f, RBZ - 0.016f, 1)) - 0.016f; *m = N_RING;            // le jonc, arêtes arrondies
    {   const float st = 6.2831853f / 150.0f; float ab = roundf(ang / st) * st, zs = q.z > 0 ? 1.0f : -1.0f;   // perlage : un rang de billes d'or sur chaque arête extérieure
        V bc = VV((RR + RBW - 0.006f) * cosf(ab), (RR + RBW - 0.006f) * sinf(ab), zs * (RBZ - 0.006f));
        d = fminf(d, len(sub(q, bc)) - 0.0115f);
        bc = VV((RR - RBW + 0.006f) * cosf(ab), (RR - RBW + 0.006f) * sinf(ab), zs * (RBZ - 0.006f));
        d = fminf(d, len(sub(q, bc)) - 0.0095f); }
    {   const float st = 6.2831853f / NGEM, a0 = 0.5f * st; float k = roundf((ang - a0) / st); int ki = ((int)k % NGEM + NGEM) % NGEM;
        float ak = k * st + a0, zs = q.z > 0 ? 1.0f : -1.0f, rg = gem_r(ki);
        V c = VV(RR * cosf(ak), RR * sinf(ak), zs * RBZ);
        V g = sub(q, c); float h = g.z * zs, lat = sqrtf(g.x * g.x + g.y * g.y);
        // la monture : un chaton d'or et quatre griffes
        float bz = fmaxf(fabsf(lat - rg - 0.006f) - 0.0065f, fabsf(h - 0.012f) - 0.014f);
        float ca = cosf(ak), sa = sinf(ak);
        for (int j = 0; j < 4; j++) { float th = ak + 0.785398f + j * 1.5708f, cs = cosf(th), sn = sinf(th);
            bz = fminf(bz, cap(g, VV(cs * (rg + 0.006f), sn * (rg + 0.006f), zs * 0.0f), VV(cs * rg * 0.72f, sn * rg * 0.72f, zs * (0.03f + 0.32f * rg)), 0.0062f)); }
        (void)ca; (void)sa;
        if (bz < d) { d = bz; *m = N_RING; }
        // la pierre taillée : table, couronne à 8 facettes, rondiste, posée un peu haut dans son chaton
        float hh = h - 0.022f, e = fmaxf(lat - rg, fmaxf(hh - 0.42f * rg, -hh - 0.02f));
        const float sA = 0.70711f, cA = 0.70711f; float cf = rg * (sA + 0.12f * cA), mf = -1e9f;
        for (int j = 0; j < 8; j++) { float th = j * 0.785398f + 0.39f; mf = fmaxf(mf, (g.x * cosf(th) + g.y * sinf(th)) * sA + hh * cA - cf); }
        e = fmaxf(e, mf);
        if (e < d) { d = e; *m = (ki & 1) ? N_DIAM : N_RUBY; GL = VV(g.x, g.y, hh); }
    }
    {   V c = sub(q, VV(0, RR + RBW - 0.01f, 0));                                                       // la couronne de Porkonia, au sommet
        float e = box(sub(c, VV(0, 0.022f, 0)), VV(0.078f, 0.018f, 0.026f)) - 0.006f;
        for (int j = -2; j <= 2; j++) { float x = 0.036f * j, ty = 0.115f - 0.022f * abs(j);
            e = fminf(e, cap(c, VV(x * 0.85f, 0.035f, 0), VV(x * 1.18f, ty, 0), 0.0105f));
            float eb = len(sub(c, VV(x * 1.18f, ty + 0.014f, 0))) - 0.0175f;
            if (j == 0 && eb < d && eb < e) { d = eb; *m = N_RUBY; GL = VV(0, 0, 1); }
            e = fminf(e, j ? eb : 1e9f); }
        if (e < d) { d = e; *m = N_RING; }
    }
    return d;
}
static float ring(V p, int *m) {
    float b = len(sub(p, VV(0, RY, 0))) - 0.62f * RSC;
    P3D_EARLY(b, 0.3f);
    return ring_q(mul(ring_local(p), 1 / RSC), m) * RSC;
}

// ---------------------------------------------------------------- socle, coussin de velours, pompons, pupitre
static float pedestal(V p, int *m) {
    float b = box(sub(p, VV(0, 1.6f, -0.6f)), VV(1.3f, 1.7f, 1.9f));
    P3D_EARLY(b, 0.4f);
    float d = sqrtf(p.x * p.x + p.z * p.z) - 0.72f; d = fmaxf(d, fabsf(p.y - 1.1f) - 1.0f); *m = N_MARBLE;
    float e = box(sub(p, VV(0, 0.15f, 0)), VV(1.05f, 0.15f, 1.05f)) - 0.03f; if (e < d) { d = e; *m = M_GOLD; }
    e = box(sub(p, VV(0, 2.17f, 0)), VV(0.92f, 0.08f, 0.92f)) - 0.02f; if (e < d) { d = e; *m = M_GOLD; }
    e = box(sub(p, VV(0, 2.40f, 0)), VV(0.70f, 0.10f, 0.70f)) - 0.10f; if (e < d) { d = e; *m = N_VELVET; }
    V t = VV(fabsf(p.x) - 0.78f, p.y, fabsf(p.z) - 0.78f);                       // pompons dorés aux quatre coins
    e = fminf(sph(t, VV(0, 2.34f, 0), 0.07f), cap(t, VV(0, 2.3f, 0), VV(0, 2.05f, 0), 0.045f)); if (e < d) { d = e; *m = M_GOLD; }
    e = cap(p, VV(0, 0, -1.78f), VV(0, 1.0f, -1.68f), 0.05f); if (e < d) { d = e; *m = M_GOLD; }   // pied du pupitre
    V q = sub(p, PANC); float c = cosf(TILT), s = sinf(TILT);
    V l = VV(q.x, q.y * c + q.z * s, q.z * c - q.y * s);
    e = box(l, VV(0.82f, 0.62f, 0.03f)) - 0.01f; if (e < d) { d = e; *m = N_PANEL; }
    return d;
}

// ---------------------------------------------------------------- la colonnade (20 colonnes, architrave)
static float colonnade(V p, int *m) {
    float r = sqrtf(p.x * p.x + p.z * p.z), b = fmaxf(fabsf(r - 15.0f) - 1.6f, p.y - 11.6f);
    *m = N_MARBLE;
    P3D_EARLY(b, 0.5f);
    const float st = 6.28318f / 20.0f;
    float a = atan2f(p.x, p.z), k = roundf((a - st * 0.5f) / st), ac = k * st + st * 0.5f;
    V c = VV(15.0f * sinf(ac), 0, 15.0f * cosf(ac)), q = sub(p, c);
    float d = fmaxf(sqrtf(q.x * q.x + q.z * q.z) - 0.6f + 0.03f * cosf(atan2f(q.x, q.z) * 16.0f), fabsf(p.y - 5.0f) - 5.0f);
    d = fminf(d, box(sub(q, VV(0, 0.25f, 0)), VV(0.85f, 0.25f, 0.85f)));
    d = fminf(d, box(sub(q, VV(0, 9.85f, 0)), VV(0.85f, 0.2f, 0.85f)));
    float e = fmaxf(fabsf(r - 15.0f) - 0.95f, fabsf(p.y - 10.65f) - 0.6f); if (e < d) d = e;
    e = fmaxf(fabsf(r - 15.0f) - 1.05f, fabsf(p.y - 11.33f) - 0.08f); if (e < d) { d = e; *m = M_GOLD; }
    return d;
}

// ---------------------------------------------------------------- les visiteurs : cochons couronnés en larmes, tournés vers l'objet
#define CROWD_RMAX 1.40f      // un cochon de la foule : 1,583 x 0,85 = 1,35 au plus autour du centre de sa case (mesuré), larmes comprises
static float crowd_cell(V q, float cx, float cz, float ix, float iz, int *m) {
    float rc = sqrtf(cx * cx + cz * cz);
    if (rc < 6.6f || rc > 13.2f || (fabsf(cx) < 2.9f && cz < 0)) return 1e9f;
    float h = hash2(ix, iz), h2 = hash2(iz + 3.0f, ix);
    float ang = atan2f(-cx, -cz) + (h - 0.5f) * 0.5f;
    V dir = VV(sinf(ang), 0, cosf(ang)), rt = VV(dir.z, 0, -dir.x);
    const float SC = 0.85f;
    float sob = 0.045f * fabsf(sinf(T * 7.0f + h * 20.0f));                           // les sanglots
    V l = mul(VV(q.x * rt.x + q.z * rt.z, q.y - sob, q.x * dir.x + q.z * dir.z), 1 / SC);
    int mm; float d = pig(l, 0.0f, h2 < 0.12f ? M_GOLD : (h2 < 0.22f ? M_CHROME : M_PINK), &mm) * SC;
    float f = fract(T * 0.42f + h * 7.0f);                                             // une larme par œil : elle glisse sur la joue, puis tombe
    float fs = fminf(f, 0.75f) / 0.75f, u = fmaxf(f - 0.75f, 0) * 2.4f;
    V tdir = norm(VV(0.50f + 0.12f * fs, 0.40f - 1.05f * fs, 0.80f - 0.12f * fs));
    V tc = add(VV(0, 1.15f, 0.95f), mul(tdir, 0.495f + 0.03f * sstep(0.85f, 1.0f, fs)));
    tc = add(tc, VV(0.08f * u, -4.5f * u * u, 0.12f * u));
    float e = (len(sub(VV(fabsf(l.x), l.y, l.z), tc)) - (0.042f - 0.008f * (u > 0))) * SC;
    if (e < d) { d = e; mm = N_TEAR; }
    *m = mm; return d;
}
// les visiteurs : cochons couronnés en larmes, tournés vers l'objet. La zone de la foule est élargie de CROWD_RMAX :
// les cochons du premier rang regardent vers le centre et leur groin dépasse vers l'intérieur (c'est lui qui disparaissait).
static float crowd(V p, int *m) {
    float r = sqrtf(p.x * p.x + p.z * p.z);
    float db = fmaxf(fmaxf(6.6f - CROWD_RMAX - r, r - 13.2f - CROWD_RMAX), p.y - 2.1f);
    *m = M_PINK;
    P3D_EARLY(db, 0.6f);
    return grid_xz(p, 3.0f, CROWD_RMAX, crowd_cell, m);
}

// ---------------------------------------------------------------- Martin Plaisir : le vrai modèle (le Danseur), qui roule des hanches en marchant
// Une foulée = deux temps ; les hanches mènent et passent au-dessus du pied d'appui, le buste suit avec un retard, la tête avec un retard de plus.
static void mp_walk(float ph, float tt, dn_pose_t *P) {
    dn_rest(P); const float L = 0.40f;
    float w = 6.2831853f * ph, hx = 0.10f * sinf(w);
    P->pel = V(hx, 1.0f - 0.03f - 0.022f * cosf(2 * w), 0);
    P->proll = -2.4f * hx; P->pyaw = 0.22f * sinf(w - 0.6f); P->ptilt = 0.05f;
    float wc = w - 0.31f, wh = w - 0.63f;
    P->chest = V(-0.06f * sinf(wc), 0.36f, 0.02f * sinf(2 * wc)); P->croll = 0.16f * sinf(wc); P->cyaw = -0.26f * sinf(wc - 0.6f); P->cpitch = 0.03f;
    P->hroll = -0.13f * sinf(wh); P->hyaw = 0.06f * sinf(wh * 0.5f); P->hpitch = -0.03f;
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, f = fract(ph + 0.5f * s), off, lift = 0, heel = 0;
        if (f < 0.5f) off = L * 0.5f - L * (f / 0.5f);
        else { float u = (f - 0.5f) / 0.5f; off = -L * 0.5f + L * ease(u); lift = 0.08f * sinf(PI * u); heel = 0.9f * sinf(PI * u); }
        P->foot[s] = V(sg * 0.055f, lift, off); P->heel[s] = heel; P->fyaw[s] = sg * 0.15f; P->kpole[s] = V(sg * 0.2f, 0, 1); }
    P->hand[1] = V(P->pel.x - 0.225f, P->pel.y + 0.005f, P->pel.z + 0.065f); P->epole[1] = V(-1, 0, -0.5f);       // la main sur la hanche
    P->hand[0] = V(0.30f, 0.93f + 0.03f * sinf(2 * w), -0.17f * cosf(w)); P->epole[0] = V(0.6f, -0.2f, -1);         // l'autre bras se balance
    P->tail = tt * 9.0f; P->smile = 0.6f; P->lid = 0.45f;
}

static float map(V p, int *m) {
    float d = p.y, e; int mm; *m = N_FLOOR;
    e = pedestal(p, &mm); if (e < d) { d = e; *m = mm; }
    e = ring(p, &mm); if (e < d) { d = e; *m = mm; }
    e = colonnade(p, &mm); if (e < d) { d = e; *m = mm; }
    if (CROWD) { e = crowd(p, &mm); if (e < d) { d = e; *m = mm; } }
    if (MART) d = dn_map(p, m, d);
    if (LYC < 100) {                                                        // TRÉSOR NATIONAL : lettres de prestige, biseau d'or arrondi
        V q = sub(p, VV(0, 0, 12.5f)); const float r = 0.09f;
        float d2 = lsdf(q.x, q.y - LYC) + r, dz = fabsf(q.z) - 0.55f + r;
        e = fminf(fmaxf(d2, dz), 0) + sqrtf(sq(fmaxf(d2, 0)) + sq(fmaxf(dz, 0))) - r;
        if (e < d) { d = e; *m = N_LET; } }
    return d;
}

// ---------------------------------------------------------------- le ciel : crépuscule étoilé, Voie lactée, comète, étoiles filantes
static float vnoise(V p) {                                            // bruit de valeur 3D
    V i = VV(floorf(p.x), floorf(p.y), floorf(p.z)), f = sub(p, i);
    f = VV(f.x * f.x * (3 - 2 * f.x), f.y * f.y * (3 - 2 * f.y), f.z * f.z * (3 - 2 * f.z));
    #define HH(a, b, c) fract(sinf((i.x + a) * 127.1f + (i.y + b) * 311.7f + (i.z + c) * 74.7f) * 43758.5453f)
    float x00 = mixf(HH(0, 0, 0), HH(1, 0, 0), f.x), x10 = mixf(HH(0, 1, 0), HH(1, 1, 0), f.x);
    float x01 = mixf(HH(0, 0, 1), HH(1, 0, 1), f.x), x11 = mixf(HH(0, 1, 1), HH(1, 1, 1), f.x);
    #undef HH
    return mixf(mixf(x00, x10, f.y), mixf(x01, x11, f.y), f.z);
}
static float fbm(V p, int o) { float s = 0, a = 0.5f; for (int k = 0; k < o; k++) { s += a * vnoise(p); p = mul(p, 2.03f); a *= 0.5f; } return s; }
static V hash3(V c) { return VV(fract(sinf(dot(c, VV(127.1f, 311.7f, 74.7f))) * 43758.5f), fract(sinf(dot(c, VV(269.5f, 183.3f, 246.1f))) * 43758.5f), fract(sinf(dot(c, VV(113.5f, 271.9f, 124.6f))) * 43758.5f)); }
static V star_col(float k) {                                         // température : bleues, blanches, dorées, orangées
    return k < 0.25f ? VV(0.65f, 0.78f, 1.0f) : k < 0.6f ? VV(1.0f, 0.97f, 0.93f) : k < 0.85f ? VV(1.0f, 0.86f, 0.62f) : VV(1.0f, 0.68f, 0.45f);
}
static V stars(V rd, float scale, float dens, float size, float bright, float boost, int full) {     // une couche d'étoiles, une au plus par cellule de l'espace
    V p = mul(rd, scale), c = VV(floorf(p.x), floorf(p.y), floorf(p.z)), acc = VV(0, 0, 0);
    int nc = full ? 1 : 0;                                                // les petites étoiles : la seule cellule où l'on est (elles y tiennent)
    for (int i = 0; i <= nc; i++) for (int j = 0; j <= nc; j++) for (int k = 0; k <= nc; k++) {
        V cc = full ? add(c, VV(i - (fract(p.x) < 0.5f), j - (fract(p.y) < 0.5f), k - (fract(p.z) < 0.5f))) : c;
        V h = hash3(cc); if (h.x > dens * boost) continue;
        V sp = full ? add(add(cc, mul(h, 0.8f)), VV(0.1f, 0.1f, 0.1f)) : add(add(cc, mul(h, 0.4f)), VV(0.3f, 0.3f, 0.3f));
        V sd = norm(sp); float ang = len(sub(rd, sd));
        float mag = powf(h.y, 3.0f), r = size * (0.5f + 1.3f * mag);
        float tw = 0.75f + 0.25f * sinf(T * (3.0f + 5.0f * h.z) + h.x * 50.0f);
        float I = expf(-sq(ang / r)) * (0.4f + 2.6f * mag) * bright * tw;
        if (mag > 0.70f) {                                                // aigrettes en étoile pour les plus brillantes
            V up = norm(cross(sd, VV(0.31f, 0.92f, 0.23f))), rt = cross(sd, up); V dv = sub(rd, sd);
            float x = fabsf(dot(dv, rt)), y = fabsf(dot(dv, up)), L = r * 9.0f * mag;
            I += (expf(-x / (r * 0.35f)) * expf(-y / L) + expf(-y / (r * 0.35f)) * expf(-x / L)) * 0.8f * bright * mag * tw;
        }
        acc = add(acc, mul(star_col(h.z), I));
    }
    return acc;
}
static V sky(V rd) {
    float h = rd.y, az = atan2f(rd.x, rd.z);
    // le dégradé du crépuscule : indigo profond en haut, violet, lueur rose-orangée à l'horizon
    V col = mixv(VV(0.42f, 0.12f, 0.20f), VV(0.12f, 0.04f, 0.18f), sstep(0.0f, 0.16f, h));
    col = mixv(col, VV(0.025f, 0.025f, 0.11f), sstep(0.10f, 0.45f, h));
    col = mixv(col, VV(0.004f, 0.006f, 0.03f), sstep(0.40f, 0.95f, h));
    col = add(col, mul(VV(0.30f, 0.11f, 0.05f), expf(-h * 14.0f) * (0.6f + 0.4f * sinf(az * 1.3f + 0.4f))));
    if (h > -0.02f) {
        float hv = sstep(-0.01f, 0.12f, h);                                 // les étoiles et la nuit apparaissent en montant
        // la Voie lactée : une bande qui traverse le ciel, cœur lumineux, nuages d'étoiles, bandes de poussière
        V MWN = norm(VV(-0.36f, 0.77f, -0.47f)); float bd = dot(rd, MWN);
        float band = expf(-sq(bd / 0.20f)), core = expf(-sq(bd / 0.07f));
        float n1 = 0, n2 = 0, dust = 0;
        if (band > 0.01f) { n1 = fbm(mul(rd, 7.0f), 5); n2 = fbm(add(mul(rd, 16.0f), VV(3.1f, 1.7f, 9.2f)), 4);       // le bruit seulement près de la bande
            dust = sstep(0.48f, 0.68f, fbm(add(mul(rd, 11.0f), VV(7.0f, 2.0f, 5.0f)), 4)) * expf(-sq(bd / 0.10f)); }
        float mw = band * (0.35f + 1.1f * n1 * n1) + core * 0.9f * n2;
        mw *= 1.0f - 0.85f * dust;
        V mwc = mixv(VV(0.35f, 0.45f, 0.95f), VV(1.0f, 0.80f, 0.62f), core * 0.8f + 0.2f * n2);
        col = add(col, mul(mwc, mw * 0.42f * hv));
        col = add(col, mul(VV(0.55f, 0.25f, 0.75f), band * 0.05f * hv));            // halo violet de la bande
        float mwb = 1.0f + 3.0f * band;                                     // plus d'étoiles dans la Voie lactée
        V st = add(add(stars(rd, 200.0f, 0.035f, 0.0011f, 0.16f, 1.0f + 4.0f * band, 0), stars(rd, 90.0f, 0.04f, 0.0016f, 0.45f, 1.0f + band, 0)),
                   stars(rd, 34.0f, 0.05f, 0.0024f, 1.5f, 1.0f, 1));
        col = add(col, mul(st, hv * (1.0f - 0.6f * dust)));
        // la comète : tête éclatante, queue d'ions turquoise fine et droite, queue de poussière dorée large et courbée
        V CH = norm(VV(0.62f + 0.004f * T, 0.46f, 0.64f)), TD = norm(cross(CH, VV(0.15f, 0.95f, -0.25f))), TN = cross(CH, TD);
        V dv = sub(rd, CH); float u = dot(dv, TD), v = dot(dv, TN);
        if (u > -0.02f && u < 0.95f && fabsf(v) < 0.35f) {
            float uu = fmaxf(u, 0), fall = powf(fmaxf(1 - uu / 0.95f, 0), 1.4f);
            float ion = expf(-sq((v - 0.02f * uu) / (0.0035f + 0.018f * uu))) * fall;
            float dst = expf(-sq((v + 0.25f * uu * uu) / (0.006f + 0.07f * uu))) * fall * fall;
            float streak = 0.75f + 0.25f * sinf(v * 900.0f + uu * 40.0f);
            col = add(col, mul(VV(0.45f, 0.95f, 1.0f), ion * 2.2f * streak));
            col = add(col, mul(VV(1.0f, 0.80f, 0.55f), dst * 1.1f));
        }
        float hd = len(dv);
        col = add(col, add(mul(VV(1.0f, 1.0f, 1.0f), 8.0f * expf(-sq(hd / 0.006f))), mul(VV(0.55f, 0.9f, 1.0f), 1.4f * expf(-hd / 0.03f))));
        // étoiles filantes : une toutes les 1,7 s environ
        float sn = floorf(T / 1.7f), sl = (T - sn * 1.7f) / 0.55f;
        if (sl < 1) {
            V a = hash3(VV(sn, 3, 7)), s0 = norm(VV(a.x * 1.6f - 0.8f, 0.30f + 0.45f * a.y, 0.6f + 0.4f * a.z)), sdir = norm(cross(s0, VV(a.z - 0.5f, 1, 0.3f)));
            V hp = norm(add(s0, mul(sdir, 0.35f * sl))); V w = sub(rd, hp); float along = dot(w, sdir), perp = len(sub(w, mul(sdir, along)));
            if (along < 0 && along > -0.12f) col = add(col, mul(VV(0.85f, 0.95f, 1.0f), expf(-sq(perp / 0.0016f)) * (1 + along / 0.12f) * sinf(3.14159f * sl) * 2.5f));
        }
    }
    // la lune : un fin croissant
    float cm = dot(rd, MOON);
    if (cm > 0.99955f && dot(rd, norm(add(MOON, VV(0.012f, 0.008f, 0)))) < 0.99962f) col = VV(1.3f, 1.2f, 0.95f);
    else col = add(col, mul(VV(0.35f, 0.30f, 0.35f), powf(fmaxf(cm, 0), 600)));
    // de fins nuages bas, éclairés en rose par en dessous
    int cld = 0;
    if (h > 0.0f && h < 0.22f) {
        float cn = fbm(VV(az * 3.0f + 0.02f * T, h * 22.0f, 1.3f), 5), th = sstep(0.0f, 0.06f, h) * sstep(0.22f, 0.10f, h);
        float c = sstep(0.52f, 0.70f, cn) * th;
        if (c > 0.01f) { V cc = mixv(VV(0.10f, 0.05f, 0.18f), VV(0.95f, 0.42f, 0.42f), sstep(0.70f, 0.52f, cn) * 0.8f + 0.2f * (1 - h / 0.22f)); col = mixv(col, cc, c * 0.9f); cld = c > 0.5f; }
    }
    // la ville en silhouette, fenêtres allumées
    float bx = floorf(az * 40.0f); float bh = 0.03f + 0.08f * hash2(bx, 3.0f) * hash2(bx, 7.0f);
    int sil = h < bh;
    if (sil) {
        col = VV(0.03f, 0.02f, 0.05f);
        float wx = fract(az * 160.0f), wy = fract(h * 300.0f);
        if (h > 0.004f && hash2(floorf(az * 160.0f), floorf(h * 300.0f)) > 0.62f && wx > 0.3f && wy > 0.35f) col = VV(0.9f, 0.62f, 0.22f);
    }
    if (h < 0) col = VV(0.03f, 0.02f, 0.05f);
    if (!sil && !cld && T > FW0) {
        float n0 = floorf((T - FW0) / 0.40f);
        for (int j = 0; j < 5; j++) {
            float n = n0 - j; if (n < 0) break;
            float u = (T - (FW0 + n * 0.40f)) / 1.9f; if (u >= 1) continue;
            float a = hash2(n, 1.0f), b = hash2(n, 2.0f), c = hash2(n, 5.0f);
            float fa = (a - 0.5f) * 1.7f, el = 0.34f + 0.36f * b - 0.05f * u * u;
            V cd = VV(sinf(fa) * cosf(el), sinf(el), cosf(fa) * cosf(el));
            if (dot(rd, cd) < 0.93f) continue;
            V rt = norm(cross(VV(0, 1, 0), cd)), up = cross(cd, rt);
            float x = dot(rd, rt), y = dot(rd, up), rho = sqrtf(x * x + y * y), phi = atan2f(y, x);
            float R = 0.21f * (1 - expf(-3.2f * u)) * (0.7f + 0.6f * c);
            const float st = 6.2832f / 26.0f;
            float dphi = phi - roundf(phi / st) * st;
            float I = expf(-(sq(rho - R) + sq(rho * dphi)) / sq(0.0048f));
            if (rho < R) I += expf(-sq(rho * dphi / 0.0032f)) * 0.3f * powf(rho / R, 4);
            float dphi2 = phi + st * 0.5f; dphi2 = dphi2 - roundf(dphi2 / (2 * st)) * 2 * st;
            I += 0.7f * expf(-(sq(rho - 0.55f * R) + sq(rho * dphi2)) / sq(0.0042f));
            float fade = powf(1 - u, 1.5f) * fminf(u * 20.0f, 1.0f);
            V fc = c < 0.33f ? VV(1.0f, 0.75f, 0.3f) : (c < 0.66f ? VV(1.0f, 0.35f, 0.55f) : VV(0.8f, 0.9f, 1.0f));
            col = add(col, mul(fc, (I * 3.0f + 0.10f * expf(-sq(rho / (R + 0.03f)))) * fade));
        }
    }
    return col;
}

static void material(int m, V p, V n, Mat *o) {
    switch (m) {
    case N_FLOOR: {
        float ax = fabsf(p.x), r = sqrtf(p.x * p.x + p.z * p.z);
        if ((ax < 1.35f && p.z < -1.9f && p.z > -17.0f) || r < 2.5f) {                // tapis rouge et rond sous le socle
            int edge = (r < 2.5f) ? (r > 2.32f) : (ax > 1.18f);
            o->alb = edge ? VV(0.85f, 0.62f, 0.2f) : VV(0.34f, 0.03f, 0.07f);
        } else {
            int ck = ((int)floorf(p.x / 2.0f) + (int)floorf(p.z / 2.0f)) & 1;
            float v = 0.92f + 0.08f * sinf(p.x * 2.1f + 3.0f * sinf(p.z * 1.3f + p.x * 0.4f));
            o->alb = mul(ck ? VV(0.06f, 0.05f, 0.065f) : VV(0.55f, 0.52f, 0.48f), v);
            o->f0 = 0.10f; o->shin = 120; o->rfl = 0.9f; o->rtrace = 1;
        }
        for (int b = 0; b < NB; b++) {                                    // la lumière des projecteurs posée sur le sol
            V q = sub(p, BO[b]); float sb = dot(q, BA[b]); if (sb <= 0) continue;
            float rr = BR[b] * sb / BL[b] + 0.05f, dd = len(sub(q, mul(BA[b], sb)));
            float pool = expf(-sq(sq(dd / rr))) * (0.75f + 0.25f * sinf(dd / rr * 9.0f)) * 0.25f;
            o->emi = add(o->emi, mul(mulv(o->alb, BC[b]), pool * (BR[b] < 1.5f ? 0.5f : 0.5f) * (T >= CUT[0] && T < CUT[1] ? 0.3f : 1.0f)));
        }
        break; }
    case N_LET: {                                                         // face en émail bordeaux, liseré et biseau d'or poli, ampoules de music-hall
        V q = sub(p, VV(0, 0, 12.5f)); float d2 = lsdf(q.x, q.y - LYC);
        o->alb = VV(1.0f, 0.72f, 0.26f); o->metal = 1; o->shin = 220; o->emi = mul(VV(0.40f, 0.26f, 0.08f), 0.5f);
        if (n.z < -0.7f && d2 < -0.11f) {
            o->metal = 0; o->alb = VV(0.30f, 0.015f, 0.05f); o->f0 = 0.18f; o->shin = 260; o->rfl = 1;
            if (fabsf(d2 + 0.26f) < 0.075f) {                                 // une rangée d'ampoules suit le contour, en retrait
                float cx = fract(q.x / 0.32f) - 0.5f, cy = fract((q.y - LYC) / 0.32f) - 0.5f;
                if (cx * cx + cy * cy < 0.07f) { float on = 0.5f + 0.5f * sinf(T * 7.0f - q.x * 0.9f - (q.y - LYC) * 0.5f);
                    o->alb = VV(1.0f, 0.9f, 0.7f); o->emi = mul(VV(1.6f, 1.15f, 0.6f), 0.35f + 1.4f * on * on); o->rfl = 0.3f; }
            }
        }
        break; }
    case N_MARBLE:
        o->alb = mul(VV(0.84f, 0.82f, 0.78f), 0.9f + 0.1f * sinf(p.y * 3.0f + 2.0f * sinf(p.x * 2.0f + p.z * 3.0f)));
        o->f0 = 0.06f; o->shin = 70; break;
    case N_VELVET: {
        float fr = powf(1 - fabsf(n.y), 2.0f);
        o->alb = mul(VV(0.30f, 0.012f, 0.05f), 0.65f + 0.9f * fr); o->shin = 400; o->f0 = 0.0f; break; }
    case N_RING: {                                                                     // or poli, volutes ciselées sur les faces
        V q = mul(ring_local(p), 1 / RSC); float rad = sqrtf(q.x * q.x + q.y * q.y), ang = atan2f(q.y, q.x), dr = rad - RR;
        float eng = 1.0f;
        if (fabsf(q.z) > RBZ - 0.004f && fabsf(dr) < RBW - 0.02f) {
            float w1 = fabsf(dr - 0.024f * sinf(ang * 32.0f)), w2 = fabsf(dr + 0.024f * sinf(ang * 32.0f));      // une tresse ciselée
            float lf = len(VV(fract(ang * 32.0f / 6.2831853f + 0.25f) - 0.5f, dr * 6.0f, 0));                       // et une perle gravée entre les brins
            if (w1 < 0.0045f || w2 < 0.0045f) eng = 0.42f; else if (lf < 0.11f) eng = 1.25f; else if (fabsf(fabsf(dr) - 0.040f) < 0.0025f) eng = 0.6f;
        }
        o->alb = mul(VV(1.0f, 0.70f, 0.22f), eng); o->metal = 1; o->shin = 260;
        o->emi = mul(VV(1.0f, 0.65f, 0.25f), GLOW * eng); break; }
    case N_RUBY: {                                                                     // rubis : rouge profond, feu intérieur qui change d'une facette à l'autre
        float fa = 0.5f + 0.5f * sinf(atan2f(GL.y, GL.x) * 4.0f + GL.z * 90.0f + T * 0.8f);
        o->alb = VV(0.55f, 0.0f, 0.03f); o->f0 = 0.22f; o->shin = 400; o->rfl = 1; o->rtrace = 1;
        o->emi = mul(VV(0.75f, 0.02f, 0.06f), (0.35f + 0.55f * fa * fa) * (0.8f + GLOW)); break; }
    case N_DIAM: {                                                                     // diamant : blanc glacé, éclats de couleur selon la facette
        float fa = atan2f(GL.y, GL.x) * 4.0f + GL.z * 120.0f + T * 1.3f;
        V rb = VV(0.5f + 0.5f * sinf(fa), 0.5f + 0.5f * sinf(fa + 2.1f), 0.5f + 0.5f * sinf(fa + 4.2f));
        o->alb = VV(0.25f, 0.27f, 0.30f); o->f0 = 0.5f; o->shin = 500; o->rfl = 1; o->rtrace = 1;
        o->emi = add(mul(rb, 0.30f + 0.2f * GLOW), mul(VV(0.6f, 0.65f, 0.75f), 0.18f)); break; }
    case N_TEAR: o->alb = VV(0.55f, 0.8f, 1.0f); o->f0 = 0.3f; o->shin = 200; o->rfl = 1; o->emi = VV(0.12f, 0.2f, 0.3f); break;
    case N_PANEL: {
        V q = sub(p, PANC); float c = cosf(TILT), s = sinf(TILT);
        V l = VV(q.x, q.y * c + q.z * s, q.z * c - q.y * s);
        if (l.z < -0.02f && fabsf(l.x) < 0.82f && fabsf(l.y) < 0.62f) {
            o->alb = texel(PLQ, 1024, 768, (l.x + 0.82f) / 1.64f, (0.62f - l.y) / 1.24f); o->emi = mul(o->alb, 0.35f);
        } else { o->alb = VV(1.0f, 0.72f, 0.26f); o->metal = 1; o->shin = 90; }
        break; }
    default: if (m >= D_SKIN) dn_to_mat(p, m, o); break;
    }
}

static void scene_init(void) {
    PIG_NOSHADOW = 1; P3D_GROUND = 1;
    grid_check("un cochon de la foule", 3.0f, CROWD_RMAX);
    LSDF = slurp("letters.f32", LW * LH * 4); PLQ = slurp("plaque.rgb", 1024 * 768 * 3);
    MOON = norm(VV(-0.55f, 0.36f, 1)); LDIR = norm(VV(-0.35f, 0.8f, -0.5f));
    SUNC = VV(1.7f, 1.45f, 1.15f); AMBU = VV(0.12f, 0.10f, 0.22f); AMBD = VV(0.06f, 0.03f, 0.03f); AMBC = VV(0.06f, 0.035f, 0.05f);
    RIMC = VV(0.6f, 0.35f, 0.2f); RIMD = norm(VV(0.3f, 0.3f, 1.0f));
    FOGC = VV(0.06f, 0.03f, 0.09f); FOGD = 0.003f; MAXD = 240; STEPS = 170;
    EXPO = 1.2f; CONTRAST = 0.4f; METAL_LIT = 0.5f; METAL_ENV = 0.9f; LETTER_EMI = VV(0.42f, 0.28f, 0.08f);
}

static void frame_setup(V *cam, V *tgt, float *focal) {
    float C0 = CUT[0], C1 = CUT[1], C2 = CUT[2], C3 = CUT[3], C4 = CUT[4], C5 = CUT[5];
    const float RY0 = 2.64f + RR + RBW - 0.02f, BEAT = 0.489f;
    RY = RY0; RSC = 1; ROT = 0.35f * T; GLOW = 0.04f + 0.03f * sinf(T * 1.3f); LYC = 200; FW0 = C5 + 0.2f;
    CROWD = T >= C1; MART = T >= C3; DNN = 0; DNLOOK = DN_MARTIN;
    NB = 0;                                                               // les projecteurs du musée : quatre douches chaudes sur l'objet
    for (int k = 0; k < 4; k++) { float a = 0.785f + k * 1.5708f;
        beam(VV(14.0f * sinf(a), 10.9f, 14.0f * cosf(a)), VV(0, (T >= C5 ? RY0 + 2.3f * sstep(C5, C5 + 2.2f, T) : 2.2f), 0), VV(1.0f, 0.80f, 0.52f), 0.8f, (T >= C0 && T < C1) ? 0.06f : 0.30f); }       // en gros plan, la caméra est dans les faisceaux : on les adoucit
    const float MS = 2.3f, MYAW = atan2f(1.75f, 2.9f);       // Martin Plaisir : 1,9 m dans son repère, à l'échelle des cochons de la rotonde
    if (T < C0) {                                   // on entre dans la rotonde par le tapis rouge
        float u = T / C0;
        *cam = VV(0.5f * sinf(0.35f * T), mixf(1.6f, 2.4f, u), mixf(-27.0f, -8.5f, sstep(0, 1, u) * 0.6f + u * 0.4f));
        V CHd = norm(VV(0.62f + 0.004f * T, 0.46f, 0.64f)), up = add(*cam, mul(CHd, 40.0f));
        float tilt = sstep(1.0f, 6.2f, T);
        *tgt = mixv(up, VV(0, 2.9f, 0), tilt); *focal = mixf(1.35f, mixf(1.6f, 2.1f, u), tilt);
    } else if (T < C1) {                            // macro : l'objet tourne sur son coussin
        float tl = T - C0, a = mixf(-0.55f, 0.45f, tl / (C1 - C0));
        float dist = mixf(2.3f, 1.9f, tl / (C1 - C0));
        *cam = VV(sinf(a) * dist, RY0 + 0.75f - 0.25f * tl / (C1 - C0), -cosf(a) * dist); *tgt = VV(0, RY0 - 0.05f, 0); *focal = 2.6f;
        GLOW = 0.10f; ROT = -a + 0.7f * sinf(tl * 0.45f);
    } else if (T < C2) {                            // les visiteurs en larmes
        float tl = T - C1, mid = (C2 - C1) * 0.5f;
        if (tl < mid) { float u = tl / mid;
            *cam = VV(mixf(-3.2f, -1.0f, u), 2.1f, 2.6f); *tgt = VV(mixf(1.5f, 2.5f, u), 1.3f, 9.0f); *focal = 1.45f; }
        else { float u = (tl - mid) / mid;
            *cam = VV(mixf(0.2f, 0.55f, u), 1.35f, mixf(3.6f, 4.2f, u)); *tgt = VV(1.3f, 1.05f, 6.2f); *focal = 2.3f; }
    } else if (T < C3) {                            // la fiche technique sur son pupitre
        float u = (T - C2) / (C3 - C2);
        *cam = VV(0.12f * sinf(u * 2.0f), mixf(2.30f, 2.20f, u), mixf(-3.15f, -2.95f, u)); *tgt = VV(0, 1.24f, -1.62f); *focal = 1.75f;
        CROWD = 1;
    } else if (T < C4) {                            // Martin Plaisir remonte le tapis, de dos
        float tl = T - C3; const float cyc = 4 * BEAT, L = 0.40f;
        dn_pose_t P; mp_walk(tl / cyc, T, &P); float mz = -11.2f + tl * L * MS / (cyc * 0.5f);
        dn_place(&P, VV(0, 0, mz), 0, MS);
        *cam = VV(0.9f, 1.05f, mz - 5.2f); *tgt = VV(0, 3.2f, mz + 6.0f); *focal = 1.35f;
    } else if (T < C5) {                            // cérémonie : TRÉSOR NATIONAL descend du ciel
        float tl = T - C4, u = tl / (C5 - C4);
        { float k = clampf((tl - 0.6f) / 3.0f, 0, 1), e = 1 - powf(1 - k, 3.0f);                    // descente qui ralentit, puis un léger rebond
          LYC = mixf(48.0f, 6.0f, e) + 0.35f * sinf(3.14159f * clampf((tl - 3.6f) / 0.6f, 0, 1)) * (tl > 3.6f); }
        for (int k = 0; k < 6; k++) { float a = 0.5f + k * 1.047f, sw = 0.45f * sinf(T * 0.6f + k * 1.7f);
            V cl = k % 3 == 0 ? VV(1.0f, 0.78f, 0.35f) : k % 3 == 1 ? VV(1.0f, 0.45f, 0.75f) : VV(0.45f, 0.85f, 1.0f);
            beam(VV(17.5f * sinf(a), 0.4f, 17.5f * cosf(a)), VV(9.0f * sinf(a + sw), 46.0f, 9.0f * cosf(a + sw)), cl, 2.2f, 0.22f); }
        *cam = VV(mixf(5.5f, -4.5f, u), mixf(6.0f, 5.0f, u), mixf(-12.5f, -11.5f, u)); *tgt = VV(0, 4.6f, 4.0f); *focal = 1.3f;
        { dn_pose_t P; ml_sexy(tl / BEAT, 1.0f, &P); dn_place(&P, VV(-1.75f, 0, -2.9f), MYAW, MS); }
    } else {                                        // l'objet lévite, Martin lève le bras, feu d'artifice
        float tl = T - C5, k = sstep(0.0f, 2.2f, tl);
        LYC = 6.0f; RSC = 1 + 1.6f * k; RY = RY0 + 2.3f * k + 0.06f * sinf(tl * 2.2f) * k; ROT = 0.35f * C5 + (0.35f + 1.6f * k) * tl;
        GLOW = 0.08f + 1.4f * k;
        for (int j = 0; j < 6; j++) { float a = 0.5f + j * 1.047f, sw = 0.45f * sinf(T * 0.6f + j * 1.7f), cv = sstep(1.0f, 3.0f, tl) * (1 - sstep(4.5f, 6.5f, tl));
            V cl = j % 3 == 0 ? VV(1.0f, 0.78f, 0.35f) : j % 3 == 1 ? VV(1.0f, 0.45f, 0.75f) : VV(0.45f, 0.85f, 1.0f);
            V sky_t = VV(9.0f * sinf(a + sw), 46.0f, 9.0f * cosf(a + sw)), ring_t = VV(0, RY + 25.0f, 0);       // les projecteurs se croisent au-dessus de l'anneau
            beam(VV(17.5f * sinf(a), 0.4f, 17.5f * cosf(a)), mixv(sky_t, ring_t, cv), cl, 2.2f, 0.24f); }
        { dn_pose_t P; ml_sexy((C5 - C4 + tl) / BEAT, 1.0f, &P); dn_place(&P, VV(-1.75f, 0, -2.9f), MYAW, MS); }
        float v = sstep(0, 1, tl / 7.0f);
        *cam = VV(mixf(1.5f, 3.2f, v), mixf(0.9f, 3.0f, v), mixf(-8.2f, -14.5f, v)); *tgt = VV(0, mixf(4.2f, 5.6f, v), 2.0f); *focal = 1.45f;
    }
    for (int i = 0; i < NPART; i++) {                                    // poussières d'or ; au final, 90 étincelles tournent autour de l'anneau
        V h = hash3(VV(i, 17, 3));
        if (T > C5 && i < 90) { float a = T * (1.2f + h.x) + i * 0.7f, r = (0.7f + 1.6f * h.y) * RSC; PP[i] = VV(r * cosf(a), RY + (h.z - 0.5f) * 2.2f * RSC + 0.4f * sinf(T * 2 + i), r * sinf(a)); }
        else { float a = h.x * 6.2832f, r = 1.5f + 11.5f * sqrtf(h.y); PP[i] = VV(r * cosf(a) + 0.35f * sinf(T * 0.4f + i), 0.4f + fract(h.z + T * 0.025f * (0.5f + h.x)) * 9.0f, r * sinf(a) + 0.35f * cosf(T * 0.33f + i * 1.3f)); }
        PSZ[i] = 0.018f + 0.03f * h.z; PIN[i] = (0.55f + 0.45f * sinf(T * (2.0f + 3.0f * h.y) + i)) * (T > C5 && i < 90 ? 9.0f : 5.0f);
        PCOL[i] = h.y < 0.8f ? VV(1.0f, 0.78f, 0.38f) : VV(1.0f, 0.55f, 0.65f);
    }
    {   float c0 = cosf(ROT), s0 = sinf(ROT); const float st = 6.2831853f / NGEM;
        for (int f = 0; f < 2; f++) for (int k = 0; k < NGEM; k++) { int g = f * NGEM + k; float zs = f ? 1.0f : -1.0f, ak = (k + 0.5f) * st, rg = gem_r(k);
            V gl = VV(RR * cosf(ak), RR * sinf(ak), zs * (RBZ + 0.022f + 0.3f * rg));
            GW[g] = add(VV(0, RY, 0), mul(VV(c0 * gl.x + s0 * gl.z, gl.y, -s0 * gl.x + c0 * gl.z), RSC)); GN[g] = VV(s0 * zs, 0, c0 * zs);
            float tw = sinf(T * 2.3f + k * 2.39f + f * 1.1f); GTW[g] = tw > 0 ? powf(tw, 10.0f) : 0; } }
#ifdef DANCECAM
    if (T >= C4) { *cam = VV(-2.72f, 2.6f, -9.34f); *tgt = VV(-1.75f, 2.3f, -2.9f); *focal = 1.7f; }
#endif
}

// halo doré façon image de synthèse des années 90 : lueur et rayons autour de l'objet, plus forts quand il lévite
static V halo(V ro, V rd, V c) {
    float hitT = P3D_HITT;
    for (int b = 0; b < NB; b++) {                                       // les faisceaux dans l'air (calcul exact, sans échantillons)
        V w0 = sub(ro, BO[b]); float bb = dot(rd, BA[b]), dd = dot(rd, w0), ee = dot(BA[b], w0), den = fmaxf(1 - bb * bb, 1e-4f);
        float t = (bb * ee - dd) / den, sb = clampf((ee - bb * dd) / den, 0, BL[b]);
        if (t < 0) continue;
        t = fmaxf(dot(sub(add(BO[b], mul(BA[b], sb)), ro), rd), 0);
        float occl = hitT > 1e8f ? 1.0f : clampf((hitT + 0.6f - t) / 1.2f, 0, 1);       // devant le ciel : rien ne cache le faisceau
        if (occl <= 0) continue;
        float dist = len(sub(add(ro, mul(rd, t)), add(BO[b], mul(BA[b], sb)))), rr = BR[b] * sb / BL[b] + 0.06f;
        float dust = 0.7f + 0.3f * sinf(sb * 2.3f + T * 0.7f) * sinf(sb * 0.9f - T * 0.4f);
        float I = (expf(-sq(dist / rr)) + 1.6f * expf(-sq(dist / (0.28f * rr)))) * rr / fmaxf(sqrtf(den), 0.18f) * BS[b] * dust * occl * sstep(0, 2.0f, sb);
        c = add(c, mul(BC[b], I));
    }
    for (int i = 0; i < NPART; i++) {                                    // les poussières d'or (positions calculées une fois par image)
        V q = sub(PP[i], ro); float pj = dot(q, rd); if (pj < 0.25f || pj > hitT) continue;
        float dd = len(sub(q, mul(rd, pj))), eff = fmaxf(PSZ[i], pj * 0.0022f);
        float I = expf(-sq(dd / eff)) * sq(PSZ[i] / eff) * PIN[i];
        c = add(c, mul(PCOL[i], I));
    }
    for (int g = 0; g < 2 * NGEM; g++) {                                 // les pierres scintillent (étoile à quatre branches)
        if (GTW[g] < 0.01f) continue;
        V tg = sub(GW[g], ro); float dist = len(tg); tg = mul(tg, 1 / dist);
        if (dot(GN[g], mul(tg, -1)) < 0.15f) continue;
        V rt = norm(cross(VV(0, 1, 0), tg)), up = cross(tg, rt), dv = sub(rd, tg);
        float x = fabsf(dot(dv, rt)) * dist, y = fabsf(dot(dv, up)) * dist, sz = 0.11f * RSC;
        float star = expf(-x / (0.006f * RSC)) * expf(-y / sz) + expf(-y / (0.006f * RSC)) * expf(-x / sz) + 2.0f * expf(-(x * x + y * y) / sq(0.012f * RSC));
        c = add(c, mul((g & 1) ? VV(0.9f, 0.95f, 1.0f) : VV(1.0f, 0.55f, 0.55f), GTW[g] * star * 1.6f));
    }
    if (GLOW < 0.05f) return c;
    V tc = sub(VV(0, RY, 0), ro); float dist = len(tc); tc = mul(tc, 1 / dist);
    float ca = dot(rd, tc); if (ca < 0.8f) return c;
    float th = acosf(fminf(ca, 1.0f)) * dist;                     // écart en unités, à la distance de l'objet
    V rt = norm(cross(VV(0, 1, 0), tc)), up = cross(tc, rt);
    float phi = atan2f(dot(rd, up), dot(rd, rt));
    float g = GLOW * (0.22f * expf(-sq(th / 0.9f)) + 0.2f * expf(-th / 1.4f));
    float rays = powf(fabsf(cosf(4.0f * (phi + 0.3f * T))), 30.0f) + 0.6f * powf(fabsf(cosf(3.0f * (phi - 0.21f * T) + 0.5f)), 50.0f);
    g += GLOW * 0.5f * rays * expf(-th / 2.2f) * sstep(0.1f, 0.6f, GLOW);
    return add(c, mul(VV(1.0f, 0.72f, 0.32f), g));
}
