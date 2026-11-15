// ============================ LE CITOYEN : le cochon de base de Porkonia ============================
// Un seul modèle, trois réglages indépendants :
//   cz_t    la personne  : morphologie (taille, ventre, tête, groin, oreilles, yeux), teinte, tenue par couches
//   pose_t  ce qu'elle fait : bassin, torse, tête, mains et pieds (cinématique inverse), oreilles, queue, visage
//   rig_t   le squelette calculé une fois par image à partir des deux autres (cz_rig)
// Repère : pieds sur y = 0, regarde vers +z, 1 unité = 1 m. Le citoyen de base mesure 1,43 m (1,6 m avec les oreilles).
// cz_sdf(C, R, P, p, &m) : distance + matière ; cz_mat(...) : peinture (visage, motifs, boutons, bretelles...).
// N'utilise que les primitives du moteur (v3, sdEll, sdCap, smin, box2, cylY...).

enum { C_SKIN = 200, C_HOOF, C_EYE, C_MOUTH, C_BROW, C_TOP, C_PANTS, C_JACKET, C_APRON, C_DRESS, C_SCARF, C_HAT, C_GLASSES, C_MUG, C_FOAM, C_CANE, C_KNOB, C_TONGUE };
enum { HAT_NONE, HAT_BERET, HAT_CAP, HAT_BOATER, HAT_TOP, HAT_BEANIE, HAT_TOQUE, HAT_KEPI, HAT_BOWLER };
enum { TOP_NONE, TOP_SHIRT, TOP_TEE };
enum { PROP_NONE, PROP_MUG, PROP_CANE };
enum { PAT_PLAIN, PAT_VSTRIPE, PAT_HSTRIPE, PAT_CHECK, PAT_DOTS };
#define F_PANTS 1
#define F_JACKET 2
#define F_APRON 4
#define F_DRESS 8
#define F_SCARF 16
#define F_TIE 32
#define F_BRACES 64
#define F_GLASSES 128
#define F_STACHE 256
#define F_BIB 512
#define F_COAT 1024
#define F_SASH 2048
#define F_BOW 4096
#define F_BUTTONS 8192
#define F_CLOSED 16384     // veste fermée jusqu'au col
#define F_LASH 32768        // de longs cils

typedef struct {
    float sc, fat, head, leg, snout, ear, eye;      // 0 = le citoyen de base ; de -1 à +1
    v3 skin;
    int top, hat, prop, flags, ptop, pbot, pdress, papron;
    v3 ctop, ctop2, cbot, cjack, chat, chat2, cacc, capron, capron2, cdress, cdress2;
} cz_t;
typedef struct {
    float bob, sway, lean, twist, roll, hyaw, hpitch, hroll;
    v3 hand[2], foot[2];        // 0 : côté +x, 1 : côté -x ; mains dans le repère du torse, pieds dans celui du sol
    float footp[2];             // le sabot bascule : pointe vers le bas (+) ou vers le haut (-)
    float earflop[2], tail, blink, lookx, looky, smile, open, brow, frown, happy;
} pose_t;
typedef struct {
    v3 ot, tx, ty, tz;          // torse : origine (le bassin) et axes
    v3 nk, hx, hy, hz;          // tête : pivot et axes, dans le repère du torse
    v3 hip[2], knee[2], foot[2];            // repère du sol
    v3 sh[2], el[2], ha[2], fd[2], hn[2];   // repère du torse ; fd : direction de l'avant-bras ; hn : plan de la fente du sabot
    v3 tb, tu, tw, ta, tlc;     // la queue : son repère (base, deux travers, l'axe) et le centre de sa sphère englobante
    float F, HS, hipy, fc[2], fs[2];
} rig_t;

#ifndef CZ_BND0
#define CZ_BND0 0.12f
#endif
static __thread float CZ_BND = CZ_BND0;      // au-delà de cette distance, une sphère englobante suffit (à élargir pour les rayons d'ombre)
static __thread int CZ_PART;                // 0 corps, 1 tête, 2 groin, 3 oreille, 4 queue
static __thread v3 CZ_Q, CZ_H, CZ_EAR;      // le point dans le repère du torse, de la tête, de l'oreille

S v3 cz_loc(v3 d, v3 ax, v3 ay, v3 az) { return V(dot(d, ax), dot(d, ay), dot(d, az)); }
S float cz_smax(float a, float b, float k) { return -smin(-a, -b, k); }
S float sdVes(float x, float y, float r, float d) {       // amande : deux arcs de cercle
    x = fabsf(x); y = fabsf(y); float b = sqrtf(r * r - d * d);
    return ((y - b) * d > x * b) ? len2(x, y - b) : len2(x + d, y) - r;
}
S v3 cz_rot(v3 v, float yaw, float pitch, float roll) {     // roulis (z), puis tangage (x, positif = vers l'avant), puis lacet (y)
    float c = cosf(roll), s = sinf(roll); v = V(v.x * c - v.y * s, v.x * s + v.y * c, v.z);
    c = cosf(pitch); s = sinf(pitch); v = V(v.x, v.y * c - v.z * s, v.y * s + v.z * c);
    c = cosf(yaw); s = sinf(yaw); return V(v.x * c + v.z * s, v.y, -v.x * s + v.z * c);
}
static v3 cz_ik(v3 a, v3 t, float l1, float l2, v3 pole, float stretch, v3 *end) {      // deux os : renvoie l'articulation du milieu
    v3 d = sub(t, a); float L = len(d) + 1e-6f, Lc = clampf(L, 0.02f, (l1 + l2) * stretch - 0.002f);
    if (Lc > l1 + l2 - 0.002f) { float k = Lc / (l1 + l2 - 0.002f); l1 *= k; l2 *= k; }
    v3 n = mul(d, 1 / L); float x = (l1 * l1 - l2 * l2 + Lc * Lc) / (2 * Lc), h = sqrtf(fmaxf(l1 * l1 - x * x, 0));
    v3 pp = nrm(sub(pole, mul(n, dot(pole, n))));
    *end = add(a, mul(n, Lc));
    return add(add(a, mul(n, x)), mul(pp, h));
}

