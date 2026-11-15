// ================================================================ 24 HEURES À PORKONIA : les séquences de jour. Un moteur commun (soleil, ombres,
// habitants placés un par un, cochons de marche), et une scène par séquence. Temps : 300 + 100 x numéro de séquence (0 : le monument, 06 h 00).
#include "porc4.h"
enum { W_GROUND = 300, W_MARBLE, W_STONE, W_PGOLD, W_PEYE, W_PCHROME, W_PPINK, W_POLE, W_FINIAL, W_FLAG, W_LAMP, W_FIR, W_TRUNK, W_CHALET, W_LETTER, W_SPH, W_TOWER, W_SENTRY, W_BRONZE, W_ROPE, W_RAIL, W_STAND, W_PENNANT, W_TRIB, W_ARCH, W_PLAT, W_MEAT, W_GATE, W_LECT, W_CANDLE, W_FLAME,
       W_WALLT, W_BEAM, W_COUNTER, W_CTOP, W_BRASS, W_HANDLE, W_TABLE, W_BOTTLE, W_BARREL, W_HAM, W_SAUS, W_FIRE, W_CLOCK, W_SIGN, W_BOARD, W_MUGH, W_HFOAM, W_BEERS,
       W_SEABED, W_HULL, W_SAIL, W_PORT, W_LIGHT, W_CAP, W_HULLIN, W_DECK, W_BULK, W_RIB, W_PIPE, W_DFLOOR, W_CONSOLE, W_VINYL, W_MIXER, W_SPEAKER, W_NEON, W_LED, W_BALL };
