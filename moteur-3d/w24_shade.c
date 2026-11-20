// ---------------------------------------------------------------- 24 heures à Porkonia : matières, ciels, lumières, plans
S float w_sunI(v3 p) { return sstep(WLINE - 2.5f, WLINE + 2.5f, p.y); }       // le jour descend le long des pentes : au-dessus de WLINE, on est au soleil
static const v3 WSNW = {0.80f, 0.84f, 0.93f};

// ================================================================ SÉQUENCE 0 : le monument, l'aube
static float MSUNE, WMTN = 1.55f;      // la hauteur du soleil (radians), l'échelle des montagnes
S float msa(v3 rd) { return 0.5f + 0.5f * dot(nrm(V(rd.x, 0, rd.z)), nrm(V(WSUN.x, 0, WSUN.z))); }      // 1 du côté du soleil, 0 à l'opposé
static v3 mon_fogc(v3 rd) {
    float k = sstep(-0.10f, 0.14f, MSUNE), g = sstep(-0.14f, 0.03f, MSUNE) * (1 - 0.8f * sstep(0.14f, 0.36f, MSUNE)), sa = msa(rd);
    v3 c = add(mix3(V(0.022f, 0.030f, 0.065f), V(0.26f, 0.32f, 0.46f), k), mul(V(0.80f, 0.32f, 0.12f), g * sa * sa * sa));
    return add(c, mul(V(0.50f, 0.27f, 0.12f), sstep(-30.0f, -62.0f, WLINE) * (rd.y < 0 ? 1.0f : 0.3f)));        // la brume de la vallée s'allume quand le soleil l'atteint
}
static float mon_fog(v3 ro, v3 rd, float t) {      // un peu d'air partout, une nappe de brume au fond de la vallée
    float a = rd.y * t / 16.0f, k = fabsf(a) < 1e-3f ? 1.0f : (1 - expf(-a)) / a;
    return 0.0007f * t + 0.010f * t * expf(-fmaxf(ro.y + 64.0f, 0) / 16.0f) * k;
}
static v3 mon_sky(v3 ro, v3 rd) {
    if (rd.y < 0) return mon_fogc(rd);
    float y = rd.y, k = sstep(-0.10f, 0.14f, MSUNE), g = sstep(-0.14f, 0.03f, MSUNE) * (1 - 0.8f * sstep(0.14f, 0.36f, MSUNE)), sa = msa(rd);
    v3 col = mix3(mix3(V(0.030f, 0.040f, 0.085f), V(0.30f, 0.34f, 0.46f), k), mix3(V(0.006f, 0.010f, 0.034f), V(0.045f, 0.12f, 0.36f), k), 1 - powf(1 - y, 3.0f));
    col = add(col, mul(V(1.30f, 0.50f, 0.16f), g * sa * sa * sa * expf(-y * 7.0f)));                                    // l'embrasement, du côté du soleil
    col = add(col, mul(V(0.42f, 0.20f, 0.30f), g * (1 - sa) * (1 - sa) * expf(-fabsf(y - 0.10f) * 9.0f) * 0.6f));      // la ceinture rose, à l'opposé
    v3 hd = nrm(V(rd.x, 0, rd.z));
    if (k < 0.9f && y > 0.03f) { float u = atan2f(rd.x, rd.z) * 110.0f, v = y * 150.0f, s = hash2(floorf(u), floorf(v));                 // les étoiles s'éteignent
        if (s > 0.988f) { float fx = fract(u) - 0.5f, fy = fract(v) - 0.5f; col = add(col, mul(V(1, 1, 0.92f), 1.6f * fmaxf(0, 1 - 14 * (fx * fx + fy * fy)) * (1 - k / 0.9f) * (0.6f + 0.4f * sinf(T * 5 + s * 100)))); } }
    float cl = fbm(rd.x / (y + 0.12f) * 1.1f + T * 0.006f, rd.z / (y + 0.12f) * 2.6f), cm = sstep(0.50f, 0.80f, cl) * (1 - y) * (1 - y);      // des nuages hauts, éclairés par-dessous
    col = add(mul(col, 1 - 0.35f * cm), mul(add(mul(V(1.25f, 0.42f, 0.22f), g * (0.35f + 0.65f * sa)), mul(V(0.05f, 0.06f, 0.10f), 1 - g)), cm * 0.55f));
    float sd = fmaxf(dot(rd, WSUN), 0); col = add(col, mul(WSUNC, (sd > 0.9996f ? 6.0f : 0.0f) + 0.5f * powf(sd, 600.0f) + 0.10f * powf(sd, 30.0f)));
    float pk = fbm(hd.x * 5.0f + 9.1f, hd.z * 5.0f + 3.3f), lit = sstep(-0.11f, 0.02f, MSUNE) * (0.2f + 0.8f * (1 - sa));                    // les montagnes : les faces tournées vers le soleil rosissent les premières
    float mh = (0.030f + 0.085f * fbm(hd.x * 1.6f + 5, hd.z * 1.6f + 1.7f) + 0.040f * fbm(hd.x * 4.0f, hd.z * 4.0f + 4.2f) + 0.15f * pk * pk * pk + 0.010f * fbm(hd.x * 25.0f, hd.z * 25.0f + 2.2f)) * WMTN * (1 - ((SEQ == 0 || SEQ == 5) ? 0.80f : 0.0f) * sstep(0.90f, 0.995f, sa));
    float al = sstep(mh + 0.0016f, mh - 0.0016f, y);
    if (al > 0) { float rg = fbm(hd.x * 22.0f + y * 60.0f, hd.z * 22.0f + y * 90.0f), sn = sstep(0.075f, 0.13f, y + 0.035f * (rg - 0.5f)) * sstep(0.25f, 0.55f, rg + 0.6f * sstep(0.12f, 0.20f, y));
        v3 rock = mul(V(0.028f, 0.030f, 0.052f), (0.7f + 0.6f * rg) * (1 + 3.0f * k)), snw = add(mul(V(0.115f, 0.135f, 0.20f), 1 + 2.5f * k), mul(V(0.95f, 0.42f, 0.34f), lit * (1 - 0.45f * k)));
        col = mix3(col, mix3(rock, snw, sn), al * 0.96f); }
    {   float fh = 0.010f + 0.036f * fbm(hd.x * 3.1f + 11, hd.z * 3.1f + 3.3f) + 0.008f * fbm(hd.x * 14.0f, hd.z * 14.0f + 6.1f);              // devant, des collines de sapins
        col = mix3(col, add(mul(V(0.016f, 0.024f, 0.030f), 1 + 2.2f * k), mul(mon_fogc(rd), 0.35f)), 0.88f * sstep(fh + 0.0016f, fh - 0.0016f, y)); }
    return mix3(col, mon_fogc(rd), 1 - expf(-0.05f * expf(-fmaxf(ro.y + 64.0f, 0) / 60.0f) / fmaxf(y, 0.003f)));
}
static void mon_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    switch (m) {
    case W_GROUND: { float r = len2(p.x, p.z);
        if (r < 21.4f && p.y < 0.6f && p.y > -0.6f) {     // le dallage de l'esplanade : un damier de marbre, un filet d'or
            float cx = floorf(p.x * 0.5f), cz = floorf(p.z * 0.5f), e = fminf(fabsf(fract(p.x * 0.5f) - 0.5f), fabsf(fract(p.z * 0.5f) - 0.5f));
            M->alb = mul(((int)(cx + cz + 200) & 1) ? V(0.30f, 0.03f, 0.07f) : V(0.78f, 0.74f, 0.64f), 0.55f * (0.9f + 0.1f * vnoise(p.x * 3, p.z * 3)));
            M->alb = mul(M->alb, 0.6f + 0.4f * sstep(0.495f, 0.47f, e)); M->env = 0.06f; M->spec = 1.0f; M->gloss = 120;
            if (fabsf(r - 20.4f) < 0.22f) { M->alb = V(0.62f, 0.44f, 0.16f); M->env = 0.5f; }
            *snow = 0.92f * sstep(0.52f, 0.70f, fbm(p.x * 0.5f, p.z * 0.5f) + 0.75f * sstep(16.0f, 21.4f, r) - 0.12f); *up = 1;
        } else { float st = sstep(0.80f, 0.60f, n.y);      // la neige ; la roche où c'est trop raide ; la glace de la rivière
            M->alb = mix3(mul(WSNW, 0.86f + 0.14f * fbm(p.x * 0.4f, p.z * 0.4f)), mul(V(0.16f, 0.15f, 0.15f), 0.6f + 0.8f * vnoise(p.x * 0.5f + p.y, p.z * 0.5f)), st);
            M->spec = 0.25f * (1 - st); M->gloss = 20; M->sss = 0.3f * (1 - st);
            if (p.y < -60.0f && fabsf(p.z + 232.0f - 18.0f * sinf(p.x * 0.02f)) < 7.0f) { M->alb = V(0.05f, 0.08f, 0.12f); M->env = 0.55f; M->spec = 1.0f; M->gloss = 150; M->sss = 0; } }
        break; }
    case W_MARBLE: { float th = atan2f(p.x, -p.z), rr = len2(p.x, p.z); M->alb = mul(V(0.74f, 0.72f, 0.68f), 0.82f + 0.18f * sinf(p.y * 3.0f + 3.0f * fbm(p.x * 1.3f + p.z, p.z * 1.3f - p.y))); M->spec = 0.6f; M->gloss = 70; *snow = 0.9f;
        if (rr < 3.75f && rr > 3.3f && p.y > 1.15f && p.y < 3.95f) {       // sur le fût : une frise, et face à la vallée une plaque de bronze
            if (fabsf(p.y - 3.62f) < 0.16f) M->alb = fract(th * 9.0f) < 0.5f ? V(0.30f, 0.03f, 0.07f) : V(0.62f, 0.44f, 0.16f);
            float e = fminf(0.42f - fabsf(th), fminf(p.y - 1.55f, 3.25f - p.y) / 3.6f);
            if (e > 0) { M->alb = e < 0.02f ? V(0.62f, 0.44f, 0.16f) : mul(V(0.13f, 0.085f, 0.04f), 0.8f + 0.4f * vnoise(th * 30, p.y * 9)); M->spec = 1.2f; M->gloss = 60; *snow = 0;
                if (e > 0.06f && fabsf(fract((p.y - 1.75f) / 0.30f) - 0.5f) * 0.30f < 0.035f && hash2(floorf((p.y - 1.75f) / 0.30f), floorf(th * 11.0f)) < 0.7f) M->alb = V(0.34f, 0.24f, 0.09f); } }
        break; }
    case W_STONE: M->alb = mul(V(0.36f, 0.34f, 0.31f), 0.85f + 0.3f * vnoise(p.x * 3 + p.y * 2, p.z * 3)); *snow = 1; break;
    case W_BRONZE: M->alb = V(0.26f, 0.17f, 0.07f); M->spec = 1.2f; M->gloss = 50; M->env = 0.25f; break;
    case W_ROPE: M->alb = V(0.55f, 0.50f, 0.40f); break;
    case W_FLAG: { float dx = p.x - MMAST[0], dz = p.z - MMAST[1]; M->alb = V(0.34f, 0.03f, 0.07f); M->sss = 0.9f; M->spec = 0.08f;
        if (len2(dx, dz) < 5.5f) {       // les couleurs : PORKONIA en crème sur bordeaux, un galon d'or
            float u = dx * 0.8f - dz * 0.6f - 0.14f, v = p.y - MFLAGH + 1.25f + 0.12f * (u / 3.9f) * (u / 3.9f) * 1.25f, ut = (n.x * 0.6f + n.z * 0.8f) > 0 ? 3.9f - u : u, sf = 3.1f / (TR[T_TITLE].w - 80.0f);
            float dt = text2d(T_TITLE, (ut - 1.95f) / sf + TR[T_TITLE].w * 0.5f, TR[T_TITLE].base - (v + 0.20f) / sf) * sf;
            if (dt < 0) M->alb = V(0.82f, 0.78f, 0.66f);
            if (fabsf(fabsf(v) - 0.98f) < 0.07f) M->alb = V(0.70f, 0.50f, 0.18f);
        } else if (fabsf(fract((p.y - 5.35f) / 1.6f) - 0.5f) < 0.07f) M->alb = V(0.70f, 0.50f, 0.18f);
        break; }
    case W_LAMP: M->alb = V(0.25f, 0.24f, 0.22f); M->emis = mul(V(15.0f, 9.5f, 3.8f), WLAMP); M->spec = 0.8f; break;
    case W_CHALET: { float c[4], h; mon_chalet(p.x, p.z, c, &h); v3 q = V(p.x - c[0], p.y - c[2], p.z - c[1]), nn = n; if (c[3] > 0.5f) { q = V(q.z, q.y, q.x); nn = V(n.z, n.y, n.x); }
        float H = 3.4f + 1.6f * fract(h * 41.0f);
        if (nn.y > 0.3f) { M->alb = V(0.11f, 0.07f, 0.05f); *snow = 1; *up = 1; }
        else { float u = fabsf(nn.x) > 0.5f ? q.z : q.x, fu = fract(u / 1.9f + 0.5f);
            M->alb = q.y < 1.1f ? mul(V(0.40f, 0.37f, 0.32f), 0.8f + 0.3f * vnoise(u * 2, q.y * 3)) : mul(V(0.17f, 0.095f, 0.05f), 0.75f + 0.25f * sstep(0.3f, 0.5f, fract(q.y * 3.3f)));
            if (fu > 0.28f && fu < 0.72f && q.y > 1.35f && q.y < 2.55f && q.y < H - 0.4f) { M->alb = V(0.02f, 0.02f, 0.03f);
                if (hash2(floorf(u / 1.9f + 0.5f) + 9 * roundf(nn.x) + 5 * roundf(nn.z), h * 77.0f) < 0.55f) M->emis = mul(V(2.4f, 1.35f, 0.5f), (fabsf(fu - 0.5f) > 0.02f) * (0.7f + 0.6f * fract(h * 91.0f))); } }
        break; }
    case W_TOWER: { v3 q = V(p.x + 30.0f, p.y - mon_h(-30.0f, -170.0f), p.z + 170.0f); M->alb = mul(V(0.40f, 0.37f, 0.31f), 0.85f + 0.3f * vnoise(q.x * 2 + q.z * 2, q.y * 2)); *snow = 1;
        if (q.y > 18.05f) { M->alb = V(0.09f, 0.10f, 0.13f); *snow = 0.75f; *up = sstep(0.2f, 0.5f, n.y); }
        else if (fabsf(n.y) < 0.5f) { float u = fabsf(n.x) > 0.5f ? q.z : q.x, cv = q.y - 15.9f, cr = len2(u, cv);      // quatre cadrans : six heures
            if (cr < 1.55f) { int ink = cr > 1.38f || (cr > 1.12f && fabsf(fract(atan2f(u, cv) * 1.9099f + 0.5f) - 0.5f) < 0.08f) || cr < 0.12f || (fabsf(u) < 0.055f && cv < 1.05f && cv > -0.72f);
                M->alb = V(0.02f, 0.02f, 0.02f); M->emis = ink ? V(0.01f, 0.01f, 0.01f) : V(1.7f, 1.35f, 0.85f); *snow = 0; }
            else if (fabsf(u) < 0.35f && q.y > 3.0f && q.y < 12.0f && fract(q.y / 4.0f) < 0.4f) { M->alb = V(0.02f, 0.02f, 0.03f); M->emis = V(1.6f, 0.9f, 0.35f); } }
        break; }
    case W_SENTRY: { v3 q = V(p.x + 13.5f, p.y, p.z - 9.0f); M->alb = fract((q.x + q.z + q.y) * 1.3f) < 0.5f ? V(0.34f, 0.03f, 0.07f) : V(0.78f, 0.74f, 0.64f); if (q.y > 2.5f) { M->alb = V(0.30f, 0.03f, 0.06f); *snow = 1; *up = sstep(0.3f, 0.6f, n.y); }
        if (q.x * q.x + q.z * q.z < 0.3f && q.y < 2.4f) M->alb = V(0.10f, 0.06f, 0.04f); break; }
    default: break;
    }
}
static const float KM1[] = {        // du village au pied de la vallée jusqu'à l'esplanade
    0.0f, -58.0f, -42.0f, -292.0f,   -30.0f, -46.0f, -170.0f,
    3.2f, -45.0f, -45.5f, -214.0f,   -30.0f, -45.5f, -168.0f,
    5.6f, -16.0f, -36.0f, -128.0f,    -2.0f,  -8.0f,  -30.0f,
    8.85f,  5.0f,   7.5f,  -47.0f,     0.0f,   6.5f,    0.0f };