static void cz_base(cz_t *C) {
    memset(C, 0, sizeof(*C)); C->sc = 1; C->skin = V(0.92f, 0.37f, 0.38f);
    C->ctop = V(0.85f, 0.82f, 0.74f); C->ctop2 = V(0.30f, 0.03f, 0.07f); C->cbot = V(0.20f, 0.13f, 0.09f); C->cjack = V(0.06f, 0.07f, 0.10f);
    C->chat = V(0.30f, 0.03f, 0.07f); C->chat2 = V(0.85f, 0.62f, 0.20f); C->cacc = V(0.55f, 0.04f, 0.08f);
    C->capron = V(0.88f, 0.86f, 0.80f); C->capron2 = V(0.42f, 0.04f, 0.09f); C->cdress = V(0.42f, 0.04f, 0.09f); C->cdress2 = V(0.90f, 0.86f, 0.76f);
}
static void cz_rest(const cz_t *C, pose_t *P) {
    memset(P, 0, sizeof(*P));
    P->hand[0] = V(0.365f + 0.06f * C->fat, 0.112f, 0.075f); P->hand[1] = V(-0.365f - 0.06f * C->fat, 0.112f, 0.075f);
    P->foot[0] = V(0.138f, 0.060f, 0); P->foot[1] = V(-0.138f, 0.060f, 0);
    P->earflop[0] = P->earflop[1] = 0.55f; P->smile = 0.6f; P->looky = 0.15f;
}
static void cz_rig(const cz_t *C, const pose_t *P, rig_t *R) {
    float LEG = 0.285f + 0.09f * C->leg, l = LEG * 0.492f;
    R->F = 1 + 0.22f * C->fat; R->HS = 1 + 0.15f * C->head; R->hipy = 0.060f + LEG * 0.97f + P->bob;
    R->ot = V(P->sway, R->hipy, 0);
    R->tx = cz_rot(V(1, 0, 0), P->twist, P->lean, P->roll); R->ty = cz_rot(V(0, 1, 0), P->twist, P->lean, P->roll); R->tz = cz_rot(V(0, 0, 1), P->twist, P->lean, P->roll);
    R->nk = V(0, 0.54f, 0);
    R->hx = cz_rot(V(1, 0, 0), P->hyaw, P->hpitch, P->hroll); R->hy = cz_rot(V(0, 1, 0), P->hyaw, P->hpitch, P->hroll); R->hz = cz_rot(V(0, 0, 1), P->hyaw, P->hpitch, P->hroll);
    for (int s = 0; s < 2; s++) {
        float sg = s ? -1.0f : 1.0f; v3 e;
        R->hip[s] = V(P->sway + sg * 0.128f * (0.6f + 0.4f * R->F), R->hipy - 0.01f, 0);
        R->knee[s] = cz_ik(R->hip[s], P->foot[s], l, l, V(sg * 0.15f, 0, 1), 1.0f, &e); R->foot[s] = e; R->fc[s] = cosf(P->footp[s]); R->fs[s] = sinf(P->footp[s]);
        R->sh[s] = V(sg * (0.215f + 0.02f * C->fat), 0.450f, 0);
        R->el[s] = cz_ik(R->sh[s], P->hand[s], 0.175f, 0.175f, V(sg * 0.7f, -0.25f, -0.65f), 1.2f, &e); R->ha[s] = e;
        R->fd[s] = nrm(sub(R->ha[s], R->el[s]));
        R->hn[s] = nrm(cross(R->fd[s], V(sg, 0.2f, 0.1f)));
    }
    {   float c = cosf(P->tail), sn = sinf(P->tail);       // la queue : elle frétille autour de sa base
        R->tb = V(0, 0.07f, -0.236f * R->F); R->tu = V(c, 0, -sn); R->tw = V(0.34f * sn, 0.94f, 0.34f * c); R->ta = V(-0.94f * sn, 0.34f, -0.94f * c);
        R->tlc = add(R->tb, mul(R->ta, 0.055f)); }
}

