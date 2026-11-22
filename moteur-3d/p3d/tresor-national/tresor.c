// Canal Pork : « Le Cockring Homologué Martin Plaisir, trésor national ». Rotonde du Musée national de Porkonia, la nuit.
// Coupes : CUT[0] macro de l'objet, CUT[1] les visiteurs en larmes, CUT[2] la fiche technique, CUT[3] Martin Plaisir
// remonte le tapis (de dos), CUT[4] cérémonie : les lettres TRÉSOR NATIONAL descendent, CUT[5] l'objet lévite, feu d'artifice.
#define POSTFX halo

#include "p3d.h"
#include "martin_dn.h"
enum { N_FLOOR = M_USER, N_MARBLE, N_VELVET, N_RING, N_RUBY, N_DIAM, N_TEAR, N_PANEL, N_SATIN, N_FLARE, N_BOOT, N_MSKIN };
static unsigned char *PLQ;
static V MOON;
static float RY, ROT, GLOW, LYC, FW0, RSC = 1;
static int CROWD, MART;

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
    if (b > 0.3f) return b;
    return ring_q(mul(ring_local(p), 1 / RSC), m) * RSC;
}

// ---------------------------------------------------------------- socle, coussin de velours, pompons, pupitre
static float pedestal(V p, int *m) {
    float b = box(sub(p, VV(0, 1.6f, -0.6f)), VV(1.3f, 1.7f, 1.9f));
    if (b > 0.4f) return b;
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
    if (b > 0.5f) return b;
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
static float crowd(V p, int *m) {
    float r = sqrtf(p.x * p.x + p.z * p.z);
    float db = fmaxf(fmaxf(6.2f - r, r - 14.0f), p.y - 2.0f);
    if (db > 0.6f) return db;
    const float cs = 3.0f;
    float ix = floorf(p.x / cs), iz = floorf(p.z / cs);
    float cx = (ix + 0.5f) * cs, cz = (iz + 0.5f) * cs, lx = p.x - cx, lz = p.z - cz;
    float dborder = fminf(cs * 0.5f - fabsf(lx), cs * 0.5f - fabsf(lz)) + 0.45f;
    float rc = sqrtf(cx * cx + cz * cz);
    if (rc < 6.6f || rc > 13.2f || (fabsf(cx) < 2.9f && cz < 0)) return dborder;
    float h = hash2(ix, iz), h2 = hash2(iz + 3.0f, ix);
    float ang = atan2f(-cx, -cz) + (h - 0.5f) * 0.5f;
    V dir = VV(sinf(ang), 0, cosf(ang)), rt = VV(dir.z, 0, -dir.x);
    const float SC = 0.85f;
    float sob = 0.045f * fabsf(sinf(T * 7.0f + h * 20.0f));                           // les sanglots
    V l = mul(VV(lx * rt.x + lz * rt.z, p.y - sob, lx * dir.x + lz * dir.z), 1 / SC);
    int mm; float d = pig(l, 0.0f, h2 < 0.12f ? M_GOLD : (h2 < 0.22f ? M_CHROME : M_PINK), &mm) * SC;
    float f = fract(T * 0.42f + h * 7.0f);                                             // une larme par œil : elle glisse sur la joue, puis tombe
    float fs = fminf(f, 0.75f) / 0.75f, u = fmaxf(f - 0.75f, 0) * 2.4f;
    V tdir = norm(VV(0.50f + 0.12f * fs, 0.40f - 1.05f * fs, 0.80f - 0.12f * fs));
    V tc = add(VV(0, 1.15f, 0.95f), mul(tdir, 0.495f + 0.03f * sstep(0.85f, 1.0f, fs)));
    tc = add(tc, VV(0.08f * u, -4.5f * u * u, 0.12f * u));
    float e = (len(sub(VV(fabsf(l.x), l.y, l.z), tc)) - (0.042f - 0.008f * (u > 0))) * SC;
    if (e < d) { d = e; mm = N_TEAR; }
    if (d < dborder) { *m = mm; return d; }
    return dborder;
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
    if (LYC < 100) { e = letters(sub(p, VV(0, 0, 12.5f)), LYC, 0.6f); if (e < d) { d = e; *m = M_LETTER; } }
    return d;
}

static V sky(V rd) {
    float h = rd.y;
    V col = mixv(VV(0.26f, 0.06f, 0.15f), VV(0.02f, 0.02f, 0.08f), sstep(0.0f, 0.45f, h));
    float az = atan2f(rd.x, rd.z);
    if (h > 0.05f) {
        float u = az * 90.0f, v = h * 140.0f; float s = hash2(floorf(u), floorf(v));
        if (s > 0.985f) { float fx = fract(u) - 0.5f, fy = fract(v) - 0.5f;
            col = add(col, mul(VV(1, 1, 0.9f), 1.5f * fmaxf(0, 1 - 12 * (fx * fx + fy * fy)) * (0.6f + 0.4f * sinf(T * 5 + s * 100)))); }
    }
    float cm = dot(rd, MOON);
    if (cm > 0.9988f) col = VV(1.0f, 0.95f, 0.8f); else col = add(col, mul(VV(0.4f, 0.35f, 0.3f), powf(fmaxf(cm, 0), 300)));
    float bx = floorf(az * 40.0f); float bh = 0.03f + 0.08f * hash2(bx, 3.0f) * hash2(bx, 7.0f);
    int sil = h < bh;
    if (sil) {
        col = VV(0.03f, 0.02f, 0.05f);
        float wx = fract(az * 160.0f), wy = fract(h * 300.0f);
        if (h > 0.004f && hash2(floorf(az * 160.0f), floorf(h * 300.0f)) > 0.62f && wx > 0.3f && wy > 0.35f) col = VV(0.9f, 0.62f, 0.22f);
    }
    if (h < 0) col = VV(0.03f, 0.02f, 0.05f);
    if (!sil && T > FW0) {
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
    PIG_NOSHADOW = 1;
    LSDF = slurp("letters.f32", LW * LH * 4); PLQ = slurp("plaque.rgb", 1024 * 768 * 3);
    MOON = norm(VV(-0.5f, 0.42f, 1)); LDIR = norm(VV(-0.35f, 0.8f, -0.5f));
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
    const float MS = 2.3f, MYAW = atan2f(1.75f, 2.9f);       // Martin Plaisir : 1,9 m dans son repère, à l'échelle des cochons de la rotonde
    if (T < C0) {                                   // on entre dans la rotonde par le tapis rouge
        float u = T / C0;
        *cam = VV(0.5f * sinf(0.35f * T), mixf(1.6f, 2.4f, u), mixf(-27.0f, -8.5f, sstep(0, 1, u) * 0.6f + u * 0.4f));
        *tgt = VV(0, 2.9f, 0); *focal = mixf(1.6f, 2.1f, u);
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
        LYC = mixf(48.0f, 6.0f, sstep(0.6f, 3.4f, tl));
        *cam = VV(mixf(5.5f, -4.5f, u), mixf(6.0f, 5.0f, u), mixf(-12.5f, -11.5f, u)); *tgt = VV(0, 4.6f, 4.0f); *focal = 1.3f;
        { dn_pose_t P; ml_sexy(tl / BEAT, 1.0f, &P); dn_place(&P, VV(-1.75f, 0, -2.9f), MYAW, MS); }
    } else {                                        // l'objet lévite, Martin lève le bras, feu d'artifice
        float tl = T - C5, k = sstep(0.0f, 2.2f, tl);
        LYC = 6.0f; RSC = 1 + 1.6f * k; RY = RY0 + 2.3f * k + 0.06f * sinf(tl * 2.2f) * k; ROT = 0.35f * C5 + (0.35f + 1.6f * k) * tl;
        GLOW = 0.08f + 1.4f * k;
        { dn_pose_t P; ml_sexy((C5 - C4 + tl) / BEAT, 1.0f, &P); dn_place(&P, VV(-1.75f, 0, -2.9f), MYAW, MS); }
        float v = sstep(0, 1, tl / 7.0f);
        *cam = VV(mixf(1.5f, 3.2f, v), mixf(0.9f, 3.0f, v), mixf(-8.2f, -14.5f, v)); *tgt = VV(0, mixf(4.2f, 5.6f, v), 2.0f); *focal = 1.45f;
    }
#ifdef DANCECAM
    if (T >= C4) { *cam = VV(-2.72f, 2.6f, -9.34f); *tgt = VV(-1.75f, 2.3f, -2.9f); *focal = 1.7f; }
#endif
}

// halo doré façon image de synthèse des années 90 : lueur et rayons autour de l'objet, plus forts quand il lévite
static V halo(V ro, V rd, V c) {
    {   // les pierres scintillent : une étoile à quatre branches, de temps en temps, sur les pierres tournées vers la caméra
        float c0 = cosf(ROT), s0 = sinf(ROT); const float st = 6.2831853f / NGEM;
        for (int f = 0; f < 2; f++) for (int k = 0; k < NGEM; k++) {
            float zs = f ? 1.0f : -1.0f, ak = (k + 0.5f) * st, rg = gem_r(k);
            V gl = VV(RR * cosf(ak), RR * sinf(ak), zs * (RBZ + 0.022f + 0.3f * rg));
            V gw = add(VV(0, RY, 0), mul(VV(c0 * gl.x + s0 * gl.z, gl.y, -s0 * gl.x + c0 * gl.z), RSC));
            V nw = VV(s0 * zs, 0, c0 * zs), tg = sub(gw, ro); float dist = len(tg); tg = mul(tg, 1 / dist);
            if (dot(nw, mul(tg, -1)) < 0.15f) continue;
            float tw = sinf(T * 2.3f + k * 2.39f + f * 1.1f); tw = tw > 0 ? powf(tw, 10.0f) : 0; if (tw < 0.01f) continue;
            V rt = norm(cross(VV(0, 1, 0), tg)), up = cross(tg, rt), dv = sub(rd, tg);
            float x = fabsf(dot(dv, rt)) * dist, y = fabsf(dot(dv, up)) * dist, sz = 0.11f * RSC;
            float star = expf(-x / (0.006f * RSC)) * expf(-y / sz) + expf(-y / (0.006f * RSC)) * expf(-x / sz) + 2.0f * expf(-(x * x + y * y) / sq(0.012f * RSC));
            c = add(c, mul((k & 1) ? VV(0.9f, 0.95f, 1.0f) : VV(1.0f, 0.55f, 0.55f), tw * star * 1.6f));
        }
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