static void mon_setup(float s) {
    WFAR = 900.0f; MSUNE = -0.05f + 0.0075f * s; WSUN = nrm(V(0.25f, sinf(MSUNE), -cosf(MSUNE)));
    float k = sstep(-0.10f, 0.14f, MSUNE);
    WSUNC = mul(mix3(V(2.6f, 0.95f, 0.30f), V(2.9f, 1.55f, 0.75f), sstep(0.02f, 0.22f, MSUNE)), sstep(0.020f, 0.045f, MSUNE));
    WLINE = s < 11 ? 80.0f : 20.0f - 4.4f * (s - 11);
    WAMBU = mix3(V(0.030f, 0.045f, 0.105f), V(0.16f, 0.22f, 0.38f), k); WAMBD = mix3(V(0.012f, 0.012f, 0.020f), V(0.10f, 0.085f, 0.075f), k);
    WLAMP = sstep(17.5f, 15.0f, s); EXPO = 1.15f - 0.22f * sstep(0.03f, 0.10f, MSUNE); STAR = 0.6f;
    MPH = T * 3.2f; MFLAGH = 3.4f + 9.2f * ease((s - 9.6f) / 6.0f);
    for (int i = 0; i < 3; i++) { float a = T * (0.55f - 0.09f * i) + 2.1f * i, R = 5.6f + 0.8f * i; MSP[i] = V(R * cosf(a), 9.2f + 0.9f * i + 0.4f * sinf(T * 0.8f + i), R * sinf(a)); }
    // ---- ceux qui sont là : trois gardes, cinq lève-tôt, un sénateur qui arrive
    static int init; if (!init) { init = 1; WCZ[0] = CZA[6]; WCZ[1] = CZA[6]; WCZ[1].fat = 0.5f; WCZ[2] = CZA[6]; WCZ[2].leg = 0.2f; WCZ[3] = CZA[NCZ]; WCZ[4] = CZA[NCZ + 1]; WCZ[5] = CZA[2]; WCZ[6] = CZA[7]; WCZ[7] = CZA[4]; WCZ[8] = CZA[3]; }
    WN = 9; float up = ease((s - 9.6f) / 6.0f), done = ease((s - 15.5f) / 0.5f), hoist = sstep(9.4f, 9.8f, s) * sstep(15.8f, 15.4f, s);
    #define FACE(x, z, tx, tz) atan2f((tx) - (x), (tz) - (z))
    {   pose_t *P = &WPP[0]; cz_rest(&WCZ[0], P); cz_idle(P, T, 0.3f); P->hpitch = -0.12f - 0.2f * up;                                        // le garde qui hisse les couleurs
        for (int i = 0; i < 2; i++) P->hand[i] = mix3(P->hand[i], V(i ? -0.09f : 0.09f, 0.50f + 0.20f * sinf(T * 5.5f + i * PI), 0.34f), hoist);
        cz_salute(P, done); w_place(0, MMAST[0] + 0.72f, 0, MMAST[1] + 0.30f, FACE(MMAST[0] + 0.72f, MMAST[1] + 0.30f, MMAST[0] + 0.13f, MMAST[1])); }
    {   pose_t *P = &WPP[1]; cz_rest(&WCZ[1], P); cz_idle(P, T, 0.7f); P->hpitch = -0.10f - 0.25f * up; P->hyaw *= 0.3f; cz_salute(P, ease((s - 10.0f) / 0.6f));      // celui qui salue
        w_place(1, MMAST[0] - 0.4f, 0, MMAST[1] - 1.9f, FACE(MMAST[0] - 0.4f, MMAST[1] - 1.9f, MMAST[0] + 1.5f, MMAST[1] - 0.6f)); }
    {   pose_t *P = &WPP[2]; cz_rest(&WCZ[2], P); cz_idle(P, T, 0.5f); P->hyaw *= 0.4f; w_place(2, -12.5f, 0, 7.7f, FACE(-12.5f, 7.7f, 0, 0)); }                         // la sentinelle, devant sa guérite
    static const float OL[5][2] = {{5.6f, -13.6f}, {4.6f, -12.9f}, {7.0f, -13.0f}, {6.6f, -15.2f}, {2.8f, -14.2f}};
    for (int j = 0; j < 5; j++) { int i = 3 + j; pose_t *P = &WPP[i]; cz_rest(&WCZ[i], P); cz_idle(P, T, 0.13f + 0.19f * j); P->hpitch = -0.08f - 0.24f * up; P->looky = 0.4f; P->smile = 0.7f + 0.3f * done;
        if (j == 0) P->hand[1] = mix3(P->hand[1], V(-0.10f, 0.42f, 0.30f), sstep(9.0f, 10.5f, s));                                              // la dame, la main sur le cœur
        if (j == 1) { cz_wave(P, T, done); float hop = fabsf(sinf(T * 6.0f)) * done; P->bob += 0.05f * hop; P->foot[0].y += 0.05f * hop; P->foot[1].y += 0.05f * hop; P->open = 0.7f * done; }      // le petit salue et sautille
        if (j == 2 || j == 3) for (int a = 0; a < 2; a++) P->hand[a] = mix3(P->hand[a], V(a ? -0.40f : 0.40f, 0.86f + 0.05f * sinf(T * 9.0f + a * 2.0f + j), 0.14f), done);    // les bras au ciel
        if (j == 4) cz_salute(P, ease((s - 11.0f) / 0.7f));
        float tx = s < 17.85f ? MMAST[0] + 1.2f : 0, tz = s < 17.85f ? MMAST[1] - 0.5f : 0; w_place(i, OL[j][0], 0, OL[j][1], FACE(OL[j][0], OL[j][1], tx, tz)); }
    {   pose_t *P = &WPP[8]; cz_rest(&WCZ[8], P); float L = clampf(0.9f * (s - 6.0f), 0, 8.2f), go = L < 8.15f ? 1.0f : 0.0f; if (WCZ[8].prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.12f);       // le sénateur arrive, puis regarde
        if (go > 0) cz_walk(P, 6.2832f * T / 0.7556f, 1); else { cz_idle(P, T, 0.9f); P->hpitch = -0.08f - 0.24f * up; }
        float x = 14.6f - 0.66f * L, z = -6.2f - 0.75f * L; w_place(8, x, 0, z, go > 0 ? atan2f(-0.66f, -0.75f) : FACE(x, z, MMAST[0], MMAST[1])); }
    // ---- les plans
    v3 pos, tar;
    if (s < 8.85f) { crk(KM1, 4, s, &pos, &tar); cam(pos, tar, 1.4f); }
    else if (s < 17.85f) { float u = (s - 8.85f) / 9.0f; cam(mix3(V(4.2f, 1.5f, -20.2f), V(3.6f, 1.75f, -18.6f), ease(u)), mix3(V(-3.0f, 7.0f, 0), V(-3.2f, 8.6f, 0), up), 1.3f); FOCUS = 14; APERT = 0.008f; }
    else if (s < 26.85f) { float u = (s - 17.85f) / 9.0f, ph = -0.5f + 1.9f * u, R = 14.5f - 2.5f * u; cam(V(R * sinf(ph), 1.5f + 2.2f * u, -R * cosf(ph)), V(0, 6.6f + 0.3f * u, 0), 1.3f); FOCUS = R - 1.0f; APERT = 0.009f; }
    else { float u = ease((s - 26.85f) / 9.15f); cam(mix3(V(5.5f, 3.2f, -15.0f), V(15.0f, 25.0f, -60.0f), u), mix3(V(0, 6.5f, 0), V(-1.0f, 9.0f, 20.0f), u), mixf(1.4f, 1.35f, u)); }
}

static float confetti(v3 ro, v3 rd, float tmax, float *hc) {      // un confetti par case d'air ; hc : de quoi choisir sa couleur
    const float c = 1.7f, far = 30.0f, size = 0.026f; float acc = 0, t = 1.6f; v3 o = add(ro, V(T * 0.5f, T * 0.9f, T * 0.22f)); tmax = fminf(tmax, far); *hc = 0;
    for (int i = 0; i < 28 && t < tmax; i++) {
        v3 q = add(o, mul(rd, t)); float ix = floorf(q.x / c), iy = floorf(q.y / c), iz = floorf(q.z / c), h = hash2(ix + iy * 57.3f, iz + iy * 13.7f);
        v3 oc = sub(V((ix + 0.2f + 0.6f * fract(h * 17.3f)) * c, (iy + 0.2f + 0.6f * fract(h * 39.1f)) * c, (iz + 0.2f + 0.6f * fract(h * 71.7f)) * c), o);
        float tc = dot(oc, rd);
        if (tc > 1.6f && tc < tmax) { float d2 = dot(oc, oc) - tc * tc, r = size * (0.7f + 0.6f * h), R = r + tc * 0.0016f, a = sstep(R * R, R * R * 0.2f, d2) * fminf(r / R * 1.5f, 1.0f) * (1 - tc / far); if (a > acc) { acc = a; *hc = h; } }
        float tx = rd.x > 1e-5f ? ((ix + 1) * c - q.x) / rd.x : rd.x < -1e-5f ? (ix * c - q.x) / rd.x : 1e9f;
        float ty = rd.y > 1e-5f ? ((iy + 1) * c - q.y) / rd.y : rd.y < -1e-5f ? (iy * c - q.y) / rd.y : 1e9f;
        float tz = rd.z > 1e-5f ? ((iz + 1) * c - q.z) / rd.z : rd.z < -1e-5f ? (iz * c - q.z) / rd.z : 1e9f;
        t += fminf(tx, fminf(ty, tz)) + 0.01f;
    }
    return fminf(acc, 1);
}
// ================================================================ SÉQUENCE 1 : la Grande Parade, 08 h 00
static float WCONF;
static v3 par_sky(v3 ro, v3 rd) {      // le ciel du matin, les montagnes ; tout au bout de l'avenue, à contre-jour, le Cube de Porc
    v3 col = mon_sky(ro, rd); float az = atan2f(rd.x, rd.z);
    if (rd.y > 0 && fabsf(az) < 0.12f && rd.y < 0.23f) { float u = az / 0.095f, v = rd.y / 0.20f, e = fmaxf(fabsf(u) - 1, v - 1);
        if (e < 0) { float fat = sstep(0.42f, 0.58f, fbm(u * 2.3f + 7, v * 6.0f) + 0.25f * sinf(v * 19.0f));
            col = mix3(col, mix3(mul(mix3(V(0.42f, 0.10f, 0.11f), V(0.80f, 0.70f, 0.62f), fat), 0.34f + 0.22f * v), mon_fogc(rd), 0.50f), sstep(0, -0.025f, e)); } }
    return col;
}
static void par_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    float ax = fabsf(p.x);
    switch (m) {
    case W_GROUND:
        if (ax < 14.6f) { v3 rd = nrm(sub(p, RO));        // l'avenue : le damier, qui se fond en un seul ton quand il passe sous le pixel
            float cx = floorf(p.x * 0.5f), cz = floorf(p.z * 0.5f), e = fminf(fabsf(fract(p.x * 0.5f) - 0.5f), fabsf(fract(p.z * 0.5f) - 0.5f)), k = sstep(0.55f, 0.18f, t * 0.0034f / fmaxf(fabsf(rd.y), 0.03f) / 2.0f);
            v3 c = ((int)(cx + cz + 2000) & 1) ? V(0.30f, 0.03f, 0.07f) : V(0.78f, 0.74f, 0.64f);
            M->alb = mul(mix3(V(0.54f, 0.385f, 0.355f), mul(c, 0.6f + 0.4f * sstep(0.495f, 0.47f, e)), k), 0.60f * (0.92f + 0.08f * vnoise(p.x * 3, p.z * 3))); M->env = 0.05f; M->spec = 0.8f; M->gloss = 100;
            *snow = 0.9f * sstep(0.50f, 0.68f, fbm(p.x * 0.6f, p.z * 0.6f) + 0.8f * sstep(12.6f, 14.6f, ax) - 0.15f); *up = 1;
        } else { M->alb = mul(WSNW, (0.84f + 0.16f * fbm(p.x * 0.4f, p.z * 0.4f)) * (ax < 26.0f ? 0.86f : 1.0f)); M->spec = 0.25f; M->gloss = 20; M->sss = 0.3f; }
        break;
    case W_FLAG: M->alb = (ax > 20.0f && ((int)roundf(p.z / 20.0f) & 1)) ? V(0.80f, 0.76f, 0.64f) : V(0.36f, 0.03f, 0.07f); M->sss = 0.9f; M->spec = 0.08f; break;
    case W_RAIL: M->alb = V(0.30f, 0.31f, 0.33f); M->spec = 0.9f; M->gloss = 60;
        if (p.y < 1.07f && fabsf(ax - 15.33f) < 0.03f) { M->alb = p.y < 0.24f ? V(0.70f, 0.50f, 0.18f) : mul(V(0.36f, 0.03f, 0.07f), 0.8f + 0.2f * cosf(p.z * 5.0f)); M->spec = 0.1f; M->sss = 0.4f; }
        break;
    case W_STAND: M->alb = mul(V(0.24f, 0.19f, 0.15f), 0.7f + 0.3f * sstep(0.03f, 0.08f, fabsf(fract(p.z / 0.5f) - 0.5f) * 0.5f)); break;
    case W_PENNANT: { int i = (int)(roundf(p.x / 1.3f) + 3 * roundf(p.z / 20.0f) + 300) % 3; float yc = 9.6f - 2.4f * (1 - (p.x / 22.8f) * (p.x / 22.8f));
        M->alb = fabsf(p.y - yc) < 0.04f ? V(0.08f, 0.07f, 0.06f) : i == 0 ? V(0.40f, 0.03f, 0.08f) : i == 1 ? V(0.84f, 0.80f, 0.68f) : V(0.80f, 0.55f, 0.14f); M->sss = 0.9f; M->spec = 0.05f; break; }
    case W_TRIB: M->alb = mul(V(0.36f, 0.03f, 0.07f), 0.75f + 0.25f * cosf(p.z * 4.0f)); M->spec = 0.15f; M->sss = 0.2f;
        if (p.y > 5.0f) { M->alb = fract(p.z / 2.4f) < 0.5f ? V(0.36f, 0.03f, 0.07f) : V(0.80f, 0.76f, 0.64f); if (fabsf(n.y) < 0.5f) M->alb = V(0.70f, 0.50f, 0.18f); *snow = 1; }       // le dais rayé, son galon d'or
        else if (p.y < 1.32f) { if (n.y > 0.5f) M->alb = V(0.30f, 0.05f, 0.08f);                                                                                              // l'estrade : PORKONIA en or sur le drapé
            else if (n.x < -0.5f) { float sf = 9.5f / (TR[T_TITLE].w - 80.0f), dt = text2d(T_TITLE, p.z / sf + TR[T_TITLE].w * 0.5f, TR[T_TITLE].base - (p.y - 0.36f) / sf) * sf;
                if (dt < 0) { M->alb = V(0.85f, 0.60f, 0.20f); M->spec = 1.2f; } }
            if (fabsf(p.y - 1.22f) < 0.05f || p.y < 0.10f) M->alb = V(0.70f, 0.50f, 0.18f); }
        break;
    case W_ARCH: { float u = fabsf(n.z) > 0.5f ? p.x : p.z, row = floorf(p.y / 1.1f), jb = fminf(fabsf(fract(p.y / 1.1f) - 0.5f) * 1.1f, fabsf(fract(u / 2.2f + 0.5f * row) - 0.5f) * 2.2f);
        M->alb = mul(V(0.50f, 0.47f, 0.41f), (0.9f + 0.2f * hash2(floorf(u / 2.2f + 0.5f * row + 0.5f), row)) * (1 - 0.3f * sstep(160.0f, 40.0f, t) * sstep(0.07f, 0.03f, jb))); *snow = 1;
        if (fabsf(n.z) > 0.5f && p.y > 21.2f && p.y < 25.6f && ax < 13.0f) { float sf = 22.0f / (TR[T_PLUS].w - 80.0f), dt = text2d(T_PLUS, (n.z < 0 ? p.x : -p.x) / sf + TR[T_PLUS].w * 0.5f, TR[T_PLUS].base - (p.y - 22.3f) / sf) * sf;
            if (dt < 0) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 1.6f; M->gloss = 150; *snow = 0; } }
        break; }
    default: break;
    }
}
static void par_setup(float s) {
    WFAR = 900.0f; MSUNE = 0.33f; WSUN = nrm(V(-0.35f, 0.33f, 0.87f)); WSUNC = V(2.55f, 2.05f, 1.55f); WAMBU = V(0.20f, 0.27f, 0.42f); WAMBD = V(0.17f, 0.16f, 0.15f); EXPO = 0.86f; STAR = 0.5f;
    PLZ = -36.8f + PV * s; PPHI = PI * T / 0.5625f; MPH = T * 3.4f;
    for (int k = 0; k < NCR; k++) { pose_t *P = &CRP[k]; int a = k % 12; cz_rest(&CZA[a], P); cz_idle(P, T, 0.07f * k); P->smile = 0.9f; P->hyaw += 0.25f * sinf(T * 0.7f + k);       // la foule : les uns lèvent les bras, les autres saluent
        if (CZA[a].prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.12f);
        if (k < 12) { float hop = fabsf(sinf(T * 5.6f + k)); P->bob += 0.03f * hop; P->foot[0].y += 0.03f * hop; P->foot[1].y += 0.03f * hop; P->open = 0.5f + 0.4f * sinf(T * 3.0f + k);
            for (int i = 0; i < 2; i++) if (!(i == 1 && CZA[a].prop == PROP_CANE)) P->hand[i] = V(i ? -0.40f : 0.40f, 0.84f + 0.06f * sinf(T * 8.0f + i * 2.0f + k), 0.14f); }
        else cz_wave(P, T + 0.3f * k, 1);
        cz_rig(&CZA[a], P, &CRR[k]); }
    static int init; if (init != 2) { init = 2; WCZ[0] = CZA[6]; WCZ[1] = CZA[1]; WCZ[2] = CZA[3]; WCZ[3] = CZA[7]; WCZ[4] = CZA[6]; WCZ[4].fat = 0.5f; WCZ[5] = CZA[NCZ]; WCZ[6] = CZA[NCZ + 1]; }
    WN = 7; float pass = sstep(20.5f, 21.5f, s) * sstep(27.0f, 26.0f, s);
    static const float TBZ[5] = {-5.6f, -2.5f, 0.0f, 2.5f, 5.6f};
    for (int i = 0; i < 5; i++) { pose_t *P = &WPP[i]; cz_rest(&WCZ[i], P); cz_idle(P, T, 0.21f + 0.17f * i); P->hyaw += 0.5f * clampf((PLZ - TBZ[i]) * 0.08f, -1, 1) * 0 - 0.45f * clampf((TBZ[i] - PLZ) * 0.06f, -1, 1);      // la tribune : on suit le meneur des yeux
        if (WCZ[i].prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.12f);
        if (i == 0 || i == 4 || i == 2) cz_salute(P, i == 2 ? pass : 1); else if (i == 1) cz_wave(P, T, 1); else for (int a = 0; a < 2; a++) P->hand[a] = V(a ? -0.40f : 0.40f, 0.85f + 0.05f * sinf(T * 8.5f + a * 2.0f), 0.14f);
        w_place(i, 17.0f, 1.3f, TBZ[i], -PI * 0.5f); }
    {   pose_t *P = &WPP[5]; cz_rest(&WCZ[5], P); cz_idle(P, T, 0.4f); cz_wave(P, T, 1); P->smile = 1; w_place(5, 16.15f, 0.25f, -79.6f, -PI * 0.5f - 0.25f); }                           // au premier rang, la dame et le petit
    {   pose_t *P = &WPP[6]; cz_rest(&WCZ[6], P); cz_idle(P, T, 0.8f); float hop = fabsf(sinf(T * 6.2f)); P->bob += 0.06f * hop; P->foot[0].y += 0.06f * hop; P->foot[1].y += 0.06f * hop; P->open = 0.8f; P->brow = 1;
        for (int a = 0; a < 2; a++) P->hand[a] = V(a ? -0.38f : 0.38f, 0.82f + 0.07f * sinf(T * 9.0f + a * 2.0f), 0.16f); w_place(6, 16.1f, 0.25f, -80.9f, -PI * 0.5f - 0.2f); }
    WCONF = s < 31.5f ? 0.9f : 0.0f; WNL = 1; { static const float TL[8] = {18.2f, 4.5f, 0, 1.0f, 0.85f, 0.62f, 7.0f, 0.09f}; memcpy(WLT[0], TL, sizeof(TL)); }
    if (s < 9.0f) { float zc = PLZ + 8.6f - 0.28f * s; cam(V(0.9f + 0.5f * sinf(s * 0.5f), 0.62f + 0.03f * s, zc), V(0, 1.5f, PLZ), 1.6f); FOCUS = zc - PLZ - 1.0f; APERT = 0.014f; }                    // le meneur, de face, à hauteur de sabot
    else if (s < 18.0f) { float u = (s - 9.0f) / 9.0f; cam(V(12.95f, 1.62f, -75.0f + 3.0f * u), V(16.5f, 1.38f, -81.6f + 1.5f * u), 1.5f); FOCUS = 6.0f; APERT = 0.012f; }                               // le long des gradins
    else if (s < 27.0f) { float u = ease((s - 22.6f) / 4.4f); cam(mix3(V(-4.0f, 0.9f, 5.5f), V(-11.0f, 8.5f, 16.0f), u), mix3(V(17.5f, 2.7f, -1.5f), V(0, 1.0f, -22.0f), u), mixf(1.7f, 1.3f, u)); }      // la tribune salue, puis on s'élève
    else if (s < 31.5f) { float u = (s - 27.0f) / 4.5f; cam(V(0.6f, 5.2f + 1.2f * u, PLZ - 31.0f + 2.0f * u), V(0, 7.0f, 120.0f), 1.6f); }                                                   // par-dessus les rangs : l'arc, et tout au bout le Cube
    else { float u = ease((s - 31.5f) / 4.5f); cam(mix3(V(3.5f, 4.0f, PLZ + 13.0f), V(9.0f, 17.0f, PLZ + 36.0f), u), mix3(V(0, 1.4f, PLZ - 6.0f), V(0, 1.0f, PLZ - 40.0f), u), 1.5f); }             // le défilé jusqu'à l'horizon
    if (WDBG & 16) { cam(V(9.0f, 1.7f, -78.0f), V(17.0f, 1.3f, -80.5f), 1.6f); APERT = 0; WCONF = 0; }
}