static float cz_torso(const rig_t *R, v3 q) {
    float F = R->F, G = 0.5f + 0.5f * F;
    float b = sdEll(sub(q, V(0, 0.15f, 0.015f)), V(0.295f * F, 0.250f, 0.262f * F));
    float c = sdEll(sub(q, V(0, 0.37f, 0)), V(0.218f * G, 0.20f, 0.188f * G));
    return smin(smin(b, c, 0.12f), len(sub(q, V(0, 0.11f, 0.115f * F))) - 0.19f * F, 0.10f);
}
#define CZ_HEADC V(0, 0.22f, 0.03f)
#define CZ_HEADR V(0.37f, 0.31f, 0.32f)
static float cz_head(const cz_t *C, const pose_t *P, v3 h, int *m, int *part) {
    v3 a = V(fabsf(h.x), h.y, h.z); int sd = h.x > 0 ? 0 : 1; *m = C_SKIN; *part = 1;
    float d = sdEll(sub(a, CZ_HEADC), CZ_HEADR);
    d = smin(d, len(sub(a, V(0.185f, 0.115f, 0.145f))) - 0.165f, 0.10f);                                      // les joues
    d = smin(d, len(sub(a, V(0, 0.055f, 0.185f))) - 0.135f, 0.08f);                                             // le menton
    float sl = 0.335f + 0.05f * C->snout;
    {   v3 s = sub(a, V(0, 0.190f, sl));                                                                        // le groin
        float sn = box2(len2(s.x / 1.25f, s.y) - 0.066f, fabsf(s.z) - 0.075f) - 0.032f;
        if (sn < d) *part = 2;
        d = smin(d, sn, 0.045f); }
    {   float E = 1.06f + 0.3f * C->ear, fl = P->earflop[sd];                                                    // les oreilles : une feuille en cornet, tournée vers l'avant et le dehors, dont la pointe retombe
        v3 e = sub(a, V(0.245f, 0.415f, -0.015f));
        float ex = 0.857f * e.x - 0.515f * e.y, ey0 = 0.515f * e.x + 0.857f * e.y, ey = 0.955f * ey0 + 0.296f * e.z, ez0 = -0.296f * ey0 + 0.955f * e.z;
        float ex2 = 0.732f * ex - 0.682f * ez0, ez = 0.682f * ex + 0.732f * ez0, yy = fmaxf(ey + 0.02f, 0), xc = clampf(ex2, -0.13f, 0.13f);
        ez -= fl * 1.9f * yy * yy + 1.8f * xc * xc;
        float d2 = sdVes(ex2, ey - 0.13f * E, 0.186f * E, 0.0764f * E), w = fabsf(ez) - 0.010f;
        float kb = len2(3.8f * fabsf(fl) * fminf(yy, 0.36f), 3.6f * fabsf(xc));                                   // la courbure étire l'espace : on raccourcit le pas d'autant
        float de = (fminf(fmaxf(d2, w), 0) + len2(fmaxf(d2, 0), fmaxf(w, 0)) - 0.016f) * 0.92f / sqrtf(1 + kb * kb);
        if (de < d) { *part = 3; CZ_EAR = V(d2, ey, ez); }
        d = smin(d, de, 0.035f); }
    {   float nd = sdEll(sub(a, V(0.043f, 0.205f, sl + 0.108f)), V(0.019f, 0.030f, 0.032f));                     // les narines
        if (-nd > d) { d = -nd; *m = C_MOUTH; } }
    float sm = P->smile, op = P->open, mcy = 0.022f - 0.030f * op, mhy = 0.008f + 0.060f * op;
    {   float xe = 0.134f + 0.022f * sm, y0 = 0.034f - 0.030f * sm, y1 = 0.070f + 0.075f * sm, xc = fminf(a.x, xe), u = xc / xe;       // la bouche : un arc qui s'affine vers les coins
        float md = len(sub(a, V(xc, y0 + (y1 - y0) * u * u, 0.322f - 0.019f * u * u))) - 0.0135f * (1 - 0.4f * u * u) + 0.03f * sstep(0.1f, 0.5f, op) * (1 - sm);
        float me = sdEll(sub(a, V(0, mcy, 0.305f)), V(0.045f + 0.012f * op + 0.045f * sm * op, mhy, 0.080f)), dsk = d;
        if (op > 0.01f) md = fminf(md, fmaxf(me, a.y - mcy - mhy * mixf(1.0f, 0.15f, sm)));
        if (-md > d) { d = -md; *m = C_MOUTH; }
        if (op > 0.15f) UN(fmaxf(fmaxf(sdEll(sub(a, V(0, mcy - 0.8f * mhy, 0.262f)), V(0.036f + 0.03f * sm * op, 0.55f * mhy, 0.05f)), me + 0.003f), dsk + 0.014f), C_TONGUE) }
    float es = 1 + 0.25f * C->eye;
    UN(sdEll(sub(a, V(0.160f, 0.318f, 0.250f)), V(0.078f * es, 0.092f * es, 0.062f * es)), C_EYE)
    if (C->flags & F_STACHE) UN(sdEll(sub(a, V(0.070f, 0.080f, 0.322f)), V(0.082f, 0.028f, 0.030f)), C_BROW)
    if (C->flags & F_GLASSES) {
        UN(len2(len2(a.x - 0.160f, a.y - 0.318f) - 0.100f, a.z - 0.326f) - 0.008f, C_GLASSES)
        UN(sdCap(a, V(0, 0.335f, 0.362f), V(0.064f, 0.335f, 0.346f), 0.007f), C_GLASSES)
        UN(sdCap(a, V(0.258f, 0.322f, 0.330f), V(0.345f, 0.340f, 0.08f), 0.007f), C_GLASSES) }
    switch (C->hat) {       // les chapeaux descendent jusqu'au crâne : pas de jour entre le bord et la tête
    case HAT_BERET: { v3 b = V(0.98f * h.x - 0.20f * h.y, 0.20f * h.x + 0.98f * h.y, h.z);
        UN(sdEll(sub(b, V(0.10f, 0.512f, -0.02f)), V(0.225f, 0.070f, 0.220f)), C_HAT) UN(sdCap(b, V(0.10f, 0.578f, -0.02f), V(0.10f, 0.608f, -0.02f), 0.012f), C_HAT) break; }
    case HAT_CAP: UN(smin(sdEll(sub(a, V(0, 0.518f, -0.01f)), V(0.215f, 0.078f, 0.225f)), sdEll(sub(a, V(0, 0.488f, 0.205f)), V(0.15f, 0.02f, 0.11f)), 0.03f), C_HAT) break;
    case HAT_BOATER: UN(fminf(cylY(sub(a, V(0, 0.545f, 0)), 0.165f, 0.055f), cylY(sub(a, V(0, 0.503f, 0)), 0.262f, 0.005f)) - 0.007f, C_HAT) break;
    case HAT_TOP: { float u = (a.y - 0.51f) / 0.27f;
        UN(fminf(box2(len2(a.x, a.z) - (0.146f + 0.022f * u * u), fabsf(a.y - 0.635f) - 0.145f) * 0.9f, cylY(sub(a, V(0, 0.500f, 0)), 0.232f, 0.006f)) - 0.006f, C_HAT) break; }
    case HAT_BEANIE: { float he = sdEll(sub(a, CZ_HEADC), CZ_HEADR);
        float bn = fminf(fmaxf(he - 0.014f, 0.44f - a.y), fmaxf(he - 0.024f, fabsf(a.y - 0.466f) - 0.030f) - 0.004f);
        UN(fminf(bn, len(sub(a, V(0, 0.585f, 0.02f))) - 0.066f), C_HAT) break; }
    case HAT_TOQUE: UN(smin(cylY(sub(a, V(0, 0.555f, 0)), 0.150f, 0.085f) - 0.01f, sdEll(sub(a, V(0, 0.695f, 0)), V(0.215f, 0.09f, 0.215f)), 0.04f), C_HAT) break;
    case HAT_KEPI: UN(fminf(cylY(sub(a, V(0, 0.555f, 0)), 0.160f, 0.062f) - 0.008f, sdEll(sub(a, V(0, 0.498f, 0.195f)), V(0.14f, 0.014f, 0.085f))), C_HAT) break;
    case HAT_BOWLER: UN(fminf(fmaxf(len(sub(a, V(0, 0.490f, 0))) - 0.178f, 0.490f - a.y), len2(len2(a.x, a.z) - 0.205f, a.y - 0.497f) - 0.022f), C_HAT) break;
    }
    return d;
}