static int SEQ, WN, WNL, WDBG; static float WS, WLINE = -1e9f, WFAR = 900.0f, WLAMP, WSNOW; static v3 WSUN, WSUNC, WAMBU, WAMBD;
static float WLT[12][8];                      // lumières ponctuelles : x y z, r g b, intensité, atténuation
// ---- les habitants placés : le citoyen standard, chacun à sa place, tourné où il faut
#define WNP 40
static cz_t WCZ[WNP]; static pose_t WPP[WNP]; static rig_t WRG[WNP]; static float WPX[WNP], WPY[WNP], WPZ[WNP], WPC[WNP], WPS[WNP]; static v3 WB0, WB1;
S v3 w_local(v3 p, int k) { float sc = WCZ[k].sc, dx = p.x - WPX[k], dz = p.z - WPZ[k]; return V((dx * WPC[k] - dz * WPS[k]) / sc, (p.y - WPY[k]) / sc, (dx * WPS[k] + dz * WPC[k]) / sc); }
static void w_place(int k, float x, float y, float z, float yaw) { WPX[k] = x; WPY[k] = y; WPZ[k] = z; WPC[k] = cosf(yaw); WPS[k] = sinf(yaw); cz_rig(&WCZ[k], &WPP[k], &WRG[k]); }
static void w_bounds(void) {
    WB0 = V(1e9f, 1e9f, 1e9f); WB1 = V(-1e9f, -1e9f, -1e9f);
    for (int k = 0; k < WN; k++) { WB0 = V(fminf(WB0.x, WPX[k] - 1.2f), fminf(WB0.y, WPY[k] - 0.1f), fminf(WB0.z, WPZ[k] - 1.2f)); WB1 = V(fmaxf(WB1.x, WPX[k] + 1.2f), fmaxf(WB1.y, WPY[k] + 2.3f), fmaxf(WB1.z, WPZ[k] + 1.2f)); }
}
static float wpeople(v3 p, int *m) {
    *m = C_SKIN; if (NOCZ || WN == 0 || (WDBG & 8)) return 1e9f;
    float bb = fmaxf(fmaxf(fmaxf(WB0.x - p.x, p.x - WB1.x), fmaxf(WB0.z - p.z, p.z - WB1.z)), fmaxf(WB0.y - p.y, p.y - WB1.y)); if (bb > 1.0f) return bb;
    float d = 1e9f; int mm;
    for (int k = 0; k < WN; k++) { float sc = WCZ[k].sc; v3 l = w_local(p, k); float bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc;
        float e = bd > 0.65f ? bd : fmaxf(cz_sdf(&WCZ[k], &WRG[k], &WPP[k], l, &mm) * sc, bd); if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
    return d;
}
static int wwho(v3 p, v3 *l) { int kb = 0; float db = 1e9f; for (int k = 0; k < WN; k++) { float e = len2(p.x - WPX[k], p.z - WPZ[k]); if (e < db) { db = e; kb = k; } } *l = w_local(p, kb); return kb; }
S float cone(v3 q, float R, float H) { return fmaxf((len2(q.x, q.z) - R * (1 - q.y / H)) * H / sqrtf(H * H + R * R), -q.y); }      // un cône posé sur y = 0
S float wflag(v3 q, float L, float hh, float ph) {      // un drapeau qui claque : q.x le long de l'étoffe depuis le mât, q.y la hauteur, q.z l'épaisseur
    float u = clampf(q.x / L, 0, 1), w = q.z - (0.10f + 0.30f * u) * u * sinf(q.x * 2.6f - ph) - 0.10f * u * sinf(q.x * 5.3f - ph * 1.7f + q.y * 1.5f);
    return sdBox(V(q.x - L * 0.5f, q.y + 0.12f * u * u * hh, w), V(L * 0.5f, hh, 0.012f)) * 0.55f - 0.01f;
}

// ================================================================ SÉQUENCE 0 : 06 h 00, LE PORC COURONNÉ. L'esplanade du monument au bord du plateau,
// la vallée et son village au pied (vers -z, d'où vient le soleil), la butte des lettres derrière (+z).
#define MLS 0.0786f
#define MLZ 58.0f
#define MLY 8.6f
static float MFLAGH, MPH; static v3 MSP[3];          // la hauteur du drapeau, la phase du vent, les trois sphères de chrome
static const float MMAST[2] = {-7.5f, -6.0f};
S float mon_h(float x, float z) {
    float r = len2(x, z), h = -64.0f * sstep(-30.0f, -135.0f, z) + 9.0f * sstep(30.0f, 52.0f, z) + 22.0f * sstep(75.0f, 180.0f, z) + 90.0f * sstep(-330.0f, -620.0f, z);
    return h + (1.6f * sinf(x * 0.047f + 1.0f) * sinf(z * 0.041f + 2.0f) + 0.28f * sinf(x * 0.21f) * sinf(z * 0.17f + 1.3f)) * sstep(24.0f, 60.0f, r) * sstep(4.0f, 14.0f, fabsf(z - MLZ));
}
static int mon_fir(float px, float pz, float *c) {      // un sapin par case, ou rien : c = x, z, le sol, la taille
    float ix = roundf(px / 8.0f), iz = roundf(pz / 8.0f), h = hash2(ix * 1.7f + 3.3f, iz * 2.1f + 5.9f);
    c[0] = ix * 8.0f + 3.2f * (fract(h * 13.7f) - 0.5f); c[1] = iz * 8.0f + 3.2f * (fract(h * 29.3f) - 0.5f); c[3] = 0.55f + 0.5f * fract(h * 7.7f);
    float r = len2(c[0], c[1]); if (r < 29.0f || (c[1] > 32.0f && c[1] < 76.0f && fabsf(c[0]) < 52.0f) || c[1] > 236.0f || fabsf(c[0]) > 236.0f || (c[1] < -112.0f && c[1] > -345.0f)) return 0;
    if (h > 0.30f + 0.62f * sstep(0.30f, 0.62f, vnoise(ix * 0.16f + 2.2f, iz * 0.16f + 7.7f))) return 0;       // des bois serrés, des clairières
    if (c[1] > -130.0f && c[1] < -26.0f && fabsf(c[0] - (-16.0f + (c[1] + 128.0f) * 0.25f)) < 10.0f) return 0;       // la trouée du chemin, par où monte la caméra
    c[2] = mon_h(c[0], c[1]); return 1;
}
static int mon_chalet(float px, float pz, float *c, float *h) {      // le village : un chalet par case, ou rien
    float ix = roundf(px / 20.0f), iz = roundf(pz / 20.0f); *h = hash2(ix * 2.3f + 1.1f, iz * 1.9f + 4.7f);
    c[0] = ix * 20.0f + 5.0f * (fract(*h * 11.3f) - 0.5f); c[1] = iz * 20.0f + 5.0f * (fract(*h * 23.1f) - 0.5f);
    if (c[1] > -128.0f || c[1] < -325.0f || fabsf(c[0]) > 190.0f || *h > 0.62f || len2(c[0] + 30.0f, c[1] + 170.0f) < 13.0f || fabsf(c[1] + 232.0f - 18.0f * sinf(c[0] * 0.02f)) < 15.0f) return 0;
    c[2] = mon_h(c[0], c[1]); c[3] = fract(*h * 5.3f) < 0.5f ? 1.0f : 0.0f; return 1;        // c[3] : le faîtage le long de x ou de z
}
static float mon_map(v3 p, int *m) {
    int mm; float ht = p.y - mon_h(p.x, p.z), d = ht * 0.62f, b, r = len2(p.x, p.z); *m = W_GROUND;
    if (r < 27.0f && p.y >= 17.0f) d = fminf(d, p.y - 16.0f);
    else if (r < 27.0f && p.y > -1.0f) {       // l'esplanade
        if (r < 7.5f) { UN(fminf(fminf(cylY(V(p.x, p.y - 0.25f, p.z), 5.4f, 0.25f), cylY(V(p.x, p.y - 0.75f, p.z), 4.6f, 0.25f)), fminf(cylY(V(p.x, p.y - 2.5f, p.z), 3.56f + 0.05f * cosf(atan2f(p.x, p.z) * 28.0f), 1.5f) * 0.8f, cylY(V(p.x, p.y - 4.1f, p.z), 3.85f, 0.10f))) - 0.03f, W_MARBLE)   // le socle rond, à trois degrés
            UN(len2(len2(p.x, p.z) - 3.66f, p.y - 2.5f) - 0.11f, W_BRONZE)                                                                                   // un jonc de bronze
            b = p4_sdf(V(-p.x / 3.0f, (p.y - 4.23f) / 3.0f, -p.z / 3.0f), 0, 0, W_PGOLD, W_PEYE, W_PGOLD, &mm) * 3.0f; if (b < d) { d = b; *m = mm; } }      // le Porc couronné : le cochon de marche, en or
        else d = fminf(d, r - 6.4f);
        if (p.z < -3.0f && r > 20.5f && p.y >= 3.2f) d = fminf(d, p.y - 2.2f);
        else if (p.z < -1.5f && r <= 20.5f) d = fminf(d, 21.6f - r);
        else if (p.z < -3.0f && r > 20.5f) {      // la balustrade, côté vallée : un socle, des balustres, une main courante, un pilier à lanterne tous les huit
            float a = atan2f(p.x, -p.z), sa = 6.2831853f / 96, ka = roundf(a / sa), la = (a - ka * sa) * 22.0f, rr = r - 22.0f; int pil = ((int)ka & 7) == 0;
            UN(fminf(box2(fabsf(rr) - 0.24f, fabsf(p.y - 0.11f) - 0.11f), box2(fabsf(rr) - 0.17f, fabsf(p.y - 1.02f) - 0.07f)) - 0.02f, W_STONE)
            UN(pil ? sdBox(V(la, p.y - 0.62f, rr), V(0.21f, 0.62f, 0.21f)) : fmaxf(len2(la, rr) - 0.065f - 0.035f * sinf((p.y - 0.25f) * 4.2f), fabsf(p.y - 0.6f) - 0.4f) * 0.8f, W_STONE)
            if (pil) { UN(len(V(la, p.y - 1.52f, rr)) - 0.20f, W_LAMP) UN(cylY(V(la, p.y - 1.29f, rr), 0.05f, 0.06f), W_POLE) }
        }
        {   v3 q = V(p.x - MMAST[0], p.y, p.z - MMAST[1]);                                                    // le grand mât, ses couleurs
            if (len2(q.x, q.z) < 5.5f) { UN(fminf(cylY(V(q.x, q.y - 6.6f, q.z), 0.085f, 6.6f), sdBox(V(q.x, q.y - 0.3f, q.z), V(0.42f, 0.3f, 0.42f)) - 0.03f), W_POLE)
                UN(len(V(q.x, q.y - 13.35f, q.z)) - 0.20f, W_FINIAL)
                UN(fmaxf(len2(q.x - 0.13f, q.z) - 0.012f, fabsf(q.y - 6.9f) - 6.2f), W_ROPE)
                UN(wflag(V(q.x * 0.8f - q.z * 0.6f - 0.14f, q.y - MFLAGH + 1.25f, q.x * 0.6f + q.z * 0.8f), 3.9f, 1.25f, MPH), W_FLAG) }
            else d = fminf(d, len2(q.x, q.z) - 4.8f); }
        {   float best = 1e9f;                                                                              // huit mâts autour, un drapeau chacun
            for (int k = 0; k < 8; k++) { float th = (0.44f + 0.70f * (k >> 1)) * ((k & 1) ? -1.0f : 1.0f); v3 q = V(p.x - 19.0f * sinf(th), p.y, p.z - 19.0f * cosf(th)); float rq = len2(q.x, q.z);
                if (rq < 3.2f) { UN(cylY(V(q.x, q.y - 3.6f, q.z), 0.06f, 3.6f), W_POLE) UN(len(V(q.x, q.y - 7.3f, q.z)) - 0.13f, W_FINIAL)
                    UN(wflag(V(q.x * 0.8f - q.z * 0.6f - 0.1f, q.y - 6.15f, q.x * 0.6f + q.z * 0.8f), 2.3f, 0.8f, MPH + k * 1.9f), W_FLAG) }
                else best = fminf(best, rq - 2.7f); }
            d = fminf(d, best); }
        {   v3 q = V(p.x + 13.5f, p.y, p.z - 9.0f);                                                            // la guérite
            UN(fminf(fmaxf(sdBox(V(q.x, q.y - 1.25f, q.z), V(0.62f, 1.25f, 0.62f)), -sdBox(V(q.x - 0.3f, q.y - 1.1f, q.z + 0.5f), V(0.42f, 1.0f, 0.5f))), cone(V(q.x, q.y - 2.5f, q.z), 0.95f, 0.8f)), W_SENTRY) }
        for (int k = 0; k < 3; k++) UN(len(sub(p, MSP[k])) - (0.55f + 0.12f * k), W_SPH)                       // trois sphères de chrome en orbite
    } else if (p.y > -1.0f) d = fminf(d, r - 26.0f);
    if (fabsf(p.z - MLZ) < 4.0f && fabsf(p.x) < 45.0f && p.y > 5.0f && p.y < 23.0f) {       // PORKONIA, en lettres d'or sur la butte
        float d2 = text2d(T_TITLE, p.x / MLS + TR[T_TITLE].w * 0.5f, TR[T_TITLE].base - (p.y - MLY) / MLS) * MLS;
        UN(fmaxf(d2, fabsf(p.z - MLZ) - 1.1f) * 0.9f - 0.05f, W_LETTER)
    } else d = fminf(d, fmaxf(fmaxf(fabsf(p.z - MLZ) - 3.0f, fabsf(p.x) - 44.0f), fmaxf(6.0f - p.y, p.y - 22.0f)));
    if (r > 24.0f && p.z > -640.0f && ht > 15.5f) d = fminf(d, (ht - 14.0f) * 0.6f);
    else if (r > 24.0f && p.z > -640.0f && p.z < 245.0f && fabsf(p.x) < 245.0f) {       // les sapins
        float c[4];
        if (mon_fir(p.x, p.z, c)) { v3 q = V((p.x - c[0]) / c[3], (p.y - c[2]) / c[3], (p.z - c[1]) / c[3]);
            if (q.y < 11.5f) { UN(cylY(V(q.x, q.y - 0.9f, q.z), 0.22f, 1.1f) * c[3], W_TRUNK)
                UN(fminf(fminf(cone(V(q.x, q.y - 1.5f, q.z), 2.3f, 4.2f), cone(V(q.x, q.y - 3.9f, q.z), 1.8f, 3.7f)), cone(V(q.x, q.y - 6.2f, q.z), 1.25f, 3.4f)) * c[3] * 0.9f, W_FIR) }
            else d = fminf(d, (q.y - 9.7f) * c[3]); }
        d = fminf(d, 4.0f - fmaxf(fabsf(p.x - roundf(p.x / 8.0f) * 8.0f), fabsf(p.z - roundf(p.z / 8.0f) * 8.0f)) + 2.0f);
    }
    if (p.z < -112.0f && p.z > -340.0f && p.y < -30.0f) {       // le village au fond de la vallée, son beffroi
        float c[4], h;
        if (mon_chalet(p.x, p.z, c, &h)) { v3 q = V(p.x - c[0], p.y - c[2], p.z - c[1]); if (c[3] > 0.5f) q = V(q.z, q.y, q.x);
            float wx = 3.6f + 1.2f * fract(h * 17.0f), wz = 4.6f + 1.4f * fract(h * 31.0f), H = 3.4f + 1.6f * fract(h * 41.0f), ow = wx + 0.7f, rh = 2.6f;
            UN(sdBox(V(q.x, q.y - H * 0.5f, q.z), V(wx, H * 0.5f, wz)), W_CHALET)
            UN(fmaxf(fmaxf((fabsf(q.x) * rh + (q.y - H) * ow - rh * ow) / sqrtf(rh * rh + ow * ow), H - 0.12f - q.y), fabsf(q.z) - wz - 0.6f), W_CHALET)
            UN(sdBox(V(q.x - 1.4f, q.y - H - 2.0f, q.z + 1.2f), V(0.32f, 1.1f, 0.32f)), W_STONE) }
        d = fminf(d, 10.0f - fmaxf(fabsf(p.x - roundf(p.x / 20.0f) * 20.0f), fabsf(p.z - roundf(p.z / 20.0f) * 20.0f)) + 2.2f);
        {   v3 q = V(p.x + 30.0f, p.y - mon_h(-30.0f, -170.0f), p.z + 170.0f);
            if (len2(q.x, q.z) < 9.0f) { UN(fminf(sdBox(V(q.x, q.y - 9.0f, q.z), V(2.3f, 9.0f, 2.3f)), sdBox(V(q.x, q.y - 14.0f, q.z), V(2.55f, 0.25f, 2.55f))), W_TOWER)
                UN(fmaxf((fmaxf(fabsf(q.x), fabsf(q.z)) * 6.5f + (q.y - 18.0f) * 2.8f - 18.2f) / 7.08f, 18.0f - q.y), W_TOWER)
                UN(len(V(q.x, q.y - 24.9f, q.z)) - 0.3f, W_FINIAL) }
            else d = fminf(d, len2(q.x, q.z) - 7.5f); }
    } else if (p.y < -30.0f) d = fminf(d, fmaxf(p.z + 113.0f, -339.0f - p.z));
    else d = fminf(d, p.y + 31.0f);
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}

// ================================================================ SÉQUENCE 1 : 08 h 00, LA GRANDE PARADE. Une avenue en damier vers +z, les cochons de marche en rangs
// derrière le meneur chromé, la foule sur des gradins des deux côtés, la tribune officielle à droite, l'arc TOUJOURS PLUS au bout.
#define PV 1.6f
#define NCR 24
static float PLZ, PPHI; static pose_t CRP[NCR]; static rig_t CRR[NCR];       // le meneur, la phase du pas ; la foule : douze silhouettes, deux attitudes
S int par_pig(v3 p, v3 *l, float *id) {       // le rang : la case du cochon et son repère ; 0 s'il n'y a personne
    float zz = p.z - PV * T, ix = floorf(p.x / 3.2f), iz = roundf(zz / 4.0f), zc = iz * 4.0f + PV * T;
    *l = V(p.x - (ix + 0.5f) * 3.2f, p.y, zz - iz * 4.0f); *id = hash2(ix * 3.1f + 1.7f, iz * 1.3f + 0.3f);
    return fabsf((ix + 0.5f) * 3.2f) < 12.0f && zc < PLZ - 12.0f && zc > PLZ - 720.0f;
}
static float par_ranks(v3 p, int *m) {
    *m = W_PPINK; float ax = fabsf(p.x);
    if (ax > 13.6f || p.y > 6.6f) return fmaxf(ax - 12.9f, p.y - 5.9f);
    int mm; float d = 1e9f, b, id, bob = 0.06f * fabsf(sinf(PPHI)); v3 l;
    b = p4_sdf(V(p.x / 1.3f, (p.y - bob) / 1.3f, (p.z - PLZ) / 1.3f), PPHI, 1, W_PCHROME, W_PEYE, W_PGOLD, &mm) * 1.3f; if (b < d) { d = b; *m = mm; }      // le meneur, chromé, seul devant
    {   float xr = p.x - 3.2f * (clampf(roundf(p.x / 3.2f - 0.5f), -2, 1) + 0.5f);                                                                         // derrière lui, quatre porcs d'or
        b = p4_sdf(V(xr, p.y - bob, p.z - PLZ + 6.5f), PPHI, 1, W_PGOLD, W_PEYE, W_PGOLD, &mm); if (b < d) { d = b; *m = mm; } }
    if (par_pig(p, &l, &id)) { int sk = id < 0.06f ? W_PGOLD : id < 0.20f ? W_PCHROME : W_PPINK;
        b = p4_sdf(V(l.x, l.y - bob, l.z), PPHI, 1, sk, W_PEYE, W_PGOLD, &mm); if (b < d) { d = b; *m = mm; }
        if (fract(id * 37.7f) < 0.09f) { UN(fmaxf(len2(l.x, l.z + 0.2f) - 0.035f, fabsf(l.y - 3.3f) - 2.2f), W_POLE) UN(len(V(l.x, l.y - 5.56f, l.z + 0.2f)) - 0.07f, W_FINIAL)       // un porte-drapeau
            UN(wflag(V(-(l.z + 0.2f) - 0.05f, l.y - 4.85f, l.x), 1.65f, 0.55f, MPH + id * 40.0f), W_FLAG) } }
    float zz = p.z - PV * T;
    return fminf(d, fminf(1.6f - fabsf(p.x - (floorf(p.x / 3.2f) + 0.5f) * 3.2f), 2.0f - fabsf(zz - roundf(zz / 4.0f) * 4.0f)) + 0.35f);
}
S int par_fan(v3 p, v3 *l) {      // la foule des gradins : le repère du citoyen de la case, et sa silhouette (0 à 23 ; -1 : personne)
    float sg = p.x > 0 ? 1.0f : -1.0f, ax = fabsf(p.x), row = clampf(floorf((ax - 15.9f) / 1.4f), 0, 2), xc = 16.6f + 1.4f * row, iz = roundf(p.z / 1.25f), h = hash2(iz * 1.9f + row * 7.3f, sg * 3.1f + 2.2f);
    int k = (int)(fract(h * 11.3f) * 11.999f); float sc = CZA[k].sc;
    *l = V(sg * (p.z - iz * 1.25f - 0.22f * (fract(h * 5.1f) - 0.5f)) / sc, (p.y - 0.25f - 0.5f * row) / sc, -(ax - xc - 0.2f * (fract(h * 9.7f) - 0.5f)) / sc);
    if (fract(h * 3.7f) > 0.88f || (sg > 0 && fabsf(p.z) < 8.6f) || fabsf(p.z) > 330.0f || (sg > 0 && row == 0 && fabsf(p.z + 80.2f) < 1.8f)) return -1;      // des trous dans les rangs ; la tribune officielle
    return k + 12 * (fract(h * 23.0f) < 0.5f);
}
static float par_crowd(v3 p, int *m) {
    *m = C_SKIN; float ax = fabsf(p.x);
    if (NOCZ) return 1e9f;
    if (ax < 15.2f || ax > 20.8f || p.y > 4.2f) return fmaxf(fmaxf(15.9f - ax, ax - 20.1f), p.y - 3.5f);
    v3 l; int k = par_fan(p, &l), mm; float d = 1e9f, row = clampf(floorf((ax - 15.9f) / 1.4f), 0, 2);
    if (k >= 0) { float sc = CZA[k % 12].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc; d = bd > 0.65f ? bd : cz_sdf(&CZA[k % 12], &CRR[k], &CRP[k], l, &mm) * sc; if (bd <= 0.65f) *m = mm; }
    float lo = row < 0.5f ? 15.2f : 15.9f + 1.4f * row, hi = row > 1.5f ? 20.8f : 17.3f + 1.4f * row;
    return fminf(d, fminf(0.625f - fabsf(p.z - roundf(p.z / 1.25f) * 1.25f), fminf(ax - lo, hi - ax)) + 0.3f);
}
static float par_map(v3 p, int *m) {
    int mm; float d = p.y, b, ax = fabsf(p.x); *m = W_GROUND;
    b = par_ranks(p, &mm); if (b < d) { d = b; *m = mm; }
    b = par_crowd(p, &mm); if (b < d) { d = b; *m = mm; }
    if (ax > 29.0f && ax < 230.0f && fabsf(p.z) < 420.0f) {       // des sapins dans la plaine
        if (p.y > 14.0f) d = fminf(d, p.y - 12.5f);
        else { float ix = roundf(p.x / 8.0f), iz = roundf(p.z / 8.0f), h = hash2(ix * 1.7f + 3.3f, iz * 2.1f + 5.9f), cx = ix * 8.0f + 3.2f * (fract(h * 13.7f) - 0.5f), cz = iz * 8.0f + 3.2f * (fract(h * 29.3f) - 0.5f), sc = 0.55f + 0.5f * fract(h * 7.7f);
            if (fabsf(cx) > 34.0f && h < 0.12f + 0.75f * sstep(0.35f, 0.65f, vnoise(ix * 0.16f + 2.2f, iz * 0.16f + 7.7f))) { v3 q = V((p.x - cx) / sc, p.y / sc, (p.z - cz) / sc);
                UN(cylY(V(q.x, q.y - 0.9f, q.z), 0.22f, 1.1f) * sc, W_TRUNK)
                UN(fminf(fminf(cone(V(q.x, q.y - 1.5f, q.z), 2.3f, 4.2f), cone(V(q.x, q.y - 3.9f, q.z), 1.8f, 3.7f)), cone(V(q.x, q.y - 6.2f, q.z), 1.25f, 3.4f)) * sc * 0.9f, W_FIR) }
            d = fminf(d, 4.0f - fmaxf(fabsf(p.x - ix * 8.0f), fabsf(p.z - iz * 8.0f)) + 2.0f); }
    }
    if (ax > 14.6f && ax < 24.5f && p.y < 12.0f) {       // les bords de l'avenue : barrières drapées, gradins, mâts et guirlandes
        float zb = p.z - 2.5f * roundf(p.z / 2.5f);
        UN(fminf(fminf(fmaxf(len2(ax - 15.3f, zb) - 0.035f, p.y - 1.12f), len2(ax - 15.3f, p.y - 1.10f) - 0.03f), sdBox(V(ax - 15.33f, p.y - 0.60f, 0), V(0.012f, 0.46f + 0.04f * cosf(p.z * 2.513f), 1.0f))), W_RAIL)
        UN(fminf(fminf(sdBox(V(ax - 18.0f, p.y - 0.125f, 0), V(2.1f, 0.125f, 1.0f)), sdBox(V(ax - 18.7f, p.y - 0.375f, 0), V(1.4f, 0.375f, 1.0f))), sdBox(V(ax - 19.4f, p.y - 0.625f, 0), V(0.7f, 0.625f, 1.0f))), W_STAND)
        {   float zm = p.z - 20.0f * roundf(p.z / 20.0f); v3 q = V(ax - 22.8f, p.y, zm);
            if (len2(q.x, q.z) < 3.4f) { UN(cylY(V(q.x, q.y - 5.0f, q.z), 0.09f, 5.0f), W_POLE) UN(len(V(q.x, q.y - 10.2f, q.z)) - 0.17f, W_FINIAL)
                UN(wflag(V(-q.z - 0.1f, q.y - 8.9f, q.x), 2.6f, 0.9f, MPH + roundf(p.z / 20.0f) * 2.3f + (p.x > 0 ? 0.0f : 1.1f)), W_FLAG) }
            else d = fminf(d, len2(q.x, q.z) - 2.9f); }
    } else if (p.y < 12.0f) d = fminf(d, fmaxf(14.0f - ax, ax - 25.1f) > 0 ? fmaxf(fmaxf(14.0f - ax, ax - 25.1f), 0.6f) : 0.6f);
    else d = fminf(d, p.y - 11.0f);
    {   float gb = fmaxf(fmaxf(6.4f - p.y, p.y - 9.7f), ax - 22.9f);       // les guirlandes de fanions, en travers de l'avenue
        if (gb < 0.6f) { float zg = p.z - 20.0f * roundf(p.z / 20.0f), yc = 9.6f - 2.4f * (1 - (p.x / 22.8f) * (p.x / 22.8f)), u = p.x - 1.3f * roundf(p.x / 1.3f), t = (yc - p.y) / 0.75f;
            float pen = fmaxf(fmaxf(fabsf(u) - 0.50f * (1 - t), -t), t - 1) * 0.75f;
            UN(fmaxf(fminf(pen, fabsf(p.y - yc) - 0.025f), fabsf(zg) - 0.015f), W_PENNANT) }
        else d = fminf(d, gb); }
    if (p.x > 15.0f && fabsf(p.z) < 9.5f && p.y < 7.5f) {       // la tribune officielle : une estrade drapée, un dais sur quatre poteaux dorés
        UN(sdBox(V(p.x - 19.2f, p.y - 0.65f, p.z), V(3.3f, 0.65f, 7.6f)), W_TRIB)
        UN(sdBox(V(p.x - 19.3f, p.y - 5.35f, p.z), V(3.9f, 0.22f, 8.3f)), W_TRIB)
        UN(sdBox(V(p.x - 22.45f, p.y - 3.2f, p.z), V(0.08f, 2.0f, 7.6f)), W_TRIB)
        UN(cylY(V(fabsf(p.x - 19.2f) - 3.1f, p.y - 3.2f, fabsf(p.z) - 7.4f), 0.11f, 2.0f), W_FINIAL)
    } else if (p.x > 14.0f) d = fminf(d, fmaxf(fmaxf(fabsf(p.z) - 9.0f, p.y - 7.0f), 0.5f));
    else if (fabsf(p.z) < 9.5f && p.y < 7.5f) d = fminf(d, 15.3f - p.x);
    if (fabsf(p.z - 120.0f) < 5.0f && ax < 19.5f && p.y < 29.0f) {       // l'arc TOUJOURS PLUS
        float blk = sdBox(V(p.x, p.y - 13.5f, p.z - 120.0f), V(17.5f, 13.5f, 3.0f)), op = fminf(box2(ax - 13.5f, fabsf(p.y - 3.0f) - 3.2f), len2(p.x, p.y - 6.0f) - 13.5f);
        UN(fminf(fmaxf(blk, -op), sdBox(V(p.x, p.y - 20.6f, p.z - 120.0f), V(18.1f, 0.3f, 3.5f))), W_ARCH)
    } else d = fminf(d, fmaxf(fmaxf(fabsf(p.z - 120.0f) - 4.0f, ax - 18.6f), p.y - 28.0f) > 0.9f ? fmaxf(fmaxf(fabsf(p.z - 120.0f) - 4.0f, ax - 18.6f), p.y - 28.0f) : 0.9f);
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}

// ================================================================ SÉQUENCE 2 : LE CUBE DE PORC, 120 m. Une vallée fermée ; une plateforme de pierre à galeries, le Cube dessus.
// Les pèlerins montent par l'allée (depuis -z), passent la porte, gravissent l'escalier, entrent dans le porche des Galeries ; d'autres font le tour du Cube.
#define QH 9.0f
#define QP 92.0f
#define QC 60.0f
#define QTOP (QH + 2 * QC)
S float cub_h(float x, float z) { float r = len2(x, z); return 150.0f * sstep(345.0f, 800.0f, r) + 2.2f * sinf(x * 0.021f + 1.0f) * sinf(z * 0.019f + 2.0f) * sstep(120.0f, 260.0f, r) * (z < 0 ? sstep(7.0f, 28.0f, fabsf(x)) : 1.0f); }
S float cub_stair(float z) { return clampf((z + 110.0f) / 18.0f, 0, 1) * QH; }
S int cub_side(v3 p, float *u, float *w) {       // de quel côté de la plateforme on est (0 : -z, 1 : +x, 2 : +z, 3 : -x) ; u le long du côté (dans le sens de la marche), w la distance au centre
    if (fabsf(p.z) > fabsf(p.x)) { if (p.z < 0) { *u = p.x; *w = -p.z; return 0; } *u = -p.x; *w = p.z; return 2; }
    if (p.x > 0) { *u = p.z; *w = p.x; return 1; } *u = -p.z; *w = -p.x; return 3;
}
S int cub_pil(v3 p, v3 *l) {       // la file des pèlerins, dans l'allée, sur l'escalier, jusque dans le porche (-1 : personne)
    float sx = p.x > 0 ? 1.0f : -1.0f, zz = p.z - 0.9f * T, iz = roundf(zz / 2.6f), id = hash2(iz * 1.3f + 0.7f, sx * 2.1f + 4.4f), zc = iz * 2.6f + 0.9f * T;
    int k = (int)(fract(id * 11.3f) * 11.999f); float sc = CZA[k].sc;
    *l = V((p.x - sx * (1.45f + 0.5f * fract(id * 3.1f))) / sc, (p.y - cub_stair(zc)) / sc, (zz - iz * 2.6f) / sc);
    return (fract(id * 5.7f) < 0.8f && zc > -338.0f && zc < -87.6f) ? k : -1;
}
S int cub_ring(v3 p, v3 *l, float *du) {      // ceux qui font le tour, au pied du Cube : une boucle fermée, ils tournent aux coins (*du : distance au milieu de la case, le long de la marche)
    float u, w; int sd = cub_side(p, &u, &w); float row = w < 67.55f ? 0.0f : 1.0f, L = 66.5f + 2.1f * row, n = row < 0.5f ? 190.0f : 196.0f;
    float uu = sd * 2 * L + L + u * L / w - 0.9f * T, iu = roundf(uu / 2.8f), ic = iu - n * floorf(iu / n), id = hash2(ic * 1.7f + row * 5.3f, 3.1f);
    int k = (int)(fract(id * 11.3f) * 11.999f); float sc = CZA[k].sc; *du = uu - iu * 2.8f;
    *l = V(-(w - L - 0.4f * (fract(id * 3.1f) - 0.5f)) / sc, (p.y - QH) / sc, *du / sc);
    return fract(id * 5.7f) < 0.7f ? k : -1;
}
static float cub_map(v3 p, int *m) {
    int mm; float ht = p.y - cub_h(p.x, p.z), d = ht * 0.7f, b, r = len2(p.x, p.z), ax = fabsf(p.x), u, w; *m = W_GROUND; int sd = cub_side(p, &u, &w);
    if (w < QP + 2.0f) {       // la plateforme et ses galeries, le parapet, le Cube, les cierges à son pied
        float pl = sdBox(V(p.x, p.y - QH * 0.5f, p.z), V(QP, QH * 0.5f, QP));
        if (p.y < QH + 1.0f && w > QP - 5.0f && !(sd == 0 && ax < 15.0f)) { float ua = u - 8.0f * roundf(u / 8.0f);       // les arcades : une tous les huit mètres
            if (fabsf(u) < QP - 6.0f) pl = fmaxf(pl, -fmaxf(fminf(box2(fabsf(ua) - 2.0f, fabsf(p.y - 2.0f) - 2.1f), len2(ua, p.y - 4.0f) - 2.0f), QP - 3.2f - w)); }
        UN(pl, W_PLAT)
        if (p.y >= QH + 2.4f && w > QP - 3.2f) d = fminf(d, p.y - QH - 1.4f);
        else if (p.y > QH - 0.5f && w > QP - 3.2f) { float pa = fminf(box2(fabsf(w - QP + 0.4f) - 0.3f, fabsf(p.y - QH - 0.45f) - 0.45f), box2(fabsf(w - QP + 0.4f) - 0.42f, fabsf(p.y - QH - 0.98f) - 0.08f));
            float up = u - 6.0f * roundf(u / 6.0f); pa = fminf(pa, fmaxf(box2(fabsf(up) - 0.42f, fabsf(w - QP + 0.4f) - 0.42f), p.y - QH - 1.5f));       // un dé de pierre tous les six mètres
            if (sd == 0) pa = fmaxf(pa, 12.9f - fabsf(u)); UN(pa, W_PLAT) }
        UN((sdBox(V(p.x, p.y - QH - QC, p.z), V(QC - 2.0f, QC - 2.0f, QC - 2.0f)) - 2.0f - 0.35f * sinf(p.y * 0.55f + 1.5f * sinf(u * 0.06f)) * sinf(u * 0.09f + p.y * 0.04f)) * 0.8f, W_MEAT)
        { float sl = fmaxf(fmaxf(60.4f - w, w - 63.1f), p.y - QH - 1.3f);
            if (sl < 0 && p.y > QH - 0.3f) { float iu = roundf(u / 0.6f), iw = clampf(roundf((w - 61.7f) / 0.6f), -1, 1), h = hash2(iu * 1.31f + sd * 7.7f, iw * 2.17f + 3.3f);
                if (h < 0.55f && fabsf(u) < 60.3f) { float hc = 0.10f + 0.28f * fract(h * 7.1f); v3 q = V(u - iu * 0.6f - 0.2f * (fract(h * 17.3f) - 0.5f), p.y - QH, w - 61.7f - iw * 0.6f - 0.2f * (fract(h * 31.7f) - 0.5f));
                    UN(cylY(V(q.x, q.y - hc, q.z), 0.075f, hc), W_CANDLE)
                    UN(len(V(q.x, (q.y - 2 * hc - 0.10f) * 0.6f, q.z)) - 0.06f, W_FLAME) }
                { float lw = w - 61.7f - iw * 0.6f; d = fminf(d, fminf(0.3f - fabsf(u - iu * 0.6f), (iw * lw > 0 ? 1e9f : 0.3f - fabsf(lw))) + 0.12f); } }
            else if (sl > 0) d = fminf(d, sl + 0.35f); }
        if (p.y > QTOP - 3.0f) {       // au sommet : un mât, le drapeau
            if (r < 19.5f) { UN(cylY(V(p.x, p.y - QTOP - 16.0f, p.z), 0.32f, 17.0f), W_POLE) UN(len(V(p.x, p.y - QTOP - 33.5f, p.z)) - 0.7f, W_FINIAL)
                UN(wflag(V(p.x - 0.35f, p.y - QTOP - 26.5f, p.z), 15.0f, 5.0f, MPH * 0.5f), W_FLAG) }
            else d = fminf(d, r - 18.5f); }
    } else d = fminf(d, w - QP - 1.0f);
    { float bz = fmaxf(fmaxf(ax - 14.0f, -116.0f - p.z), fmaxf(p.z + 83.0f, p.y - QH - 8.0f));       // l'escalier, ses rampes ; la porte au pied ; le porche des Galeries en haut
        if (bz < 0) { float sp = fmaxf(-110.0f - p.z, p.z + 92.0f);
            UN(fmaxf(fmaxf((p.y - 0.5f * (p.z + 110.0f)) * 0.894f, ax - 12.0f), sp), W_PLAT)
            UN(fmaxf(fmaxf(fabsf(p.y - 0.5f * (p.z + 110.0f) - 0.55f) * 0.894f - 0.55f, fabsf(ax - 12.4f) - 0.4f), sp), W_PLAT)
            UN(fminf(fminf(sdBox(V(ax - 7.0f, p.y - 4.2f, p.z + 113.0f), V(1.1f, 4.2f, 1.1f)), sdBox(V(p.x, p.y - 9.5f, p.z + 113.0f), V(9.2f, 1.25f, 1.3f))), sdBox(V(p.x, p.y - 10.93f, p.z + 113.0f), V(9.6f, 0.18f, 1.6f))), W_GATE)
            UN(len(V(ax - 7.0f, p.y - 11.55f, p.z + 113.0f)) - 0.42f, W_LAMP)
            { v3 q = V(p.x, p.y - QH, p.z + 88.2f); float body = sdBox(V(q.x, q.y - 3.1f, q.z), V(5.2f, 3.1f, 3.8f));
                body = fmaxf(body, -fminf(sdBox(V(q.x, q.y - 2.4f, q.z + 0.2f), V(4.4f, 2.6f, 3.2f)), fmaxf(box2(ax - 3.6f, q.y - 4.4f), q.z + 3.0f)));
                UN(fminf(body, sdBox(V(q.x, q.y - 6.38f, q.z), V(5.6f, 0.18f, 4.2f))), W_GATE)
                UN(len(V(ax - 4.45f, q.y - 5.0f, q.z + 4.0f)) - 0.3f, W_LAMP) }
        } else d = fminf(d, bz + 1.0f); }
    { float ba = fmaxf(fmaxf(ax - 7.0f, p.z + 112.0f), fmaxf(-350.0f - p.z, p.y - 5.5f));       // l'allée : une lanterne et un fanion tous les douze mètres ; le pupitre du guide
        if (ba < 0) { float zl = p.z - 12.0f * roundf(p.z / 12.0f);
            UN(cylY(V(ax - 5.2f, p.y - 1.9f, zl), 0.06f, 1.9f), W_POLE)
            UN(len(V(ax - 5.2f, p.y - 4.0f, zl)) - 0.20f, W_LAMP)
            UN(wflag(V(-zl - 0.08f, p.y - 3.1f, ax - 5.2f), 0.9f, 0.32f, MPH + roundf(p.z / 12.0f) * 1.7f), W_FLAG)
            UN(fminf(sdBox(V(p.x - 4.7f, p.y - 1.0f, p.z + 117.5f), V(0.32f, 0.5f, 0.25f)), sdBox(V(p.x - 4.7f, (p.y - 1.58f) - 0.35f * (p.z + 117.5f), p.z + 117.5f), V(0.42f, 0.04f, 0.32f))), W_LECT)
            UN(sdBox(V(p.x - 4.7f, p.y - 0.25f, p.z + 117.0f), V(0.75f, 0.25f, 1.0f)), W_GATE)
        } else d = fminf(d, ba + 1.0f); }
    if (r > 215.0f && r < 620.0f) {       // les sapins des pentes
        if (ht > 15.5f) d = fminf(d, (ht - 14.0f) * 0.6f);
        else { float ix = roundf(p.x / 8.0f), iz = roundf(p.z / 8.0f), h = hash2(ix * 1.7f + 3.3f, iz * 2.1f + 5.9f), cx = ix * 8.0f + 3.2f * (fract(h * 13.7f) - 0.5f), cz = iz * 8.0f + 3.2f * (fract(h * 29.3f) - 0.5f), sc = 0.55f + 0.5f * fract(h * 7.7f);
            if (len2(cx, cz) > 222.0f && !(fabsf(cx) < 12.0f && cz < 0) && h < 0.15f + 0.75f * sstep(0.35f, 0.65f, vnoise(ix * 0.16f + 2.2f, iz * 0.16f + 7.7f))) { v3 q = V((p.x - cx) / sc, (p.y - cub_h(cx, cz)) / sc, (p.z - cz) / sc);
                UN(cylY(V(q.x, q.y - 0.9f, q.z), 0.22f, 1.1f) * sc, W_TRUNK)
                UN(fminf(fminf(cone(V(q.x, q.y - 1.5f, q.z), 2.3f, 4.2f), cone(V(q.x, q.y - 3.9f, q.z), 1.8f, 3.7f)), cone(V(q.x, q.y - 6.2f, q.z), 1.25f, 3.4f)) * sc * 0.9f, W_FIR) }
            d = fminf(d, 4.0f - fmaxf(fabsf(p.x - ix * 8.0f), fabsf(p.z - iz * 8.0f)) + 2.0f); }
    }
    if (!NOCZ) {       // les pèlerins
        if (p.z < -86.5f && p.z > -340.0f) { float bp = fmaxf(ax - 3.4f, (p.y - cub_stair(p.z) - 2.7f) * 0.85f);
            if (bp < 0) { v3 l; int k = cub_pil(p, &l); float zz = p.z - 0.9f * T;
                if (k >= 0) { float sc = CZA[k].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc, e = bd > 0.65f ? bd : fmaxf(cz_sdf(&CZA[k], &CZR[k], &CZP[k], l, &mm) * sc, bd); if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
                d = fminf(d, fminf(1.3f - fabsf(zz - roundf(zz / 2.6f) * 2.6f), fminf(ax, 3.4f - ax)) + 0.3f); }
            else d = fminf(d, bp + 0.5f); }
        { float br = fmaxf(fmaxf(65.2f - w, w - 69.9f), fmaxf(QH - 0.2f - p.y, p.y - QH - 2.7f));
            if (br < 0) { v3 l; float du; int k = cub_ring(p, &l, &du), row = w < 67.55f ? 0 : 1;
                if (k >= 0) { float sc = CZA[k].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc, e = bd > 0.65f ? bd : fmaxf(cz_sdf(&CZA[k], &CZR[k], &CZP[k], l, &mm) * sc, bd); if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
                d = fminf(d, fminf(1.4f - fabsf(du), fminf(w - (row ? 67.55f : 65.2f), (row ? 69.9f : 67.55f) - w)) + 0.3f); }
            else if (p.y > QH - 0.2f && w < QP) d = fminf(d, br + 0.5f); }
    }
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}
// ================================================================ SÉQUENCE 3 : 12 h 00, la taverne de Brassefort. La Douzi Ambrée.
// Une salle de 14 m sur 11 : trois fenêtres à gauche (le soleil de midi entre), la cheminée à droite, le comptoir au fond ; quatre tables, onze buveurs.
#define TVX 7.0f
#define TVZ 5.5f
#define TVH 4.0f
#define CTY 0.74f
#define HMX -0.6f
#define HMZ 3.31f
#define NTB 4
static const float TVTX[NTB] = {-3.9f, -0.9f, 2.7f, 4.4f}, TVTZ[NTB] = {-0.5f, -2.5f, -1.9f, 1.0f};
static const int TVNS[NTB] = {3, 3, 3, 2};
static const float TVSA[NTB][3] = {{0.4f, 2.5f, 4.6f}, {1.2f, 3.3f, 5.3f}, {0.2f, 2.2f, 4.2f}, {3.6f, 5.5f, 0}};      // autour de chaque table : l'angle de chaque tabouret
static float HLEV, HFOAM, HPOUR, HTAP, TBEER, TDEEP; static int HMUG;       // la chope du comptoir : niveau, mousse, filet, manette ; la caméra dans la bière ; la chope est là
static __thread int NOMUG;
S float tav_mug(v3 q, int *m) {       // une chope pleine posée (base en q = 0) : la même que celle des citoyens
    v3 g = V(q.x, q.y - 0.092f, q.z); float d = fminf(cylY(g, 0.068f, 0.088f) - 0.004f, len2(len2(g.x - 0.076f, g.y) - 0.044f, g.z) - 0.013f), f = sdEll(V(g.x, g.y - 0.098f, g.z), V(0.074f, 0.036f, 0.074f));
    *m = C_MUG; if (f < d) { d = f; *m = C_FOAM; } return d;
}
static float tav_map(v3 p, int *m) {
    int mm; float d = p.y + 0.02f, b, ax = fabsf(p.x), az = fabsf(p.z); *m = W_GROUND;
    {   float rb = sdBox(V(p.x, p.y - 2.0f, p.z), V(TVX, 2.0f, TVZ)), sh = fmaxf(sdBox(V(p.x - 0.025f, p.y - 1.7f, p.z), V(TVX + 0.375f, 2.7f, TVZ + 0.4f)), -rb);      // les murs, le sol, le plafond
        sh = fminf(sh, sdBox(V(p.x - 6.7f, p.y - 2.0f, p.z - 0.6f), V(0.3f, 2.0f, 1.25f)));                                                                             // le manteau de la cheminée
        sh = fmaxf(sh, -sdBox(V(p.x - 6.75f, p.y - 0.62f, p.z - 0.6f), V(0.45f, 0.62f, 0.8f)));                                                                         // le foyer
        if (p.x < -6.4f) { float zl = p.z - 3.4f * clampf(roundf(p.z / 3.4f), -1, 1), w2 = box2(fabsf(zl) - 0.65f, fabsf(p.y - 2.1f) - 0.9f);                           // trois fenêtres : de vrais trous, le soleil passe
            sh = fmaxf(sh, -fmaxf(w2, fabsf(p.x + 7.175f) - 0.4f));
            UN(sh, W_WALLT)
            UN(fmaxf(fmaxf(box2(fabsf(zl) - 0.74f, fabsf(p.y - 2.1f) - 0.99f), -box2(fabsf(zl) - 0.62f, fabsf(p.y - 2.1f) - 0.87f)), fabsf(p.x + 7.03f) - 0.07f), W_BEAM)       // le cadre
            UN(fmaxf(fminf(fabsf(zl) - 0.024f, fabsf(p.y - 2.1f) - 0.024f), fmaxf(w2, fabsf(p.x + 7.14f) - 0.03f)), W_BEAM)                                               // les petits bois
            UN(sdBox(V(p.x + 6.9f, p.y - 1.14f, zl), V(0.14f, 0.03f, 0.80f)), W_BEAM)                                                                                   // l'appui
        } else { UN(sh, W_WALLT) d = fminf(d, p.x + 6.96f); }
    }
    if (p.y > 3.4f) { float zb = p.z - 1.6f * roundf(p.z / 1.6f); UN(fmaxf(box2(fabsf(zb) - 0.11f, fabsf(p.y - 3.84f) - 0.16f), fmaxf(ax - TVX, az - TVZ)), W_BEAM) }       // les solives
    else d = fminf(d, 3.68f - p.y);
    { float xb = p.x - 2.33f * roundf(p.x / 2.33f), zq = p.z - 3.4f * roundf((p.z - 1.7f) / 3.4f) - 1.7f;                                                 // les poteaux le long des murs, la cimaise
        float in = fmaxf(fmaxf(ax - TVX, az - TVZ), fmaxf(p.y - TVH, -p.y));
        UN(fmaxf(fmaxf(fabsf(xb) - 0.10f, TVZ - 0.07f - az), in), W_BEAM) UN(fmaxf(fmaxf(fabsf(zq) - 0.10f, TVX - 0.07f - ax), in), W_BEAM)
        UN(fmaxf(fmaxf(fabsf(p.y - 1.1f) - 0.04f, fminf(TVX - ax, TVZ - az) - 0.05f), in), W_BEAM) }
    UN(sdBox(V(p.x - 6.3f, p.y - 1.38f, p.z - 0.6f), V(0.18f, 0.045f, 1.45f)), W_BEAM)                                                                    // la tablette de la cheminée, l'horloge au-dessus
    UN(fmaxf(len2(p.y - 2.75f, p.z - 0.6f) - 0.36f, fabsf(p.x - 6.37f) - 0.035f), W_CLOCK)
    if (p.x > 6.2f && fabsf(p.z - 0.6f) < 0.9f && p.y < 1.3f) {                                                                                           // le feu
        UN(fminf(sdCap(p, V(6.72f, 0.09f, 0.15f), V(6.72f, 0.09f, 1.05f), 0.075f), sdCap(p, V(6.88f, 0.20f, 0.25f), V(6.66f, 0.20f, 0.95f), 0.065f)), W_TRUNK)
        for (int i = 0; i < 3; i++) { float ph = T * (6.0f + i) + i * 2.1f, hh = 0.22f + 0.07f * sinf(ph) + 0.04f * sinf(ph * 2.3f);
            UN(sdEll(V(p.x - 6.76f - 0.03f * sinf(ph * 0.7f), p.y - 0.22f - hh, p.z - 0.32f - 0.28f * i), V(0.085f, hh, 0.085f)), W_FIRE) }
    } else d = fminf(d, fmaxf(fmaxf(6.55f - p.x, fabsf(p.z - 0.6f) - 0.72f), p.y - 1.15f) > 0.12f ? fmaxf(fmaxf(6.55f - p.x, fabsf(p.z - 0.6f) - 0.72f), p.y - 1.15f) : 0.12f);
    if (p.z > 2.7f) {       // le fond de la salle : le comptoir, les tireuses, la chope, les étagères, l'enseigne, l'ardoise, les tonneaux
        UN(fminf(sdBox(V(p.x + 1.0f, p.y - 0.36f, p.z - 3.07f), V(4.0f, 0.36f, 0.05f)), sdBox(V(p.x + 1.0f, p.y - 0.36f, p.z - 3.38f), V(3.95f, 0.36f, 0.30f))), W_COUNTER)
        UN(sdBox(V(p.x + 1.0f, p.y - 0.715f, p.z - 3.32f), V(4.12f, 0.022f, 0.42f)) - 0.008f, W_CTOP)
        UN(sdCap(p, V(-4.9f, 0.13f, 2.93f), V(2.9f, 0.13f, 2.93f), 0.02f), W_BRASS)
        UN(sdBox(V(p.x + 1.0f, p.y - 0.13f, p.z - 4.3f), V(4.0f, 0.13f, 0.5f)), W_COUNTER)
        if (fabsf(p.x - HMX) < 0.6f && p.z < 4.0f && p.y < 1.4f) { float xl = p.x - HMX, k = clampf(roundf(xl / 0.26f), -1, 1); xl -= 0.26f * k; v3 q = V(xl, p.y - CTY, p.z - 3.58f);       // trois tireuses de laiton
            float t = fminf(cylY(V(q.x, q.y - 0.125f, q.z), 0.028f, 0.125f), cylY(V(q.x, q.y - 0.012f, q.z), 0.05f, 0.012f));
            t = fminf(t, fminf(sdCap(q, V(0, 0.245f, 0.02f), V(0, 0.245f, -0.26f), 0.022f), sdCap(q, V(0, 0.245f, -0.26f), V(0, 0.208f, -0.27f), 0.014f)));
            UN(t, W_BRASS)
            float a = 0.15f + 0.65f * (k == 0 ? HTAP : 0); v3 h1 = V(0, 0.27f + 0.15f * cosf(a), -0.02f + 0.15f * sinf(a));
            UN(fminf(sdCap(q, V(0, 0.27f, -0.02f), h1, 0.013f), len(sub(q, h1)) - 0.024f), W_HANDLE)
            if (HMUG && !NOMUG) { v3 g = V(p.x - HMX, p.y - CTY, p.z - HMZ);                                                                                                               // la chope de verre, sous le bec
                UN(fminf(cylY(V(g.x, g.y - 0.09f, g.z), 0.074f, 0.09f) - 0.004f, len2(len2(g.x - 0.086f, g.y - 0.09f) - 0.05f, g.z) - 0.013f), W_MUGH)
                if (HFOAM > 0.01f) UN(sdEll(V(g.x, g.y - 0.182f, g.z), V(0.074f, 0.012f + 0.05f * HFOAM, 0.074f)), W_HFOAM) }
            if (HPOUR > 0.01f) { float y0 = CTY + 0.012f + 0.15f * HLEV, y1 = CTY + 0.208f;                                                                                               // le filet de bière
                UN(cylY(V(p.x - HMX + 0.003f * sinf(p.y * 70.0f + T * 31.0f), p.y - 0.5f * (y0 + y1), p.z - HMZ), 0.0075f * HPOUR, 0.5f * (y1 - y0)), W_BEERS) }
        } else d = fminf(d, fmaxf(fmaxf(fabsf(p.x - HMX) - 0.45f, p.z - 3.85f), p.y - 1.25f) > 0.12f ? fmaxf(fmaxf(fabsf(p.x - HMX) - 0.45f, p.z - 3.85f), p.y - 1.25f) : 0.12f);
        { float sb = fmaxf(fmaxf(-4.65f - p.x, p.x - 2.65f), fmaxf(fmaxf(1.40f - p.y, p.y - 2.88f), 5.25f - p.z));                                                                         // trois étagères, des bouteilles
            if (sb < 0.1f) { float k = clampf(roundf((p.y - 1.5f) / 0.5f), 0, 2), yl = p.y - 1.5f - 0.5f * k, xc = 0.2f * roundf(p.x / 0.2f), h = hash2(xc * 3.7f + 1.1f, k * 5.3f + 2.2f);
                UN(sdBox(V(p.x + 1.0f, yl + 0.02f, p.z - 5.38f), V(3.6f, 0.018f, 0.12f)), W_BEAM)
                if (h < 0.78f && fabsf(xc + 1.0f) < 3.5f) { float r = 0.034f + 0.014f * fract(h * 7.3f), hh = 0.09f + 0.05f * fract(h * 13.1f); v3 q = V(p.x - xc, yl, p.z - 5.38f);
                    UN(fminf(cylY(V(q.x, q.y - hh, q.z), r, hh) - 0.004f, cylY(V(q.x, q.y - 2 * hh - 0.045f, q.z), 0.013f, 0.05f)), W_BOTTLE) }
                d = fminf(d, 0.1f - fabsf(p.x - xc) + 0.03f); }
            else d = fminf(d, sb); }
        UN(sdBox(V(p.x + 1.0f, p.y - 3.3f, p.z - 5.46f), V(2.4f, 0.32f, 0.04f)), W_SIGN)
        UN(sdBox(V(p.x + 5.75f, p.y - 2.1f, p.z - 5.46f), V(0.78f, 0.62f, 0.04f)), W_BOARD)
        if (p.x > 3.7f && p.z > 3.7f && p.y < 2.4f) {                                                                                                                                      // trois tonneaux sur leur chantier
            static const float BX[3] = {4.7f, 6.0f, 5.35f}, BY[3] = {0.66f, 0.66f, 1.62f};
            for (int i = 0; i < 3; i++) { v3 q = V(p.x - BX[i], p.y - BY[i], p.z - 4.75f); float zz = q.z / 0.65f; UN(fmaxf(len2(q.x, q.y) - 0.56f + 0.09f * zz * zz, fabsf(q.z) - 0.65f) * 0.9f, W_BARREL) }
            UN(fminf(sdBox(V(p.x - 5.35f, p.y - 0.06f, p.z - 4.35f), V(1.35f, 0.06f, 0.07f)), sdBox(V(p.x - 5.35f, p.y - 0.06f, p.z - 5.15f), V(1.35f, 0.06f, 0.07f))), W_BEAM)
            UN(sdCap(p, V(4.7f, 0.42f, 4.08f), V(4.7f, 0.42f, 3.96f), 0.02f), W_BRASS)
        } else d = fminf(d, fmaxf(fmaxf(3.95f - p.x, 3.98f - p.z), p.y - 2.22f) > 0.15f ? fmaxf(fmaxf(3.95f - p.x, 3.98f - p.z), p.y - 2.22f) : 0.15f);
    } else d = fminf(d, 2.88f - p.z);
    for (int k = 0; k < NTB; k++) { float dx = p.x - TVTX[k], dz = p.z - TVTZ[k], dk = len2(dx, dz) - 1.22f;                                                                               // les tables, les tabourets, une chope de plus
        if (dk < 0.25f && p.y < 1.0f) {
            UN(fminf(fminf(cylY(V(dx, p.y - 0.595f, dz), 0.62f, 0.022f) - 0.006f, cylY(V(dx, p.y - 0.3f, dz), 0.055f, 0.3f)), fminf(cylY(V(dx, p.y - 0.02f, dz), 0.30f, 0.02f), cylY(V(dx, p.y - 0.54f, dz), 0.16f, 0.03f))), W_TABLE)
            for (int j = 0; j < TVNS[k]; j++) { float a = TVSA[k][j], sx = dx - 0.95f * sinf(a), sz = dz - 0.95f * cosf(a);
                UN(fminf(cylY(V(sx, p.y - 0.235f, sz), 0.19f, 0.022f) - 0.006f, fminf(cylY(V(sx, p.y - 0.11f, sz), 0.035f, 0.11f), cylY(V(sx, p.y - 0.015f, sz), 0.15f, 0.015f))), W_TABLE) }
            { float a = TVSA[k][0] + 1.1f; b = tav_mug(V(dx - 0.22f * sinf(a), p.y - 0.623f, dz - 0.22f * cosf(a)), &mm); if (b < d) { d = b; *m = mm; } }
        } else d = fminf(d, fmaxf(dk, p.y - 0.86f)); }
    if (p.y > 2.45f) {       // ce qui pend : la guirlande de saucisses, quatre jambons, trois lanternes
        if (fabsf(p.z - 3.2f) < 0.25f && p.y > 3.2f && p.y < 3.75f) { float xc = 0.21f * roundf(p.x / 0.21f);
            if (fabsf(xc + 1.0f) < 3.9f) UN(sdEll(V(p.x - xc, p.y - 3.64f + 0.20f * (0.5f - 0.5f * cosf((xc + 1.0f) * 2.618f)), p.z - 3.2f), V(0.095f, 0.036f, 0.036f)), W_SAUS)
            d = fminf(d, 0.105f - fabsf(p.x - xc) + 0.03f); }
        else d = fminf(d, fmaxf(fmaxf(fabsf(p.z - 3.2f) - 0.05f, 3.38f - p.y), p.y - 3.69f) > 0.1f ? fmaxf(fmaxf(fabsf(p.z - 3.2f) - 0.05f, 3.38f - p.y), p.y - 3.69f) : 0.1f);
        static const float HX[4] = {-3.2f, -1.7f, 0.7f, 2.3f}, LX[3] = {-3.6f, 2.9f, 0.3f}, LZ[3] = {-1.6f, -1.6f, 0.0f};
        for (int i = 0; i < 4; i++) { v3 q = V(p.x - HX[i], p.y - 3.0f, p.z - 1.6f); float e = len(q) - 0.48f;
            if (e < 0.2f) { UN(sdEll(V(q.x, q.y + 0.02f, q.z), V(0.17f, 0.27f, 0.13f)), W_HAM) UN(sdCap(q, V(0, 0.2f, 0), V(0, 0.70f, 0), 0.03f), W_HAM) } else d = fminf(d, e); }
        for (int i = 0; i < 3; i++) { v3 q = V(p.x - LX[i], p.y - 3.0f, p.z - LZ[i]); float e = len(q) - 0.3f;
            if (e < 0.2f || (len2(q.x, q.z) < 0.2f && q.y > 0)) { UN(len(q) - 0.11f, W_LAMP) UN(fminf(cylY(V(q.x, q.y - 0.12f, q.z), 0.07f, 0.02f), cylY(V(q.x, q.y - 0.42f, q.z), 0.008f, 0.29f)), W_POLE)
                UN(cylY(V(q.x, q.y + 0.12f, q.z), 0.05f, 0.012f), W_POLE) } else d = fminf(d, fminf(e, len2(q.x, q.z) - 0.1f > 0.1f ? len2(q.x, q.z) - 0.1f : 0.1f)); }
    } else d = fminf(d, 2.6f - p.y);
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}
// ================================================================ SÉQUENCE 4 : la nuit. Au fond du verre, le Gras-Fond II ; à bord, Fatbass aux platines (04 h 12).
// Dehors : le sous-marin posé sur le fond, dans la bière. Dedans : un tunnel de 23 m, la piste, la foule, la cabine du DJ au bout (+z).
static int GFIN; static float GBEAT, GDROP = 1, GCZ1 = 4.2f;      // dedans ou dehors ; le battement (1 sur le temps, puis il retombe) ; l'ardeur de la foule
#define GSY -4.3f
#define GFL -1.7f
S float gf_hullr(float z) { if (z > 12.0f) { float u = (z - 12.0f) / 4.2f; return 3.2f * sqrtf(fmaxf(1 - u * u, 0.0f)); } if (z > -10.0f) return 3.2f; return fmaxf(3.2f - 2.8f * powf((-10.0f - z) / 9.5f, 1.6f), 0.4f); }
static float gf_out_map(v3 p, int *m) {
    float d = (p.y - GSY - 0.5f * sinf(p.x * 0.13f + 1.0f) * sinf(p.z * 0.11f) - 0.10f * sinf(p.x * 0.9f + p.z * 0.6f)) * 0.8f, ax = fabsf(p.x); *m = W_SEABED;
    { float bs = fmaxf(len2(p.x, p.y - 1.5f) - 7.6f, fabsf(p.z + 2.0f) - 19.6f);                                                                 // le sous-marin
        if (bs > 1.0f) d = fminf(d, bs);
        else { float r = len2(p.x, p.y), rz = gf_hullr(p.z);
            UN(fmaxf((r - rz) * 0.75f, fmaxf(p.z - 16.2f, -19.6f - p.z)), W_HULL)
            UN(sdBox(V(p.x, p.y - 4.3f, p.z - 3.0f), V(0.55f, 1.35f, 2.3f)) - 0.45f, W_SAIL)                                                    // le kiosque, le périscope, l'antenne
            UN(fminf(cylY(V(p.x, p.y - 7.0f, p.z - 3.8f), 0.11f, 1.1f), sdBox(V(p.x, p.y - 8.2f, p.z - 3.9f), V(0.14f, 0.14f, 0.26f))), W_BRASS)
            UN(cylY(V(p.x, p.y - 7.2f, p.z - 2.0f), 0.035f, 1.3f), W_SAIL)
            UN(sdBox(V(p.x, p.y - 3.1f, p.z), V(0.8f, 0.12f, 10.0f)), W_SAIL)                                                                   // le pont
            UN(fminf(sdBox(V(p.x, p.y, p.z + 17.0f), V(3.3f, 0.10f, 0.9f)) - 0.04f, sdBox(V(p.x, p.y, p.z + 17.0f), V(0.10f, 3.0f, 0.9f)) - 0.04f), W_SAIL)     // l'empennage, les barres de plongée
            UN(sdBox(V(ax - 3.7f, p.y - 0.2f, p.z - 10.5f), V(1.0f, 0.08f, 0.6f)) - 0.03f, W_SAIL)
            { float a = T * 1.1f, c = cosf(a), sn = sinf(a), qx = p.x * c + p.y * sn, qy = -p.x * sn + p.y * c; if (fabsf(qx) < fabsf(qy)) { float t = qx; qx = qy; qy = t; }       // l'hélice : quatre pales
                UN(fminf(sdCap(p, V(0, 0, -19.4f), V(0, 0, -20.4f), 0.3f), sdEll(V(fabsf(qx) - 1.25f, qy, p.z + 20.0f + 0.25f * qy), V(1.05f, 0.36f, 0.07f)) * 0.8f), W_BRASS) }
            { float zl = p.z - 1.0f - 3.0f * clampf(roundf((p.z - 1.0f) / 3.0f), -3, 3), rr = len2(zl, p.y - 0.7f);                                                                   // sept hublots de chaque bord
                UN(len2(rr - 0.50f, ax - 3.13f) - 0.075f, W_BRASS) UN(fmaxf(rr - 0.50f, fabsf(ax - 3.10f) - 0.06f), W_PORT) }
            UN(len(V(p.x, p.y - 2.5f, p.z - 13.3f)) - 0.42f, W_SAIL) UN(fmaxf(len2(p.x, p.y - 2.5f) - 0.33f, fabsf(p.z - 13.72f) - 0.03f), W_LIGHT)                                  // le projecteur d'étrave
            UN(fmaxf(sdBox(V(p.x, p.y - GSY - 0.7f, fabsf(p.z + 1.0f) - 7.0f), V(2.7f, 1.0f, 0.35f)), 3.1f - r), W_SAIL) }                                                          // deux bers
    }
    { v3 q0 = V(p.x - 11.0f, p.y - GSY - 1.25f, p.z + 8.0f); float e = len(q0) - 4.2f;                                                                                              // une capsule de bouteille, à demi enfouie
        if (e > 1.0f) d = fminf(d, e);
        else { float c = cosf(0.55f), sn = sinf(0.55f); v3 q = V(q0.x * c + q0.y * sn, -q0.x * sn + q0.y * c, q0.z); float rr = len2(q.x, q.z), ang = atan2f(q.z, q.x);
            UN(fmaxf(rr - 3.3f - 0.14f * cosf(21.0f * ang) * sstep(-0.1f, -0.5f, q.y), fabsf(q.y + 0.25f) - 0.33f - 0.0f) * 0.8f, W_CAP) } }
    return d;
}
S int gf_fan(v3 p, v3 *l) {       // la foule sur la piste : le repère du danseur de la case, sa silhouette et son attitude (0 à 23 ; -1 : personne)
    float ix = clampf(floorf((p.x + 2.1f) / 1.05f), 0, 3), iz = roundf(p.z / 1.15f), h = hash2(ix * 3.7f + 1.3f, iz * 1.9f + 4.1f);
    int k = (int)(fract(h * 11.3f) * 11.999f); if (k == 5) k = 0; if (k == 11) k = 4; float sc = CZA[k].sc, yw = 0.6f * (fract(h * 29.0f) - 0.5f)       /* pas d'enfants à bord à quatre heures du matin */, c = cosf(yw), sn = sinf(yw), dx = p.x + 2.1f - (ix + 0.5f) * 1.05f - 0.2f * (fract(h * 5.1f) - 0.5f), dz = p.z - iz * 1.15f - 0.2f * (fract(h * 9.7f) - 0.5f);
    *l = V((dx * c - dz * sn) / sc, (p.y - GFL - 0.06f) / sc, (dx * sn + dz * c) / sc);      // tournés vers la cabine (+z)
    if (fract(h * 3.7f) > 0.86f || iz * 1.15f < -9.3f || iz * 1.15f > GCZ1) return -1;
    return k + 12 * (fract(h * 23.0f) < 0.5f);
}
static float gf_in_map(v3 p, int *m) {
    int mm; float r = len2(p.x, p.y), d = 3.0f - r, b, ax = fabsf(p.x); *m = W_HULLIN;
    UN(p.y - GFL, W_DECK) UN(fminf(11.0f - p.z, p.z + 12.0f), W_BULK)
    { float zl = p.z - 2.0f * roundf(p.z / 2.0f); UN(fmaxf(fabsf(zl) - 0.07f, 2.80f - r), W_RIB) }                                                    // les couples
    UN(fminf(fminf(len2(ax - 1.15f, p.y - 2.42f) - 0.085f, len2(p.x, p.y - 2.72f) - 0.11f), len2(ax - 2.35f, p.y - 1.45f) - 0.06f), W_PIPE)           // les tuyaux
    if (r > 2.5f && p.y > -0.4f && p.y < 1.5f) { float zl = p.z - 1.0f - 3.0f * clampf(roundf((p.z - 1.0f) / 3.0f), -3, 2), rr = len2(zl, p.y - 0.5f);   // les hublots, vus de l'intérieur
        UN(len2(rr - 0.50f, ax - 2.93f) - 0.07f, W_BRASS) }
    if (p.y < GFL + 0.12f) UN(sdBox(V(p.x, p.y - GFL - 0.03f, p.z + 1.9f), V(2.2f, 0.03f, 7.9f)), W_DFLOOR)                                            // la piste lumineuse
    if (p.z > 5.6f) {       // la cabine : l'estrade, la console, les platines, les enceintes, l'enseigne, l'horloge
        UN(sdBox(V(p.x, p.y - GFL - 0.17f, p.z - 8.6f), V(2.4f, 0.17f, 2.4f)), W_DECK)
        UN(sdBox(V(p.x, p.y - GFL - 0.65f, p.z - 7.35f), V(1.3f, 0.29f, 0.40f)) - 0.02f, W_CONSOLE)
        { float sx = ax - 0.38f, ty = p.y - GFL - 0.985f; UN(fminf(cylY(V(sx, ty, p.z - 7.60f), 0.275f, 0.018f), cylY(V(sx, ty - 0.026f, p.z - 7.60f), 0.25f, 0.010f)), W_VINYL)       // deux platines, les bras
            UN(fminf(sdCap(V(sx, ty, p.z - 7.60f), V(0.27f, 0.06f, -0.20f), V(0.11f, 0.05f, 0.05f), 0.011f), cylY(V(sx - 0.27f, ty - 0.03f, p.z - 7.40f), 0.028f, 0.035f)), W_PIPE)
            UN(sdBox(V(p.x, ty - 0.015f, p.z - 7.52f), V(0.085f, 0.03f, 0.22f)), W_MIXER) UN(sdBox(V(p.x, ty - 0.01f, p.z - 7.12f), V(1.1f, 0.025f, 0.12f)), W_MIXER) }
        UN(sdBox(V(ax - 2.0f, p.y - GFL - 1.42f, p.z - 9.9f), V(0.52f, 1.08f, 0.42f)) - 0.02f, W_SPEAKER)
        UN(sdBox(V(p.x, p.y - 1.45f, p.z - 10.9f), V(1.75f, 0.42f, 0.05f)), W_NEON)
        UN(sdBox(V(p.x - 1.02f, p.y - 0.36f, p.z - 10.92f), V(0.62f, 0.24f, 0.04f)), W_LED)
    } else d = fminf(d, 6.1f - p.z);
    UN(len(V(p.x, p.y - 2.0f, p.z + 2.5f)) - 0.42f, W_BALL) UN(cylY(V(p.x, p.y - 2.7f, p.z + 2.5f), 0.02f, 0.3f), W_PIPE)                              // la boule à facettes
    if (!NOCZ) {       // la foule
        float bz = fmaxf(fmaxf(ax - 2.2f, p.y - GFL - 2.9f), fmaxf(-9.95f - p.z, p.z - GCZ1 - 0.65f));
        if (bz < 0) { v3 l; int k = gf_fan(p, &l);
            if (k >= 0) { float sc = CZA[k % 12].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc, e = bd > 0.65f ? bd : fmaxf(cz_sdf(&CZA[k % 12], &CRR[k], &CRP[k], l, &mm) * sc, bd); if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
            { float ix = clampf(floorf((p.x + 2.1f) / 1.05f), 0, 3), lx = p.x + 2.1f - (ix + 0.5f) * 1.05f, lz = p.z - roundf(p.z / 1.15f) * 1.15f;
                d = fminf(d, fminf((ix < 0.5f && lx < 0) || (ix > 2.5f && lx > 0) ? 1e9f : 0.525f - fabsf(lx), 0.575f - fabsf(lz)) + 0.3f); } }
        else d = fminf(d, bz + 0.45f); }
    b = wpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}
static float wmap(v3 p, int *m) {
    switch (SEQ) { case 1: return par_map(p, m); case 2: return cub_map(p, m); case 3: return tav_map(p, m); case 4: return GFIN ? gf_in_map(p, m) : gf_out_map(p, m); default: return mon_map(p, m); }
}