// ================================================================ SÉQUENCE 2 : le Cube de Porc
S float cub_shadow(v3 p) {      // l'ombre du Cube et de sa plateforme (les rayons d'ombre du moteur ne vont pas assez loin pour un objet de 120 m)
    v3 lo = V(-QC, 0, -QC), hi = V(QC, QTOP, QC); float t0 = 0, t1 = 1e9f;
    for (int a = 0; a < 3; a++) { float o = a == 0 ? p.x : a == 1 ? p.y : p.z, dd = a == 0 ? WSUN.x : a == 1 ? WSUN.y : WSUN.z, l = a == 0 ? lo.x : a == 1 ? lo.y : lo.z, h = a == 0 ? hi.x : a == 1 ? hi.y : hi.z;
        float ta = (l - o) / dd, tb = (h - o) / dd; if (ta > tb) { float x = ta; ta = tb; tb = x; } t0 = fmaxf(t0, ta); t1 = fminf(t1, tb); }
    return t1 > t0 + 4.0f ? 0.0f : 1.0f;
}
S float cub_dark(v3 p) { return (fabsf(p.x) < 4.45f && p.z > -92.0f && p.z < -84.9f && p.y < QH + 5.05f && p.y > QH - 0.1f) ? 0.02f + 0.9f * sstep(-88.4f, -91.9f, p.z) : 1.0f; }     // dans le porche des Galeries, on ne voit plus rien
static void cub_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    float ax = fabsf(p.x), u, w; cub_side(p, &u, &w);
    switch (m) {
    case W_GROUND: { float st = sstep(0.80f, 0.60f, n.y);
        M->alb = mix3(mul(WSNW, 0.84f + 0.16f * fbm(p.x * 0.4f, p.z * 0.4f)), mul(V(0.15f, 0.14f, 0.14f), 0.6f + 0.8f * vnoise(p.x * 0.5f + p.y, p.z * 0.5f)), st); M->spec = 0.25f * (1 - st); M->gloss = 20; M->sss = 0.3f * (1 - st);
        if (p.z < -110.0f && p.z > -344.0f) M->alb = mix3(M->alb, mul(V(0.46f, 0.48f, 0.54f), 0.8f + 0.3f * vnoise(p.x * 2.5f, p.z * 2.5f)), sstep(4.9f, 4.2f, ax + 0.5f * vnoise(p.z * 0.9f, 3.0f)));       // l'allée, tassée par les pas
        break; }
    case W_PLAT: { float uu = fabsf(n.y) > 0.5f ? p.x : u, vv = fabsf(n.y) > 0.5f ? p.z : p.y, row = floorf(vv / 1.5f), jb = fminf(fabsf(fract(vv / 1.5f) - 0.5f) * 1.5f, fabsf(fract(uu / 3.0f + 0.5f * row) - 0.5f) * 3.0f);
        M->alb = mul(V(0.33f, 0.31f, 0.29f), (0.85f + 0.3f * hash2(floorf(uu / 3.0f + 0.5f * row + 0.5f), row)) * (1 - 0.35f * sstep(220.0f, 40.0f, t) * sstep(0.08f, 0.03f, jb)));
        if (w < QP - 0.9f && p.y < QH - 0.2f && n.y < 0.9f) { M->alb = mul(M->alb, 0.4f); M->emis = mul(V(0.9f, 0.42f, 0.14f), 0.14f * sstep(QP - 0.6f, QP - 3.1f, w)); }                        // sous les arcades : les Galeries, une lueur au fond
        if (ax < 12.0f && p.z > -110.0f && p.z < -92.0f && n.y > 0.3f) M->alb = mul(M->alb, 1 - 0.35f * sstep(70.0f, 25.0f, t) * sstep(0.10f, 0.03f, fabsf(fract((p.z + 110.0f) / 0.6f) - 0.5f) * 0.6f));     // les marches
        else if (n.y > 0.5f && !(ax < 4.45f && p.z < -84.9f)) { *snow = 0.8f * sstep(0.30f, 0.55f, fbm(p.x * 0.3f, p.z * 0.3f) + 0.25f - 0.6f * sstep(3.6f, 1.2f, fabsf(w - 67.5f)) - 0.9f * sstep(63.4f, 62.4f, w)); *up = 1; }      // la neige, sauf où l'on marche et où brûlent les cierges
        break; }
    case W_MEAT: { float y = p.y - QH, v = y + 3.0f * sinf(u * 0.031f + 1.0f) + 1.6f * sinf(u * 0.083f + y * 0.05f) + 6.0f * (fbm(u * 0.021f + 3.0f, y * 0.035f) - 0.5f), top = 2 * QC - v;        // du lard : la couenne, le gras, des couches de maigre et de gras, le persillé
        float fat = sstep(0.54f, 0.60f, vnoise(u * 0.006f + 7.0f, v * 0.105f));
        fat = fmaxf(fat, 0.9f * sstep(0.66f, 0.72f, vnoise(u * 0.035f, v * 0.75f + 3.0f)));
        fat = fmaxf(fat, 0.8f * sstep(0.60f, 0.70f, fbm(u * 0.45f + v * 0.08f, v * 1.5f)) * sstep(260.0f, 90.0f, t));
        fat = fmaxf(fat, sstep(13.0f, 10.0f, top));
        v3 lean = mul(mix3(V(0.42f, 0.07f, 0.09f), V(0.72f, 0.22f, 0.22f), fbm(u * 0.11f, v * 0.19f)), 0.85f + 0.3f * vnoise(u * 0.7f, v * 9.0f) * sstep(120.0f, 30.0f, t));
        M->alb = mix3(lean, mul(V(0.93f, 0.74f, 0.62f), 0.88f + 0.17f * vnoise(u * 0.9f, v * 0.9f)), fat);
        if (top < 3.2f + 0.6f * sinf(u * 0.3f)) M->alb = mul(V(0.60f, 0.34f, 0.15f), 0.75f + 0.35f * vnoise(u * 0.8f, y * 0.8f));
        M->spec = 0.7f; M->gloss = 50; M->sss = 0.6f; if (n.y > 0.5f) { *snow = 0.92f; *up = 1; }
        break; }
    case W_GATE: { float rw = floorf(p.y / 0.7f), jb = fminf(fabsf(fract(p.y / 0.7f) - 0.5f) * 0.7f, fabsf(fract((p.x + p.z) / 1.4f + 0.5f * rw) - 0.5f) * 1.4f);
        M->alb = mul(V(0.40f, 0.38f, 0.34f), (0.85f + 0.3f * vnoise(p.x * 2 + p.z, p.y * 2)) * (1 - 0.3f * sstep(90.0f, 20.0f, t) * sstep(0.05f, 0.02f, jb))); *snow = 1;
        if (p.z > -100.0f) { if (ax < 4.45f && p.z > -91.7f && p.z < -84.9f && p.y < QH + 5.05f) { M->alb = V(0.02f, 0.018f, 0.016f); *snow = 0; } }      // le porche : dedans, le noir
        else if (fabsf(n.z) > 0.5f && p.y > 8.45f && p.y < 10.6f && ax < 8.8f) { float sf = 15.5f / (TR[T_COMPTE].w - 80.0f), dt = text2d(T_COMPTE, (n.z < 0 ? p.x : -p.x) / sf + TR[T_COMPTE].w * 0.5f, TR[T_COMPTE].base - (p.y - 9.08f) / sf) * sf;
            M->alb = mul(V(0.40f, 0.38f, 0.34f), 0.9f + 0.2f * vnoise(p.x * 2, p.y * 2)); if (dt < 0) { M->alb = V(0.10f, 0.07f, 0.04f); M->spec = 1.0f; *snow = 0; } }
        break; }
    case W_LECT: M->alb = V(0.20f, 0.11f, 0.06f); if (n.y > 0.4f && p.y > 1.45f) { M->alb = mul(V(0.84f, 0.80f, 0.68f), 0.75f + 0.25f * sstep(0.01f, 0.03f, fabsf(fract(p.z * 12.0f) - 0.5f) / 12.0f)); if (fabsf(p.x - 4.7f) < 0.012f) M->alb = V(0.2f, 0.1f, 0.06f); } break;
    case W_FLAG: if (p.y > QTOP) M->alb = fabsf(p.y - QTOP - 26.5f) < 1.3f ? V(0.85f, 0.62f, 0.20f) : V(0.40f, 0.03f, 0.08f); else M->alb = ((int)roundf(p.z / 12.0f) & 1) ? V(0.36f, 0.03f, 0.07f) : V(0.80f, 0.60f, 0.20f);
        M->sss = 0.9f; M->spec = 0.05f; break;
    case W_LAMP: M->alb = V(0.25f, 0.24f, 0.22f); M->emis = mul(V(15.0f, 9.5f, 3.8f), WLAMP); break;
    case W_CANDLE: M->alb = V(0.85f, 0.80f, 0.68f); M->sss = 0.6f; M->emis = V(0.22f, 0.10f, 0.03f); break;
    case W_FLAME: M->alb = V(0, 0, 0); M->emis = mul(V(18.0f, 9.0f, 2.6f), 0.75f + 0.25f * sinf(T * 11.0f + 40.0f * hash2(roundf(u / 0.6f), roundf((w - 61.7f) / 0.6f)))); break;
    default: break;
    }
}
static void cub_setup(float s) {
    WFAR = 1150.0f; WMTN = 2.6f; MSUNE = 0.42f; WSUN = nrm(V(0.22f, 0.42f, 0.88f)); WSUNC = V(2.5f, 2.1f, 1.7f); WAMBU = V(0.19f, 0.25f, 0.40f); WAMBD = V(0.16f, 0.16f, 0.18f); EXPO = 0.95f; STAR = 0.5f; WLAMP = 1; MPH = T * 3.0f;
    people_pose(0);
    static int init; if (init != 3) { init = 3; WCZ[0] = CZA[6];
        cz_t *C = &WCZ[1]; cz_base(C); C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_SCARF; C->hat = HAT_BOWLER; C->ctop = C->cbot = C->cjack = C->chat = C->chat2 = V(0.022f, 0.022f, 0.028f); C->cacc = V(0.16f, 0.02f, 0.04f); C->leg = 0.5f; C->fat = -0.3f; }
    WN = 2; { pose_t *P = &WPP[0]; cz_rest(&WCZ[0], P); cz_idle(P, T, 0.4f); P->hpitch = 0.12f + 0.10f * sinf(T * 1.6f); P->hand[0] = V(0.20f, 0.42f + 0.02f * sinf(T * 7.0f), 0.36f); w_place(0, 4.7f, 0.5f, -116.6f, PI); }      // le guide, à son registre : il compte
    { pose_t *P = &WPP[1]; float xh = -2.0f - 0.6f * (27.0f - s), xc = 6.0f - 0.6f * (s - 18.0f); cz_rest(&WCZ[1], P); cz_walk(P, 6.2832f * 0.6f * T / 0.68f, 0.9f);        // celui qui refait le tour : seul, en noir, entre les cierges et les rangs ; il nous regarde
        P->hyaw = -atan2f(0.55f, fmaxf(xc - xh, 0.5f)) * ease((s - 19.0f) / 2.0f); P->looky = 0; P->smile = 0.15f; w_place(1, xh, QH, -64.3f, 0.5f * PI); }
    WNL = 4; { static const float GL[4][8] = {{-7.0f, 11.55f, -113.0f, 1.0f, 0.62f, 0.30f, 12.0f, 0.12f}, {7.0f, 11.55f, -113.0f, 1.0f, 0.62f, 0.30f, 12.0f, 0.12f},
                                              {-4.45f, QH + 5.0f, -92.6f, 1.0f, 0.62f, 0.30f, 6.0f, 0.20f}, {4.45f, QH + 5.0f, -92.6f, 1.0f, 0.62f, 0.30f, 6.0f, 0.20f}}; memcpy(WLT, GL, sizeof(GL)); }
    if (s < 9.0f) { float u = ease(s / 9.0f) * 0.6f + s / 22.5f; cam(mix3(V(-175.0f, 63.0f, -480.0f), V(-90.0f, 44.0f, -325.0f), u), mix3(V(0, 62.0f, 0), V(0, 52.0f, 0), u), 1.5f); }                                 // on passe la crête : le Cube
    else if (s < 18.0f) { float u = (s - 9.0f) / 9.0f; cam(V(0.0f, 1.95f, -150.0f + 12.0f * u), V(0, 11.0f + 9.0f * ease(u), -100.0f), 1.25f); FOCUS = 12; APERT = 0.006f; }                                         // dans la file des pèlerins, vers la porte
    else if (s < 27.0f) { float x = 6.0f - 0.6f * (s - 18.0f); cam(V(x, QH + 1.5f, -63.75f), V(x - 10.0f, QH + 1.45f, -65.2f), 1.3f); FOCUS = fmaxf(x - WPX[1], 2.2f); APERT = 0.006f; }                                   // au pied du Cube, entre les cierges et les rangs : il vient vers nous
    else { float u = ease(fminf((s - 27.0f) / 6.8f, 1.0f)), u2 = ease(clampf((s - 32.6f) / 3.4f, 0, 1)), cy = mixf(QH + 2.5f, QTOP + 8.0f, u); v3 c = V(mixf(10.0f, 4.0f, u), cy, -73.0f - u + 24.0f * u2);               // on s'élève le long de la paroi ; au sommet, le drapeau et le soleil
        v3 ta = V(40.0f, QH + 1.0f, -64.0f), tb = V(25.0f, cy - 40.0f, -62.0f), tc = V(10.0f, QTOP + 22.0f, 60.0f);
        cam(c, u < 0.5f ? mix3(ta, tb, ease(u * 2)) : mix3(tb, tc, ease(u * 2 - 1)), 1.25f); }
}