static float cz_sdf(const cz_t *C, const rig_t *R, const pose_t *P, v3 p, int *m) {
    int fl = C->flags; float F = R->F, d;
    v3 q = cz_loc(sub(p, R->ot), R->tx, R->ty, R->tz), h = mul(cz_loc(sub(q, R->nk), R->hx, R->hy, R->hz), 1 / R->HS);
    CZ_Q = q; CZ_H = h; CZ_PART = 0; *m = C_SKIN;
    int mh = C_SKIN, ph = 1; float bh = len(sub(h, V(0, 0.27f, 0.02f))) - 0.68f;
    float dh = (bh > CZ_BND ? bh : cz_head(C, P, h, &mh, &ph)) * R->HS;
    float bb = len(sub(p, V(0, R->hipy + 0.10f, 0))) - 0.86f;
    if (bb > CZ_BND) { if (dh < bb) { *m = mh; CZ_PART = ph; return dh; } return bb; }
    // ---- le corps
    float t = cz_torso(R, q), legs = 1e9f, arms = 1e9f, hoof = 1e9f, plegs = 1e9f, sl_short = 1e9f, sl_long = 1e9f;
    for (int s = 0; s < 2; s++) {
        v3 fo = R->foot[s], f0 = sub(p, fo), f = V(f0.x, R->fc[s] * f0.y + R->fs[s] * f0.z, -R->fs[s] * f0.y + R->fc[s] * f0.z), fm = V(fabsf(f.x), f.y, f.z);
        float th = sdCap(p, R->hip[s], R->knee[s], 0.100f), sn = sdCap(p, R->knee[s], add(fo, V(0, 0.035f, 0)), 0.090f);
        legs = fminf(legs, smin(th, sn, 0.03f));
        if (fl & (F_PANTS | F_BIB)) plegs = fminf(plegs, smin(th - 0.015f, sdCap(p, R->knee[s], add(fo, V(0, 0.078f, 0)), 0.106f), 0.03f));
        float hf = smin(sdEll(sub(fm, V(0.044f, 0, 0.035f)), V(0.051f, 0.068f, 0.122f)), sdEll(sub(f, V(0, 0, -0.008f)), V(0.096f, 0.066f, 0.088f)), 0.025f);      // le sabot : deux onglons, un talon rond, la semelle à plat
        hoof = fminf(hoof, fmaxf(hf, -f.y - 0.055f));
        float ua = sdCap(q, R->sh[s], R->el[s], 0.072f), fa = sdCap(q, R->el[s], R->ha[s], 0.066f);
        arms = fminf(arms, smin(ua, fa, 0.025f));
        sl_short = fminf(sl_short, sdCap(q, R->sh[s], mix3(R->sh[s], R->el[s], 0.62f), 0.091f));
        sl_long = fminf(sl_long, fminf(ua, sdCap(q, R->el[s], sub(R->ha[s], mul(R->fd[s], 0.035f)), 0.068f)));
        v3 o1 = add(R->ha[s], mul(R->hn[s], 0.031f)), o2 = sub(R->ha[s], mul(R->hn[s], 0.031f)), tip = mul(R->fd[s], 0.058f);                                    // la main : un sabot à deux doigts
        hoof = fminf(hoof, smin(fminf(sdCap(q, o1, add(o1, tip), 0.045f), sdCap(q, o2, add(o2, tip), 0.045f)), len(sub(q, R->ha[s])) - 0.068f, 0.02f));
    }
    float ds = smin(smin(t, legs, 0.06f), arms, 0.045f);
    if (!(fl & (F_COAT | F_DRESS))) {                                                                           // la queue en tire-bouchon (cachée sous les vêtements longs)
        float tb = len(sub(q, R->tlc)) - 0.125f;
        if (tb < CZ_BND * 0.5f) {       // 1,7 tour de tire-bouchon, qui s'élargit puis s'affine : on cherche le point de l'hélice situé au même angle
            v3 dl = sub(q, R->tb); float a_ = dot(dl, R->tu), b_ = dot(dl, R->tw), c_ = dot(dl, R->ta), phi = atan2f(b_, a_), td = 1e9f;
            for (int k = -1; k <= 2; k++) { float u = clampf((phi + 6.2832f * k - 0.6f) / (6.2832f * 1.7f), 0, 1), th = 6.2832f * 1.7f * u + 0.6f, rr = 0.050f * sstep(0, 0.3f, u);
                td = fminf(td, len(V(a_ - rr * cosf(th), b_ - rr * sinf(th), c_ + 0.025f - 0.135f * u)) - (0.0255f - 0.014f * u)); }
            td *= 0.9f;
            if (td < ds) CZ_PART = 4;
            ds = smin(ds, td, 0.02f); }
        else ds = fminf(ds, tb); }
    if (mh == C_SKIN) { if (dh < ds) CZ_PART = ph; d = smin(ds, dh, 0.045f); }
    else if (dh < ds) { d = dh; *m = mh; CZ_PART = ph; } else d = ds;
    UN(hoof, C_HOOF)
    // ---- la tenue, par couches : chaque vêtement est la peau gonflée, coupée là où il s'arrête
    float hcut = sdEll(sub(h, CZ_HEADC), CZ_HEADR) * R->HS;
    if (C->top) {
        float sh = cz_smax(fmaxf(t - 0.016f, 0.012f - hcut), ((fl & (F_PANTS | F_BIB)) ? 0.085f : 0.015f) - q.y, 0.02f);
        sh = smin(sh, (C->top == TOP_TEE ? sl_short : sl_long - 0.016f), 0.03f);
        sh = fminf(sh, fmaxf(len2(len2(q.x, q.z - 0.01f) - 0.168f, (q.y - 0.488f) * 1.3f) - 0.022f, 0.004f - hcut));  // le col
        UN(sh, C_TOP) }
    if (fl & (F_PANTS | F_BIB)) {
        float pa = smin(cz_smax(t - 0.014f, q.y - 0.108f, 0.012f), plegs, 0.05f);
        if (fl & F_BIB) pa = fminf(pa, fmaxf(fmaxf(t - 0.024f, 0.02f - q.z), fmaxf(fabsf(q.x) - 0.135f, fmaxf(q.y - 0.40f, 0.09f - q.y))));
        UN(pa, C_PANTS) }
    if (fl & F_DRESS) {
        float rad = mixf(0.41f, 0.285f, clampf((q.y + 0.2f) / 0.36f, 0, 1)) * (0.6f + 0.4f * F) + 0.012f * sinf(atan2f(q.x, q.z) * 9) * clampf((0.1f - q.y) / 0.3f, 0, 1);
        float dr = smin(fmaxf(fmaxf(t - 0.018f, 0.012f - hcut), 0.05f - q.y), fmaxf(len2(q.x, q.z) - rad, fabsf(q.y + 0.02f) - 0.18f) * 0.85f, 0.09f);
        UN(fminf(dr, sl_short + 0.012f), C_DRESS) }
    if (fl & F_APRON) {                                                                                         // une feuille de toile devant le corps, pas un bloc
        float body = smin(t - 0.03f, fmaxf(len2(q.x * 0.95f, q.z) - 0.30f * F, fabsf(q.y + 0.05f) - 0.16f), 0.05f);
        float w = mixf(0.27f * F, 0.125f, sstep(0.18f, 0.26f, q.y));
        float ap = fmaxf(fmaxf(body, 0.015f - q.z), fmaxf(fabsf(q.x) - w, fmaxf(q.y - 0.43f, -0.19f - q.y)));
        UN(fmaxf(ap, -body - 0.016f) - 0.003f, C_APRON) }
    if (fl & (F_JACKET | F_COAT)) {
        float jk = fmaxf(t - 0.034f, 0.02f - hcut);
        if (fl & F_COAT) jk = smin(jk, fmaxf(len2(q.x * 0.92f, q.z) - mixf(0.335f, 0.29f, clampf((q.y + 0.17f) / 0.3f, 0, 1)) * F, fabsf(q.y + 0.02f) - 0.15f), 0.04f);
        jk = cz_smax(jk, ((fl & F_COAT) ? -0.17f : 0.0f) - q.y, 0.02f);
        UN(smin(jk, sl_long - 0.032f, 0.03f), C_JACKET) }
    if (fl & F_SCARF) {
        float sf = len2(len2(q.x, q.z - 0.012f) - 0.185f, (q.y - 0.468f) * 1.2f) - 0.055f;
        UN(fminf(sf, sdCap(q, V(0.08f, 0.44f, 0.17f), V(0.115f, 0.20f, 0.225f * F), 0.045f)) * 0.85f, C_SCARF) }
    if (fl & F_BOW) { v3 b = V(fabsf(q.x), q.y - 0.392f, q.z - ((fl & (F_JACKET | F_COAT)) ? 0.232f : 0.212f) * (0.5f + 0.5f * F));
        UN(fminf(sdEll(sub(b, V(0.058f, 0, 0)), V(0.055f, 0.036f, 0.018f)), len(b) - 0.024f), C_SCARF) }
    // ---- ce qu'il tient
    if (C->prop == PROP_MUG) { v3 g = sub(q, add(R->ha[0], V(0, 0.045f, 0.118f)));
        UN(cylY(g, 0.068f, 0.088f) - 0.004f, C_MUG) UN(len2(len2(g.z + 0.076f, g.y) - 0.044f, g.x) - 0.013f, C_MUG)
        UN(sdEll(sub(g, V(0, 0.098f, 0)), V(0.074f, 0.036f, 0.074f)), C_FOAM) }
    if (C->prop == PROP_CANE) { v3 a = add(R->ha[1], V(0.0f, 0.07f, 0.05f));
        UN(sdCap(q, a, V(a.x - 0.03f, -R->hipy + 0.012f, a.z + 0.10f), 0.013f), C_CANE) UN(len(sub(q, a)) - 0.03f, C_KNOB) }
    return d;
}

