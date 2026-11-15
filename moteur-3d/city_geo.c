// ================================================================ DOUZI CITY, cinq heures du matin : la neige, les néons
// The avenue runs along z (the tram in its middle), and opens at z = SQ0 on the square: the chrome pig, the Ministry at the far end.
#define CL 12.0f
#define FX 9.5f
#define SQ0 58.0f
#define SQ1 112.0f
#define SQW 34.0f
#define TSF 0.1493f
#define TZ (-70.0f)
#define TYB 33.0f
#define TXC 24.3f
S float bh(float iz, float sg) { return hash2(iz * 1.37f + 2.1f, sg * 3.3f + 0.7f); }
static float rowcell(v3 p, float *bx, float *lz, float *H, float *h, float *wall) {       // the house of the avenue around p (-1: none)
    float sg = p.x > 0 ? 1.0f : -1.0f, iz = roundf(p.z / CL);
    *lz = p.z - iz * CL; *bx = fabsf(p.x) - FX - 6.5f; *h = bh(iz, sg); *H = 12.0f + 7.0f * *h; *wall = CL * 0.5f - fabsf(*lz);
    return (iz * CL < 49 && iz * CL > -229) ? 1.0f : -1.0f;
}
static float shopgoods(float a, float b, float c, int *m) {       // la charcuterie, derrière la vitre (a : le long de la vitrine, b : la hauteur, c : la profondeur)
    float d = len2(b - 2.78f, c - 0.55f) - 0.018f; *m = M_GOLD;                                                       // la tringle de laiton
    {   float i = roundf(a / 0.82f), la = a - i * 0.82f, hs = hash2(i, 7.7f), top = 2.62f - 0.12f * hs, sw = 0.035f * sinf(T * 1.3f + i * 2.1f);     // les jambons, un tous les 82 cm
        v3 q = V(la - sw * (2.78f - b), b - (top - 0.62f), c - 0.55f - 0.02f * sinf(i * 3.7f));
        UN(smin(sdEll(q, V(0.165f, 0.25f, 0.105f)), sdCap(q, V(0, 0.16f, 0), V(0, 0.55f, 0), 0.046f), 0.13f), M_HAM)
        UN(cylY(V(q.x, q.y - 0.615f, q.z), 0.024f, 0.03f), M_IVORY)                                                   // l'os
        UN(fmaxf(len2(q.x, q.z) - 0.007f, fmaxf(0.62f - q.y, b - 2.78f)), M_IVORY) }                                    // la ficelle
    {   float i = roundf(a / 0.82f - 0.5f) + 0.5f, la = a - i * 0.82f, hs = hash2(i, 3.1f);                              // entre deux jambons, trois saucissons
        for (int k = -1; k <= 1; k++) { float L = 0.30f + 0.08f * fract(hs * (7 + 3 * k)), y1 = 2.56f - 0.05f * (k == 0);
            v3 q = V(la - 0.075f * k, b, c - 0.55f - 0.03f * k);
            UN(sdCap(q, V(0, y1, 0), V(0, y1 - L, 0), 0.036f), M_SAUC)
            UN(fmaxf(len2(q.x, q.z) - 0.006f, fmaxf(y1 - b, b - 2.78f)), M_IVORY) } }
    {   float la = a - 1.64f * roundf(a / 1.64f), u = la / 0.82f, sl = 0.634f * u, yc = 2.80f - 0.26f * (1 - u * u);     // le chapelet de saucisses, en guirlande devant
        UN((len2((b - yc) / sqrtf(1 + sl * sl), c - 0.20f) - 0.032f * (0.5f + 0.5f * fabsf(sinf(la * 27.0f)))) * 0.8f, M_CHAP) }
    UN(fminf(sdBox(V(a + 0.8f, b - 0.72f, c - 1.25f), V(4.1f, 0.22f, 0.45f)), sdBox(V(a + 0.8f, b - 0.61f, c - 0.50f), V(4.1f, 0.11f, 0.42f))), M_CLOTH)   // l'étal : deux marches sous une nappe à carreaux
    {   float i = roundf(a / 1.1f), la = a - i * 1.1f;                                                                // devant : pâtés en croûte et terrines, une étiquette chacun
        if (((int)i) & 1) UN(sdBox(V(la, b - 0.785f, c - 0.50f), V(0.20f, 0.060f, 0.075f)) - 0.02f, M_PATE)
        else UN(fmaxf(sdEll(V(la, b - 0.74f, c - 0.50f), V(0.19f, 0.10f, 0.13f)), 0.725f - b), M_PATE)
        UN(sdBox(V(la - 0.18f, b - 0.775f, c - 0.20f), V(0.045f, 0.035f, 0.004f)), M_IVORY) }
    {   float i = roundf(a / 0.15f), la = a - i * 0.15f, grp = fract(roundf(a / 2.4f - 0.25f) * 0.5f);                   // derrière : des bocaux en rangs, et un jambon sur son support
        float la2 = a - 2.4f * (roundf(a / 2.4f - 0.25f) + 0.25f);
        if (1) { UN(cylY(V(la, b - 1.01f, c - 1.15f), 0.052f, 0.07f) - 0.004f, M_JAR) UN(cylY(V(la - 0.075f, b - 1.01f, c - 1.34f), 0.052f, 0.07f) - 0.004f, M_JAR) }
        else { v3 q = V(la2, b - 1.16f, c - 1.22f); float cr = 0.94f, sr = 0.34f; q = V(cr * q.x - sr * q.y, sr * q.x + cr * q.y, q.z);
            UN(smin(sdEll(q, V(0.27f, 0.15f, 0.13f)), sdCap(q, V(0.2f, 0, 0), V(0.47f, 0.02f, 0), 0.04f), 0.1f), M_HAM)
            UN(sdBox(V(la2, b - 0.975f, c - 1.22f), V(0.30f, 0.035f, 0.10f)), M_GOLD) }
        (void)grp; }
    UN(len(V(a - 3.0f * (roundf(a / 3.0f - 0.4f) + 0.4f), b - 2.70f, c - 1.15f)) - 0.11f, M_LBULB)                       // les globes
    return d;
}
static float rows(v3 p, int *m) {
    float ax = fabsf(p.x); *m = M_FACADE;
    float db = fmaxf(fmaxf(FX - 1.5f - ax, ax - FX - 13.3f), fmaxf(fmaxf(p.z - 54.0f, -234.0f - p.z), p.y - 26.0f));
    if (db > 0.3f) return db;
    float bx, lz, H, h, wall; if (rowcell(p, &bx, &lz, &H, &h, &wall) < 0) return wall + 0.12f;
    float d = sdBox(V(bx, p.y - H * 0.5f, lz), V(6.5f, H * 0.5f, 5.8f)), rh = 2.6f + 1.6f * fract(h * 7.3f), fr = bx + 6.5f;       // fr : la profondeur depuis le nu de la façade
    if (fr < 0.6f) {        // la façade sur l'avenue : les fenêtres en creux, leurs appuis, la corniche, la vitrine
        float ur = lz - 2.3f * roundf(lz / 2.3f), vv = p.y - 4.2f, vr = vv - 3.1f * (floorf(vv / 3.1f) + 0.48f);
        if (vv > 0 && p.y < H - 0.6f && fabsf(lz) < 5.2f) {
            d = fmaxf(d, -sdBox(V(fr - 0.02f, vr, ur), V(0.18f, 0.99f, 0.53f)));
            d = fminf(d, sdBox(V(fr + 0.07f, vr + 1.04f, ur), V(0.13f, 0.045f, 0.64f))); }
        d = fminf(d, sdBox(V(fr + 0.12f, p.y - H + 0.22f, lz), V(0.20f, 0.20f, 5.8f)));
        d = fmaxf(d, -sdBox(V(fr - 0.02f, p.y - 1.725f, lz), V(0.14f, 1.225f, 4.9f)));
    }
    if (CPX < 1e8f && p.x > 0 && roundf(p.z / CL) == 1 && fr < 1.95f) {       // la charcuterie du gros plan : une vraie boutique derrière la vitre
        d = fmaxf(d, -sdBox(V(fr - 0.85f, p.y - 1.725f, lz + 0.8f), V(0.85f, 1.225f, 4.1f)));
        if (fr > -0.05f && fr < 1.75f && p.y < 3.0f && lz > -4.95f && lz < 3.35f) { int mm; float g = shopgoods(lz, p.y, fr, &mm); if (g < d) { d = g; *m = mm; } }
        UN(sdBox(V(fr - 0.03f, p.y - 1.725f, lz + 0.8f - 2.05f * roundf((lz + 0.8f) / 2.05f)), V(0.035f, 1.225f, 0.03f)), M_LPOST) }
    UN(fmaxf(fmaxf((fabsf(bx) * rh + (p.y - H) * 6.9f - rh * 6.9f) / sqrtf(rh * rh + 47.6f), H - 0.05f - p.y), fabsf(lz) - 5.85f), M_ROOF)
    UN(sdBox(V(fabsf(bx) - 2.6f, p.y - H - rh * 0.62f - 0.9f, fabsf(lz) - 3.2f), V(0.32f, 1.0f, 0.32f)), M_CHIM)
    UN(sdBox(V(bx + 7.15f, p.y - 3.35f, lz), V(0.65f, 0.06f, 4.6f)), M_AWN)                                    // the canopy over the shop
    if (fract(h * 11.3f) < 0.62f) UN(sdBox(V(bx + 7.25f, p.y - 5.4f, lz - 5.0f), V(0.55f, 1.0f, 0.06f)), M_BLADE)   // a sign hung out over the pavement
    return fminf(d, wall + 0.12f);
}
#define BS 24.0f
static int blockcell(float px, float pz, float *c, float *h) {       // the rest of the town: c = cx, cz, wx, wz, H, rise, stack
    float ix = roundf(px / BS), iz = roundf(pz / BS); *h = hash2(ix * 1.9f + 4.1f, iz * 1.3f + 8.3f);
    c[0] = ix * BS; c[1] = iz * BS; c[6] = 0;
    if (*h > 0.92f || (fabsf(c[0]) < 36 && c[1] < 135 && c[1] > -250) || (fabsf(c[0]) < 60 && c[1] > 50 && c[1] < 135)) return 0;
    float far = clampf(len2(c[0], c[1] + 40.0f) / 520.0f, 0, 1), dst = vnoise(ix * 0.23f + 3.1f, iz * 0.23f + 1.7f);       // les quartiers
    int title = fabsf(c[1] + 70.0f) < 40.0f && c[0] > -50 && c[0] < 100, tall = fract(*h * 41.3f) > 0.95f && far < 0.8f && !title;
    c[2] = 7.0f + 2.0f * fract(*h * 5.1f); c[3] = 7.0f + 2.0f * fract(*h * 9.7f);
    c[4] = 7.0f + 8.0f * (1 - far) * (0.3f + 0.7f * dst) + 7.0f * fract(*h * 17.9f) * (1 - 0.45f * far) + (tall ? 12.0f : 0.0f);
    c[5] = 2.5f + 2.0f * fract(*h * 31.1f);
    if (fract(*h * 57.7f) > 0.955f && !title && !tall && far > 0.12f) c[6] = c[4] + 15.0f;      // une cheminée de fumoir
    return 1;
}
static float blocks(v3 p, int *m) {
    *m = M_BLOCK;
    float db = fmaxf(fmaxf(fabsf(p.x) - 900.0f, fmaxf(p.z - 700.0f, -1300.0f - p.z)), p.y - 50.0f); if (db > 1.0f) return db;
    float c[7], h, d = 1e9f;
    if (blockcell(p.x, p.z, c, &h)) {
        v3 q = V(p.x - c[0], p.y, p.z - c[1]);
        d = sdBox(V(q.x, q.y - c[4] * 0.5f, q.z), V(c[2], c[4] * 0.5f, c[3]));
        float ow = c[2] + 0.25f;
        d = fminf(d, fmaxf(fmaxf((fabsf(q.x) * c[5] + (q.y - c[4]) * ow - c[5] * ow) / sqrtf(c[5] * c[5] + ow * ow), c[4] - 0.05f - q.y), fabsf(q.z) - c[3] - 0.2f));
        if (c[6] > 0) UN(fmaxf((len2(q.x - 4.5f, q.z + 4.0f) - 1.5f + 0.75f * q.y / c[6]) * 0.97f, q.y - c[6]), M_STACK)
    }
    return fminf(d, BS * 0.5f - fmaxf(fabsf(p.x - roundf(p.x / BS) * BS), fabsf(p.z - roundf(p.z / BS) * BS)) + 2.5f);
}
static float street(v3 p, int *m) {         // lamp posts, the wires of the tram
    float ax = fabsf(p.x), d = 1e9f; *m = M_LPOST;
    if (p.z > SQ0 + 1 || p.z < -237.0f || ax > 8.0f || p.y > 7.2f) return fmaxf(fmaxf(fmaxf(p.z - SQ0 - 0.5f, -236.5f - p.z), ax - 7.5f), p.y - 6.7f);
    float zl = p.z - (roundf(p.z / CL - 0.5f) + 0.5f) * CL;
    d = cylY(V(ax - 6.6f, p.y - 2.6f, zl), 0.07f, 2.6f);
    d = fminf(d, sdCap(V(ax, p.y, zl), V(6.6f, 5.2f, 0), V(5.9f, 5.55f, 0), 0.05f));
    UN(len(V(ax - 5.9f, p.y - 5.35f, zl)) - 0.22f, M_LBULB)
    float zs = p.z - (roundf((p.z - 6.0f) / 24.0f) * 24.0f + 6.0f);
    UN(fmaxf(len2(p.y - 6.0f, zs) - 0.03f, ax - 6.6f), M_WIRE)
    UN(len2(p.x, p.y - 5.75f) - 0.03f, M_WIRE)
    return fminf(d, CL * 0.5f - fabsf(zl) + 5.6f);
}
static float tram(v3 p, int *m) {
    v3 q = V(p.x, p.y, p.z - TRZ); *m = M_TRAM;
    float bd = len(V(q.x, q.y - 2.5f, fmaxf(fabsf(q.z) - 7.0f, 0))) - 4.2f; if (bd > 1.0f) return bd;
    float d = sdBox(sub(q, V(0, 1.95f, 0)), V(1.05f, 1.25f, 6.6f)) - 0.22f;
    d = fminf(d, sdBox(sub(q, V(0, 3.55f, 0)), V(0.8f, 0.12f, 4.5f)));
    UN(fminf(sdCap(q, V(0, 3.65f, 1.0f), V(0, 4.75f, 2.1f), 0.04f), sdCap(q, V(0, 4.75f, 2.1f), V(0, 5.70f, 1.0f), 0.04f)), M_LPOST)
    UN(sdBox(sub(q, V(0, 5.70f, 1.0f)), V(0.55f, 0.03f, 0.05f)), M_LPOST)
    UN(sdBox(sub(q, V(0, 0.42f, 0)), V(1.0f, 0.32f, 6.3f)), M_LPOST)
    return d;
}
static float cpig(v3 p, int *m) {           // the crowned pig, the monument of the nation (faces +z, feet on y = 0, about 2.6 long)
    *m = M_CHROME;
    float bd = len(sub(p, V(0, 0.95f, 0.25f))) - 1.8f; if (bd > 0.9f) return bd;
    p.x = fabsf(p.x);
    float d = sdEll(sub(p, V(0, 0.9f, 0)), V(0.62f, 0.6f, 0.95f));
    d = smin(d, len(sub(p, V(0, 1.05f, 0.95f))) - 0.48f, 0.15f);
    d = smin(d, cylZ(sub(p, V(0, 0.98f, 1.42f)), 0.19f, 0.11f) - 0.035f, 0.06f);
    d = fmaxf(d, -(len(sub(p, V(0.075f, 0.99f, 1.585f))) - 0.045f));
    v3 q = sub(p, V(0.31f, 1.5f, 0.8f));
    d = smin(d, sdEll(V(0.8776f * q.x + 0.4794f * q.y, -0.4794f * q.x + 0.8776f * q.y, q.z), V(0.13f, 0.23f, 0.06f)), 0.06f);
    d = smin(d, sdCap(p, V(0.32f, 0.6f, 0.55f), V(0.32f, 0.14f, 0.55f), 0.14f), 0.08f);
    d = smin(d, sdCap(p, V(0.32f, 0.6f, -0.55f), V(0.32f, 0.14f, -0.55f), 0.14f), 0.08f);
    q = sub(p, V(0, 1.12f, -0.99f));
    d = fminf(d, len2(len2(q.y, q.z) - 0.12f, q.x) - 0.035f);
    q = sub(p, V(0, 1.53f, 0.9f));
    if (len(q) < 0.7f) {    // the crown
        float r = len2(q.x, q.z), cr = box2(fabsf(r - 0.24f) - 0.02f, fabsf(q.y) - 0.07f) - 0.008f;
        float sec = 6.2831853f / 5, a = atan2f(q.z, q.x) - 1.5707963f; a -= sec * roundf(a / sec);
        float lx = r * cosf(a) - 0.24f, lz = r * sinf(a), ly = q.y - 0.07f;
        cr = fminf(cr, fminf(fmaxf(len2(lx, lz) - 0.075f * (1 - ly / 0.2f), fmaxf(-ly, ly - 0.19f)) * 0.8f, len(V(lx, ly - 0.2f, lz)) - 0.035f));
        UN(cr, M_GOLD)
    }
    return d;
}
static float sqpeople(v3 p, int *m);
static float square(v3 p, int *m) {         // the statue and its ring of chrome balls, the Ministry, the Senate, the Halles
    *m = M_PED;
    if (p.z < SQ0 - 6) return SQ0 - 5.0f - p.z;
    int mm; float d = 1e9f, b;
    v3 q = V(p.x, p.y, p.z - 85.0f); float r = len2(q.x, q.z);
    if (r < 13) {
        d = fminf(fminf(sdBox(sub(q, V(0, 0.2f, 0)), V(3.5f, 0.2f, 4.9f)), sdBox(sub(q, V(0, 0.55f, 0)), V(2.9f, 0.15f, 4.3f))),          // le socle : un emmarchement, une plinthe, le dé, une corniche
                  fminf(sdBox(sub(q, V(0, 1.42f, 0)), V(2.55f, 0.73f, 3.95f)), sdBox(sub(q, V(0, 2.3f, 0)), V(2.85f, 0.15f, 4.25f))));
        b = cpig(V(-q.x / 2.7f, (q.y - 2.45f) / 2.7f, -q.z / 2.7f), &mm) * 2.7f; if (b < d) { d = b; *m = mm; }
        float sec = 6.2831853f / 12, a = atan2f(q.z, q.x); a -= sec * roundf(a / sec);
        UN(len(V(r * cosf(a) - 10.0f, q.y - 0.62f, r * sinf(a))) - 0.5f, M_CHROME)
        UN(cylY(V(r * cosf(a) - 10.0f, q.y - 0.07f, r * sinf(a)), 0.34f, 0.07f), M_PED)
    } else d = r - 12.2f;
    {   // four candelabra
        v3 c = V(fabsf(q.x) - 20.0f, q.y, fabsf(q.z) - 15.0f);
        UN(fminf(cylY(V(c.x, c.y - 3.4f, c.z), 0.10f, 3.4f), cylY(V(c.x, c.y - 0.35f, c.z), 0.26f, 0.35f)), M_LPOST)
        UN(fminf(len(V(c.x, c.y - 7.2f, c.z)) - 0.34f, len(V(len2(c.x, c.z) - 0.55f, c.y - 6.5f, 0)) - 0.24f), M_LBULB)
    }
    if (p.z > 103) {        // the Ministry of Pork
        float ax = fabsf(p.x);
        UN(sdBox(sub(p, V(0, 7.4f, 124.0f)), V(30.0f, 7.4f, 9.0f)), M_MIN)                                                 // le corps et ses deux ailes
        {   float xr = p.x - 5.5f * roundf(p.x / 5.5f), wn;                                                                    // les baies en creux : sept sous le portique (la porte au milieu), deux par étage sur chaque aile
            if (ax < 19.3f) wn = ax < 2.75f ? box2(ax - 1.7f, fabsf(p.y - 4.6f) - 3.7f) : box2(fabsf(xr) - 1.25f, fabsf(p.y - 5.9f) - 3.7f);
            else wn = box2(fabsf(fabsf(ax - 26.25f) - 1.85f) - 0.95f, fminf(fabsf(p.y - 4.2f) - 2.3f, fabsf(p.y - 10.4f) - 2.0f));
            d = fmaxf(d, -fmaxf(wn, fabsf(p.z - 115.0f) - 0.32f)); }
        UN(fminf(fminf(sdBox(sub(p, V(0, 0.15f, 111.3f)), V(24.9f, 0.15f, 3.9f)), sdBox(sub(p, V(0, 0.45f, 111.7f)), V(24.3f, 0.15f, 3.5f))), sdBox(sub(p, V(0, 0.75f, 112.1f)), V(23.7f, 0.15f, 3.1f))), M_PED)   // trois marches
        UN(sdBox(sub(p, V(0, 13.5f, 112.85f)), V(22.5f, 1.3f, 2.35f)), M_MIN)                                               // l'entablement
        {   float pr = fmaxf(fmaxf((ax * 5.5f + (p.y - 14.8f) * 22.5f - 123.75f) / 23.16f, 14.8f - p.y), fabsf(p.z - 112.85f) - 2.35f);      // le fronton, son tympan en creux
            float ty = fmaxf(fmaxf((ax * 5.5f + (p.y - 14.8f) * 22.5f - 111.0f) / 23.16f, 15.3f - p.y), fabsf(p.z - 110.5f) - 0.3f);
            UN(fmaxf(pr, -ty), M_MIN) }
        UN(sdBox(sub(p, V(0, 14.35f, 124.0f)), V(30.35f, 0.45f, 9.35f)), M_MIN)                                              // la corniche, qui fait le tour
        UN(sdBox(sub(p, V(0, 16.0f, 125.0f)), V(26.0f, 1.2f, 7.0f)), M_MIN)                                                  // l'attique
        {   float cx = (clampf(floorf(p.x / 5.5f), -4, 3) + 0.5f) * 5.5f; v3 c = V(p.x - cx, p.y, p.z - 111.4f);                // huit colonnes : la base, le fût, le chapiteau
            UN(cylY(V(c.x, c.y - 6.45f, c.z), 0.70f, 5.2f), M_COL)
            UN(fminf(cylY(V(c.x, c.y - 1.08f, c.z), 0.92f, 0.18f), sdBox(V(c.x, c.y - 11.92f, c.z), V(0.95f, 0.28f, 0.95f))), M_MIN) }
        UN(cylY(sub(p, V(0, 17.6f, 124.0f)), 8.4f, 1.6f), M_MIN)                                                             // le tambour, le dôme, la lanterne d'or
        UN(fmaxf(len(sub(p, V(0, 19.2f, 124.0f))) - 8.0f, 19.2f - p.y), M_DOME)
        UN(fminf(cylY(sub(p, V(0, 27.9f, 124.0f)), 0.6f, 0.9f), len(sub(p, V(0, 29.4f, 124.0f))) - 0.75f), M_GOLD)
    } else d = fminf(d, 104.0f - p.z);
    if (p.x < -30) {        // the Senate of the Tavern-keepers : une grande taverne, la pierre en bas, les pans de bois au-dessus
        v3 s = sub(p, V(-43.0f, 0, 85.0f)); float azs = fabsf(s.z);
        float bd = sdBox(V(s.x, s.y - 2.2f, s.z), V(9.25f, 2.2f, 21.25f));
        {   float k = roundf(s.z / 6.0f), zr = s.z - 6.0f * k;                                                                 // la porte, et trois fenêtres en plein cintre de chaque côté
            float op = k == 0 ? fminf(box2(fabsf(zr) - 1.3f, fabsf(s.y - 1.25f) - 1.25f), len2(zr, s.y - 2.5f) - 1.3f) : fminf(box2(fabsf(zr) - 1.3f, fabsf(s.y - 2.0f) - 0.75f), len2(zr, s.y - 2.75f) - 1.3f);
            if (fabsf(k) <= 3) bd = fmaxf(bd, -fmaxf(op, fabsf(s.x - 9.25f) - (k == 0 ? 0.45f : 0.28f))); }
        UN(bd, M_SENAT)
        UN(sdBox(V(s.x, s.y - 8.3f, s.z), V(9.45f, 3.9f, 21.45f)), M_SENAT)                                                  // les étages, en encorbellement
        UN(fmaxf(fmaxf((fabsf(s.x) * 7.5f + (s.y - 12.2f) * 10.1f - 75.75f) / 12.58f, 12.15f - s.y), azs - 21.9f), M_ROOF)    // le grand toit
        {   float zr = s.z - 8.0f * roundf(s.z / 8.0f);                                                                        // cinq lucarnes
            if (azs < 20.0f && s.x > 0) { UN(sdBox(V(s.x - 6.3f, s.y - 14.3f, zr), V(1.7f, 1.0f, 1.05f)), M_SENAT)
                UN(fmaxf(fmaxf((fabsf(zr) * 0.9f + (s.y - 15.3f) * 1.3f - 1.17f) / 1.581f, 15.25f - s.y), fabsf(s.x - 6.2f) - 2.0f), M_ROOF) } }
        UN(sdBox(V(s.x, s.y - 20.4f, azs - 13.0f), V(0.75f, 1.7f, 0.75f)), M_CHIM)                                             // deux souches
        UN(len(V(s.x - 9.75f, s.y - 3.35f, azs - 2.1f)) - 0.17f, M_LBULB)                                                      // deux lanternes à la porte
        UN(sdCap(V(s.x, s.y, azs), V(9.3f, 3.6f, 2.1f), V(9.75f, 3.6f, 2.1f), 0.03f), M_LPOST)
    } else d = fminf(d, p.x + 31.0f);
    if (p.x > 30) {         // the Halles, iron and glass
        v3 s = sub(p, V(43.0f, 0, 85.0f)); float axs = fabsf(s.x), azs = fabsf(s.z), rv = len2(s.x, s.y - 10.0f);
        UN(sdBox(V(s.x, s.y - 5.0f, s.z), V(9.0f, 5.0f, 21.0f)), M_HGLASS)                                                    // les murs de verre, la voûte
        UN(fmaxf(fmaxf(rv - 9.0f, 10.0f - s.y), azs - 21.0f), M_HGLASS)
        UN(sdBox(V(s.x, s.y - 0.55f, s.z), V(9.2f, 0.55f, 21.2f)), M_HALLES)                                                  // le soubassement de brique
        {   float zr = s.z - 6.0f * roundf(s.z / 6.0f), xr = s.x - 4.5f * roundf(s.x / 4.5f), z3 = s.z - 3.0f * roundf(s.z / 3.0f);   // la charpente de fer : poteaux, sablière, traverse, arcs
            float ir = fminf(sdBox(V(axs - 9.08f, s.y - 5.0f, zr), V(0.20f, 5.0f, 0.20f)), sdBox(V(xr, s.y - 5.0f, azs - 21.08f), V(0.20f, 5.0f, 0.20f)));
            ir = fminf(ir, fminf(sdBox(V(axs - 9.12f, s.y - 10.0f, s.z), V(0.30f, 0.38f, 21.4f)), sdBox(V(s.x, s.y - 10.0f, azs - 21.12f), V(9.4f, 0.38f, 0.30f))));
            ir = fminf(ir, fminf(sdBox(V(axs - 9.05f, s.y - 5.6f, s.z), V(0.12f, 0.14f, 21.1f)), sdBox(V(s.x, s.y - 5.6f, azs - 21.05f), V(9.1f, 0.14f, 0.12f))));
            ir = fminf(ir, fmaxf(fmaxf(fabsf(rv - 9.06f) - 0.13f, fabsf(z3) - 0.11f), 10.0f - s.y));
            ir = fminf(ir, fmaxf(fmaxf(fabsf(rv - 9.06f) - 0.17f, fabsf(azs - 21.0f) - 0.17f), 10.0f - s.y));
            ir = fminf(ir, sdBox(V(s.x, s.y - 19.5f, s.z), V(1.5f, 0.55f, 19.0f)));                                               // le lanterneau du faîte
            UN(ir, M_HALLES) }
        UN(fmaxf(fmaxf((axs * 0.8f + (s.y - 20.05f) * 1.9f - 1.52f) / 2.062f, 20.0f - s.y), azs - 19.3f), M_ROOF)
        {   float pd = sdBox(V(s.x + 9.9f, s.y - 3.7f, s.z), V(0.9f, 3.7f, 3.6f));                                                // le porche, côté place, et l'enseigne au-dessus
            pd = fmaxf(pd, -fmaxf(fminf(box2(azs - 2.6f, fabsf(s.y - 1.8f) - 1.8f), len2(s.z, s.y - 3.6f) - 2.6f), fabsf(s.x + 9.9f) - 1.2f));
            UN(pd, M_HALLES)
            UN(sdBox(V(s.x + 9.36f, s.y - 8.6f, s.z), V(0.14f, 1.45f, 12.0f)), M_HSIGN) }
    } else d = fminf(d, 31.0f - p.x);
    b = sqpeople(p, &mm); if (b < d) { d = b; *m = mm; }
    return d;
}
#define GZ 55.4f
#define GY 23.6f
static float tower(v3 q) {      // un pylône en treillis (q.y depuis le sol), 1,3 m de côté : quatre montants, une ceinture tous les 2,2 m, des croisillons
    float ax = fabsf(q.x), az = fabsf(q.z), d = len2(ax - 0.65f, az - 0.65f) - 0.075f;
    float ky = roundf(q.y / 2.2f), yy = q.y - 2.2f * ky, sg = ((int)ky & 1) ? 1.0f : -1.0f;
    d = fminf(d, fminf(fmaxf(len2(yy, az - 0.65f) - 0.04f, ax - 0.68f), fmaxf(len2(yy, ax - 0.65f) - 0.04f, az - 0.68f)));
    float y2 = q.y - 1.1f - 2.2f * floorf(q.y / 2.2f);
    d = fminf(d, fmaxf(len2(fabsf(q.x * sg * 2.2f - y2 * 1.3f) / 2.555f, az - 0.65f) - 0.035f, ax - 0.68f));
    return fminf(d, fmaxf(len2(fabsf(q.z * sg * 2.2f - y2 * 1.3f) / 2.555f, ax - 0.65f) - 0.035f, az - 0.68f));
}
static float bigsign(v3 p, int *m) {        // l'enseigne ne flotte plus : deux pylônes au débouché de l'avenue, une poutre en treillis, le panneau dessus
    *m = M_STEEL;
    float ax = fabsf(p.x), zz = p.z - GZ - 0.55f, bd = fmaxf(fmaxf(ax - 13.0f, fabsf(p.z - GZ) - 2.4f), p.y - 28.6f); if (bd > 1.0f) return bd;
    float d = fmaxf(tower(V(ax - 11.4f, p.y, zz)), p.y - 26.6f);
    {   float xr = p.x - 2.6f * roundf(p.x / 2.6f), sg = ((int)roundf(p.x / 2.6f) & 1) ? 1.0f : -1.0f, in = fmaxf(fabsf(p.y - 23.6f) - 2.7f, ax - 11.4f);
        d = fminf(d, fmaxf(fminf(len2(p.y - 20.9f, zz), len2(p.y - 26.3f, zz)) - 0.09f, ax - 11.4f));                 // les deux membrures
        d = fminf(d, fmaxf(len2(xr, zz) - 0.05f, in));                                                                // les montants
        d = fminf(d, fmaxf(len2(fabsf((xr - 1.3f * sg) * sg * 5.4f - (p.y - 23.6f) * 2.6f) / 5.993f, zz) - 0.04f, in)); }      // les diagonales
    UN(len(V(ax - 11.4f, p.y - 27.2f, zz)) - 0.42f, M_LBULB)                                                           // un globe au sommet de chaque pylône
    UN(sdBox(sub(p, V(0, GY, GZ)), V(10.4f, 2.3f, 0.16f)), M_NSIGN)
    return d;
}
static float title3d(v3 p, int *m) {        // DOUZI CITY, in gold, hanging over the town; the camera goes through its O
    *m = M_TITLE;
    float bz = fabsf(p.z - TZ) - 1.1f; if (bz > 2.0f || p.y < TYB - 6 || p.y > TYB + 18) return fmaxf(bz, fmaxf(TYB - 5.0f - p.y, p.y - TYB - 17.0f)) + 0.5f;
    float d2 = text2d(T_CITY, (p.x - TXC) / TSF + TR[T_CITY].w * 0.5f, 134.0f - (p.y - TYB) / TSF) * TSF;
    return fmaxf(d2, bz) * 0.9f - 0.06f;
}
// ---- les habitants : le citoyen standard (citizen.h), en tenue d'hiver. Douze silhouettes pour les trottoirs, deux pour la vitrine.
#define NCZ 12
#define NSQ 5
static cz_t CZA[NCZ + 2 + NSQ]; static pose_t CZP[NCZ + 2 + NSQ]; static rig_t CZR[NCZ + 2 + NSQ]; static float CZPER[NCZ + 2 + NSQ]; static int CZINIT;
static float SQX[NSQ], SQZ[NSQ], SQY[NSQ], SQC[NSQ], SQS[NSQ];       // ceux de la place : où ils sont, le sol sous leurs sabots, vers où ils regardent
static __thread int NOCZ;                                             // 1 : on ne voit pas les habitants (le reflet dans la vitrine)
static const float HERO[2][2] = {{8.55f, 14.00f}, {9.12f, 12.25f}};
static void people_init(void) {
    const v3 IV = V(0.86f, 0.83f, 0.75f), WH = V(0.90f, 0.90f, 0.88f), BX = V(0.33f, 0.03f, 0.07f), NAVY = V(0.035f, 0.055f, 0.17f), BLK = V(0.035f, 0.035f, 0.045f),
             BRN = V(0.15f, 0.09f, 0.06f), GRN = V(0.04f, 0.17f, 0.10f), MUS = V(0.80f, 0.48f, 0.08f), GRY = V(0.16f, 0.17f, 0.19f), TEAL = V(0.04f, 0.19f, 0.21f);
    for (int k = 0; k < NCZ + 2 + NSQ; k++) cz_base(&CZA[k]);
    cz_t *C;
    C = &CZA[0]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_SCARF | F_BUTTONS; C->hat = HAT_BERET; C->cjack = BRN; C->ctop = IV; C->cbot = BLK; C->chat = BX; C->cacc = BX; C->chat2 = IV;
    C = &CZA[1]; C->flags = F_DRESS | F_JACKET | F_SCARF | F_GLASSES | F_LASH; C->top = TOP_SHIRT; C->pdress = PAT_DOTS; C->cdress = BX; C->cdress2 = IV; C->cjack = NAVY; C->ctop = IV; C->cacc = MUS; C->chat2 = IV; C->hat = HAT_BERET; C->chat = TEAL; C->eye = 0.5f; C->fat = -0.3f;
    C = &CZA[2]; C->top = TOP_SHIRT; C->ptop = PAT_HSTRIPE; C->ctop = IV; C->ctop2 = NAVY; C->flags = F_BIB | F_SCARF; C->cbot = V(0.07f, 0.16f, 0.42f); C->hat = HAT_CAP; C->chat = V(0.16f, 0.13f, 0.10f); C->cacc = GRY; C->chat2 = IV; C->leg = 0.3f;
    C = &CZA[3]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_TIE | F_SASH | F_STACHE | F_BUTTONS; C->hat = HAT_TOP; C->prop = PROP_CANE; C->ctop = WH; C->cbot = BLK; C->cjack = BLK; C->chat = BLK; C->chat2 = BX; C->cacc = BX; C->fat = 1.0f; C->snout = 0.5f;
    C = &CZA[4]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_JACKET | F_BOW | F_BUTTONS | F_SCARF; C->hat = HAT_BOWLER; C->ctop = IV; C->cjack = GRN; C->cbot = BRN; C->chat = V(0.10f, 0.07f, 0.05f); C->chat2 = MUS; C->cacc = MUS; C->fat = 0.5f; C->ear = 0.5f;
    C = &CZA[5]; C->sc = 0.70f; C->head = 1.0f; C->leg = -0.6f; C->eye = 0.7f; C->top = TOP_SHIRT; C->flags = F_PANTS | F_SCARF; C->hat = HAT_BEANIE; C->ctop = MUS; C->cbot = NAVY; C->chat = BX; C->chat2 = IV; C->cacc = BX; C->skin = V(0.96f, 0.42f, 0.42f);
    C = &CZA[6]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_JACKET | F_BUTTONS | F_STACHE | F_CLOSED; C->hat = HAT_KEPI; C->ctop = NAVY; C->cjack = NAVY; C->cbot = NAVY; C->chat = NAVY; C->chat2 = BX; C->leg = 0.5f; C->fat = -0.2f;
    C = &CZA[7]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_JACKET | F_BUTTONS | F_BOW | F_CLOSED; C->hat = HAT_TOQUE; C->ctop = WH; C->cjack = WH; C->chat = WH; C->cbot = GRY; C->pbot = PAT_CHECK; C->cacc = BX; C->fat = 0.4f;
    C = &CZA[8]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_SCARF | F_LASH | F_BUTTONS; C->hat = HAT_BEANIE; C->cjack = BX; C->ctop = IV; C->cbot = BLK; C->chat = IV; C->chat2 = BX; C->cacc = IV; C->eye = 0.4f; C->fat = -0.2f; C->skin = V(0.94f, 0.40f, 0.40f);
    C = &CZA[9]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_APRON | F_STACHE | F_BOW | F_SCARF; C->hat = HAT_BOATER; C->papron = PAT_VSTRIPE; C->ctop = WH; C->cbot = BLK; C->chat = V(0.82f, 0.64f, 0.32f); C->chat2 = BX; C->cacc = BX; C->fat = 0.7f;
    C = &CZA[10]; C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_SCARF | F_GLASSES | F_BUTTONS; C->hat = HAT_CAP; C->cjack = GRY; C->ctop = IV; C->cbot = BLK; C->chat = GRY; C->cacc = NAVY; C->chat2 = IV; C->fat = 0.3f; C->leg = 0.4f;
    C = &CZA[11]; C->sc = 0.76f; C->head = 0.8f; C->leg = -0.4f; C->eye = 0.6f; C->top = TOP_SHIRT; C->flags = F_PANTS | F_JACKET | F_SCARF | F_BUTTONS; C->hat = HAT_BERET; C->cjack = MUS; C->ctop = IV; C->cbot = BRN; C->chat = NAVY; C->cacc = IV; C->chat2 = NAVY;
    CZA[NCZ] = CZA[8]; CZA[NCZ].hat = HAT_BERET; CZA[NCZ].chat = TEAL; CZA[NCZ].flags |= F_GLASSES;                       // devant la vitrine : une dame et un petit
    CZA[NCZ + 1] = CZA[5];
    CZA[NCZ + 2] = CZA[6]; CZA[NCZ + 3] = CZA[6]; CZA[NCZ + 3].fat = 0.5f; CZA[NCZ + 3].leg = 0.1f;                     // sur la place : deux gardes, deux curieux, un sénateur
    CZA[NCZ + 4] = CZA[10]; CZA[NCZ + 5] = CZA[11]; CZA[NCZ + 6] = CZA[3];
    for (int k = 0; k < NCZ; k++) CZPER[k] = 0.7556f * CZA[k].sc;        // la cadence qui va avec 0,9 m/s : le pied posé ne glisse pas
    CZINIT = 1;
}
static void people_pose(float s) {      // à chaque trame : les douze marcheurs, et les deux de la vitrine
    if (!CZINIT) people_init();
    for (int k = 0; k < NCZ; k++) { pose_t *P = &CZP[k]; cz_rest(&CZA[k], P);
        if (CZA[k].prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.12f);
        cz_walk(P, 6.2832f * T / CZPER[k] + 2.4f * k, 1); P->hyaw += 0.18f * sinf(0.6f * T + k * 1.7f); P->looky = 0;
        float u = fract((T + k * 0.77f) / 3.3f) * 3.3f; if (u < 0.16f) P->blink = sinf(PI * u / 0.16f);
        cz_rig(&CZA[k], P, &CZR[k]); }
    {   pose_t *P = &CZP[NCZ]; cz_rest(&CZA[NCZ], P); cz_idle(P, T, 0.21f);                                          // la dame montre un jambon et se tourne vers le petit
        P->hand[1] = V(-0.30f, 0.44f + 0.015f * sinf(T * 2.1f), 0.34f); P->hyaw = 0.45f * ease((s - 29.6f) / 0.8f) - 0.15f; P->hpitch = 0.10f; P->smile = 0.9f; P->open = 0.25f + 0.2f * sinf(T * 9.0f) * (s > 30.2f);
        cz_rig(&CZA[NCZ], P, &CZR[NCZ]); }
    {   pose_t *P = &CZP[NCZ + 1]; cz_rest(&CZA[NCZ + 1], P); float hop = fabsf(sinf(T * 3.4f));                       // le petit, sur la pointe des sabots, les mains vers la vitre
        P->hand[0] = V(0.17f, 0.50f, 0.30f); P->hand[1] = V(-0.17f, 0.50f + 0.02f * hop, 0.30f); P->hpitch = -0.26f; P->open = 0.8f; P->smile = 0.4f; P->brow = 1.0f; P->looky = 0.5f;
        P->bob = 0.035f + 0.02f * hop; P->foot[0].y += 0.035f + 0.02f * hop; P->foot[1].y += 0.035f + 0.02f * hop; P->footp[0] = P->footp[1] = 0.42f; P->lean = 0.06f;
        P->earflop[0] += 0.1f * hop; P->earflop[1] += 0.1f * hop; P->tail = 0.6f * sinf(T * 14);
        cz_rig(&CZA[NCZ + 1], P, &CZR[NCZ + 1]); }
    for (int k = 0; k < 2; k++) { int i = NCZ + 2 + k; pose_t *P = &CZP[i]; cz_rest(&CZA[i], P); cz_idle(P, T, 0.37f + 0.4f * k); P->smile = 0.25f; P->hyaw *= 0.6f;      // les gardes, au pied des colonnes
        SQX[k] = k ? 3.4f : -3.4f; SQZ[k] = 109.9f; SQY[k] = 0.9f; SQC[k] = -1; SQS[k] = 0; cz_rig(&CZA[i], P, &CZR[i]); }
    {   int i = NCZ + 4; pose_t *P = &CZP[i]; cz_rest(&CZA[i], P); cz_idle(P, T, 0.63f); P->hpitch -= 0.30f; P->looky = 0.5f; P->smile = 0.8f;                      // deux curieux lèvent le nez vers la statue
        SQX[2] = 2.6f; SQZ[2] = 77.6f; SQY[2] = 0; SQC[2] = cosf(-0.34f); SQS[2] = sinf(-0.34f); cz_rig(&CZA[i], P, &CZR[i]); }
    {   int i = NCZ + 5; pose_t *P = &CZP[i]; cz_rest(&CZA[i], P); cz_idle(P, T, 0.11f); P->hpitch -= 0.34f; P->looky = 0.6f; P->open = 0.6f; P->brow = 1.0f;
        P->hand[1] = V(-0.30f, 0.80f + 0.02f * sinf(T * 2.3f), 0.40f);                                                                                              // le petit montre le porc du doigt
        SQX[3] = 3.9f; SQZ[3] = 78.3f; SQY[3] = 0; SQC[3] = cosf(-0.47f); SQS[3] = sinf(-0.47f); cz_rig(&CZA[i], P, &CZR[i]); }
    {   int i = NCZ + 6; pose_t *P = &CZP[i]; cz_rest(&CZA[i], P); float L = clampf(0.9f * (s - 14.0f), 0, 30.0f), a = atan2f(-0.903f, -0.43f);                     // un sénateur traverse la place vers son Sénat
        if (CZA[i].prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.12f);
        cz_walk(P, 6.2832f * T / 0.7556f, 1); P->looky = 0;
        SQX[4] = -6.0f - 0.903f * L; SQZ[4] = 97.0f - 0.43f * L; SQY[4] = 0; SQC[4] = cosf(a); SQS[4] = sinf(a); cz_rig(&CZA[i], P, &CZR[i]); }
}
S v3 sq_local(v3 p, int k) { float sc = CZA[NCZ + 2 + k].sc, dx = p.x - SQX[k], dz = p.z - SQZ[k]; return V((dx * SQC[k] - dz * SQS[k]) / sc, (p.y - SQY[k]) / sc, (dx * SQS[k] + dz * SQC[k]) / sc); }
static float sqpeople(v3 p, int *m) {
    float d = 1e9f; int mm; *m = C_SKIN; if (NOCZ) return d;
    for (int k = 0; k < NSQ; k++) { int i = NCZ + 2 + k; float sc = CZA[i].sc; v3 l = sq_local(p, k); float bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc;
        float e = bd > 0.65f ? bd : cz_sdf(&CZA[i], &CZR[i], &CZP[i], l, &mm) * sc; if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
    return d;
}
S v3 hero_local(v3 p, int k) { float sc = CZA[NCZ + k].sc; return V(-(p.z - HERO[k][1]) / sc, (p.y - 0.15f) / sc, (p.x - HERO[k][0]) / sc); }
S int walkcell(v3 p, v3 *l, float *zl) {  // les lève-tôt sur les trottoirs : le numéro de leur silhouette (-1 : personne), et le point dans leur repère
    float sg = p.x > 0 ? 1.0f : -1.0f, zz = p.z - sg * 0.9f * T, iz = roundf(zz / 9.0f), id = hash2(iz * 1.3f + 0.7f, sg * 2.1f + 4.4f);
    int k = (int)(fract(id * 11.3f) * (NCZ - 0.001f)); float sc = CZA[k].sc, zc = iz * 9.0f + sg * 0.9f * T; *zl = zz - iz * 9.0f;
    *l = V(sg * (p.x - sg * (7.95f + 0.55f * fract(id * 3.1f))) / sc, (p.y - 0.15f) / sc, sg * *zl / sc);
    if (CPX < 1e8f && p.x > 0 && fabsf(zc - 12.9f) < 3.5f) return -1;        // on laisse la place aux deux de la vitrine
    return (fract(id * 5.7f) < 0.66f && zc < SQ0 - 8 && zc > -150) ? k : -1;
}
static int city_cz(v3 p, v3 *l) {         // à qui est ce point : le numéro de l'habitant, et le point dans son repère
    if (CPX < 1e8f) for (int k = 0; k < 2; k++) if (len2(p.x - HERO[k][0], p.z - HERO[k][1]) < 0.55f) { *l = hero_local(p, k); return NCZ + k; }
    if (p.z > SQ0 - 4) { int kb = 0; float db = 1e9f; for (int k = 0; k < NSQ; k++) { float e = len2(p.x - SQX[k], p.z - SQZ[k]); if (e < db) { db = e; kb = k; } } *l = sq_local(p, kb); return NCZ + 2 + kb; }
    float zl; int k = walkcell(p, l, &zl); return k < 0 ? 0 : k;
}
static float cguest(v3 p, v3 *l) { *l = p; return 0.5f; }
static float walkers(v3 p, int *m) {
    float ax = fabsf(p.x); *m = C_SKIN; if (NOCZ) return 1e9f;
    if (ax < 7.2f || ax > 9.6f || p.y > 2.6f) return fmaxf(fmaxf(7.35f - ax, ax - 9.1f), p.y - 2.3f);
    int mm; float d = 1e9f; v3 l;
    if (CPX < 1e8f) for (int k = NCZ; k < NCZ + 2; k++) { l = hero_local(p, k - NCZ); float sc = CZA[k].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc;
        float e = bd > 0.65f ? bd : cz_sdf(&CZA[k], &CZR[k], &CZP[k], l, &mm) * sc; if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
    float zl; int k = walkcell(p, &l, &zl);
    if (k >= 0) { float sc = CZA[k].sc, bd = (len(sub(l, V(0, 0.80f, 0))) - 1.08f) * sc;
        float e = bd > 0.65f ? bd : cz_sdf(&CZA[k], &CZR[k], &CZP[k], l, &mm) * sc; if (e < d) { d = e; if (bd <= 0.65f) *m = mm; } }
    return fminf(d, 4.5f - fabsf(zl) + 0.3f);
}
static float cmap(v3 p, int *m) {
    int mm; float d = p.y, b; *m = M_GROUND;
    if (p.z < SQ0 - 2 && fabsf(p.x) > 6.0f) d = fminf(d, fmaxf(p.y - 0.15f, 6.0f - fabsf(p.x)));           // the pavements
    b = rows(p, &mm); if (b < d) { d = b; *m = mm; }
    b = blocks(p, &mm); if (b < d) { d = b; *m = mm; }
    b = street(p, &mm); if (b < d) { d = b; *m = mm; }
    b = tram(p, &mm); if (b < d) { d = b; *m = mm; }
    b = square(p, &mm); if (b < d) { d = b; *m = mm; }
    b = bigsign(p, &mm); if (b < d) { d = b; *m = mm; }
    b = walkers(p, &mm); if (b < d) { d = b; *m = mm; }
    if (TITLEON > 0) { b = title3d(p, &mm); if (b < d) { d = b; *m = mm; } }
    return d;
}