// ================================================================ SÉQUENCE 3 : la taverne à midi
S float tav_inwin(v3 q) {       // ce point voit-il le soleil par une fenêtre ? (on ignore ce qui se trouve entre les deux)
    float tt = (q.x + TVX) / -WSUN.x; if (tt < 0) return 0;
    float y = q.y + WSUN.y * tt, z = q.z + WSUN.z * tt, zl = z - 3.4f * clampf(roundf(z / 3.4f), -1, 1);
    return (fabsf(zl) < 0.62f && fabsf(y - 2.1f) < 0.87f && fabsf(zl) > 0.03f && fabsf(y - 2.1f) > 0.03f) ? 1.0f : 0.0f;
}
static v3 tav_shafts(v3 ro, v3 rd, float tmax, float jit) {       // les rais de soleil dans la poussière de la salle
    float acc = 0, dt = fminf(tmax, 16.0f) / 44.0f;
    for (int i = 0; i < 44; i++) { v3 q = add(ro, mul(rd, (i + jit) * dt));
        if (fabsf(q.x) < TVX && fabsf(q.z) < TVZ && q.y > 0 && q.y < TVH) acc += tav_inwin(q) * (0.55f + 0.9f * vnoise(q.x * 1.3f + q.y + T * 0.06f, q.z * 1.3f - q.y * 0.7f)) * dt; }
    return mul(V(1.0f, 0.86f, 0.62f), acc * 0.20f);
}
static float GSCR;
static void tav_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    float ax = fabsf(p.x), az = fabsf(p.z); int in = ax < TVX + 0.02f && az < TVZ + 0.02f && p.y > -0.01f && p.y < TVH + 0.02f;
    float u = fabsf(n.x) > 0.5f ? p.z : p.x, fd = sstep(13.0f, 4.0f, t);
    switch (m) {
    case W_GROUND: M->alb = mul(WSNW, 0.92f); M->sss = 0.3f; break;
    case W_WALLT:
        if (!in) { M->alb = V(0.62f, 0.58f, 0.50f); *snow = 1; break; }
        if (n.y > 0.5f) { float bx = p.x / 0.24f, id = floorf(bx), fz = p.z / 2.1f + hash2(id, 3.0f) * 7.0f, jb = fminf((0.5f - fabsf(fract(bx) - 0.5f)) * 0.24f, (0.5f - fabsf(fract(fz) - 0.5f)) * 2.1f);       // le plancher
            M->alb = mul(V(0.30f, 0.175f, 0.085f), (0.72f + 0.5f * hash2(id, floorf(fz))) * (0.9f + 0.2f * vnoise(p.x * 45.0f, p.z * 3.0f) * fd) * (1 - 0.55f * fd * sstep(0.012f, 0.004f, jb)));
            M->spec = 0.5f; M->gloss = 40; break; }
        if (n.y < -0.5f) { M->alb = mul(V(0.36f, 0.30f, 0.21f), 0.85f + 0.25f * fbm(p.x * 1.2f, p.z * 1.2f)); break; }
        if (p.x > 6.2f && fabsf(p.z - 0.6f) < 1.3f) {       // la cheminée : de la pierre ; le foyer : la suie, la braise
            float rw = floorf(p.y / 0.32f), uu = (fabsf(n.x) > 0.5f ? p.z : p.x) / 0.55f + 0.5f * rw, jb = fminf((0.5f - fabsf(fract(p.y / 0.32f) - 0.5f)) * 0.32f, (0.5f - fabsf(fract(uu) - 0.5f)) * 0.55f);
            M->alb = mul(V(0.36f, 0.32f, 0.27f), (0.75f + 0.45f * hash2(floorf(uu), rw)) * (1 - 0.5f * fd * sstep(0.02f, 0.006f, jb)));
            if (p.x > 6.42f && fabsf(p.z - 0.6f) < 0.81f && p.y < 1.25f) { M->alb = V(0.03f, 0.025f, 0.02f); M->emis = mul(V(0.9f, 0.28f, 0.05f), (0.5f + 0.3f * sinf(T * 9.0f + p.z * 5.0f)) * sstep(0.9f, 0.0f, p.y)); }
            break; }
        if (p.z < -5.4f && n.z > 0.5f) {                    // le mur de la porte : la porte, deux fenêtres (peintes : le jour)
            if (ax < 0.62f && p.y < 2.2f) { float pn = fminf(fabsf(ax - 0.29f) - 0.2f, fminf(fabsf(p.y - 0.6f) - 0.42f, fabsf(p.y - 1.6f) - 0.42f) > 0 ? 1.0f : -1.0f);
                M->alb = mul(V(0.13f, 0.07f, 0.035f), (0.8f + 0.4f * vnoise(p.x * 30.0f, p.y * 3.0f)) * (pn < 0 ? 0.75f : 1.0f)); if (len2(p.x - 0.48f, p.y - 1.08f) < 0.035f) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; } break; }
            if (fabsf(ax - 3.6f) < 0.72f && fabsf(p.y - 2.1f) < 0.97f) { float wx = fabsf(ax - 3.6f), wy = fabsf(p.y - 2.1f);
                if (wx > 0.62f || wy > 0.87f || wx < 0.024f || wy < 0.024f) M->alb = V(0.10f, 0.058f, 0.03f);
                else { M->alb = V(0.1f, 0.1f, 0.1f); M->emis = mul(mix3(V(1.5f, 1.55f, 1.7f), V(0.85f, 1.1f, 1.7f), sstep(1.4f, 2.9f, p.y)), 1.25f); } break; } }
        if (p.y < 1.1f) { float bx = u / 0.2f; M->alb = mul(V(0.17f, 0.095f, 0.05f), (0.75f + 0.45f * hash2(floorf(bx), 7.0f)) * (1 - 0.5f * fd * sstep(0.012f, 0.004f, (0.5f - fabsf(fract(bx) - 0.5f)) * 0.2f))); M->spec = 0.3f; break; }      // le lambris
        M->alb = mul(V(0.66f, 0.565f, 0.41f), 0.86f + 0.22f * fbm(u * 1.5f, p.y * 1.5f)); break;                                                                                                                              // le plâtre
    case W_BEAM: M->alb = mul(V(0.105f, 0.060f, 0.032f), 0.75f + 0.5f * vnoise(u * 3.0f + p.y * 31.0f, p.z * 2.0f + p.x * 0.7f)); M->spec = 0.25f; break;
    case W_COUNTER: { float px = (p.x + 1.0f) / 0.8f, fx = (0.5f - fabsf(fract(px) - 0.5f)) * 0.8f, fy = fminf(p.y - 0.06f, 0.64f - p.y);
        M->alb = mul(V(0.25f, 0.135f, 0.062f), (0.85f + 0.3f * vnoise(p.x * 3.0f, p.y * 40.0f)) * ((fx < 0.07f || fy < 0.05f) ? 1.0f : 0.72f)); M->spec = 0.5f; M->gloss = 50;
        if (fabsf(p.y - 0.665f) < 0.012f && p.z < 3.03f) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; } break; }
    case W_CTOP: M->alb = mul(V(0.11f, 0.055f, 0.028f), 0.8f + 0.4f * vnoise(p.x * 4.0f, p.z * 50.0f)); M->spec = 1.0f; M->gloss = 90; M->env = 0.035f; break;
    case W_BRASS: M->alb = V(1.0f, 0.72f, 0.28f); M->env = 1; M->spec = 1.6f; M->gloss = 150; break;
    case W_HANDLE: M->alb = p.y > CTY + 0.385f ? V(0.36f, 0.03f, 0.07f) : V(0.02f, 0.02f, 0.02f); M->spec = 1.2f; M->gloss = 90; break;
    case W_TABLE: { float rb = 1e9f; for (int k = 0; k < NTB; k++) rb = fminf(rb, len2(p.x - TVTX[k], p.z - TVTZ[k]));
        M->alb = mul(V(0.27f, 0.15f, 0.07f), 0.8f + 0.35f * vnoise(p.x * 9.0f + p.y * 20.0f, p.z * 9.0f)); if (n.y > 0.5f && p.y > 0.5f) M->alb = mul(M->alb, 0.9f + 0.18f * sinf(rb * 70.0f + 3.0f * vnoise(p.x * 5.0f, p.z * 5.0f)));
        M->spec = 0.7f; M->gloss = 60; break; }
    case W_BOTTLE: { float k = clampf(roundf((p.y - 1.5f) / 0.5f), 0, 2), yl = p.y - 1.5f - 0.5f * k, xc = 0.2f * roundf(p.x / 0.2f), h = hash2(xc * 3.7f + 1.1f, k * 5.3f + 2.2f), h2 = fract(h * 3.3f), hh = 0.09f + 0.05f * fract(h * 13.1f);
        M->alb = h2 < 0.4f ? V(0.03f, 0.15f, 0.06f) : h2 < 0.75f ? V(0.18f, 0.08f, 0.03f) : V(0.55f, 0.27f, 0.04f); if (h2 >= 0.75f) M->emis = V(0.10f, 0.04f, 0.0f);
        if (fract(h * 23.0f) < 0.6f && yl > 0.5f * hh && yl < 1.3f * hh) M->alb = V(0.78f, 0.72f, 0.56f);
        M->spec = 1.6f; M->gloss = 120; break; }
    case W_BARREL: { static const float BX[3] = {4.7f, 6.0f, 5.35f}, BY[3] = {0.66f, 0.66f, 1.62f}; int ib = 0; float rb = 1e9f; for (int i = 0; i < 3; i++) { float e = len2(p.x - BX[i], p.y - BY[i]); if (e < rb) { rb = e; ib = i; } }
        float qz = p.z - 4.75f, ang = atan2f(p.y - BY[ib], p.x - BX[ib]) * 2.2282f, st = floorf(ang);
        M->alb = mul(V(0.36f, 0.215f, 0.105f), (0.75f + 0.4f * hash2(st, ib * 3.0f)) * (1 - 0.4f * fd * sstep(0.08f, 0.02f, 0.5f - fabsf(fract(ang) - 0.5f)))); M->spec = 0.3f;
        if (fabsf(fabsf(qz) - 0.30f) < 0.035f || fabsf(fabsf(qz) - 0.58f) < 0.04f) { M->alb = V(0.05f, 0.05f, 0.055f); M->spec = 1.0f; M->gloss = 60; }
        if (fabsf(qz) > 0.635f) { M->alb = mul(V(0.30f, 0.18f, 0.09f), 0.85f + 0.15f * sinf(rb * 60.0f)); if (rb < 0.2f && rb > 0.17f) M->alb = V(0.36f, 0.03f, 0.07f); }
        break; }
    case W_HAM: { static const float HX[4] = {-3.2f, -1.7f, 0.7f, 2.3f}; float xb = 1e9f; for (int i = 0; i < 4; i++) if (fabsf(p.x - HX[i]) < fabsf(xb)) xb = p.x - HX[i];
        float qy = p.y - 2.98f, ang = atan2f(p.z - 1.6f, xb), l1 = fabsf(fract(ang * 1.9f + qy * 7.0f) - 0.5f), l2 = fabsf(fract(ang * 1.9f - qy * 7.0f) - 0.5f);
        M->alb = mix3(V(0.50f, 0.20f, 0.13f), V(0.62f, 0.36f, 0.17f), sstep(-0.1f, 0.2f, qy + 0.1f * vnoise(ang * 3.0f, qy * 6.0f)));
        if (fminf(l1, l2) < 0.06f && qy < 0.2f) M->alb = V(0.80f, 0.74f, 0.60f); if (qy > 0.24f) M->alb = V(0.70f, 0.60f, 0.42f);
        M->sss = 0.5f; M->spec = 0.6f; M->gloss = 40; break; }
    case W_SAUS: { float xc = 0.21f * roundf(p.x / 0.21f); M->alb = mul(V(0.42f, 0.12f, 0.07f), 0.8f + 0.4f * hash2(xc, 1.0f)); if (fabsf(p.x - xc) > 0.08f) M->alb = V(0.75f, 0.68f, 0.55f); M->spec = 1.0f; M->gloss = 60; M->sss = 0.4f; break; }
    case W_FIRE: M->alb = V(0, 0, 0); M->emis = mul(mix3(V(11.0f, 7.0f, 2.2f), V(8.0f, 2.2f, 0.3f), sstep(0.25f, 0.75f, p.y)), 0.8f + 0.2f * sinf(T * 13.0f + p.z * 9.0f)); break;
    case W_CLOCK: { float dy = p.y - 2.75f, dz = p.z - 0.6f, r = len2(dy, dz), a = atan2f(dz, dy) * 1.9099f;
        if (r > 0.315f || n.x > -0.5f) { M->alb = V(1.0f, 0.72f, 0.28f); M->env = 1; M->spec = 1.6f; M->gloss = 150; break; }
        M->alb = V(0.86f, 0.82f, 0.70f); M->emis = V(0.05f, 0.045f, 0.035f);
        if (r > 0.24f && r < 0.295f && fabsf(fract(a) - 0.5f) > 0.5f - 0.035f / fmaxf(r, 0.1f)) M->alb = V(0.03f, 0.03f, 0.03f);
        if ((fabsf(dz) < 0.017f && dy > -0.03f && dy < 0.16f) || (fabsf(dz) < 0.010f && dy > -0.03f && dy < 0.265f) || r < 0.028f) { M->alb = V(0.03f, 0.03f, 0.03f); M->emis = V(0, 0, 0); }      // midi : les deux aiguilles en haut
        break; }
    case W_SIGN: { float uu = p.x + 1.0f, v = p.y - 3.3f; M->alb = V(0.30f, 0.03f, 0.06f); M->spec = 0.4f;
        if (fabsf(uu) > 2.31f || fabsf(v) > 0.235f) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; }
        else if (n.z < -0.5f) { float sf = 3.9f / (TR[T_BRASS].w - 80.0f), dt = text2d(T_BRASS, uu / sf + TR[T_BRASS].w * 0.5f, TR[T_BRASS].base - (v + 0.135f) / sf) * sf; if (dt < 0) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 1.6f; M->gloss = 150; } }
        break; }
    case W_BOARD: { float uu = p.x + 5.75f, v = p.y - 2.1f; M->alb = mul(V(0.045f, 0.06f, 0.055f), 0.8f + 0.5f * vnoise(p.x * 6.0f, p.y * 6.0f)); M->spec = 0.1f;
        if (fabsf(uu) > 0.70f || fabsf(v) > 0.54f) { M->alb = V(0.20f, 0.11f, 0.05f); break; }
        if (n.z < -0.5f) { float sf = 1.24f / (TR[T_AMBREE].w - 80.0f), dt = text2d(T_AMBREE, uu / sf + TR[T_AMBREE].w * 0.5f, TR[T_AMBREE].base - (v - 0.30f) / sf) * sf, ch = 0;
            if (dt < 0) ch = 1;
            for (int i = 0; i < 3; i++) { float vy = 0.12f - 0.14f * i; if (fabsf(v - vy - 0.006f * sinf(uu * 23.0f + i)) < 0.011f && uu > -0.58f && uu < 0.05f + 0.12f * i && vnoise(uu * 28.0f, i * 3.0f) > 0.3f) ch = 0.8f; }
            { float mx = uu - 0.42f, my = v + 0.24f, bx = box2(fabsf(mx) - 0.085f, fabsf(my) - 0.13f); if (fabsf(bx) < 0.010f || fabsf(len2(mx - 0.11f, my) - 0.055f) < 0.009f && mx > 0.09f || (my > 0.13f && my < 0.19f && fabsf(mx) < 0.10f && vnoise(mx * 40.0f, my * 40.0f) > 0.45f)) ch = 0.9f; }
            if (ch > 0) M->alb = mul(V(0.86f, 0.86f, 0.80f), ch * (0.75f + 0.25f * vnoise(p.x * 90.0f, p.y * 90.0f))); }
        break; }
    case W_HFOAM: M->alb = mul(V(0.96f, 0.93f, 0.84f), 0.9f + 0.1f * vnoise(p.x * 160.0f + p.y * 90.0f, p.z * 160.0f)); M->sss = 0.7f; M->spec = 0.1f; break;
    case W_BEERS: M->alb = V(0.85f, 0.45f, 0.05f); M->emis = V(0.75f, 0.33f, 0.03f); M->spec = 1.2f; M->gloss = 100; break;
    case W_LAMP: M->alb = V(0.25f, 0.24f, 0.22f); M->emis = mul(V(15.0f, 9.5f, 3.8f), WLAMP); break;
    default: break;
    }
}
static v3 tav_mugshade(v3 ro, v3 rd, v3 p, v3 n, v3 L, v3 bc) {       // la chope de verre : ce qu'on voit derrière, la bière, la mousse, les bulles
    v3 g = V(p.x - HMX, p.y - CTY, p.z - HMZ), rf = refl(rd, n); float rr = len2(g.x, g.z), nd = fabsf(dot(rd, n)), fres = powf(1 - nd, 3), yl = 0.012f + 0.15f * HLEV, fth = 0.028f * sstep(0.05f, 0.5f, HLEV);
    float hl = 1.6f * powf(fmaxf(dot(rf, nrm(V(-0.85f, 0.35f, -0.4f))), 0), 50.0f) + 0.5f * powf(fmaxf(dot(rf, nrm(V(0.6f, 0.5f, -0.6f))), 0), 30.0f); v3 col;
    if (rr < 0.083f && g.y > 0.010f && g.y < yl) {             // la bière
        v3 tr = mix3(V(1.0f, 0.62f, 0.18f), V(0.78f, 0.32f, 0.035f), powf(nd, 0.6f));
        col = add(mul(mulv(bc, tr), 0.55f), mulv(V(0.55f, 0.235f, 0.03f), mul(L, 0.85f)));
        for (int j = 0; j < 2; j++) { v3 q = add(p, mul(rd, 0.012f + 0.022f * j)); float ua = atan2f(q.z - HMZ, q.x - HMX) * 0.07f / 0.011f, vy = (q.y - T * (0.045f + 0.02f * j)) / 0.011f, h = hash2(floorf(ua) + 17.0f * j, floorf(vy));
            if (h < 0.10f) col = add(col, mul(V(1.0f, 0.85f, 0.5f), 0.5f * sstep(0.38f, 0.15f, len2(fract(ua) - 0.5f, fract(vy) - 0.5f)))); }
    } else if (rr < 0.083f && g.y >= yl && g.y < yl + fth) col = mulv(mul(V(0.96f, 0.93f, 0.84f), 0.9f + 0.1f * vnoise(p.x * 200.0f, p.y * 200.0f + p.z * 130.0f)), L);      // le col de mousse
    else col = add(mulv(bc, V(0.92f, 0.95f, 0.94f)), mul(V(0.55f, 0.45f, 0.33f), 0.5f * fres));                                                                       // le verre vide
    if (g.y > 0.172f && rr < 0.083f) col = add(col, V(0.25f, 0.24f, 0.22f));
    return add(col, mul(V(1.0f, 0.95f, 0.85f), hl));
}
static v3 tav_deep(v3 rd) {       // le fond du verre : un abîme ambré, des rais qui descendent, très loin une lueur
    v3 c = mix3(V(0.60f, 0.26f, 0.03f), V(0.07f, 0.022f, 0.002f), sstep(0.45f, -0.75f, rd.y));
    c = add(c, mul(V(0.95f, 0.62f, 0.20f), 0.30f * powf(fmaxf(rd.y, 0), 1.5f) * (0.55f + 0.45f * sinf(atan2f(rd.z, rd.x) * 9.0f + T * 0.5f + 2.0f * sinf(T * 0.3f)))));
    float sd = fmaxf(dot(rd, nrm(V(0.18f, -0.62f, 0.76f))), 0);
    return add(c, mul(V(1.0f, 0.88f, 0.55f), (2.5f * powf(sd, 900.0f) + 0.22f * powf(sd, 40.0f)) * sstep(0.55f, 1.0f, TDEEP)));
}
static void tav_setup(float s) {
    WFAR = 60.0f; WMTN = 1.55f; MSUNE = 0.75f; WSUN = nrm(V(-0.80f, 0.50f, 0.30f)); WSUNC = V(3.4f, 2.9f, 2.2f); WAMBU = V(0.21f, 0.17f, 0.125f); WAMBD = V(0.115f, 0.085f, 0.06f); EXPO = 1.0f; STAR = 0.35f; WLAMP = 0.5f;
    HMUG = 1; HLEV = 0; HFOAM = 0; HPOUR = 0; HTAP = 0; TBEER = 0; TDEEP = 0;
    static int init; if (init != 4) { init = 4; static const int AR[14] = {9, 3, 2, 0, 1, 10, 4, 8, 7, 2, 0, 4, 10, 8};
        for (int k = 0; k < 14; k++) { WCZ[k] = CZA[AR[k]]; WCZ[k].prop = k ? PROP_MUG : PROP_NONE; }
        WCZ[9].ctop2 = V(0.36f, 0.03f, 0.07f); WCZ[9].cbot = V(0.16f, 0.13f, 0.10f); WCZ[9].hat = HAT_BERET; WCZ[9].chat = V(0.035f, 0.055f, 0.17f);
        WCZ[10].cjack = V(0.04f, 0.17f, 0.10f); WCZ[10].hat = HAT_CAP; WCZ[10].chat = V(0.16f, 0.17f, 0.19f);
        WCZ[11].cjack = V(0.33f, 0.03f, 0.07f); WCZ[11].hat = HAT_BOATER; WCZ[11].chat = V(0.82f, 0.64f, 0.32f);
        WCZ[12].cjack = V(0.15f, 0.09f, 0.06f); WCZ[12].hat = HAT_BOWLER; WCZ[12].chat = V(0.05f, 0.05f, 0.05f);
        WCZ[13].cjack = V(0.035f, 0.055f, 0.17f); WCZ[13].chat = V(0.80f, 0.48f, 0.08f); }
    WN = 14; const float KX = -0.82f, KY = 0.26f, KZ = 3.93f, HIP = 0.336f;
    // ---- la chope du comptoir
    if (s < 18.0f) { HTAP = ease((s - 10.0f) / 0.5f) * (1 - ease((s - 15.2f) / 0.4f)); HPOUR = ease((s - 10.35f) / 0.3f) * (1 - ease((s - 15.25f) / 0.25f));
        HLEV = ease(clampf((s - 10.5f) / 4.7f, 0, 1) * 0.5f) * 2.0f; HLEV = clampf(HLEV, 0, 1); HFOAM = sstep(0.82f, 1.0f, HLEV) * (1 - 0.3f * ease((s - 15.6f) / 2.0f)); }
    else if (s < 27.0f) HMUG = 0;
    else { HLEV = 1; HFOAM = 0.7f; }
    // ---- les gens
    for (int k = 0; k < WN; k++) { pose_t *P = &WPP[k]; cz_rest(&WCZ[k], P); cz_idle(P, T, 0.13f + 0.071f * k); float x, y = 0, z, yaw;
        if (k == 0) { x = KX; y = KY; z = KZ; yaw = PI; }
        else if (k == 1) { x = 1.3f; z = 2.52f; yaw = 0; }
        else if (k == 2) { x = 2.15f; z = 2.56f; yaw = -0.5f; }
        else { int tb = k < 6 ? 0 : k < 9 ? 1 : k < 12 ? 2 : 3, j = k - (tb == 0 ? 3 : tb == 1 ? 6 : tb == 2 ? 9 : 12); float a = TVSA[tb][j]; x = TVTX[tb] + 0.95f * sinf(a); z = TVTZ[tb] + 0.95f * cosf(a); yaw = a + PI;
            P->bob -= 0.03f; P->foot[0].z = P->foot[1].z = 0.15f; P->lean += 0.04f; }
        if (k) {      // les buveurs : la chope posée, puis levée, puis bue
            float hy = (k < 3 ? CTY : 0.62f) + 0.052f - HIP - P->bob; P->hand[0] = V(0.24f, hy, k < 3 ? 0.30f : 0.29f);
            float chat = sinf(T * 0.7f + k * 2.3f); P->hyaw += 0.35f * chat; P->open = fmaxf(0, sinf(T * 7.0f + k * 1.7f)) * 0.35f * sstep(0.2f, 0.7f, sinf(T * 0.45f + k * 1.9f));
            float sip = s < 18.0f ? sstep(0.0f, 0.6f, fract((T + k * 3.7f) / 11.0f) * 11.0f - 9.0f) * sstep(11.0f, 10.4f, fract((T + k * 3.7f) / 11.0f) * 11.0f) : 0;         // une gorgée de temps en temps
            float kc = s >= 18.0f && s < 27.0f ? ease((s - 21.6f - 0.06f * k) / 0.5f) * (1 - ease((s - 24.5f - 0.04f * k) / 0.6f)) : 0, kd = s >= 18.0f && s < 27.0f ? ease((s - 25.1f - 0.05f * k) / 0.5f) : 0;
            if (s >= 18.0f && s < 27.0f) { float want = atan2f(KX - x, KZ - z) - yaw; want = atan2f(sinf(want), cosf(want)); P->hyaw = mixf(P->hyaw, clampf(want, -1.3f, 1.3f), ease((s - 20.6f) / 0.8f) * (1 - kd)); }
            P->hand[0] = mix3(P->hand[0], V(0.30f, 0.80f, 0.20f), kc); if (kc > 0.3f) { P->smile = 1; P->open = 0.5f * kc; }
            kd = fmaxf(kd, sip); P->hand[0] = mix3(P->hand[0], V(0.10f, 0.63f, 0.20f), kd); P->hpitch -= 0.28f * kd; if (kd > 0.5f) P->open = 0; }
        else {        // le tavernier
            v3 wipe = V(0.20f + 0.07f * sinf(T * 3.0f), CTY + 0.03f - KY - HIP, 0.30f + 0.04f * cosf(T * 3.0f));
            if (s < 18.0f) { float a = 0.15f + 0.65f * HTAP, k1 = ease((s - 8.4f) / 1.2f) * (1 - ease((s - 15.7f) / 0.8f)); v3 hd = V(-(HMX - KX), CTY + 0.27f + 0.15f * cosf(a) - KY - HIP, KZ - (3.58f - 0.02f + 0.15f * sinf(a)));
                P->hand[1] = mix3(P->hand[1], hd, k1); P->hand[0] = mix3(wipe, V(0.30f, 0.17f, 0.12f), ease((s - 8.0f) / 1.0f)); P->hpitch += 0.22f * k1; P->hyaw = mixf(P->hyaw, 0.25f, k1);
                float pr = ease((s - 16.2f) / 0.6f); P->smile = mixf(0.6f, 1.0f, pr); P->hpitch -= 0.05f * pr; }
            else if (s < 27.0f) { WCZ[0].prop = PROP_MUG; float kc = ease((s - 20.6f) / 0.6f) * (1 - ease((s - 24.4f) / 0.6f)), kd = ease((s - 25.0f) / 0.5f);
                P->hand[0] = mix3(mix3(V(0.22f, CTY + 0.052f - KY - HIP, 0.33f), V(0.32f, 0.86f, 0.18f), kc), V(0.10f, 0.63f, 0.20f), kd); P->smile = 1; P->open = 0.6f * kc; P->hpitch -= 0.1f * kc + 0.28f * kd; }
            else P->hand[0] = wipe;
            if (s < 18.0f || s >= 27.0f) WCZ[0].prop = PROP_NONE; }
        w_place(k, x, y, z, yaw); }
    // ---- les lumières : trois lanternes, le feu, le jour des fenêtres
    WNL = 5; { static const float GL[5][8] = {{-3.6f, 2.9f, -1.6f, 1.0f, 0.62f, 0.30f, 1.5f, 0.30f}, {2.9f, 2.9f, -1.6f, 1.0f, 0.62f, 0.30f, 1.5f, 0.30f}, {0.3f, 2.9f, 0.0f, 1.0f, 0.62f, 0.30f, 1.5f, 0.30f},
                                              {6.2f, 0.6f, 0.6f, 1.0f, 0.42f, 0.10f, 3.2f, 0.22f}, {-5.6f, 2.1f, 0.0f, 0.62f, 0.74f, 1.0f, 2.3f, 0.06f}}; memcpy(WLT, GL, sizeof(GL));
        WLT[3][6] = 3.2f * (0.8f + 0.2f * sinf(T * 11.0f) * sinf(T * 4.3f + 1.0f)); }
    // ---- les caméras
    if (s < 9.0f) { float u = ease(s / 9.0f); cam(mix3(V(0.95f, 2.55f, -5.0f), V(0.80f, 1.32f, -0.5f), u), mix3(V(-2.6f, 1.5f, 0.5f), V(-0.75f, 1.02f, 3.6f), u), 1.25f); FOCUS = 6; APERT = 0.004f; }       // on entre : la salle, le soleil de midi
    else if (s < 18.0f) { float u = (s - 9.0f) / 9.0f; cam(V(-0.30f + 0.06f * u, 1.02f, 2.56f + 0.06f * u), V(-0.62f, 1.11f, 3.5f), 1.5f); FOCUS = 0.82f; APERT = 0.008f; }                              // au comptoir : la chope tirée
    else if (s < 27.0f) { float u = (s - 18.0f) / 9.0f; cam(V(-3.7f + 0.5f * u, 1.85f, 4.72f), V(1.0f, 0.75f, -1.0f), 1.15f); FOCUS = 5; APERT = 0.004f; }                                            // derrière le comptoir : toute la salle lève sa chope
    else { float u = ease((s - 27.0f) / 5.0f), dv = ease((s - 32.3f) / 3.0f); v3 c0 = V(0.15f, 0.99f, 2.45f), c1 = V(HMX + 0.03f, CTY + 0.105f, HMZ - 0.105f), c = mix3(c0, c1, u);                    // on plonge dans la chope
        c = add(c, V(-0.03f * dv, -0.05f * dv, 0.09f * dv)); TBEER = ease((s - 31.9f) / 0.4f); TDEEP = ease((s - 33.0f) / 1.6f); if (s > 31.85f) HMUG = 0;
        cam(c, add(mix3(V(HMX, CTY + 0.10f, HMZ), V(HMX - 0.05f, CTY + 0.08f, HMZ + 1.0f), ease((s - 30.0f) / 2.0f)), V(0, -1.3f * dv, 0)), mixf(1.6f, 1.2f, u)); FOCUS = fmaxf(len(sub(c, V(HMX, CTY + 0.1f, HMZ))) - 0.06f, 0.05f); APERT = (0.008f * (1 - u) + 0.0012f) * (1 - TBEER); }
}