// ------------------------------------------------------------------------------------------------ la peinture
typedef struct { v3 alb, emis; float spec, gloss, sss, refl, metal, flat; } cmat_t;
S float cz_pat(int pat, float ua, float v) {      // ua : le tour du corps, de 0 à 1 (les motifs se referment sans couture) ; 0 : fond, 1 : motif
    switch (pat) {
    case PAT_VSTRIPE: return sstep(0.02f, 0.06f, fabsf(fract(ua * 17) - 0.5f) - 0.22f);
    case PAT_HSTRIPE: return sstep(0.02f, 0.06f, fabsf(fract(v / 0.065f) - 0.5f) - 0.20f);
    case PAT_CHECK: { float a = fract(ua * 14) < 0.5f, b = fract(v / 0.09f) < 0.5f; return 0.5f * (a + b); }
    case PAT_DOTS: { float fu = fract(ua * 20 + 0.5f * floorf(v / 0.085f)) - 0.5f, fv = fract(v / 0.085f) - 0.5f; return sstep(0.32f, 0.26f, len2(fu * 1.15f, fv)); }
    }
    return 0;
}
static void cz_mat(const cz_t *C, const pose_t *P, int m, cmat_t *M) {       // après un cz_sdf au point touché (CZ_Q, CZ_H, CZ_PART à jour)
    v3 q = CZ_Q, h = CZ_H, a = V(fabsf(h.x), h.y, h.z); float ua = atan2f(q.x, q.z) / 6.2832f + 0.5f;
    const v3 GOLDC = V(1.0f, 0.70f, 0.24f);
    M->alb = V(0.5f, 0.5f, 0.5f); M->emis = V(0, 0, 0); M->spec = 0.06f; M->gloss = 12; M->sss = 0; M->refl = 0; M->metal = 0; M->flat = 0;
    switch (m) {
    case C_SKIN: { v3 c = C->skin; M->sss = 1; M->spec = 0.30f; M->gloss = 26;
        if (CZ_PART == 2) c = mix3(c, mulv(c, V(0.95f, 0.66f, 0.78f)), sstep(0.37f, 0.44f, a.z));                // le bout du groin, plus rose
        if (CZ_PART == 1 || CZ_PART == 2) c = mix3(c, V(0.98f, 0.16f, 0.22f), 0.55f * sstep(0.10f, 0.03f, len2(a.x - 0.268f, a.y - 0.165f)) * sstep(0.0f, 0.1f, a.z));   // les pommettes
        if (CZ_PART == 3) c = mix3(c, mulv(c, V(0.92f, 0.55f, 0.68f)), sstep(-0.012f, -0.04f, CZ_EAR.x) * sstep(0.0f, 0.006f, CZ_EAR.z));   // l'intérieur de l'oreille
        if (CZ_PART == 0) c = mix3(c, add(mul(c, 0.6f), V(0.40f, 0.30f, 0.28f)), 0.45f * sstep(1.0f, 0.5f, len2(q.x / 0.20f, (q.y - 0.13f) / 0.21f)) * sstep(0.04f, 0.14f, q.z));   // le ventre, plus clair
        if (CZ_PART == 4) c = mulv(c, V(0.97f, 0.85f, 0.88f));
        if (CZ_PART == 0 && q.z > 0.1f) c = mix3(c, mulv(c, V(0.72f, 0.5f, 0.55f)), sstep(0.017f, 0.009f, len2(q.x, q.y - 0.065f)));       // le nombril
        if ((C->flags & F_LASH) && CZ_PART == 1 && a.z > 0.15f) { float es = 1 + 0.25f * C->eye, ex = (a.x - 0.160f) / (0.078f * es), ey = (a.y - 0.318f) / (0.092f * es), ro = len2(ex, ey), th = atan2f(ey, ex);
            if (ro > 0.97f && ro < 1.40f) for (int k = 0; k < 3; k++) if (fabsf(th - 0.30f - 0.40f * k) < 0.30f * (1.40f - ro)) c = V(0.10f, 0.04f, 0.04f); }
        if (CZ_PART == 1 && a.z > 0.1f) {       // les sourcils : deux petits arcs peints au-dessus des yeux
            float u = (a.x - 0.112f) / 0.094f, uc = clampf(u, 0, 1), yb = 0.456f - 0.012f * uc + 0.03f * (P->brow + (2 * uc - 1) * P->frown) + 0.014f * sinf(PI * uc);
            float db = len2((u - uc) * 0.094f, a.y - yb) - 0.0045f - 0.006f * sinf(PI * uc);
            c = mix3(c, V(0.30f, 0.12f, 0.08f), sstep(0.004f, -0.002f, db)); }
        M->alb = c; break; }
    case C_HOOF: M->alb = V(0.115f, 0.062f, 0.050f); M->spec = 0.7f; M->gloss = 70; M->refl = 0.10f; break;
    case C_MOUTH: M->alb = V(0.26f, 0.04f, 0.06f); M->flat = 1; break;
    case C_BROW: M->alb = V(0.30f, 0.13f, 0.09f); M->spec = 0.15f; M->gloss = 20; break;
    case C_EYE: { float es = 1 + 0.25f * C->eye, sgx = h.x > 0 ? 1.0f : -1.0f;
        v3 e = V((a.x - 0.160f) / (0.078f * es), (a.y - 0.318f) / (0.092f * es), (a.z - 0.250f) / (0.062f * es));
        float gx = e.x * sgx, pcx = -0.07f * sgx + 0.40f * P->lookx, pcy = 0.03f + 0.36f * P->looky, r = len2(gx - pcx, e.y - pcy);      // la pupille, un peu rentrée
        v3 c = V(0.93f, 0.93f, 0.90f);
        c = mix3(c, V(0.20f, 0.07f, 0.03f), sstep(0.60f, 0.55f, r)); c = mix3(c, V(0.012f, 0.008f, 0.010f), sstep(0.53f, 0.47f, r));
        float hl = sstep(0.19f, 0.14f, len2(gx - pcx - 0.20f, e.y - pcy - 0.22f)) + 0.7f * sstep(0.09f, 0.06f, len2(gx - pcx + 0.19f, e.y - pcy + 0.22f));      // deux reflets peints
        M->emis = mul(V(1, 1, 1), 1.6f * clampf(hl, 0, 1)); M->alb = c; M->spec = 1.0f; M->gloss = 220; M->refl = 0.16f;
        float ar = 1 - 1.6f * e.x * e.x, lid = mixf(0.90f, -1.4f, P->blink), line = fmaxf(lid, -0.30f + (P->happy * 0.55f - (1 - P->happy) * 0.25f) * ar);
        if (e.y > lid) { M->alb = mulv(C->skin, V(0.97f, 0.92f, 0.94f)); M->emis = V(0, 0, 0); M->spec = 0.3f; M->gloss = 26; M->sss = 1; M->refl = 0; }       // la paupière
        if (fabsf(e.y - line) < 0.035f + 0.07f * P->blink && e.y > lid - 0.1f) { M->alb = V(0.16f, 0.07f, 0.06f); M->emis = V(0, 0, 0); M->sss = 0; M->refl = 0; M->spec = 0.1f; }   // les cils
        break; }
    case C_TOP: { v3 c = mix3(C->ctop, C->ctop2, cz_pat(C->ptop, ua, q.y));
        if (fabsf(q.y - 0.488f) < 0.04f && len2(q.x, q.z) < 0.215f && C->top == TOP_SHIRT) c = mix3(C->ctop, V(1, 1, 1), 0.25f);        // le col
        if ((C->flags & F_BRACES) && fabsf(fabsf(q.x) - 0.095f) < 0.017f && q.y > 0.07f) c = C->cacc;
        if ((C->flags & F_APRON) && (fabsf(q.y - 0.135f) < 0.013f || (q.y > 0.40f && fabsf(fabsf(q.x) - 0.105f) < 0.013f))) c = C->capron2;       // les cordons du tablier
        if ((C->flags & F_BIB) && fabsf(fabsf(q.x) - 0.105f) < 0.022f && (q.y > 0.36f || q.z < 0)) c = C->cbot;
        if ((C->flags & F_TIE) && q.z > 0 && q.y > 0.15f && q.y < 0.46f && fabsf(q.x) < (0.014f + 0.11f * clampf(0.44f - q.y, 0, 0.2f)) * sstep(0.14f, 0.20f, q.y)) c = C->cacc;
        if ((C->flags & F_BUTTONS) && !(C->flags & F_TIE) && q.z > 0 && q.y > 0.10f && q.y < 0.44f && len2(q.x, (fract((q.y - 0.10f) / 0.075f) - 0.5f) * 0.075f) < 0.011f) c = mul(c, 0.35f);
        if ((C->flags & F_SASH) && q.z > 0 && fabsf(q.x * 0.72f + (q.y - 0.27f) * 0.69f) < 0.04f) c = fabsf(q.x * 0.72f + (q.y - 0.27f) * 0.69f) < 0.014f ? GOLDC : C->cacc;
        M->alb = c; break; }
    case C_PANTS: { v3 c = mix3(C->cbot, mul(C->cbot, 0.55f), 0.5f * cz_pat(C->pbot, ua, q.y));
        if (!(C->flags & F_BIB) && fabsf(q.y - 0.092f) < 0.018f) { c = V(0.07f, 0.04f, 0.03f); if (q.z > 0 && fabsf(q.x) < 0.028f) { c = GOLDC; M->spec = 0.8f; M->gloss = 60; } }     // la ceinture et sa boucle
        if ((C->flags & F_BIB) && q.z > 0.1f && q.y > 0.16f && len2(fabsf(q.x) - 0.105f, q.y - 0.365f) < 0.017f) { c = GOLDC; M->spec = 0.8f; M->gloss = 60; }                       // les boutons de la bavette
        M->alb = c; break; }
    case C_JACKET: { v3 c = C->cjack; float v = clampf((0.47f - q.y) / 0.30f, 0, 1), w = 0.105f * (1 - v), sa = q.x * 0.72f + (q.y - 0.27f) * 0.69f;      // le revers : un V où l'on voit la chemise
        if (C->flags & F_CLOSED) { if (q.z > 0 && q.y < 0.42f && q.y > -0.14f && len2(fabsf(q.x) - 0.048f, (fract((q.y + 0.16f) / 0.085f) - 0.5f) * 0.085f) < 0.014f) { c = GOLDC; M->spec = 0.8f; M->gloss = 60; } }
        else if (q.z > 0 && fabsf(q.x) < w) { c = C->ctop; if ((C->flags & F_TIE) && fabsf(q.x) < 0.012f + 0.06f * v * (1 - 0.6f * v)) c = C->cacc; }
        else if (q.z > 0 && fabsf(q.x) < w + 0.03f && q.y > 0.17f) c = add(mul(c, 1.25f), V(0.02f, 0.02f, 0.02f));
        else if ((C->flags & F_BUTTONS) && q.z > 0 && q.y < 0.19f && q.y > -0.14f && len2(fabsf(q.x) - 0.04f, (fract((q.y + 0.16f) / 0.085f) - 0.5f) * 0.085f) < 0.015f) { c = GOLDC; M->spec = 0.8f; M->gloss = 60; }
        if ((C->flags & F_SASH) && q.z > -0.05f && fabsf(sa) < 0.042f) c = fabsf(sa) < 0.015f ? GOLDC : C->cacc;
        M->alb = c; M->spec = 0.10f; break; }
    case C_APRON: M->alb = mix3(C->capron, C->capron2, cz_pat(C->papron, q.x / 1.275f, q.y)); break;
    case C_DRESS: M->alb = mix3(C->cdress, C->cdress2, cz_pat(C->pdress, ua, q.y)); if (q.y > 0.44f) M->alb = C->cdress2; break;
    case C_SCARF: M->alb = (C->flags & F_BOW) && !(C->flags & F_SCARF) ? C->cacc : mix3(C->cacc, C->chat2, cz_pat(PAT_VSTRIPE, ua + q.y * 0.5f, 0)); break;
    case C_HAT: { v3 c = C->chat;
        if (C->hat == HAT_BOATER && a.y > 0.516f && a.y < 0.556f) c = C->chat2;
        if (C->hat == HAT_TOP && a.y > 0.512f && a.y < 0.572f) c = C->chat2;
        if (C->hat == HAT_KEPI) { if (a.y < 0.540f && a.y > 0.508f) c = C->chat2; if (a.z > 0.1f && len2(a.x, a.y - 0.572f) < 0.026f) { c = GOLDC; M->spec = 0.8f; M->gloss = 60; } if (a.y < 0.504f) { c = V(0.03f, 0.03f, 0.03f); M->spec = 0.6f; M->gloss = 50; } }
        if (C->hat == HAT_BEANIE) { if (a.y > 0.53f && len(sub(a, V(0, 0.585f, 0.02f))) < 0.073f) c = C->chat2; else if (a.y < 0.498f) c = C->chat2; else c = mix3(c, C->chat2, cz_pat(PAT_HSTRIPE, 0, a.y - 0.01f)); }
        if (C->hat == HAT_TOQUE) c = mul(c, 0.86f + 0.14f * sstep(-0.3f, 0.3f, sinf(atan2f(a.x, a.z) * 22)) * (a.y < 0.63f));
        if (C->hat == HAT_BOWLER && a.y > 0.522f && a.y < 0.556f) c = C->chat2;
        M->alb = c; break; }
    case C_GLASSES: case C_KNOB: M->alb = GOLDC; M->metal = 1; M->spec = 1.0f; M->gloss = 90; M->refl = 0.7f; break;
    case C_MUG: { M->alb = V(0.80f, 0.36f, 0.03f); M->emis = V(0.30f, 0.12f, 0.01f); M->spec = 1.0f; M->gloss = 120; M->refl = 0.22f; break; }
    case C_FOAM: M->alb = V(0.95f, 0.92f, 0.82f); M->sss = 0.6f; M->spec = 0.1f; break;
    case C_TONGUE: M->alb = V(0.90f, 0.22f, 0.26f); M->sss = 0.5f; M->spec = 0.4f; M->gloss = 40; break;
    case C_CANE: M->alb = V(0.10f, 0.05f, 0.03f); M->spec = 0.6f; M->gloss = 60; break;
    }
}

// ------------------------------------------------------------------------------------------------ les mouvements (ils s'ajoutent à la pose de repos)
static void cz_idle(pose_t *P, float t, float seed) {         // il attend : il respire, il regarde un peu autour, il cligne, une oreille tique, la queue frétille
    P->bob += 0.005f * sinf(2.2f * t + seed * 9); P->lean += 0.015f * sinf(1.1f * t + seed * 5);
    P->hyaw += 0.10f * sinf(0.63f * t + seed * 17); P->hroll += 0.035f * sinf(0.9f * t + seed * 3); P->hpitch += 0.03f * sinf(0.5f * t + seed * 11);
    float u = fract((t + seed * 7.3f) / 3.3f) * 3.3f; if (u < 0.16f) P->blink = fmaxf(P->blink, sinf(PI * u / 0.16f));
    u = fract((t + seed * 3.1f) / 4.7f) * 4.7f; if (u < 0.5f) P->earflop[seed > 0.5f] += 0.28f * sinf(PI * u / 0.5f) * sinf(u * 38);
    P->earflop[0] += 0.05f * sinf(2.2f * t + seed * 9); P->earflop[1] += 0.05f * sinf(2.2f * t + seed * 9 + 1);
    u = fract((t + seed * 5.7f) / 2.9f) * 2.9f; P->tail += 0.5f * sinf(t * 17) * sstep(0, 0.2f, u) * sstep(1.3f, 0.9f, u);
    for (int s = 0; s < 2; s++) { P->hand[s].y += 0.004f * sinf(2.2f * t + seed * 9 + 1.5f); P->hand[s].z += 0.012f * sinf(1.1f * t + seed * 5 + s); }
}
static void cz_walk(pose_t *P, float ph, float amp) {         // un cycle = 2 pi ; pied posé : il recule à vitesse constante (0,34 m par demi-cycle), pied levé : il revient
    for (int s = 0; s < 2; s++) {
        float f = fmodf(ph + (s ? PI : 0), 2 * PI); if (f < 0) f += 2 * PI;
        float z = f < PI ? 0.17f * (1 - 2 * f / PI) : -0.17f * cosf(f - PI), y = f < PI ? 0 : 0.085f * sinf(f - PI);
        P->foot[s].z += z * amp; P->foot[s].y += y * amp; P->footp[s] += (f < PI ? 0 : 0.7f * sinf(f - PI) * cosf(f - PI)) * amp;
        P->hand[s].z += -0.15f * sinf(f + PI * 0.5f) * amp; P->hand[s].y += 0.03f * fabsf(cosf(f)) * amp; P->hand[s].x -= (s ? -1 : 1) * 0.02f * amp;
    }
    P->bob += (-0.012f + 0.016f * cosf(2 * ph + 0.5f)) * amp; P->sway += 0.016f * sinf(ph + 1.57f) * amp;
    P->twist += 0.10f * cosf(ph) * amp; P->roll += 0.035f * sinf(ph + 1.57f) * amp; P->lean += 0.05f * amp;
    P->hyaw -= 0.07f * cosf(ph) * amp; P->hroll -= 0.03f * sinf(ph + 1.57f) * amp;
    P->earflop[0] += 0.20f * cosf(2 * ph - 0.9f) * amp; P->earflop[1] += 0.20f * cosf(2 * ph - 1.3f) * amp; P->tail += 0.45f * sinf(2 * ph) * amp;
}
static void cz_wave(pose_t *P, float t, float k) {            // il salue de la main (k : 0 → 1)
    v3 up = V(0.50f + 0.06f * sinf(t * 9.5f), 0.60f + 0.02f * cosf(t * 9.5f), 0.12f);
    P->hand[0] = mix3(P->hand[0], up, k); P->hroll -= 0.10f * k; P->smile = mixf(P->smile, 1.0f, k); P->roll -= 0.04f * k;
}
static void cz_salute(pose_t *P, float k) {                   // la main à la tempe
    P->hand[0] = mix3(P->hand[0], V(0.36f, 0.83f, 0.20f), k); P->lean -= 0.04f * k; P->smile = mixf(P->smile, 0.15f, k);
}
static void cz_cheers(pose_t *P, float k) {                   // il lève sa chope
    P->hand[0] = mix3(P->hand[0], V(0.40f, 0.40f, 0.25f), k); P->lean -= 0.05f * k; P->smile = mixf(P->smile, 1.0f, k);
}