// ================================================================ SÉQUENCE 4 : la nuit du Gras-Fond
static const v3 GPAL[4] = {{1.0f, 0.12f, 0.55f}, {0.10f, 0.75f, 1.0f}, {1.0f, 0.55f, 0.08f}, {0.25f, 1.0f, 0.30f}};
static v3 GBD;       // la direction du projecteur d'étrave
static v3 gf_sky(v3 rd) {       // la bière vue du fond : un abîme ambré, des rais qui descendent de la surface
    v3 c = mix3(V(0.20f, 0.075f, 0.008f), V(0.016f, 0.005f, 0.001f), sstep(0.6f, -0.5f, rd.y));
    return add(c, mul(V(0.95f, 0.62f, 0.20f), 0.16f * powf(fmaxf(rd.y, 0), 1.5f) * (0.55f + 0.45f * sinf(atan2f(rd.z, rd.x) * 9.0f + T * 0.4f + 2.0f * sinf(T * 0.25f)))));
}
S float gf_caust(float x, float z) { float a = sinf(x * 2.9f + 1.5f * sinf(z * 2.3f + T * 0.7f)) * sinf(z * 2.6f + 1.5f * sinf(x * 2.1f - T * 0.6f)); return 0.45f + 0.55f * powf(fabsf(a), 0.8f); }
S float gf_beam(v3 ro, v3 rd, float tmax, v3 b0, v3 bd, float r0, float spread, float jit) {       // un faisceau dans la brume : ce que le rayon en traverse
    float acc = 0, dt = fminf(tmax, 26.0f) / 28.0f;
    for (int i = 0; i < 28; i++) { v3 v = sub(add(ro, mul(rd, (i + jit) * dt)), b0); float al = dot(v, bd); if (al < 0.05f) continue;
        v3 pr = sub(v, mul(bd, al)); float cr = r0 + al * spread; acc += expf(-2.0f * dot(pr, pr) / (cr * cr)) / (1 + 0.05f * al * al) * dt * (r0 / cr); }
    return acc;
}
S float gf_dots(v3 p) {       // les éclats de la boule à facettes sur les parois
    v3 d = nrm(sub(p, V(0, 2.0f, -2.5f))); float u = (atan2f(d.z, d.x) + T * 0.35f) * 10.0f, v = asinf(clampf(d.y, -1, 1)) * 10.0f;
    return hash2(floorf(u), floorf(v)) < 0.4f ? sstep(0.26f, 0.10f, len2(fract(u) - 0.5f, fract(v) - 0.5f)) : 0;
}
S float seg7(int dg, float u, float v) {       // un chiffre à sept segments dans un rectangle de 1 sur 2 (u : 0 → 1, v : 0 → 2)
    static const unsigned char SG[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x6F}; int on = 0, sg = SG[dg];
    float t = 0.22f, hu = fabsf(u - 0.5f);
    if (hu < 0.32f) { if ((sg & 1) && fabsf(v - 1.92f) < t * 0.5f) on = 1; if ((sg & 64) && fabsf(v - 1.0f) < t * 0.5f) on = 1; if ((sg & 8) && fabsf(v - 0.08f) < t * 0.5f) on = 1; }
    if (fabsf(u - 0.92f) < t * 0.5f) { if ((sg & 2) && v > 1.08f && v < 1.84f) on = 1; if ((sg & 4) && v > 0.16f && v < 0.92f) on = 1; }
    if (fabsf(u - 0.08f) < t * 0.5f) { if ((sg & 32) && v > 1.08f && v < 1.84f) on = 1; if ((sg & 16) && v > 0.16f && v < 0.92f) on = 1; }
    return on;
}
static void gf_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    float ax = fabsf(p.x), r = len2(p.x, p.y), fd = sstep(40.0f, 10.0f, t), bi = floorf(T / 0.5625f);
    switch (m) {
    case W_SEABED: M->alb = mul(V(0.36f, 0.21f, 0.075f), (0.7f + 0.5f * fbm(p.x * 0.35f, p.z * 0.35f)) * (0.85f + 0.3f * vnoise(p.x * 4.0f, p.z * 4.0f) * fd) * (0.55f + 0.9f * gf_caust(p.x, p.z))); M->spec = 0.1f; break;
    case W_HULL: { float zr = (0.5f - fabsf(fract(p.z / 2.0f) - 0.5f)) * 2.0f, ang = atan2f(p.y, ax) * 3.2f, ar = (0.5f - fabsf(fract(ang * 1.5f) - 0.5f)) / 1.5f * 2.1f, seam = fminf(zr, ar);
        M->alb = mul(p.y < -0.85f ? V(0.07f, 0.018f, 0.028f) : V(0.30f, 0.035f, 0.07f), (0.85f + 0.25f * vnoise(p.z * 0.8f, ang * 3.0f)) * (1 - 0.45f * fd * sstep(0.035f, 0.012f, seam)));
        if (seam < 0.14f && seam > 0.06f && len2(fract(p.z / 0.25f) - 0.5f, fract(ang * 6.0f) - 0.5f) < 0.2f && fd > 0.3f) M->alb = mul(M->alb, 1.5f);      // les rivets
        if (fabsf(p.y + 0.78f) < 0.07f) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; }
        if (ax > 2.3f && p.y > 1.35f && p.y < 2.35f && p.z > 4.0f && p.z < 12.2f) { float sf = 7.4f / (TR[T_SUB].w - 80.0f), dt = text2d(T_SUB, ((p.x > 0 ? p.z : 16.2f - p.z) - 8.1f) / sf + TR[T_SUB].w * 0.5f, TR[T_SUB].base - (p.y - 1.52f) / sf) * sf;
            if (dt < 0) { M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 1.6f; M->gloss = 150; } }
        M->spec = fmaxf(M->spec, 0.8f); M->gloss = 60; if (n.y > 0.3f) M->alb = mul(M->alb, 0.7f + 0.6f * gf_caust(p.x, p.z)); break; }
    case W_SAIL: M->alb = mul(V(0.060f, 0.064f, 0.075f), 0.8f + 0.4f * vnoise(p.x * 2.0f + p.y * 3.0f, p.z * 2.0f)); M->spec = 0.9f; M->gloss = 50; if (n.y > 0.3f) M->alb = mul(M->alb, 0.7f + 0.6f * gf_caust(p.x, p.z)); break;
    case W_BRASS: M->alb = V(1.0f, 0.72f, 0.28f); M->env = 1; M->spec = 1.6f; M->gloss = 150; break;
    case W_PORT: { float iz = clampf(roundf((p.z - 1.0f) / 3.0f), -3, 3), zl = p.z - 1.0f - 3.0f * iz, yy = p.y - 0.7f; int ci = ((int)(iz + 3 + bi) + (p.x > 0 ? 0 : 2)) & 3;       // un hublot : la fête à bord
        v3 c = mul(GPAL[ci], 1.6f + 3.5f * GBEAT); float hd = len2(zl - 0.12f * sinf(T * 5.6f + iz * 2.0f), yy + 0.36f - 0.07f * GBEAT), ea = len2(fabsf(zl - 0.12f * sinf(T * 5.6f + iz * 2.0f)) - 0.17f, yy + 0.12f - 0.07f * GBEAT);
        if (hd < 0.24f || ea < 0.08f) c = mul(c, 0.12f); M->alb = V(0.02f, 0.02f, 0.02f); M->emis = c; M->spec = 1.5f; M->gloss = 120; break; }
    case W_LIGHT: M->alb = V(0, 0, 0); M->emis = V(22.0f, 18.0f, 11.0f); break;
    case W_CAP: { v3 q0 = V(p.x - 11.0f, p.y - GSY - 1.25f, p.z + 8.0f); float c = cosf(0.55f), sn = sinf(0.55f), qx = q0.x * c + q0.y * sn, qy = -q0.x * sn + q0.y * c, rr = len2(qx, q0.z);
        M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 1.6f; M->gloss = 150;
        if (qy > 0.05f && rr < 2.9f) { M->env = 0; M->alb = (fabsf(rr - 2.2f) < 0.12f || rr < 0.5f) ? V(0.85f, 0.62f, 0.2f) : V(0.33f, 0.03f, 0.07f); M->spec = 1.0f; M->alb = mul(M->alb, 0.7f + 0.6f * gf_caust(p.x, p.z)); }
        if (qy < -0.5f) { M->env = 0; M->alb = V(0.45f, 0.36f, 0.24f); } break; }
    // ---- à bord
    case W_HULLIN: case W_BULK: case W_RIB: case W_DECK: { float dots = gf_dots(p) * GDROP;
        if (m == W_HULLIN) { float zr = (0.5f - fabsf(fract(p.z / 1.0f) - 0.5f)) * 1.0f; M->alb = mul(V(0.055f, 0.065f, 0.078f), (0.8f + 0.3f * vnoise(p.z * 1.3f, atan2f(p.y, p.x) * 4.0f)) * (1 - 0.4f * sstep(0.02f, 0.008f, zr))); M->spec = 0.6f; M->gloss = 30;
            float zl = p.z - 1.0f - 3.0f * clampf(roundf((p.z - 1.0f) / 3.0f), -3, 2), rr = len2(zl, p.y - 0.5f);
            if (rr < 0.5f && r > 2.5f) { float vy = (p.y - T * 0.25f) / 0.09f, ua = zl / 0.09f; M->alb = V(0.02f, 0.01f, 0.0f); M->emis = mul(V(0.42f, 0.17f, 0.02f), 0.8f + 0.5f * (0.5f - rr) + (hash2(floorf(ua), floorf(vy)) < 0.12f ? 1.2f * sstep(0.4f, 0.15f, len2(fract(ua) - 0.5f, fract(vy) - 0.5f)) : 0)); dots = 0; } }
        else if (m == W_RIB) { M->alb = V(0.10f, 0.105f, 0.12f); M->spec = 0.9f; M->gloss = 40; }
        else if (m == W_BULK) { float rr = len2(p.x, p.y - 0.1f); M->alb = mul(V(0.09f, 0.10f, 0.115f), 0.85f + 0.3f * vnoise(p.x * 2.0f, p.y * 2.0f));
            if (p.z < -11.0f) { if (fabsf(rr - 1.25f) < 0.07f) M->alb = V(0.22f, 0.23f, 0.25f); if (fabsf(rr - 0.42f) < 0.035f || (rr < 0.42f && fminf(fabsf(p.x), fabsf(p.y - 0.1f)) < 0.03f)) { M->alb = V(0.55f, 0.06f, 0.08f); M->spec = 1.0f; } } }
        else { float u = p.x / 0.14f, v = p.z / 0.14f, dm = fminf(fabsf(fract(u + v) - 0.5f), fabsf(fract(u - v) - 0.5f)); M->alb = mul(V(0.075f, 0.08f, 0.09f), 1 + 0.5f * sstep(0.08f, 0.03f, dm) * sstep(18.0f, 5.0f, t)); M->spec = 0.9f; M->gloss = 40; }
        M->emis = add(M->emis, mul(V(1.0f, 0.95f, 0.85f), 0.55f * dots)); break; }
    case W_PIPE: M->alb = V(0.85f, 0.45f, 0.22f); M->env = 1; M->spec = 1.4f; M->gloss = 100; break;
    case W_DFLOOR: { float ix = floorf(p.x / 0.7f), iz = floorf(p.z / 0.7f), ed = fminf(0.5f - fabsf(fract(p.x / 0.7f) - 0.5f), 0.5f - fabsf(fract(p.z / 0.7f) - 0.5f)) * 0.7f, h = hash2(ix * 3.1f + bi * 1.7f, iz * 5.3f + bi * 0.9f);
        v3 c = GPAL[(int)(h * 3.999f)]; float on = fract(h * 7.7f) < 0.25f + 0.55f * GDROP ? 1.0f : 0.08f;
        M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 1.2f; M->gloss = 90; if (ed > 0.03f && n.y > 0.5f) M->emis = mul(c, on * (0.45f + 1.5f * GBEAT) * (0.75f + 0.25f * sstep(0.03f, 0.3f, ed))); break; }
    case W_CONSOLE: M->alb = V(0.022f, 0.022f, 0.026f); M->spec = 0.8f; M->gloss = 60;
        if (n.z < -0.5f && fabsf(p.x) < 1.2f) { float cu = p.x / 0.085f, ci = floorf(cu), lv = 0.15f + 0.8f * hash2(ci * 2.3f, floorf(T / 0.140625f) * 1.1f) * (0.35f + 0.65f * GBEAT), v = (p.y - GFL - 0.42f) / 0.44f;      // les vumètres
            if (fabsf(fract(cu) - 0.5f) < 0.36f && v > 0 && v < lv && fract(v * 11.0f) > 0.25f) M->emis = mul(v > 0.75f ? V(1.0f, 0.1f, 0.05f) : v > 0.5f ? V(1.0f, 0.7f, 0.05f) : V(0.1f, 1.0f, 0.25f), 2.2f); }
        break;
    case W_VINYL: { float sx = ax - 0.38f, sz = p.z - 7.60f, rr = len2(sx, sz), a = atan2f(sz, sx) + (p.x > 0 ? 1.0f : -1.0f) * T * 3.49f + (p.x > 0 ? GSCR : 0);
        M->alb = V(0.012f, 0.012f, 0.014f); M->spec = 0.6f + 2.5f * powf(fabsf(sinf(a)), 24.0f); M->gloss = 30; M->alb = mul(M->alb, 1.0f + 6.0f * powf(fabsf(sinf(atan2f(sz, sx) * 1.0f + 0.7f)), 18.0f) + 0.8f * sinf(rr * 420.0f));
        if (rr < 0.085f) { M->alb = fabsf(fract(a / 6.2832f) - 0.5f) < 0.07f ? V(0.9f, 0.7f, 0.25f) : V(0.42f, 0.04f, 0.09f); M->spec = 0.3f; } if (rr < 0.012f || rr > 0.255f) { M->alb = V(0.5f, 0.5f, 0.52f); M->spec = 1.5f; } break; }
    case W_MIXER: { float u = p.x / 0.045f, v = p.z / 0.045f; M->alb = V(0.05f, 0.05f, 0.055f); M->spec = 0.6f;
        if (n.y > 0.5f) { if (len2(fract(u) - 0.5f, fract(v) - 0.5f) < 0.22f) M->alb = V(0.4f, 0.4f, 0.42f); if (hash2(floorf(u), floorf(v) + floorf(T * 3.0f)) < 0.2f && len2(fract(u) - 0.5f, fract(v) - 0.5f) < 0.12f) M->emis = mul(GPAL[(int)(hash2(floorf(u), 3.0f) * 3.999f)], 2.5f); } break; }
    case W_SPEAKER: { float yy = p.y - GFL - 1.42f, cy = yy > 0 ? yy - 0.52f : yy + 0.50f, rr = len2(ax - 2.0f, cy), R = yy > 0 ? 0.30f : 0.42f; M->alb = V(0.018f, 0.018f, 0.02f); M->spec = 0.3f;
        if (n.z < -0.5f) { if (rr < R) { M->alb = mul(V(0.07f, 0.07f, 0.075f), 0.7f + 0.6f * rr / R); if (rr < R * (0.30f + 0.10f * GBEAT)) M->alb = V(0.16f, 0.16f, 0.17f); M->spec = 0.9f; } else if (rr < R + 0.035f) { M->alb = V(0.45f, 0.45f, 0.48f); M->spec = 1.5f; } } break; }
    case W_NEON: { float sf = 3.1f / (TR[T_FAT].w - 80.0f), dt = text2d(T_FAT, p.x / sf + TR[T_FAT].w * 0.5f, TR[T_FAT].base - (p.y - 1.45f + 0.26f) / sf) * sf, fl = 0.85f + 0.15f * sinf(T * 37.0f) * sinf(T * 5.3f);
        M->alb = V(0.02f, 0.015f, 0.03f); if (n.z < -0.5f) { M->emis = add(mul(V(7.0f, 0.8f, 3.6f), sstep(0.012f, -0.012f, dt) * fl), mul(V(0.9f, 0.08f, 0.5f), expf(-fmaxf(dt, 0) * 14.0f) * fl * 0.9f));
            if (fabsf(fabsf(p.y - 1.45f) - 0.37f) < 0.012f || fabsf(fabsf(p.x) - 1.68f) < 0.012f) M->emis = mul(V(0.2f, 2.6f, 3.6f), fl); } break; }
    case W_LED: { float u = (p.x - 1.02f + 0.53f) / 1.06f * 5.0f, v = (p.y - 0.36f + 0.17f) / 0.34f * 2.0f; int ci = (int)floorf(u); float fu = fract(u), on = 0; M->alb = V(0.01f, 0.01f, 0.01f);        // 04:12
        static const int DG[5] = {0, 4, -1, 1, 2}; if (n.z < -0.5f && u > 0 && u < 5 && v > 0 && v < 2) { if (DG[ci] >= 0) on = seg7(DG[ci], fu * 1.25f - 0.125f, v); else on = (fabsf(fu - 0.5f) < 0.09f && (fabsf(v - 0.62f) < 0.11f || fabsf(v - 1.38f) < 0.11f) && fract(T) < 0.6f); }
        if (on > 0) M->emis = V(5.5f, 0.35f, 0.2f); break; }
    case W_BALL: { v3 d = nrm(sub(p, V(0, 2.0f, -2.5f))); float u = (atan2f(d.z, d.x) + T * 0.35f) * 7.0f, v = asinf(clampf(d.y, -1, 1)) * 7.0f, h = hash2(floorf(u), floorf(v) + floorf(T * 6.0f));
        M->alb = mul(V(0.55f, 0.56f, 0.6f), 0.5f + 0.5f * fract(h * 3.3f)); M->spec = 2.0f; M->gloss = 80; if (fminf(0.5f - fabsf(fract(u) - 0.5f), 0.5f - fabsf(fract(v) - 0.5f)) < 0.06f) M->alb = V(0.02f, 0.02f, 0.02f); else if (h < 0.12f) M->emis = mul(GPAL[(int)(fract(h * 31.0f) * 3.999f)], 5.0f * GDROP + 1.0f); break; }
    default: break;
    }
}
static void gf_dance(pose_t *P, float ph, int style, float e) {       // un rebond par temps ; quatre façons de danser
    float sn = sinf(ph * 0.5f), j = fabsf(sn);
    P->bob += (0.055f * j - 0.02f) * e; P->foot[0].y += 0.035f * fmaxf(sn, 0) * e; P->foot[1].y += 0.035f * fmaxf(-sn, 0) * e;
    P->twist += 0.20f * sn * e; P->roll += 0.07f * sn * e; P->hpitch += 0.10f * j * e; P->hroll += 0.08f * sn * e; P->earflop[0] += 0.3f * j * e; P->earflop[1] += 0.3f * j * e; P->tail += 0.5f * sn * e;
    switch (style) {
    case 0: P->hand[0] = mix3(P->hand[0], V(0.36f, 0.80f + 0.05f * j, 0.10f), e); P->hand[1] = mix3(P->hand[1], V(-0.36f, 0.80f + 0.05f * (1 - j), 0.10f), e); break;
    case 1: P->hand[0] = mix3(P->hand[0], V(0.30f, 0.52f + 0.33f * j, 0.22f), e); break;
    case 2: P->hand[0] = mix3(P->hand[0], V(0.30f, 0.40f, 0.25f + 0.09f * sn), e); P->hand[1] = mix3(P->hand[1], V(-0.30f, 0.40f, 0.25f - 0.09f * sn), e); break;
    default: P->hand[1] = mix3(P->hand[1], V(-0.34f, 0.58f + 0.27f * j, 0.15f), e); P->hand[0].z += 0.10f * sn * e; break;
    }
    P->smile = 1; P->open = 0.3f * j * e;
}
static void gf_setup(float s) {
    const float BT = 0.5625f; float bp = fract(s / BT), ph = 6.2832f * s / BT; GBEAT = expf(-4.5f * bp); GSCR = 0;
    GFIN = (s >= 9.0f && s < 31.5f); GCZ1 = s >= 27.0f ? 5.4f : 4.2f; GDROP = s < 18.0f ? 0.8f : s < 24.75f ? 0.45f : s < 27.0f ? 0.45f + 0.55f * (s - 24.75f) / 2.25f : 1.0f;
    WMTN = 1.55f; MSUNE = 0.5f; WLAMP = 0; STAR = 0.5f; EXPO = 1.0f; WN = 0; WNL = 0;
    static int init; if (init != 5) { init = 5; cz_t *C = &WCZ[0]; cz_base(C); C->sc = 1.12f; C->fat = 0.8f; C->top = TOP_SHIRT; C->ptop = PAT_HSTRIPE; C->ctop = V(0.035f, 0.055f, 0.17f); C->ctop2 = V(0.86f, 0.83f, 0.75f);
        C->flags = F_PANTS | F_GLASSES; C->cbot = V(0.035f, 0.035f, 0.045f); C->hat = HAT_BEANIE; C->chat = V(0.035f, 0.055f, 0.17f); C->chat2 = V(0.33f, 0.03f, 0.07f); C->cacc = V(0.03f, 0.03f, 0.03f); }
    if (!GFIN) {       // ---- dehors : le fond du verre
        WFAR = 110.0f; WSUN = nrm(V(0.15f, 1.0f, 0.2f)); WSUNC = V(0.80f, 0.50f, 0.18f); WAMBU = V(0.085f, 0.04f, 0.011f); WAMBD = V(0.03f, 0.014f, 0.005f);
        float a = 0.5f * sinf(T * 0.45f); GBD = nrm(V(sinf(a) + 0.25f, -0.22f, cosf(a)));
        WNL = 7; for (int i = 0; i < 6; i++) { int sd = i & 1, k = i >> 1, ci = (k + (int)floorf(s / BT) + 2 * sd) & 3; float g[8] = {sd ? -5.2f : 5.2f, -2.6f, -5.0f + 6.0f * k, GPAL[ci].x, GPAL[ci].y, GPAL[ci].z, 2.2f + 3.5f * GBEAT, 0.06f}; memcpy(WLT[i], g, sizeof(g)); }
        { v3 lp = add(V(0, 2.5f, 13.8f), mul(GBD, 14.0f)); float g[8] = {lp.x, GSY + 2.5f, lp.z, 1.0f, 0.85f, 0.55f, 5.0f, 0.03f}; memcpy(WLT[6], g, sizeof(g)); }
        if (s < 9.0f) { float u = ease(s / 9.0f); cam(mix3(V(13.0f, 17.0f, 30.0f), V(10.5f, -1.6f, 19.0f), u), mix3(V(0, 1.0f, 2.0f), V(0, 0.9f, 6.5f), u), 1.3f); }                              // on descend le long des bulles : le sous-marin
        else { float u = ease((s - 31.5f) / 4.5f); cam(mix3(V(10.0f, -1.2f, -5.0f), V(21.0f, 9.5f, -15.0f), u), mix3(V(0, 0.6f, 0), V(0, 1.5f, 1.0f), u), mixf(1.25f, 1.45f, u)); }               // on remonte : la fête continue, au fond
        return; }
    // ---- à bord
    WFAR = 40.0f; WSUN = V(0, 1, 0); WSUNC = V(0, 0, 0); WAMBU = V(0.035f, 0.032f, 0.06f); WAMBD = V(0.03f, 0.022f, 0.04f);
    for (int k = 0; k < NCR; k++) { pose_t *P = &CRP[k]; int a = k % 12; cz_rest(&CZA[a], P); cz_idle(P, T, 0.07f * k); gf_dance(P, ph + 0.35f * sinf(k * 2.7f), (k * 7 + k / 12) & 3, GDROP * (0.75f + 0.25f * sinf(k * 1.9f))); P->hyaw += 0.2f * sinf(T * 0.6f + k); cz_rig(&CZA[a], P, &CRR[k]); }
    WN = 1; { pose_t *P = &WPP[0]; cz_rest(&WCZ[0], P); cz_idle(P, T, 0.3f); float j = fabsf(sinf(ph * 0.5f)), sc = WCZ[0].sc, hy = (GFL + 1.02f - (GFL + 0.34f)) / sc - 0.336f, hz = (7.98f - 7.60f) / sc;      // Fatbass : une main sur chaque platine
        float scr = s >= 18.0f && s < 24.75f ? sinf(T * 9.0f) * sstep(0.2f, 0.6f, sinf(T * 1.4f)) : 0; GSCR = 0.9f * scr;
        P->hand[1] = V(-0.38f / sc - 0.02f, hy, hz + 0.05f * scr); P->hand[0] = V(0.38f / sc - 0.05f, hy, hz - 0.03f);
        float up = s >= 24.75f ? sstep(0.0f, 0.3f, j) * ease((s - 24.75f) / 0.6f) : 0; P->hand[0] = mix3(P->hand[0], V(0.36f, 0.86f + 0.04f * j, 0.12f), up);
        P->bob += 0.03f * j * (0.5f + 0.5f * GDROP); P->hpitch += 0.14f * j + 0.10f; P->twist += 0.08f * sinf(ph * 0.5f); P->smile = 0.9f; P->looky = -0.1f;
        w_place(0, 0, GFL + 0.34f, 7.98f, PI); }
    {   float st = 0;       // les lumières : quatre pinceaux de couleur, le néon, la console (pas de stroboscope : rien qui clignote vite)
        WNL = 7; for (int i = 0; i < 4; i++) { int ci = (i + (int)floorf(s / (2 * BT))) & 3; float g[8] = {1.6f * sinf(T * 0.9f + i * 1.7f), 0.9f, -6.5f + 3.2f * i + 1.2f * cosf(T * 0.7f + i * 2.1f), GPAL[ci].x, GPAL[ci].y, GPAL[ci].z, (0.9f + 1.4f * GBEAT) * (0.5f + 0.5f * GDROP), 0.20f}; memcpy(WLT[i], g, sizeof(g)); }
        { float g[8] = {0, 1.3f, 10.2f, 1.0f, 0.15f, 0.55f, 2.6f, 0.10f}; memcpy(WLT[4], g, sizeof(g)); }
        { float g[8] = {0, GFL + 1.5f, 6.6f, 0.2f, 0.8f, 1.0f, 0.9f, 0.5f}; memcpy(WLT[5], g, sizeof(g)); }
        { float g[8] = {0, 2.5f, -1.0f, 1.0f, 1.0f, 1.0f, 7.0f * st, 0.03f}; memcpy(WLT[6], g, sizeof(g)); } }
    if (s < 18.0f) { float u = (s - 9.0f) / 9.0f; cam(V(0.7f * sinf(u * 2.6f), 0.75f - 0.45f * u, -11.2f + 10.5f * ease(u)), V(0, -0.45f, 8.0f), 1.15f); FOCUS = 7; APERT = 0.004f; }                         // on survole la piste, vers la cabine
    else if (s < 27.0f) { float u = (s - 18.0f) / 9.0f; cam(V(1.15f - 2.1f * ease(u), GFL + 1.42f + 0.1f * u, 5.75f + 0.25f * sinf(u * 3.14f)), V(0, GFL + 1.52f, 7.9f), 1.35f); FOCUS = 2.3f; APERT = 0.006f; }  // Fatbass, aux platines
    else { float u = (s - 27.0f) / 4.5f; cam(V(0.95f - 0.5f * u, 0.80f + 0.2f * u, 10.45f), V(-0.1f, -0.85f, 0.0f), 1.6f); FOCUS = 7; APERT = 0.004f; }                                                       // de la cabine : toute la piste saute
}

// ================================================================ SÉQUENCE 5 : le lendemain, 06 h 00. Le jour se lève sur le monument ; on sort par le O de PORKONIA.
#define FOX 7.70f
#define FOY 14.18f
static void fin_setup(float s) {
    WFAR = 900.0f; MSUNE = -0.085f + 0.0062f * fminf(s, 38.0f) + 0.0030f * fmaxf(s - 38.0f, 0); WSUN = nrm(V(0.25f, sinf(MSUNE), -cosf(MSUNE)));
    float k = sstep(-0.10f, 0.14f, MSUNE);
    WSUNC = mul(mix3(V(2.6f, 0.95f, 0.30f), V(2.9f, 1.55f, 0.75f), sstep(0.02f, 0.22f, MSUNE)), sstep(0.020f, 0.045f, MSUNE));
    WLINE = s < 13.5f ? 80.0f : 21.0f - 3.6f * (s - 13.5f);
    WAMBU = mix3(V(0.030f, 0.045f, 0.105f), V(0.16f, 0.22f, 0.38f), k); WAMBD = mix3(V(0.012f, 0.012f, 0.020f), V(0.10f, 0.085f, 0.075f), k);
    WLAMP = sstep(17.0f, 14.5f, s); EXPO = 1.15f - 0.22f * sstep(0.03f, 0.10f, MSUNE); STAR = 0.6f;
    MPH = T * 3.2f; MFLAGH = 12.6f;
    for (int i = 0; i < 3; i++) { float a = T * (0.55f - 0.09f * i) + 2.1f * i, R = 5.6f + 0.8f * i; MSP[i] = V(R * cosf(a), 9.2f + 0.9f * i + 0.4f * sinf(T * 0.8f + i), R * sinf(a)); }
    // ---- ceux qui sont déjà là : la sentinelle, le boulanger qui traverse, et quelqu'un en noir, près de la balustrade
    static int init; if (init != 6) { init = 6; WCZ[0] = CZA[6]; WCZ[0].leg = 0.2f; WCZ[1] = CZA[7];
        cz_t *C = &WCZ[2]; cz_base(C); C->top = TOP_SHIRT; C->flags = F_PANTS | F_COAT | F_SCARF; C->hat = HAT_BOWLER; C->ctop = C->cbot = C->cjack = C->chat = C->chat2 = V(0.022f, 0.022f, 0.028f); C->cacc = V(0.16f, 0.02f, 0.04f); C->leg = 0.5f; C->fat = -0.3f; }
    WN = 3;
    { pose_t *P = &WPP[0]; cz_rest(&WCZ[0], P); cz_idle(P, T, 0.5f); P->hyaw *= 0.4f; cz_salute(P, ease((s - 14.6f) / 0.7f)); P->hpitch = -0.2f * ease((s - 14.6f) / 0.7f); w_place(0, -12.5f, 0, 7.7f, atan2f(12.5f, -7.7f)); }
    { pose_t *P = &WPP[1]; cz_rest(&WCZ[1], P); float L = 0.9f * s; cz_walk(P, 6.2832f * T / 0.7556f, 1); w_place(1, 15.0f - 0.80f * L, 0, -9.0f + 0.05f * L, atan2f(-0.80f, 0.05f)); }
    { pose_t *P = &WPP[2]; cz_rest(&WCZ[2], P); cz_idle(P, T, 0.2f); P->hyaw = 0.2f * sinf(T * 0.3f); P->hpitch = -0.15f; w_place(2, 9.5f, 0, 15.5f, atan2f(-9.5f, -15.5f)); }
    // ---- les plans
    v3 O = V(FOX, FOY, MLZ);
    if (s < 9.0f) { float u = s / 9.0f, a = -2.3f + 1.5f * ease(u), R = 50.0f - 20.0f * u; cam(V(R * sinf(a), 36.0f - 22.0f * ease(u), -R * cosf(a)), V(0, 7.0f, 4.0f), 1.35f); }                                     // la nuit finit : on descend en tournant vers l'esplanade
    else if (s < 18.0f) { float u = (s - 9.0f) / 9.0f; cam(V(-9.6f + 1.2f * u, 1.45f + 0.5f * u, -3.4f - 1.0f * u), V(0.4f, 8.6f - 0.6f * u, 4.0f), 1.25f); FOCUS = 11; APERT = 0.006f; }                                  // au pied de la statue : le premier rayon
    else if (s < 27.0f) { float u = (s - 18.0f) / 9.0f, e = ease(u); v3 a = V(-10.0f, 3.0f, -8.0f), b = V(-6.5f, 9.5f, 12.0f), c = V(5.2f, 13.6f, 43.0f);                                                             // on s'élève vers les lettres
        cam(mix3(mix3(a, b, e), mix3(b, c, e), e), mix3(V(0, 9.0f, 20.0f), O, ease(u * 1.6f)), 1.3f); }
    else { v3 c0 = V(5.2f, 13.6f, 43.0f), dir = nrm(V(-WSUN.x, -WSUN.y, -WSUN.z)), p1 = add(O, mul(dir, 10.5f + 0.9f * fmaxf(s - 31.2f, 0))), c, tg;                                                                   // par le O ; on se retourne : le soleil est dedans ; puis le ciel
        if (s < 29.2f) { float u = (s - 27.0f) / 2.2f; c = mix3(c0, O, u * (0.6f + 0.4f * u)); tg = add(O, V(0.3f, 0.3f, 30.0f)); }
        else { float u = ease((s - 29.2f) / 2.0f), tu = ease((s - 29.5f) / 2.2f); c = mix3(O, p1, u); tg = mix3(add(c, V(1.5f * sinf(tu * 3.1416f), 0.2f, 30.0f * cosf(tu * 3.1416f) + 0.01f)), O, sstep(0.75f, 1.0f, tu)); }
        float up = ease((s - 35.0f) / 4.0f); c = add(c, V(0, 9.0f * up + 0.25f * fmaxf(s - 39.0f, 0), 4.0f * up)); tg = add(tg, V(-6.0f * up, 60.0f * up, 0));
        cam(c, tg, mixf(1.3f, 1.2f, up)); }
}

// ================================================================ le moteur commun
static v3 wfogc(v3 rd) { switch (SEQ) { case 3: return V(0.34f, 0.27f, 0.19f); case 4: return GFIN ? V(0.030f, 0.018f, 0.045f) : gf_sky(rd); default: return mon_fogc(rd); } }
static float wfod(v3 ro, v3 rd, float t) { switch (SEQ) { case 1: return 0.0007f * t; case 2: return 0.00045f * t; case 3: return 0.010f * t; case 4: return (GFIN ? 0.016f : 0.034f) * t; default: return mon_fog(ro, rd, t); } }
static v3 wsky(v3 ro, v3 rd) { switch (SEQ) { case 1: return par_sky(ro, rd); case 4: return GFIN ? V(0.02f, 0.012f, 0.03f) : gf_sky(rd); default: return mon_sky(ro, rd); } }
S float wsh2(v3 p) { return SEQ == 2 ? cub_shadow(p) : 1.0f; }
static void wmaterial(v3 p, v3 n, int m, float t, mat_t *M) {
    M->alb = V(0.5f, 0.5f, 0.5f); M->emis = V(0, 0, 0); M->spec = 0.15f; M->gloss = 30; M->sss = 0; M->env = 0;
    float snow = 0, up = sstep(0.55f, 0.85f, n.y);
    if (m >= C_SKIN && m < W_GROUND) { v3 l; int k = wwho(p, &l), mm; cmat_t cm;
        if (SEQ == 1 && len2(p.x - WPX[k], p.z - WPZ[k]) > 0.85f) { int f = par_fan(p, &l); if (f < 0) f = 0; cz_sdf(&CZA[f % 12], &CRR[f], &CRP[f], l, &mm); cz_mat(&CZA[f % 12], &CRP[f], m, &cm); }
        else if (SEQ == 2 && len2(p.x - WPX[k], p.z - WPZ[k]) > 0.85f) { float qu, qw, du; cub_side(p, &qu, &qw); int f = (p.y > QH - 0.3f && qw > 65.0f && qw < 70.1f) ? cub_ring(p, &l, &du) : cub_pil(p, &l); if (f < 0) f = 0; cz_sdf(&CZA[f], &CZR[f], &CZP[f], l, &mm); cz_mat(&CZA[f], &CZP[f], m, &cm); }
        else if (SEQ == 4 && GFIN && p.z < 6.1f) { int f = gf_fan(p, &l); if (f < 0) f = 0; cz_sdf(&CZA[f % 12], &CRR[f], &CRP[f], l, &mm); cz_mat(&CZA[f % 12], &CRP[f], m, &cm); }
        else { cz_sdf(&WCZ[k], &WRG[k], &WPP[k], l, &mm); cz_mat(&WCZ[k], &WPP[k], m, &cm); }
        M->alb = cm.alb; M->emis = mul(cm.emis, 0.45f); M->spec = cm.spec; M->gloss = cm.gloss; M->sss = cm.sss;
        if (cm.metal > 0) { M->alb = mul(cm.alb, 0.55f); M->spec = 2.0f; }
        if (cm.flat > 0) { M->alb = mul(cm.alb, 0.45f); M->spec = 0; }
        return; }
    switch (m) {       // ce que toutes les séquences partagent : l'or, le chrome, le rose officiel du porc de marche
    case W_PGOLD: case W_FINIAL: case W_LETTER: M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 1.6f; M->gloss = 150; snow = (m == W_FINIAL || (SEQ != 0 && SEQ != 5)) ? 0 : 0.8f * sstep(0.84f, 0.97f, n.y); up = 1; break;
    case W_PCHROME: case W_SPH: M->alb = V(0.93f, 0.95f, 1.0f); M->env = 1; M->spec = 1.6f; M->gloss = 200; break;
    case W_PPINK: M->alb = V(0.95f, 0.47f, 0.52f); M->spec = 0.5f; M->gloss = 40; M->sss = 0.25f; break;
    case W_PEYE: M->alb = V(0.02f, 0.02f, 0.02f); M->spec = 1.5f; M->gloss = 120; M->env = 0.2f; break;
    case W_POLE: M->alb = V(0.72f, 0.72f, 0.70f); M->spec = 0.6f; M->gloss = 60; break;
    case W_FIR: M->alb = mul(V(0.030f, 0.075f, 0.045f), 0.6f + 0.8f * vnoise(p.x * 2.3f + p.y * 3.1f, p.z * 2.3f)); up = sstep(0.36f, 0.56f, n.y); snow = sstep(0.34f, 0.60f, vnoise(p.x * 1.3f, p.z * 1.3f + p.y * 1.9f) + 0.08f); M->sss = 0.2f; break;
    case W_TRUNK: M->alb = V(0.10f, 0.06f, 0.04f); break;
    default: switch (SEQ) { case 1: par_material(p, n, m, t, M, &snow, &up); break; case 2: cub_material(p, n, m, t, M, &snow, &up); break; case 3: tav_material(p, n, m, t, M, &snow, &up); break; case 4: gf_material(p, n, m, t, M, &snow, &up); break; default: mon_material(p, n, m, t, M, &snow, &up); } }
    if (snow > 0) { float s = snow * up; M->alb = mix3(M->alb, mul(WSNW, 0.9f + 0.1f * vnoise(p.x * 30, p.z * 30)), s); M->env *= 1 - s; M->emis = mul(M->emis, 1 - s); M->spec *= 1 - 0.8f * s; if (s > 0.5f) M->sss = fmaxf(M->sss, 0.3f); }
}
S float wglow(v3 p) {       // les halos des lanternes dans l'air froid
    if (SEQ == 0 || SEQ == 5) { float r = len2(p.x, p.z); if (p.z > -3.0f || fabsf(r - 22.0f) > 2.5f || p.y > 4.0f) return 0;
        float sa = 6.2831853f / 12, a = atan2f(p.x, -p.z), ka = roundf(a / sa) * sa, dx = p.x - 22.0f * sinf(ka), dz = p.z + 22.0f * cosf(ka), dy = p.y - 1.52f; return WLAMP / (1 + 25.0f * (dx * dx + dy * dy + dz * dz)); }
    if (SEQ == 3) { static const float LX[3] = {-3.6f, 2.9f, 0.3f}, LZ[3] = {-1.6f, -1.6f, 0.0f}; float g = 0; if (p.y < 2.3f) return 0;
        for (int i = 0; i < 3; i++) { float dx = p.x - LX[i], dy = p.y - 3.0f, dz = p.z - LZ[i]; g += 1 / (1 + 60.0f * (dx * dx + dy * dy + dz * dz)); } return g * WLAMP; }
    if (SEQ == 2) { float ax = fabsf(p.x); if (p.z > -108.0f || p.z < -350.0f || ax > 9.0f || p.y > 14.0f) return 0;
        float zl = p.z - 12.0f * roundf(p.z / 12.0f), dx = ax - 5.2f, dy = p.y - 4.0f, g = 1 / (1 + 30.0f * (dx * dx + dy * dy + zl * zl));
        if (p.z > -116.0f) { float ex = ax - 7.0f, ey = p.y - 11.55f, ez = p.z + 113.0f; g += 1.5f / (1 + 14.0f * (ex * ex + ey * ey + ez * ez)); }
        return g * WLAMP; }
    return 0;
}
static v3 wlight(v3 p, v3 n, float ao, float sun) {
    v3 L = add(mul(WSUNC, sun), mul(add(mul(WAMBU, 0.5f + 0.5f * n.y), mul(WAMBD, 0.5f - 0.5f * n.y)), 0.35f + 0.65f * ao));
    for (int i = 0; i < WNL; i++) { v3 lv = sub(V(WLT[i][0], WLT[i][1], WLT[i][2]), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
        L = add(L, mul(V(WLT[i][3], WLT[i][4], WLT[i][5]), WLT[i][6] / (1 + WLT[i][7] * d2) * fmaxf(dot(n, lv) * il, 0))); }
    if (SEQ == 2 && p.z < -104.0f && p.z > -352.0f && fabsf(p.x) < 22.0f && p.y < 10.0f) {      // les lanternes de l'allée
        float z0 = floorf(p.z / 12.0f) * 12.0f;
        for (int j = 0; j <= 1; j++) for (int sgn = -1; sgn <= 1; sgn += 2) { v3 lv = sub(V(5.2f * sgn, 4.0f, z0 + 12.0f * j), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(V(1.0f, 0.60f, 0.28f), 5.0f / (1 + 0.45f * d2) * fmaxf(dot(n, lv) * il, 0))); } }
    if (SEQ == 4 && GFIN && fabsf(p.x) < 2.6f && p.z > -10.0f && p.z < 6.0f) { int ci = ((int)floorf(p.x / 1.4f + 7.0f) + (int)floorf(p.z / 1.4f + 9.0f) + (int)floorf(T / 0.5625f)) & 3;
        L = add(L, mul(GPAL[ci], (0.25f + 0.9f * GBEAT) * GDROP * (0.55f - 0.45f * n.y) / (1 + 0.9f * fmaxf(p.y - GFL, 0)))); }
    if (SEQ == 2) { float u, w; int sd = cub_side(p, &u, &w);
        if (w > 57.0f && w < 70.5f && p.y > QH - 0.5f && p.y < QH + 9.0f && fabsf(u) < 63.0f) {      // les cierges, au pied du Cube
            static const float WX[4] = {0, 1, 0, -1}, WZ[4] = {-1, 0, 1, 0}; float dw = 61.7f - w, dy = QH + 0.5f - p.y, d2 = dw * dw + dy * dy, nw = n.x * WX[sd] + n.z * WZ[sd];
            L = add(L, mul(V(1.0f, 0.50f, 0.18f), (0.9f + 0.1f * sinf(T * 9.0f + u * 0.7f)) * 1.5f / (1 + 0.45f * d2) * fmaxf((nw * dw + n.y * dy) / sqrtf(d2 + 1e-4f), 0.12f))); }
        L = mul(L, cub_dark(p)); }
    if ((SEQ == 0 || SEQ == 5) && WLAMP > 0 && p.z < 6.0f && p.y < 12.0f && p.y > -2.0f) {      // les lanternes de la balustrade
        float sa = 6.2831853f / 12, a = atan2f(p.x, -p.z), k0 = floorf(a / sa);
        for (int j = 0; j <= 1; j++) { float ka = clampf(k0 + j, -2, 2) * sa; v3 lv = sub(V(22.0f * sinf(ka), 1.6f, -22.0f * cosf(ka)), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(V(1.0f, 0.60f, 0.28f), WLAMP * 7.0f / (1 + 0.35f * d2) * fmaxf(dot(n, lv) * il, 0))); } }
    if (__builtin_expect(NCH, 0)) L = add(L, ch_glow(p, n));
    return L;
}
static float wtrace(v3 ro, v3 rd, int *m, float tmax, int steps, float *glow) {
    float t = 0.05f, g = 0;
    for (int i = 0; i < steps && t < tmax; i++) { v3 p = add(ro, mul(rd, t)); float d = wmap(p, m);
        if (d < 0.001f * (1 + t * 0.5f)) { *glow = g; return t; }
        if (WLAMP > 0) g += wglow(p) * fminf(d, 0.9f);
        t += d; }
    *glow = g; return -1;
}
static float ambocc_cz(v3 p, v3 n) {       // (20 nov. 2026) l'occlusion ambiante des personnages : à leur échelle (2 à 20 cm) et plus légère. Celle du décor sonde jusqu'à 60 cm :
    // sur un cochon de 1,40 m, elle noircissait d'un coup les bras, le dessous du visage et l'entre-jambes dès qu'un voisin passait, en taches brunes qui bougeaient avec la caméra
    float o = 0, sc = 1; int m;
    for (int i = 1; i <= 4; i++) { float h = 0.02f + 0.045f * i, d = map(add(p, mul(n, h)), &m); o += (h - clampf(d, 0, h)) * sc; sc *= 0.75f; }
    return clampf(1 - 2.2f * o, 0.35f, 1);
}
static float shadow_cz(v3 ro, v3 rd) {       // (17 nov. 2026) l'ombre sur les personnages : des pas fins près du corps et une pénombre douce ; avec les pas de 6 cm de shadow(), le groin, les joues et les plis
    // laissaient des taches dures et irrégulières sur le visage et les vêtements
    float res = 1, t = 0.06f; int m; INSH = 1; NOBND = 1;       // (20 nov. 2026) on part plus loin de la surface, et tout près d'elle un contact ne compte pas comme une ombre franche :
    // sinon la peau se faisait de l'ombre à elle-même (taches brunes en dents de scie sur les joues, les bras, les mains) qui bougeaient avec la caméra
    for (int i = 0; i < 64 && t < 75; i++) { float d = map(add(ro, mul(rd, t)), &m); if (d < 0.002f && t > 0.15f) { INSH = 0; NOBND = 0; return 0; } res = fminf(res, sstep(0.0f, 0.15f, t) * 9.0f * fmaxf(d, 0) / t + (1 - sstep(0.0f, 0.15f, t))); t += clampf(d * 0.9f, 0.015f, t < 3.0f ? 0.25f : 6.0f); }
    INSH = 0; NOBND = 0; res = clampf(res, 0, 1); return res * res * (3 - 2 * res);
}
static v3 wshade(v3 ro, v3 rd, int depth, float *zout) {
    int m; float glow, t = wtrace(ro, rd, &m, depth ? WFAR * 0.35f : WFAR, depth ? 90 : 260, &glow); v3 col;
    if (t < 0) { col = wsky(ro, rd); *zout = 1e4f; }
    else {
        v3 p = add(ro, mul(rd, t)), n = normal(p, t); mat_t M; wmaterial(p, n, m, t, &M);
        CZ_BND = 0.7f; NOBND = 1; float ao = (depth || (WDBG & 4)) ? 0.85f : (((m >= 200 && m < 240) || (m >= 500 && m < 520)) ? ambocc_cz(p, n) : ambocc(p, n)); NOBND = 0; CZ_BND = CZ_BND0; float f5 = powf(1 - fmaxf(-dot(rd, n), 0), 5), ndl = dot(n, WSUN), si = w_sunI(p) * (WSUNC.x > 0.01f);
        float sh = (si > 0.01f && (ndl > 0 || M.sss > 0.5f)) ? ((depth || (WDBG & 2)) ? 1.0f : (((m >= 200 && m < 240) || (m >= 500 && m < 520)) ? shadow_cz(add(p, mul(n, ndl > 0 ? 0.02f : -0.05f)), WSUN) : shadow(add(p, mul(n, ndl > 0 ? 0.03f : -0.08f)), WSUN))) * si * wsh2(p) : 0;
        if (M.sss > 1.15f && M.sss < 1.25f) sh = mixf(sh, si * wsh2(p), 0.35f);       // (17 nov. 2026) le visage des citoyens : l'ombre du groin et des cheveux n'y fait plus une grosse tache brune
        float dif = fmaxf(ndl, 0); if (M.sss > 0) dif = mixf(dif, fmaxf((ndl + 0.5f) / 1.5f, 0), fminf(M.sss, 1.0f));      // la neige, l'étoffe, la peau : la lumière tourne un peu autour
        v3 L = wlight(p, n, ao, dif * sh);
        if (M.sss > 0.5f && ndl < 0) L = add(L, mul(mulv(WSUNC, V(1.0f, 0.55f, 0.45f)), 0.45f * M.sss * (-ndl) * sh));         // à contre-jour, l'étoffe laisse passer le soleil
        col = add(mulv(M.alb, L), M.emis);
        v3 rdir = refl(rd, n);
        if (M.env > 0.9f) { v3 rc; float z2;
            if (depth == 0) rc = wshade(add(p, mul(n, 0.03f)), rdir, 1, &z2); else rc = wsky(p, V(rdir.x, fabsf(rdir.y), rdir.z));
            if (M.alb.z < 0.5f * M.alb.x) { float lu = 0.3f * rc.x + 0.6f * rc.y + 0.1f * rc.z; rc = mix3(V(lu, lu, lu), rc, 0.35f); }       // l'or teinte ce qu'il reflète : un ciel bleu n'y verdit pas
            col = add(add(mulv(M.alb, rc), M.emis), mul(mulv(M.alb, L), 0.12f));
        } else if (M.env > 0.02f && depth == 0 && !(WDBG & 1)) { float z2, F = fminf(M.env + (1 - M.env) * f5 * 0.30f, 1.0f); v3 nn = n;
            if (m == W_GROUND) nn = nrm(V((vnoise(p.x * 1.9f, p.z * 1.9f) - 0.5f) * 0.09f, 1, (vnoise(p.x * 1.9f + 31, p.z * 1.9f + 17) - 0.5f) * 0.09f));
            if (m == W_CTOP) nn = nrm(V((vnoise(p.x * 23.0f, p.z * 23.0f) - 0.5f) * 0.16f, 1, (vnoise(p.x * 23.0f + 31, p.z * 23.0f + 17) - 0.5f) * 0.16f));       // un bois ciré : les reflets se brouillent       // un marbre mouillé : les reflets tremblent
            v3 rc = wshade(add(p, mul(n, 0.03f)), refl(rd, nn), 1, &z2); col = add(mul(col, 1 - F * 0.85f), mul(rc, F)); }
        col = add(col, mul(WSUNC, powf(fmaxf(dot(rdir, WSUN), 0), M.gloss) * sh * M.spec * (M.env > 0.9f ? 1.0f : 0.5f)));
        if (m == W_CHOPE) col = ch_shade(ro, rd, p, depth);       // la chope standard : son optique
        if (m == W_MUGH) { v3 bc = V(0.3f, 0.2f, 0.1f); float z2; if (depth == 0) { NOMUG = 1; bc = wshade(add(p, mul(rd, 0.004f)), rd, 1, &z2); NOMUG = 0; } col = tav_mugshade(ro, rd, p, n, L, bc); }
        col = mix3(col, wfogc(rd), 1 - expf(-wfod(ro, rd, t)));
        *zout = t;
        if (WDBG & 32) col = V(ao, ao, ao); if (WDBG & 64) col = V(0.5f + 0.5f * n.x, 0.5f + 0.5f * n.y, 0.5f - 0.5f * n.z); if (WDBG & 128) col = V(fract(p.x), fract(p.y), fract(t * 0.1f));
    }
    if (depth == 0) for (int i = 0; i < NPL; i++) { float od = cloud(ro, rd, t > 0 ? t : 1e9f, V(PLM[i][0], PLM[i][1], PLM[i][2]), V(PLM[i][3], PLM[i][4], PLM[i][3]), 2.2f, hash2(rd.x * 811.3f + T, rd.y * 433.7f)) * PLM[i][5];
        if (od > 0) col = mix3(col, V(PLM[i][6], PLM[i][7], PLM[i][8]), 1 - expf(-od)); }
    col = add(col, mul(V(1.0f, 0.62f, 0.30f), glow * 0.5f));
    if (SEQ == 4 && depth == 0) { float tm = t > 0 ? t : 60.0f, jit = hash2(rd.x * 811.3f + T, rd.y * 433.7f);
        if (GFIN) { for (int i = 0; i < 4; i++) { int ci = (i + (int)floorf(WS / 1.125f)) & 3; v3 b0 = V(i & 1 ? 1.3f : -1.3f, 2.35f, i < 2 ? -7.0f : 1.0f), bd = nrm(V(0.7f * sinf(T * 0.9f + i * 1.7f) - b0.x * 0.3f, -1, 0.9f * cosf(T * 0.7f + i * 2.1f)));
                col = add(col, mul(GPAL[ci], 0.60f * gf_beam(ro, rd, tm, b0, bd, 0.07f, 0.11f, jit) * (0.35f + 0.65f * GBEAT) * (0.4f + 0.6f * GDROP))); } }
        else { col = add(col, mul(V(1.0f, 0.85f, 0.55f), 0.45f * gf_beam(ro, rd, tm, V(0, 2.5f, 13.8f), GBD, 0.33f, 0.16f, jit)));
            float f = flakes(ro, rd, tm, 2.4f, 30.0f, -0.9f, 0.028f) + 0.6f * flakes(ro, rd, tm, 5.0f, 48.0f, -0.6f, 0.045f); col = add(col, mul(V(1.0f, 0.70f, 0.30f), 0.16f * fminf(f, 1))); } }
    if (SEQ == 3 && depth == 0) { col = add(col, tav_shafts(ro, rd, t > 0 ? t : 20.0f, hash2(rd.x * 811.3f + T, rd.y * 433.7f)));
        if (TBEER > 0) { float tm = t > 0 ? t : 1e9f, f = flakes(ro, rd, tm, 0.22f, 4.0f, -0.20f, 0.009f) + 0.7f * flakes(ro, rd, tm, 0.55f, 8.0f, -0.14f, 0.018f);
            col = mix3(col, add(mul(mulv(col, V(1.0f, 0.55f, 0.11f)), 0.62f), V(0.30f, 0.115f, 0.008f)), TBEER); col = mix3(col, tav_deep(rd), TDEEP);
            col = add(col, mul(V(1.0f, 0.80f, 0.42f), 0.40f * fminf(f, 1) * TBEER)); } }
    if (depth == 0 && WCONF > 0) { float hc, f = confetti(ro, rd, t > 0 ? t : 1e9f, &hc);       // des confettis : bordeaux, or, crème
        static const v3 CC[3] = {{0.55f, 0.05f, 0.12f}, {1.0f, 0.70f, 0.20f}, {0.95f, 0.92f, 0.82f}};
        col = mix3(col, mul(CC[(int)(fract(hc * 7.7f) * 2.999f)], 0.5f + 0.5f * WSUNC.y), f * WCONF); }
    if (depth == 0 && WSNOW > 0) { float tm = t > 0 ? t : 1e9f, f = flakes(ro, rd, tm, 1.3f, 16.0f, 1.5f, 0.022f) + 0.6f * flakes(ro, rd, tm, 3.7f, 46.0f, 1.3f, 0.03f);
        col = mix3(col, mul(add(WAMBU, mul(WSUNC, 0.25f)), 2.2f), fminf(f, 1) * 0.85f * WSNOW); }
    return col;
}
static v3 shade_w(v3 ro, v3 rd, float *zout) { return wshade(ro, rd, 0, zout); }
static void setupw(float tb) {       // tb : le temps depuis 300 ; chaque séquence a cent secondes à elle
    SEQ = (int)(tb / 100.0f); float s = tb - 100.0f * SEQ; WS = s; SHOT = 21; PARTY = 0; NOPIGS = 1; STAR = 0.6f; EXPO = 1.0f; WN = 0; WNL = 0; NPL = 0; NCH = 0; WLINE = -1e9f; WLAMP = 0; WSNOW = 0;
    if (!CZINIT) { people_init(); if (getenv("WDBG")) WDBG = atoi(getenv("WDBG")); }
    WMTN = 1.55f;
    switch (SEQ) { case 1: par_setup(s); break; case 2: cub_setup(s); break; case 3: tav_setup(s); break; case 4: gf_setup(s); break; case 5: fin_setup(s); break; default: mon_setup(s); }
    { const char *wc = getenv("WCAM"); float c[7]; if (wc && sscanf(wc, "%f %f %f %f %f %f %f", c, c + 1, c + 2, c + 3, c + 4, c + 5, c + 6) == 7) { cam(V(c[0], c[1], c[2]), V(c[3], c[4], c[5]), c[6]); APERT = 0; } }
    w_bounds();
    { const char *wp = getenv("WPROBE"); float c[6]; int n; if (wp && sscanf(wp, "%f %f %f %f %f %f %d", c, c + 1, c + 2, c + 3, c + 4, c + 5, &n) == 7) { for (int i = 0; i <= n; i++) { float u = (float)i / n; v3 p = V(c[0] + (c[3] - c[0]) * u, c[1] + (c[4] - c[1]) * u, c[2] + (c[5] - c[2]) * u); int m; float d = wmap(p, &m); fprintf(stderr, "P %.2f %.2f %.2f d=%.3f m=%d\n", p.x, p.y, p.z, d, m); } exit(0); } }
}
