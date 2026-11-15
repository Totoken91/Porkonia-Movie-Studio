// ---------------------------------------------------------------- Douzi City: its lights, its materials, its snow
static const v3 NEON[4] = {{1.0f, 0.10f, 0.45f}, {1.0f, 0.55f, 0.06f}, {0.10f, 0.75f, 1.0f}, {1.0f, 0.90f, 0.70f}};
static const v3 WALLC[5] = {{0.46f, 0.36f, 0.22f}, {0.52f, 0.48f, 0.40f}, {0.48f, 0.33f, 0.30f}, {0.30f, 0.35f, 0.40f}, {0.36f, 0.40f, 0.30f}};
static const v3 SNOWC = {0.80f, 0.84f, 0.93f};
S v3 shopcol(float h) { return NEON[(int)(fract(h * 5.3f) * 3.999f)]; }
S int shoptext(float h, float iz, float sg) {
    static const int ids[6] = {T_CHARC, T_BRASS, T_FUMOIR, T_AMBREE, T_BOUL, T_CHARC};
    if (iz == 1 && sg > 0) return T_CHARC;
    return fract(h * 3.7f) < 0.78f ? ids[(int)(fract(h * 9.1f) * 5.999f)] : -1;
}
S float neonline(float d, float w) { return sstep(w, 0.0f, fabsf(d)) * 1.0f + 0.22f * expf(-fmaxf(fabsf(d) - w, 0) * 14.0f); }
static float icon(int k, float a, float b) {        // little neon pictures: a pig, a mug of beer, a sausage (signed distance, metres)
    if (k == 0) { float d = fminf(len2(a, b + 0.05f) - 0.36f, len2(fabsf(a) - 0.25f, b - 0.30f) - 0.11f);
        return fminf(fabsf(d), fminf(fabsf(len2(a, b + 0.12f) - 0.13f), len2(fabsf(a) - 0.05f, b + 0.12f) - 0.012f)); }
    if (k == 1) { float d = box2(fabsf(a + 0.05f) - 0.22f, fabsf(b + 0.08f) - 0.36f) - 0.03f;
        float hd = fmaxf(fabsf(len2(a - 0.27f, b + 0.08f) - 0.18f), 0.2f - a);
        float fo = fabsf(fminf(fminf(len2(a + 0.19f, b - 0.36f), len2(a + 0.05f, b - 0.42f)), len2(a - 0.10f, b - 0.36f)) - 0.11f);
        return fminf(fminf(fabsf(d), hd), fmaxf(fo, 0.3f - b)); }
    { float c = 0.7071f, u = c * a + c * b, v = -c * a + c * b, d = len2(u - clampf(u, -0.32f, 0.32f), v) - 0.14f;
      return fminf(fabsf(d), fminf(len2(u - 0.52f, v), len2(u + 0.52f, v)) - 0.03f); }
}
S int shoplit(float h, int id) { return id == T_BOUL || id == T_CHARC || id == T_FUMOIR || fract(h * 17.7f) < 0.30f; }       // à cinq heures, seuls quelques commerces ont allumé
S float ham2(float x, float y) { return smin((len2(x / 0.165f, y / 0.25f) - 1) * 0.165f, len2(x, y - clampf(y, 0.16f, 0.55f)) - 0.046f, 0.12f); }
S float seg2(float px, float py, float bx, float by) { float h = clampf((px * bx + py * by) / (bx * bx + by * by), 0, 1); return len2(px - bx * h, py - by * h); }
S v3 gingham(float a, float b, float f, float k) {      // du vichy : deux jeux de rayures qui se croisent (k : 1 de près ; de loin, 0, il n'en reste qu'un rose)
    int s1 = fract(a * f) < 0.5f, s2 = fract(b * f) < 0.5f; v3 c = s1 && s2 ? V(0.42f, 0.04f, 0.09f) : (s1 || s2) ? V(0.66f, 0.30f, 0.30f) : V(0.86f, 0.82f, 0.72f);
    return mix3(V(0.65f, 0.37f, 0.35f), c, k);
}
static v3 storefront(float u, float v, v3 rd, float sg, int kind, float h, v3 sc, float t) {      // derrière une vitrine allumée : ce qui pend près de la vitre, le comptoir, le marchand, le mur du fond (la parallaxe donne la profondeur)
    float rw = fmaxf(sg * rd.x, 0.08f), fw = t * 0.0034f / rw;        // fw : ce que couvre un pixel, en mètres (de loin, les petits motifs s'effacent au lieu de fourmiller)
    float u0 = u + rd.z * 0.40f / rw, v0 = v + rd.y * 0.40f / rw, u1 = u + rd.z * 1.25f / rw, v1 = v + rd.y * 1.25f / rw;
    float us = u + rd.z * 1.75f / rw, vs = v + rd.y * 1.75f / rw, u2 = u + rd.z * 2.30f / rw, v2 = v + rd.y * 2.30f / rw;
    v3 warm = mix3(V(1.0f, 0.56f, 0.22f), sc, 0.08f), col;
    if (fabsf(u2) > 4.9f || v2 > 2.95f || v2 < 0.12f) col = mul(warm, v2 < 0.12f ? 0.09f : 0.12f);                         // les murs de côté, le plafond, le sol
    else if (v2 < 1.0f) col = mul(V(0.26f, 0.12f, 0.05f), 0.50f + 0.25f * sstep(0.03f, 0.06f, fabsf(fract(u2 / 0.6f) - 0.5f) * 0.6f));     // des lambris
    else { col = mul(warm, 0.21f + 0.13f * sstep(1.0f, 2.8f, v2));
        float sh = v2 < 1.62f ? 1.02f : v2 < 2.24f ? 1.64f : 2.26f, y = v2 - sh, pitch = 0.22f; v3 it = col; int on = 0;      // trois étagères
        if (y < 0.045f) { it = V(0.08f, 0.04f, 0.022f); on = 1; }
        else if (kind == 1) { float i = floorf(u2 / 0.22f), x = fabsf(fract(u2 / 0.22f) - 0.5f) * 0.22f, hh = hash2(i, sh), w = y < 0.30f ? 0.062f : y < 0.38f ? 0.062f - 0.5f * (y - 0.30f) : 0.022f;      // des bouteilles
            if (x < w && y < 0.52f && hh < 0.85f) { v3 g = hh < 0.5f ? V(1.0f, 0.50f, 0.06f) : hh < 0.7f ? V(0.12f, 0.45f, 0.14f) : V(0.45f, 0.18f, 0.05f); on = 1;
                it = (y > 0.10f && y < 0.22f && hh > 0.2f) ? V(0.60f, 0.56f, 0.46f) : mul(g, 0.28f + 0.62f * (1 - x / w)); } }
        else if (kind == 2) {                                                                                                                 // des miches, des baguettes debout
            if (sh < 2.0f) { pitch = 0.5f; float i = floorf(u2 / 0.5f), x = (fract(u2 / 0.5f) - 0.5f) * 0.5f, hh = hash2(i, sh), dd = len2(x / 0.21f, (y - 0.17f) / 0.125f) - 1;
                if (dd < 0 && hh < 0.9f) { on = 1; it = mul(V(0.95f, 0.55f, 0.16f), 0.32f + 0.55f * sstep(0.0f, -0.9f, dd) - 0.18f * sstep(0.1f, 0.0f, fabsf(fract(x * 14.0f + y * 6.0f) - 0.5f) - 0.3f)); } }
            else { pitch = 0.14f; float xb = (fract(u2 / 0.14f) - 0.5f) * 0.14f - 0.05f * (y - 0.3f); if (fabsf(xb) < 0.04f && y < 0.66f) { on = 1; it = mul(V(0.98f, 0.62f, 0.20f), 0.35f + 0.55f * (1 - fabsf(xb) / 0.04f)); } } }
        else { pitch = 0.2f; float i = floorf(u2 / 0.2f), x = fabsf(fract(u2 / 0.2f) - 0.5f) * 0.2f, hh = hash2(i, sh);                           // des bocaux
            if (x < 0.072f && y < 0.23f && hh < 0.9f) { on = 1; it = y > 0.19f ? V(0.36f, 0.05f, 0.08f) : (y > 0.08f && y < 0.15f) ? V(0.64f, 0.60f, 0.50f) : mul(V(0.85f, 0.60f, 0.36f), 0.42f + 0.48f * (1 - x / 0.072f)); } }
        if (on) col = mix3(mul(col, 0.85f), it, sstep(0.9f, 0.3f, fw / pitch)); }
    {   float sx = 2.2f * (fract(h * 37.3f) - 0.5f) + 0.25f * sinf(T * 0.5f + h * 40.0f), px = us - sx, py = vs - 0.13f - 0.012f * sinf(T * 2.0f + h * 9.0f);     // le marchand
        if (fabsf(px) < 0.62f && py < 1.95f && py > 0.2f && fabsf(us) < 4.6f) { v3 sk = V(0.80f, 0.33f, 0.33f), cc = col; int a = 0;
            float bd = len2(px / 0.36f, (py - 0.66f) / 0.44f) - 1; if (bd < 0) { cc = mul(kind == 0 && fract(px * 9.0f) < 0.5f ? V(0.55f, 0.10f, 0.12f) : V(0.80f, 0.78f, 0.72f), 0.36f + 0.34f * sstep(0, -0.6f, bd)); a = 1; }
            float er = len2((fabsf(px) - 0.27f) / 0.11f, (py - 1.44f) / 0.16f) - 1; if (er < 0) { cc = mul(sk, 0.50f); a = 1; }
            float hd = len2(px / 0.37f, (py - 1.14f) / 0.31f) - 1; if (hd < 0) { cc = mul(sk, 0.46f + 0.40f * sstep(0, -0.7f, hd)); a = 1;
                float sn = len2(px / 0.15f, (py - 1.07f) / 0.105f) - 1; if (sn < 0) cc = mul(V(0.90f, 0.45f, 0.44f), 0.74f + 0.16f * sstep(0, -0.6f, sn));
                if (len2(fabsf(px) - 0.055f, py - 1.07f) < 0.026f) cc = V(0.30f, 0.08f, 0.10f);
                if (len2(fabsf(px) - 0.16f, py - 1.23f) < 0.034f) cc = V(0.03f, 0.02f, 0.02f); }
            float ht = kind == 2 ? box2(fabsf(px) - 0.20f, fabsf(py - 1.56f) - 0.16f) - 0.06f                                                    // la toque, le canotier, la casquette
                     : kind == 0 ? fminf(box2(fabsf(px) - 0.40f, fabsf(py - 1.415f) - 0.018f), box2(fabsf(px) - 0.22f, fabsf(py - 1.49f) - 0.07f))
                     : fminf(box2(fabsf(px) - 0.26f, fabsf(py - 1.46f) - 0.05f) - 0.03f, box2(fabsf(px - 0.2f) - 0.2f, fabsf(py - 1.42f) - 0.012f));
            if (ht < 0) { cc = mul(kind == 2 ? V(0.80f, 0.79f, 0.75f) : kind == 0 ? V(0.72f, 0.54f, 0.24f) : V(0.16f, 0.17f, 0.20f), 0.7f); a = 1; }
            if (a) col = cc; } }
    if (fabsf(u1) < 4.4f && v1 > 0.14f && v1 < 0.78f) {        // le comptoir : du bois, et dessus une nappe, un marbre ou un zinc
        col = mul(V(0.24f, 0.11f, 0.05f), (0.36f + 0.40f * sstep(0.14f, 0.7f, v1)) * (0.85f + 0.15f * sstep(0.02f, 0.05f, fabsf(fract(u1 / 1.1f) - 0.5f) * 1.1f)));
        if (v1 > 0.60f) col = kind == 0 ? mul(gingham(u1, v1, 9.0f, sstep(0.5f, 0.15f, fw * 9.0f)), 0.60f) : kind == 2 ? V(0.52f, 0.50f, 0.45f) : mul(V(0.34f, 0.38f, 0.42f), 0.7f + 0.6f * sstep(0.70f, 0.78f, v1)); }
    else if (fabsf(u1) < 4.3f && v1 >= 0.78f && v1 < 1.07f) {   // posés dessus : des pâtés, des pains, les tireuses à bière
        float i = floorf(u1 / 0.9f), x = (fract(u1 / 0.9f) - 0.5f) * 0.9f, hh = hash2(i, 5.5f + h);
        float dd = kind == 1 ? fminf(box2(fabsf(x) - 0.018f, fabsf(v1 - 0.90f) - 0.12f), len2(x, v1 - 1.03f) - 0.035f) : len2(x / 0.24f, (v1 - 0.80f) / (kind == 2 ? 0.16f : 0.13f)) - 1;
        if (hh < 0.7f && dd < 0) col = kind == 1 ? V(0.80f, 0.56f, 0.18f) : mul(kind == 2 ? V(0.90f, 0.55f, 0.20f) : V(0.74f, 0.42f, 0.14f), 0.42f + 0.40f * sstep(0, -0.8f, dd)); }
    if (kind == 0 && fabsf(u0) < 4.85f) {      // la charcuterie pendue près de la vitre : jambons, saucissons, chapelet
        float i = roundf(u0 / 0.82f), x = u0 - i * 0.82f, hs = hash2(i, 7.7f + h), top = 2.62f - 0.12f * hs, d = ham2(x, v0 - (top - 0.62f));
        if (v0 > 2.74f && v0 < 2.80f) col = V(0.75f, 0.50f, 0.14f);                                                                                  // la tringle
        if (fabsf(x) < 0.008f && v0 > top && v0 < 2.78f) col = V(0.8f, 0.75f, 0.6f);
        if (d < 0) { float k = clampf(-d / 0.12f, 0, 1), net = sstep(0.07f, 0.03f, fminf(fabsf(fract((x + v0) * 13.0f) - 0.5f), fabsf(fract((x - v0) * 13.0f) - 0.5f))) * sstep(0.6f, 0.2f, fw * 13.0f);
            col = add(mul(mix3(V(0.50f, 0.20f, 0.08f), V(0.80f, 0.55f, 0.28f), 0.35f * net), 0.36f + 0.85f * k), mul(V(1, 0.9f, 0.7f), 0.4f * sstep(0.035f, 0.0f, fabsf(x + 0.06f + 0.2f * (v0 - top + 0.7f))) * k)); }
        float i2 = roundf(u0 / 0.82f - 0.5f) + 0.5f, x2 = u0 - i2 * 0.82f, hs2 = hash2(i2, 3.1f + h);
        for (int k = -1; k <= 1; k++) { float L = 0.30f + 0.08f * fract(hs2 * (7 + 3 * k)), y1 = 2.56f - 0.05f * (k == 0), xx = x2 - 0.075f * k, dd = len2(xx, v0 - clampf(v0, y1 - L, y1)) - 0.036f;
            if (fabsf(xx) < 0.006f && v0 > y1 && v0 < 2.78f) col = V(0.8f, 0.75f, 0.6f);
            if (dd < 0) col = mul(mix3(V(0.42f, 0.08f, 0.07f), V(0.75f, 0.66f, 0.58f), 0.45f * sstep(0.45f, 0.7f, vnoise(u0 * 60, v0 * 25)) * sstep(0.05f, 0.015f, fw)), 0.40f + 0.9f * clampf(-dd / 0.03f, 0, 1)); }
        float la = u0 - 1.64f * roundf(u0 / 1.64f), uu = la / 0.82f, yc = 2.80f - 0.26f * (1 - uu * uu), dc = fabsf(v0 - yc) / sqrtf(1 + 0.4f * uu * uu) - 0.032f * (0.5f + 0.5f * fabsf(sinf(la * 27.0f)));
        if (dc < 0) col = mul(V(0.95f, 0.30f, 0.24f), 0.45f + 0.8f * clampf(-dc / 0.025f, 0, 1));
    }
    if (kind == 1) { float uc = fract(h * 13.1f) < 0.5f ? -2.3f : 2.3f, dn = icon(1, (u0 - uc) / 1.4f, (v0 - 1.95f) / 1.4f) * 1.4f;                     // à la brasserie : une chope de néon dans la vitrine
        col = add(col, mul(V(1.0f, 0.55f, 0.08f), 2.4f * neonline(dn, 0.03f))); }
    if (kind == 2 && fabsf(u0) < 4.5f && v0 < 1.22f && v0 > 0.5f) {       // à la boulangerie : des pains ronds en devanture
        float row = v0 < 0.86f ? 0 : 1, yb = row ? 0.86f : 0.52f, x = (fract(u0 / 0.56f + 0.5f * row) - 0.5f) * 0.56f, dd = len2(x / 0.23f, (v0 - yb - 0.15f) / 0.13f) - 1;
        if (v0 - yb < 0.035f) col = V(0.10f, 0.05f, 0.03f);
        else if (dd < 0) col = mul(V(0.95f, 0.56f, 0.17f), 0.36f + 0.50f * sstep(0, -0.9f, dd) - 0.16f * sstep(0.1f, 0.0f, fabsf(fract(x * 11.0f) - 0.5f) - 0.33f)); }
    {   float ul = u + rd.z * 1.0f / rw, vl = v + rd.y * 1.0f / rw, x = ul - 2.45f * roundf(ul / 2.45f), d2 = x * x + (vl - 2.60f) * (vl - 2.60f);        // les globes du plafond
        if (fabsf(ul) < 4.6f) col = add(col, mul(V(1.0f, 0.78f, 0.45f), 2.6f * expf(-d2 / 0.010f) + 0.20f * expf(-d2 / 0.30f))); }
    return col;
}
static void cmaterial(v3 p, v3 n, int m, float t, mat_t *M) {
    if (m >= C_SKIN) {      // un habitant : le citoyen standard, avec la neige qui tient sur ses épaules et son chapeau
        v3 l; int k = city_cz(p, &l), mm; cz_sdf(&CZA[k], &CZR[k], &CZP[k], l, &mm); cmat_t cm; cz_mat(&CZA[k], &CZP[k], m, &cm);
        M->alb = cm.alb; M->emis = mul(cm.emis, 0.45f); M->spec = cm.spec; M->gloss = cm.gloss; M->sss = cm.sss; M->env = 0;
        if (cm.metal > 0) { M->alb = mul(cm.alb, 0.55f); M->spec = 2.0f; }
        if (cm.flat > 0) { M->alb = mul(cm.alb, 0.45f); M->spec = 0; }
        if (m == C_HAT || m == C_JACKET || m == C_TOP || m == C_SCARF || m == C_DRESS || m == C_APRON) { float s = 0.6f * sstep(0.72f, 0.95f, n.y) * sstep(0.35f, 0.6f, vnoise(p.x * 23, p.z * 23) + 0.25f);
            M->alb = mix3(M->alb, SNOWC, s); M->spec *= 1 - 0.8f * s; }
        return; }
    material(p, n, m, t, M);
    float ax = fabsf(p.x), snow = 0, up = sstep(0.55f, 0.8f, n.y);
    switch (m) {
    case M_GROUND: {
        float pn = fbm(p.x * 0.45f, p.z * 0.45f);
        if (p.z > SQ0 && p.z < SQ1 + 3 && ax < SQW) {           // the square: a wet checkerboard of marble
            float cx = floorf(p.x * 0.5f), cz = floorf(p.z * 0.5f), e = fminf(fabsf(fract(p.x * 0.5f) - 0.5f), fabsf(fract(p.z * 0.5f) - 0.5f));
            M->alb = mul(((int)(cx + cz + 200) & 1) ? V(0.30f, 0.03f, 0.07f) : V(0.78f, 0.74f, 0.64f), 0.5f * (0.9f + 0.1f * vnoise(p.x * 3, p.z * 3)));
            M->alb = mul(M->alb, 0.55f + 0.45f * sstep(0.495f, 0.47f, e) );
            M->env = 0.30f; M->spec = 1.2f; M->gloss = 120; snow = 0.75f * sstep(0.50f, 0.72f, pn + 0.12f * sstep(9.0f, 30.0f, len2(p.x, p.z - 85.0f)));
        } else if (p.z < SQ0 && ax < 6.0f) {                     // the roadway: wet asphalt, puddles, the rails
            float pud = sstep(0.52f, 0.62f, pn), cob = ax < 1.7f ? 0.75f + 0.25f * sstep(0.05f, 0.12f, fminf(fabsf(fract(p.z * 4.0f + floorf(p.x * 5.0f) * 0.5f) - 0.5f), fabsf(fract(p.x * 5.0f) - 0.5f))) : 1.0f;
            M->alb = mul(V(0.040f, 0.040f, 0.046f), cob * (1 - 0.5f * pud)); M->env = 0.07f + 0.45f * pud; M->spec = 0.8f; M->gloss = 60;
            if (fabsf(ax - 0.72f) < 0.05f) { M->alb = V(0.30f, 0.30f, 0.32f); M->env = 0.6f; }
            snow = sstep(4.3f, 5.7f, ax) * sstep(0.30f, 0.55f, pn + 0.2f) + 0.5f * sstep(0.60f, 0.75f, fbm(p.x * 1.3f + 9, p.z * 0.25f)) * sstep(1.2f, 2.2f, ax);
        } else if (p.z < SQ0 && ax < FX + 0.3f) {                // the pavements
            float e = fminf(fabsf(fract(p.x * 1.6f) - 0.5f), fabsf(fract(p.z * 1.6f) - 0.5f));
            M->alb = mul(V(0.17f, 0.16f, 0.15f), 0.75f + 0.25f * sstep(0.02f, 0.06f, 0.5f - e)); M->env = 0.06f; M->spec = 0.5f;
            snow = sstep(0.40f, 0.62f, fbm(p.x * 0.9f, p.z * 0.9f) + 0.25f * sstep(8.2f, 6.2f, ax));
        } else { M->alb = V(0.05f, 0.05f, 0.055f); M->env = 0.05f; snow = sstep(0.30f, 0.55f, pn + 0.15f); }
        break; }
    case M_FACADE: {
        float bx, lz, H, h, wall; rowcell(p, &bx, &lz, &H, &h, &wall);
        float sg = p.x > 0 ? 1.0f : -1.0f, iz = roundf(p.z / CL), v = p.y, fr = bx + 6.5f; v3 wc = WALLC[(int)(fract(h * 13.3f) * 4.999f)];
        M->alb = mul(wc, 0.55f); M->spec = 0.1f;
        if (CPX < 1e8f && sg > 0 && iz == 1 && fr > 0.16f && fr < 1.8f && v < 3.0f && v > 0.45f && lz < 3.4f) {       // l'intérieur de la charcuterie : du carrelage, une frise
            float a = fabsf(n.x) > 0.5f ? lz : fr, e = fminf(fabsf(fract(a / 0.30f) - 0.5f), fabsf(fract(v / 0.30f) - 0.5f));
            M->alb = mul(V(0.80f, 0.77f, 0.68f), 0.82f + 0.18f * sstep(0.44f, 0.38f, 0.5f - e + 0.38f)); M->spec = 0.7f; M->gloss = 60;
            M->alb = mul(V(0.62f, 0.59f, 0.50f), 1 - 0.14f * sstep(0.04f, 0.015f, e) * sstep(0.012f, 0.004f, t * 0.0034f / fmaxf(fabsf(dot(n, nrm(sub(p, RO)))), 0.15f)));
            if (fabsf(v - 2.02f) < 0.075f) M->alb = V(0.34f, 0.04f, 0.08f);
            if (n.y > 0.5f) M->alb = V(0.10f, 0.08f, 0.07f); if (n.y < -0.5f) M->alb = V(0.75f, 0.72f, 0.64f);
            if (fabsf(n.x) > 0.5f && fr > 1.5f) { float cu = lz - 1.8f, cv = v - 1.74f, cr = len2(cu, cv);                    // au mur du fond, la pendule : cinq heures
                if (cr < 0.24f) { M->alb = cr > 0.205f ? V(0.50f, 0.34f, 0.10f) : V(0.86f, 0.83f, 0.74f); M->spec = cr > 0.205f ? 1.4f : 0.3f;
                    if (cr < 0.205f && ((cr > 0.165f && fabsf(fract(atan2f(cu, cv) * 1.9099f + 0.5f) - 0.5f) < 0.07f) || cr < 0.022f || seg2(cu, cv, 0, 0.16f) < 0.009f || seg2(cu, cv, -0.055f, -0.095f) < 0.014f)) M->alb = V(0.04f, 0.03f, 0.03f); } }
        } else if (fabsf(n.x) > 0.5f && bx < -6.0f) {      // the front, on the avenue
            float u = lz, ut = sg > 0 ? -lz : lz; v3 sc = shopcol(h);
            if (fabsf(u) > 5.45f) M->alb = mul(wc, 0.75f);                                               // corner stones
            if (v < 3.35f) {        // la boutique
                M->alb = V(0.06f, 0.05f, 0.05f);
                if (v < 0.5f) M->alb = V(0.16f, 0.15f, 0.14f);
                else if (fabsf(u) < 4.9f && v < 2.95f && fr > 0.05f) {
                    int id = shoptext(h, iz, sg), lit = shoplit(h, id), kind = (id == T_BRASS || id == T_AMBREE) ? 1 : id == T_BOUL ? 2 : 0, door = u > 3.3f;
                    float mull = fabsf(u + 0.8f - 2.05f * roundf((u + 0.8f) / 2.05f));
                    if (mull < 0.04f || fabsf(u - 3.3f) < 0.06f || v > 2.88f || v < 0.57f) M->alb = mul(mix3(V(0.10f, 0.12f, 0.10f), sc, 0.15f), 0.6f);       // les montants de la devanture
                    else if (!lit && fract(h * 29.3f) < 0.55f) { M->alb = mul(V(0.13f, 0.15f, 0.15f), 0.55f + 0.45f * sstep(0.15f, 0.5f, fract(v * 9.0f))); M->spec = 0.4f; }   // le rideau de fer
                    else { M->alb = V(0.02f, 0.02f, 0.025f); M->spec = 1.0f; M->gloss = 100; M->env = 0.10f;
                        if (!lit) M->emis = mul(V(0.06f, 0.04f, 0.025f), sstep(2.9f, 0.6f, v));
                        else if (door) M->emis = mul(mix3(V(1.0f, 0.60f, 0.25f), sc, 0.10f), 0.55f * (0.5f + 0.5f * sstep(0.5f, 2.9f, v)) * (fabsf(u - 4.1f) > 0.03f));
                        else M->emis = storefront(u, v, nrm(sub(p, RO)), sg, kind, h, sc, t); }
                }
                if (v > 3.0f) M->alb = V(0.03f, 0.03f, 0.035f);
            } else if (v < 4.05f) {     // the fascia and its neon letters
                M->alb = V(0.035f, 0.03f, 0.035f);
                int id = shoptext(h, iz, sg);
                if (id >= 0) { float sf = fminf(8.6f / (TR[id].w - 80.0f), 0.40f / 67.0f), d = text2d(id, ut / sf + TR[id].w * 0.5f, 134.0f - (v - 3.52f) / sf) * sf;
                    float fl = fract(h * 23.7f) < 0.2f ? 0.55f + 0.45f * (hash2(floorf(T * 13), h) > 0.35f) : 1.0f;
                    M->emis = mul(sc, fl * (5.0f * sstep(0.02f, -0.02f, d) + 0.9f * expf(-fmaxf(d, 0) * 9))); }
            } else {        // the floors: windows in their reveals, some lit, some shuttered
                float fu = fract(u / 2.3f + 0.5f), rw = floorf((v - 4.2f) / 3.1f), fv = fract((v - 4.2f) / 3.1f), wi = floorf(u / 2.3f + 0.5f) + 7 * rw, hw = hash2(wi + h * 50, h * 9 + iz);
                int ok = 4.2f + 3.1f * (rw + 0.48f) + 0.99f < H - 0.5f && fabsf(u) < 5.2f;
                if (v > H - 0.45f) M->alb = mul(wc, 0.8f);
                else if (ok && fu > 0.27f && fu < 0.73f && fv > 0.16f && fv < 0.80f) {
                    if (fract(hw * 7.1f) < 0.38f) M->alb = mul(fract(h * 3.1f) < 0.5f ? V(0.30f, 0.05f, 0.08f) : V(0.10f, 0.20f, 0.18f), (0.8f + 0.2f * sstep(0.3f, 0.5f, fract(v * 6))) * (fabsf(fu - 0.5f) > 0.012f));    // shutters
                    else { M->alb = V(0.02f, 0.03f, 0.05f); M->spec = 1.0f; M->gloss = 90; M->env = 0.12f;
                        float cross = (fabsf(fu - 0.5f) > 0.02f) * (fabsf(fv - 0.52f) > 0.012f);
                        if (hw < 0.20f) M->emis = mul(V(2.4f, 1.4f, 0.6f), (0.6f + 2.0f * hw) * cross * (0.75f + 0.25f * sstep(0.21f, 0.8f, fv)));
                        else if (hw < 0.24f) M->emis = mul(V(0.4f, 0.65f, 1.3f), 0.55f + 0.45f * hash2(floorf(T * 7), hw)); }
                } else if (ok && fu > 0.22f && fu < 0.78f && fv > 0.12f && fv < 0.84f) M->alb = mul(wc, 0.85f);
                if (n.y > 0.5f) snow = 1;
            }
        } else if (bx < -6.25f && v < H + 0.2f && v > 0.3f) { M->alb = mul(wc, 0.62f); if (n.y > 0.5f) snow = 1; }      // les tableaux des fenêtres, les appuis, la corniche
        else { M->alb = mul(wc, 0.38f);
            float u = fabsf(n.z) > 0.5f ? bx : lz, fu = fract(u / 2.6f + 0.5f), fv = fract((v - 1.0f) / 3.1f), hw = hash2(floorf(u / 2.6f + 0.5f) + 7 * floorf((v - 1.0f) / 3.1f) + h * 31, iz + n.z * 3);
            if (fu > 0.3f && fu < 0.7f && fv > 0.25f && fv < 0.75f && v < H - 0.6f && v > 1.0f && fabsf(n.y) < 0.3f) { M->alb = V(0.02f, 0.03f, 0.05f); if (hw < 0.16f) M->emis = mul(V(2.4f, 1.4f, 0.6f), 0.5f + 2.5f * hw); } }
        break; }
    case M_ROOF: M->alb = hash2(roundf(p.z / CL), p.x > 0 ? 3.0f : 5.0f) < 0.5f ? V(0.09f, 0.10f, 0.13f) : V(0.20f, 0.09f, 0.07f); M->spec = 0.2f; snow = 0.55f + 0.45f * sstep(0.25f, 0.5f, fbm(p.x * 0.6f, p.z * 0.6f)); up = 1;
        if (n.y < 0.25f) { snow = 0; M->alb = mul(V(0.46f, 0.40f, 0.30f), 0.55f);                                              // un pignon : du mur (au Sénat, des pans de bois)
            if (p.x < -30 && p.z > SQ0) { M->alb = V(0.50f, 0.43f, 0.31f); if (fminf(fabsf(fract(p.x / 1.6f) - 0.5f) * 1.6f, fabsf(p.y - 15.4f)) < 0.11f || fabsf(n.x) > 0.5f && fabsf(fabsf(p.z - 85.0f - 8.0f * roundf((p.z - 85.0f) / 8.0f)) - 0.9f) < 0.12f) M->alb = V(0.085f, 0.052f, 0.034f); } }
        break;
    case M_CHIM: M->alb = V(0.24f, 0.11f, 0.08f); snow = 1; break;
    case M_AWN: { float h = bh(roundf(p.z / CL), p.x > 0 ? 1.0f : -1.0f); v3 c = fract(h * 4.3f) < 0.4f ? V(0.40f, 0.04f, 0.09f) : fract(h * 4.3f) < 0.7f ? V(0.06f, 0.22f, 0.16f) : V(0.08f, 0.12f, 0.30f);
        M->alb = fract(p.z * 1.2f) < 0.5f ? c : V(0.80f, 0.76f, 0.66f); snow = 1; break; }
    case M_BLADE: { float h = bh(roundf(p.z / CL), p.x > 0 ? 1.0f : -1.0f); M->alb = V(0.02f, 0.02f, 0.025f); M->spec = 0.6f;
        if (fabsf(n.z) > 0.5f) { int k = (int)(fract(h * 6.7f) * 2.999f); float d = icon(k, ax - (FX - 0.75f), p.y - 5.4f);
            float fl = fract(h * 31.3f) < 0.25f ? 0.4f + 0.6f * (hash2(floorf(T * 11), h * 3) > 0.3f) : 1.0f;
            M->emis = mul(NEON[(k + (int)(fract(h * 2.9f) * 2.999f)) % 3], fl * 6.0f * neonline(d, 0.022f)); }
        snow = 1; break; }
    case M_BLOCK: { float c[7], h; blockcell(p.x, p.z, c, &h);
        v3 q = V(p.x - c[0], p.y, p.z - c[1]);
        if (n.y <= 0.3f && q.y > c[4] + 0.02f) M->alb = mul(WALLC[(int)(fract(h * 7.7f) * 4.999f)], 0.40f);
        else if (n.y > 0.3f) { M->alb = fract(h * 3.3f) < 0.6f ? V(0.09f, 0.10f, 0.13f) : V(0.20f, 0.09f, 0.07f); snow = 0.6f + 0.4f * sstep(0.25f, 0.5f, fbm(p.x * 0.5f, p.z * 0.5f)); up = 1; }
        else { M->alb = mul(WALLC[(int)(fract(h * 7.7f) * 4.999f)], 0.45f);
            float u = fabsf(n.x) > 0.5f ? q.z : q.x, fu = fract(u / 2.4f + 0.5f), fv = fract((q.y - 0.6f) / 3.0f), hw = hash2(floorf(u / 2.4f + 0.5f) + 7 * floorf((q.y - 0.6f) / 3.0f) + h * 50, h * 9 + n.x * 3);
            if (fu > 0.3f && fu < 0.7f && fv > 0.25f && fv < 0.78f && q.y < c[4] - 0.3f && q.y > 0.5f) { M->alb = V(0.02f, 0.03f, 0.05f);
                if (hw < 0.16f) M->emis = mul(V(2.6f, 1.5f, 0.6f), 0.5f + 2.5f * hw); else if (hw < 0.19f) M->emis = V(0.4f, 0.6f, 1.2f); } }
        break; }
    case M_LPOST: M->alb = V(0.03f, 0.05f, 0.045f); M->spec = 0.8f; M->gloss = 60; snow = 1; break;
    case M_WIRE: M->alb = V(0.02f, 0.02f, 0.02f); break;
    case M_LBULB: M->emis = V(15.0f, 9.5f, 3.8f); M->alb = V(0, 0, 0); break;
    case M_TRAM: { v3 q = V(p.x, p.y, p.z - TRZ); float az = fabsf(q.z);
        M->alb = q.y < 1.72f ? V(0.34f, 0.035f, 0.075f) : V(0.80f, 0.76f, 0.66f); M->spec = 1.0f; M->gloss = 80; M->env = 0.12f;
        if (fabsf(q.y - 1.76f) < 0.05f) { M->alb = V(1.0f, 0.72f, 0.28f); M->env = 0.6f; }
        if (fabsf(n.x) > 0.5f && q.y > 1.92f && q.y < 3.08f && az < 6.4f) {     // le bandeau des vitres : la lumière de la voiture, des têtes
            M->alb = V(0.05f, 0.03f, 0.03f); M->env = 0.05f;
            float fu = fract(q.z / 1.55f), cu = floorf(q.z / 1.55f);
            if (fu > 0.07f && fu < 0.93f && q.y > 2.0f && q.y < 3.0f) { float hd = hash2(cu * 1.7f, q.x > 0 ? 1.0f : 2.0f), x = (fu - 0.32f - 0.36f * hd) * 1.55f, y = q.y - 2.0f;
                float sil = hd < 0.55f ? fminf(fminf(len2(x / 0.21f, (y - 0.30f) / 0.18f) - 1, len2((fabsf(x) - 0.15f) / 0.06f, (y - 0.52f) / 0.09f) - 1), len2(x / 0.27f, (y + 0.12f) / 0.30f) - 1) : 1;
                M->emis = mul(V(1.05f, 0.62f, 0.24f), (sil < 0 ? 0.10f : 1.0f) * (0.70f + 0.30f * sstep(0.0f, 1.0f, y))); M->alb = V(0.02f, 0.02f, 0.02f); M->env = 0.12f; } }
        if (fabsf(n.z) > 0.5f) {    // the ends: windscreen, lamps, the name of the stop
            int front = (q.z * TDIR) > 0;
            if (q.y > 1.95f && q.y < 2.95f && fabsf(q.x) < 0.95f) { float x = q.x + (q.z > 0 ? 0.42f : -0.42f), y = q.y - 1.95f;                // le pare-brise, et à l'avant le wattman sous sa casquette
                float sil = front ? fminf(fminf(len2(x / 0.21f, (y - 0.36f) / 0.18f) - 1, len2((fabsf(x) - 0.15f) / 0.06f, (y - 0.56f) / 0.09f) - 1), fminf(len2(x / 0.27f, (y + 0.06f) / 0.30f) - 1, box2(fabsf(x) - 0.22f, fabsf(y - 0.56f) - 0.04f))) : 1;
                M->emis = mul(V(0.80f, 0.50f, 0.22f), (fabsf(q.x) > 0.03f ? 1.0f : 0.1f) * (sil < 0 ? 0.12f : 1.0f)); M->alb = V(0.02f, 0.02f, 0.02f); M->env = 0.25f; }
            if (len2(fabsf(q.x) - 0.62f, q.y - 1.08f) < 0.15f) M->emis = front ? V(14.0f, 12.0f, 8.0f) : V(6.0f, 0.3f, 0.2f);
            if (q.y > 3.02f && q.y < 3.32f && fabsf(q.x) < 0.9f) { float uu = (q.z > 0 ? -q.x : q.x), sf = 1.6f / (TR[T_PLACE].w - 80.0f), d = text2d(T_PLACE, uu / sf + TR[T_PLACE].w * 0.5f, 134.0f - (q.y - 3.09f) / sf) * sf;
                M->alb = V(0.02f, 0.02f, 0.02f); M->emis = mul(V(1.0f, 0.6f, 0.08f), 4.0f * sstep(0.008f, -0.008f, d)); }
        }
        if (q.y > 3.3f) snow = 1;
        break; }
    case M_CHROME: M->alb = V(0.93f, 0.95f, 1.0f); M->env = 1; M->spec = 1.5f; M->gloss = 200; snow = 0.85f * sstep(0.80f, 0.97f, n.y); up = 1; break;
    case M_GOLD: M->spec = 1.5f; M->gloss = 150; break;
    case M_TITLE: { float dx = p.x - (TXC - 48.0f + 96.0f * fract(T * 0.22f));
        M->alb = V(1.0f, 0.74f, 0.30f); M->env = 1; M->spec = 2.0f; M->gloss = 150; M->emis = mul(V(1.0f, 0.9f, 0.7f), 14.0f * expf(-dx * dx * 1.2f) * sstep(0.6f, 0.2f, fabsf(n.z))); break; }
    case M_PED: M->alb = mul(V(0.30f, 0.29f, 0.27f), 0.8f + 0.4f * vnoise(p.x * 2 + p.y * 2, p.z * 2)); M->spec = 0.3f; snow = 1;
        if (fabsf(p.x) < 2.7f && fabsf(p.z - 85.0f) < 4.1f && p.y > 0.75f && p.y < 2.12f && fabsf(n.y) < 0.5f) {      // sur le dé du socle : des plaques de bronze, leur inscription
            float a = fabsf(n.x) > 0.5f ? p.z - 85.0f : p.x, hw = fabsf(n.x) > 0.5f ? 2.6f : 1.5f, e = fminf(hw - fabsf(a), fminf(p.y - 0.98f, 1.92f - p.y));
            if (e > 0) { M->alb = e < 0.07f ? V(0.62f, 0.44f, 0.16f) : mul(V(0.13f, 0.085f, 0.04f), 0.8f + 0.4f * vnoise(a * 9, p.y * 9)); M->spec = e < 0.07f ? 1.6f : 0.9f; M->gloss = 60; snow = 0;
                if (e > 0.2f && fabsf(fract((p.y - 1.05f) / 0.17f) - 0.5f) * 0.17f < 0.024f && hash2(floorf((p.y - 1.05f) / 0.17f), floorf(a * 2.2f)) < 0.75f) M->alb = V(0.50f, 0.35f, 0.13f); } }
        break;
    case M_COL: M->alb = mul(V(0.50f, 0.47f, 0.42f), 0.8f + 0.2f * sstep(0.3f, 0.7f, fract(atan2f(p.z - 111.4f, p.x - (clampf(floorf(p.x / 5.5f), -4, 3) + 0.5f) * 5.5f) * 3.18f))); M->spec = 0.3f; break;
    case M_DOME: { float a = atan2f(p.z - 124.0f, p.x) * 2.5465f; M->alb = mix3(V(0.09f, 0.24f, 0.20f), V(0.17f, 0.38f, 0.31f), sstep(0.10f, 0.04f, fabsf(fract(a) - 0.5f))); M->spec = 0.8f; M->gloss = 40; M->env = 0.1f;
        snow = 0.9f * sstep(0.35f, 0.65f, fbm(p.x * 0.5f, p.z * 0.5f) + 0.5f * (n.y - 0.6f)); break; }
    case M_MIN: { float ax = fabsf(p.x), fade = sstep(100.0f, 30.0f, t); const v3 LIT = V(1.9f, 1.28f, 0.58f);
        M->alb = mul(V(0.38f, 0.36f, 0.32f), 0.92f + 0.16f * vnoise(p.x * 0.7f + p.z, p.y * 0.7f)); M->spec = 0.2f; snow = 1;
        if (p.z < 110.6f && fabsf(p.y - 13.5f) < 1.25f && ax < 22.45f) {      // la frise : MINISTÈRE DU PORC
            float sf = 36.0f / (TR[T_MIN].w - 80.0f), d = text2d(T_MIN, p.x / sf + TR[T_MIN].w * 0.5f, 134.0f - (p.y - 12.95f) / sf) * sf;
            M->alb = V(0.03f, 0.03f, 0.04f); M->emis = mul(V(0.45f, 0.85f, 1.0f), 2.3f * sstep(0.03f, -0.03f, d) + 0.32f * expf(-fmaxf(d, 0) * 5)); snow = 0;
        } else if (p.z < 111.3f && p.y > 14.85f) {                             // le fronton : au fond du tympan, un oculus éclairé
            float ro = len2(p.x, p.y - 16.9f);
            if (p.z > 110.7f) { M->alb = mul(M->alb, 0.78f);
                if (ro < 1.3f) { float cu = p.x, cv = p.y - 16.9f;                                                          // une horloge : cinq heures
                    int ink = (ro > 1.02f && fabsf(fract(atan2f(cu, cv) * 1.9099f + 0.5f) - 0.5f) < 0.075f) || ro < 0.11f || seg2(cu, cv, 0, 0.98f) < 0.05f || seg2(cu, cv, 0.33f, -0.57f) < 0.075f;
                    M->alb = V(0.02f, 0.03f, 0.05f); M->emis = ink ? V(0.02f, 0.015f, 0.01f) : V(1.5f, 1.2f, 0.8f); snow = 0; }
                else if (ro < 1.62f) { M->alb = V(0.50f, 0.36f, 0.14f); M->spec = 1.2f; M->gloss = 60; } }
        } else if (n.z < -0.5f && p.z > 115.2f && p.z < 115.5f && p.y < 13.0f) {   // au fond des baies : des vitres hautes, la porte de bronze
            int wing = ax > 19.3f, door = ax < 2.75f; float xr = p.x - 5.5f * roundf(p.x / 5.5f), u = wing ? fabsf(ax - 26.25f) - 1.85f : door ? p.x : xr;
            float y0 = wing ? (p.y > 7.4f ? 8.4f : 1.9f) : door ? 0.9f : 2.2f, y1 = wing ? (p.y > 7.4f ? 12.4f : 6.5f) : door ? 8.3f : 9.6f, fv = (p.y - y0) / (y1 - y0);
            float id = wing ? (ax > 26.25f) + 2.0f * (p.x > 0) + 4.0f * (p.y > 7.4f) + 20 : roundf(p.x / 5.5f);
            M->alb = V(0.02f, 0.03f, 0.05f); M->spec = 1.0f; M->gloss = 90; M->env = 0.10f; snow = 0;
            if (door && fv < 0.70f) { float pu = fabsf(fabsf(u) - 0.85f), pv = fract(fv / 0.70f * 3.0f);                    // deux vantaux à caissons
                M->alb = mul(V(0.15f, 0.095f, 0.045f), (pu < 0.60f && pv > 0.12f && pv < 0.88f) ? 0.65f : 1.15f); M->spec = 1.2f; M->gloss = 50; M->env = 0;
                if (fabsf(u) < 0.03f) M->alb = V(0.02f, 0.02f, 0.02f);
                if (len2(fabsf(u) - 0.16f, p.y - 4.0f) < 0.07f) { M->alb = V(0.62f, 0.44f, 0.16f); M->spec = 1.6f; } }
            else { float lit = hash2(id * 3.7f + 1.3f, 5.1f) < 0.72f || door, bars = (fabsf(u) > 0.045f) * (fabsf(fract(fv * (wing ? 3.0f : door ? 1.0f : 5.0f)) - 0.5f) < 0.465f);
                M->emis = mul(LIT, lit * bars * (0.55f + 0.45f * fv) * (0.8f + 0.2f * hash2(id, 2.2f))); }
        } else if (fabsf(n.x) > 0.5f && ax > 29.9f && p.y < 13.4f && p.y > 1.5f && p.z > 115.0f) {                         // les côtés : des fenêtres
            float fu = fract((p.z - 115.0f) / 3.6f); int fl = p.y > 7.4f;
            if (fu > 0.24f && fu < 0.76f && p.y > (fl ? 8.4f : 1.9f) && p.y < (fl ? 12.4f : 6.5f)) { M->alb = V(0.02f, 0.03f, 0.05f); snow = 0;
                if (hash2(floorf((p.z - 115.0f) / 3.6f) + 7 * fl, p.x > 0 ? 1.0f : 2.0f) < 0.3f) M->emis = mul(LIT, 0.9f * (fabsf(fu - 0.5f) > 0.015f)); }
        } else if (p.y > 14.85f && p.y < 17.15f && fabsf(n.y) < 0.5f && len2(p.x, p.z - 124.0f) > 8.6f) {                  // l'attique : de petites fenêtres carrées
            float u = fabsf(n.x) > 0.5f ? p.z : p.x, fu = fract(u / 3.0f + 0.5f);
            if (fu > 0.3f && fu < 0.7f && p.y > 15.5f && p.y < 16.7f) { M->alb = V(0.02f, 0.03f, 0.05f); snow = 0; if (hash2(floorf(u / 3.0f + 0.5f), 7.7f + roundf(n.x)) < 0.3f) M->emis = mul(LIT, 0.8f); }
        } else if (p.y > 16.0f && len2(p.x, p.z - 124.0f) < 8.6f && fabsf(n.y) < 0.5f) {                                   // le tambour : des pilastres, des fenêtres
            float a = atan2f(p.z - 124.0f, p.x) * 2.5465f, fa = fract(a);
            if (fa > 0.3f && fa < 0.7f && p.y > 16.9f && p.y < 18.6f) { M->alb = V(0.02f, 0.03f, 0.05f); snow = 0; if (hash2(floorf(a), 4.4f) < 0.45f) M->emis = mul(LIT, 0.85f); }
            else if (fa < 0.09f || fa > 0.91f) M->alb = mul(M->alb, 1.18f);
        } else if (fabsf(n.y) < 0.5f && p.y < 7.6f && p.z > 114.9f) {                                                      // le rez-de-chaussée à refends, le bandeau
            if (p.y > 7.2f) M->alb = mul(M->alb, 1.14f); else M->alb = mul(M->alb, 1 - 0.28f * fade * sstep(0.05f, 0.02f, fabsf(fract(p.y / 0.75f) - 0.5f) * 0.75f)); }
        break; }
    case M_SENAT: { v3 s = sub(p, V(-43.0f, 0, 85.0f)); float fade = sstep(100.0f, 30.0f, t), aa = 0.015f + t * 0.0022f; int front = n.x > 0.5f;
        float u = fabsf(n.x) > 0.5f ? s.z : s.x; const v3 TIM = V(0.085f, 0.052f, 0.034f), PLA = V(0.52f, 0.45f, 0.32f), AMB = V(2.3f, 1.25f, 0.38f); M->spec = 0.1f; snow = 1;
        if (s.y < 4.4f) {       // le rez-de-chaussée : de la pierre, les fenêtres de la taverne, la porte
            float row = floorf(s.y / 0.55f), uu = u / 1.1f + 0.5f * row, jb = fminf(fabsf(fract(s.y / 0.55f) - 0.5f) * 0.55f, fabsf(fract(uu) - 0.5f) * 1.1f);
            M->alb = mul(V(0.27f, 0.235f, 0.19f), (0.85f + 0.3f * hash2(floorf(uu + 0.5f), row)) * (1 - 0.4f * fade * sstep(0.045f, 0.015f, jb)));
            if (front && s.x < 9.15f) { float k = roundf(s.z / 6.0f), zr = s.z - 6.0f * k; snow = 0;
                if (k == 0) { M->alb = mul(V(0.17f, 0.085f, 0.04f), 0.75f + 0.25f * sstep(0.015f, 0.04f, fabsf(fract(zr / 0.36f) - 0.5f) * 0.36f)); M->spec = 0.4f;       // la porte : des planches, des pentures, une imposte
                    if (fabsf(zr) < 0.025f) M->alb = V(0.02f, 0.015f, 0.01f);
                    if (fabsf(fabsf(s.y - 1.35f) - 0.75f) < 0.06f && fabsf(fabsf(zr) - 0.65f) < 0.55f) { M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 1.0f; }
                    if (s.y > 2.55f) { M->alb = V(0.02f, 0.02f, 0.02f); M->emis = mul(AMB, 0.75f * (fabsf(fract(atan2f(s.y - 2.5f, zr) * 1.59f) - 0.5f) < 0.45f) * (len2(zr, s.y - 2.5f) > 0.25f)); } }
                else { float ld = fminf(fabsf(fract((zr + s.y) / 0.4f) - 0.5f), fabsf(fract((zr - s.y) / 0.4f) - 0.5f)) * 0.4f;                                           // des vitres en losanges, la lumière ambrée de la salle
                    M->alb = V(0.02f, 0.02f, 0.02f); M->spec = 1.0f; M->gloss = 80;
                    M->emis = mul(AMB, (0.55f + 0.45f * hash2(k, 3.3f)) * (0.7f + 0.3f * sstep(1.2f, 4.0f, s.y)) * (1 - 0.75f * fade * sstep(0.03f, 0.01f, ld)) * (fabsf(zr) > 0.045f)); } }
        } else if (s.y < 12.3f && fabsf(n.y) < 0.5f) {     // les étages : le torchis clair, les pans de bois, l'enseigne
            float pk = roundf(u / 3.0f), lu = u - 3.0f * pk; int fl = s.y >= 8.3f; float lv = s.y - (fl ? 8.3f : 4.6f), hp = hash2(pk * 1.3f + fl * 7.1f, 2.0f + roundf(n.x) + 2.0f * roundf(n.z));
            float tim = fminf(1.5f - fabsf(lu) - 0.12f, fminf(fminf(fabsf(s.y - 4.62f), fabsf(s.y - 8.3f)), fabsf(s.y - 12.05f)) - 0.13f);
            int signz = front && fabsf(s.z) < 15.6f && s.y > 8.55f && s.y < 11.75f, wz = hp < 0.62f && !signz, win = wz && fabsf(lu) < 0.66f && lv > 0.95f && lv < 3.0f;
            if (wz) tim = fminf(tim, fminf(fabsf(fabsf(lu) - 0.76f) - 0.09f, fmaxf(fminf(fabsf(lv - 0.85f), fabsf(lv - 3.1f)) - 0.09f, fabsf(lu) - 0.85f)));               // l'encadrement de la fenêtre
            else tim = fminf(tim, fabsf(lu * 3.7f * (hp < 0.81f ? 1.0f : -1.0f) - (lv - 1.85f) * 3.0f) / 4.763f - 0.10f);                                              // ou une écharpe en travers du panneau
            M->alb = mix3(TIM, mul(PLA, 0.9f + 0.2f * vnoise(u * 1.3f, s.y * 1.3f)), sstep(-aa, aa, tim));
            if (signz) { float sf = 30.0f / (TR[T_SENAT].w - 80.0f), dt = text2d(T_SENAT, s.z / sf + TR[T_SENAT].w * 0.5f, 134.0f - (p.y - 9.45f) / sf) * sf, e = fminf(15.6f - fabsf(s.z), fminf(s.y - 8.55f, 11.75f - s.y));
                M->alb = V(0.035f, 0.025f, 0.03f); M->emis = mul(V(1.0f, 0.12f, 0.45f), 5.0f * sstep(0.03f, -0.03f, dt) + 0.9f * expf(-fmaxf(dt, 0) * 5)); snow = 0;
                if (fabsf(e - 0.14f) < 0.03f) M->emis = V(2.0f, 1.1f, 0.3f); }
            else if (win) { M->alb = V(0.02f, 0.02f, 0.02f); M->spec = 1.0f; M->gloss = 80; snow = 0;
                if (hash2(pk * 2.9f + fl * 3.3f, 8.8f + roundf(n.x) + roundf(n.z)) < 0.6f) M->emis = mul(AMB, (0.5f + 0.5f * hash2(pk, fl + 4.4f)) * (fabsf(lu) > 0.03f) * (fabsf(lv - 2.2f) > 0.03f) * (0.7f + 0.3f * sstep(0.95f, 3.0f, lv))); }
        } else if (s.y >= 12.3f) {      // les lucarnes
            float zr = s.z - 8.0f * roundf(s.z / 8.0f); M->alb = mul(PLA, 0.85f);
            if (front) { if (fabsf(zr) > 0.80f || s.y > 15.1f) M->alb = TIM;
                else if (fabsf(zr) < 0.62f && s.y > 13.95f && s.y < 15.0f) { M->alb = V(0.02f, 0.02f, 0.02f); snow = 0; if (hash2(roundf(s.z / 8.0f), 6.6f) < 0.7f) M->emis = mul(AMB, 0.8f * (fabsf(zr) > 0.03f)); } } }
        break; }
    case M_HALLES: { v3 s = sub(p, V(43.0f, 0, 85.0f)); M->alb = V(0.035f, 0.10f, 0.09f); M->spec = 0.9f; M->gloss = 50; snow = 1;      // le fer peint, la brique du soubassement
        if (s.y < 1.12f && fabsf(s.x) < 9.3f) { float jb = fminf(fabsf(fract(s.y / 0.28f) - 0.5f) * 0.28f, fabsf(fract((fabsf(n.x) > 0.5f ? s.z : s.x) / 0.6f + 0.5f * floorf(s.y / 0.28f)) - 0.5f) * 0.6f);
            M->alb = mul(V(0.26f, 0.10f, 0.07f), 1 - 0.35f * sstep(90.0f, 25.0f, t) * sstep(0.03f, 0.012f, jb)); M->spec = 0.15f; }
        break; }
    case M_HGLASS: { v3 s = sub(p, V(43.0f, 0, 85.0f)), rdv = nrm(sub(p, RO)); float fade = sstep(110.0f, 30.0f, t); const v3 warm = V(1.0f, 0.55f, 0.20f);
        M->alb = V(0.02f, 0.035f, 0.035f); M->spec = 1.0f; M->gloss = 90; M->env = 0.14f;
        if (s.y > 10.02f && fabsf(n.z) < 0.7f) {        // la voûte : de longs rangs de vitres, la lueur de la halle, la neige qui tient en haut
            float arc = atan2f(s.x, s.y - 10.0f) * 9.0f, g = fminf(fabsf(fract(arc / 1.5f) - 0.5f) * 1.5f, fabsf(fract(s.z) - 0.5f));
            M->emis = mul(add(mul(warm, 0.20f), V(0.02f, 0.05f, 0.04f)), 1 - 0.6f * fade * sstep(0.06f, 0.025f, g));
            snow = sstep(0.45f, 0.80f, n.y + 0.35f * (fbm(s.z * 0.35f, arc * 0.35f) - 0.5f)); up = 1;
        } else if (fabsf(n.z) > 0.7f && s.y > 10.0f) {  // les pignons : un éventail de vitres
            float rr = len2(s.x, s.y - 10.0f), sp = fabsf(fract(atan2f(s.y - 10.0f, s.x) * 4.4563f) - 0.5f) * rr * 0.2244f, rg = fabsf(fract(rr / 3.0f) - 0.5f) * 3.0f;
            M->emis = mul(warm, 0.26f * (1 - 0.65f * fade * sstep(0.07f, 0.03f, fminf(sp, rg))));
        } else {        // les murs : de grandes baies, et derrière la halle allumée, ses lampes, ses étals
            int lg = fabsf(n.x) > 0.5f; float u = lg ? s.z : s.x, v = s.y, ru = lg ? rdv.z : rdv.x, rw = fmaxf(lg ? fabsf(rdv.x) : fabsf(rdv.z), 0.12f);
            float g = fminf(fabsf(fract(u) - 0.5f), fabsf(fract(v / 1.25f) - 0.5f) * 1.25f);
            float ua = u + ru * 3.0f / rw, va = v + rdv.y * 3.0f / rw, ub = u + ru * 6.0f / rw, vb = v + rdv.y * 6.0f / rw;
            v3 col = mul(warm, 0.24f + 0.16f * sstep(9.0f, 0.5f, vb));
            {   float x = ub - 4.0f * roundf(ub / 4.0f), d2 = x * x + (vb - 6.6f) * (vb - 6.6f); col = add(col, mul(V(1.0f, 0.80f, 0.50f), 2.0f * expf(-d2 / 0.05f) + 0.22f * expf(-d2 / 1.5f))); }
            if (va < 3.1f) { float i = roundf(ua / 5.0f), x = ua - 5.0f * i, hh = hash2(i, lg ? 1.7f : 3.9f);
                if (fabsf(x) < 2.0f) { if (va > 2.3f) col = mul(fract(x * 1.6f) < 0.5f ? (hh < 0.5f ? V(0.60f, 0.08f, 0.10f) : V(0.08f, 0.35f, 0.20f)) : V(0.80f, 0.75f, 0.62f), 0.50f);
                    else if (va < 1.25f) col = mul(V(0.30f, 0.16f, 0.07f), 0.40f + 0.3f * (fract(x * 1.25f) > 0.08f));
                    else col = mul(col, 0.55f); } }
            M->emis = mul(col, 1 - 0.65f * fade * sstep(0.05f, 0.02f, g));
        }
        break; }
    case M_HSIGN: { v3 s = sub(p, V(43.0f, 0, 85.0f)); M->alb = V(0.03f, 0.04f, 0.04f); M->spec = 0.5f; snow = 1;
        if (n.x < -0.5f) { float sf = 22.0f / (TR[T_HALLES].w - 80.0f), d = text2d(T_HALLES, -s.z / sf + TR[T_HALLES].w * 0.5f, 134.0f - (p.y - 7.9f) / sf) * sf, e = fminf(12.0f - fabsf(s.z), fminf(p.y - 7.15f, 10.05f - p.y));
            M->emis = mul(V(1.0f, 0.62f, 0.10f), 3.4f * sstep(0.03f, -0.03f, d) + 0.6f * expf(-fmaxf(d, 0) * 5));
            if (fabsf(e - 0.15f) < 0.03f) M->emis = V(0.3f, 1.6f, 1.2f); }
        break; }
    case M_STACK: { float c[7], h; blockcell(p.x, p.z, c, &h); M->alb = mul(V(0.28f, 0.11f, 0.075f), fract(p.y / 3.0f) < 0.12f ? 0.6f : 1.0f); M->spec = 0.1f;      // une cheminée de fumoir, son feu rouge
        if (p.y > c[6] - 0.9f) { M->alb = V(0.04f, 0.035f, 0.035f); M->emis = mul(V(7.0f, 0.5f, 0.3f), 0.25f + 0.75f * sstep(-0.2f, 0.4f, sinf(T * 2.4f + h * 60.0f))); }
        break; }
    case M_NSIGN: { v3 q = sub(p, V(0, GY, GZ)); float uu = n.z < 0 ? q.x : -q.x, sf = 19.4f / (TR[T_PLUS].w - 80.0f), d = text2d(T_PLUS, uu / sf + TR[T_PLUS].w * 0.5f, 134.0f - (q.y + 1.05f) / sf) * sf;
        M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 0.4f;
        float chase = 0.45f + 0.55f * sstep(0.0f, 0.15f, sinf(uu * 0.6f - T * 3.0f)), bulbs = sstep(0.42f, 0.2f, len2(fract(uu * 4.2f) - 0.5f, fract(q.y * 4.2f) - 0.5f));
        if (fabsf(n.z) > 0.5f) M->emis = mul(V(1.0f, 0.42f, 0.06f), chase * (7.0f * sstep(0.04f, -0.04f, d) * bulbs + 1.0f * expf(-fmaxf(d, 0) * 3)));
        float fr = fminf(10.4f - fabsf(q.x), 2.3f - fabsf(q.y)); if (fr < 0.2f && fabsf(n.z) > 0.5f) M->emis = mul(V(1.0f, 0.10f, 0.25f), 3.0f * (0.5f + 0.5f * sinf(T * 6 + uu)));
        snow = 1; break; }
    case M_STEEL: M->alb = V(0.045f, 0.055f, 0.065f); M->spec = 0.9f; M->gloss = 50; snow = 0.8f; break;
    case M_HAM: { float a = p.z - 12.0f, net = sstep(0.06f, 0.025f, fminf(fabsf(fract((a + p.y) * 13.0f) - 0.5f), fabsf(fract((a - p.y) * 13.0f) - 0.5f)));      // la couenne dorée, dans son filet
        M->alb = mix3(V(0.32f, 0.11f, 0.04f), V(0.62f, 0.42f, 0.20f), 0.4f * net); M->spec = 0.9f; M->gloss = 35; M->sss = 0.5f; break; }
    case M_SAUC: M->alb = mix3(V(0.24f, 0.045f, 0.045f), V(0.62f, 0.56f, 0.50f), 0.45f * sstep(0.45f, 0.75f, fbm(p.z * 60, p.y * 30))); M->spec = 0.3f; break;
    case M_CHAP: M->alb = V(0.80f, 0.22f, 0.20f); M->spec = 1.2f; M->gloss = 60; M->sss = 0.6f; break;
    case M_CLOTH: { float a = p.z, b = n.y > 0.5f ? p.x : p.y; M->alb = mul(gingham(a, b, 7.0f, sstep(0.5f, 0.15f, t * 0.0034f * 7.0f)), 0.9f); break; }
    case M_PATE: M->alb = p.y < 0.745f ? V(0.82f, 0.80f, 0.74f) : mix3(V(0.72f, 0.42f, 0.12f), V(0.50f, 0.24f, 0.07f), sstep(0.4f, 0.7f, vnoise(p.z * 40, p.x * 40))); M->spec = 0.6f; M->gloss = 40; break;
    case M_JAR: M->alb = p.y > 1.062f ? V(0.40f, 0.05f, 0.09f) : (p.y > 0.985f && p.y < 1.035f) ? V(0.82f, 0.78f, 0.66f) : V(0.78f, 0.56f, 0.34f); M->spec = 1.2f; M->gloss = 90; break;
    case M_IVORY: M->alb = V(0.82f, 0.78f, 0.66f); M->spec = 0.3f; break;
    default: break;
    }
    if (snow > 0) { float s = snow * up; M->alb = mix3(M->alb, mul(SNOWC, 0.9f + 0.1f * vnoise(p.x * 30, p.z * 30)), s); M->env *= 1 - s; M->emis = mul(M->emis, 1 - s); M->spec *= 1 - 0.8f * s; }
}
S v3 cityfog(v3 rd) { return add(mix3(V(0.085f, 0.045f, 0.055f), V(0.016f, 0.02f, 0.045f), clampf(rd.y * 3 + 0.25f, 0, 1)), mul(V(0.5f, 0.2f, 0.1f), DAWN * 0.25f * fmaxf(-rd.z, 0))); }
S float fogod(v3 ro, v3 rd, float t) {       // la brume : dense au ras des rues, plus claire en hauteur (densité en exp(-y / 40))
    float a = rd.y * t / 40.0f, k = fabsf(a) < 1e-3f ? 1.0f : (1 - expf(-a)) / a;
    return 0.0062f * t * expf(-fmaxf(ro.y, 0) / 40.0f) * k;
}
static v3 citysky(v3 ro, v3 rd) {      // a sky heavy with snow, lit from below; a moon behind the clouds; the mountains all around
    if (rd.y < 0) return cityfog(rd);       // sous l'horizon, la ville continue : on n'en voit plus que la brume
    float y = rd.y; v3 col = mix3(V(0.105f, 0.055f, 0.062f), V(0.012f, 0.016f, 0.042f), 1 - powf(1 - y, 4));
    float cl = fbm(rd.x / (y + 0.15f) * 1.3f + T * 0.01f, rd.z / (y + 0.15f) * 1.3f);
    col = add(col, mul(V(0.10f, 0.055f, 0.06f), sstep(0.35f, 0.8f, cl) * powf(1 - y, 2)));
    float md = fmaxf(dot(rd, MOON), 0); col = add(col, mul(V(0.16f, 0.19f, 0.28f), powf(md, 14) * (0.4f + 0.6f * cl) + 2.0f * powf(md, 700)));
    col = add(col, mul(V(1.0f, 0.42f, 0.22f), DAWN * 0.55f * expf(-y * 9) * (0.4f + 0.6f * fmaxf(-rd.z, 0))));                      // the day, far behind the avenue
    float az = atan2f(rd.x, rd.z), pk = fbm(az * 9.0f, 9.1f), mh = 0.035f + 0.10f * fbm(az * 2.3f + 5, 1.7f) + 0.045f * fbm(az * 6.0f, 4.2f) + 0.16f * pk * pk * pk + 0.012f * fbm(az * 40.0f, 2.2f);
    float al = sstep(mh + 0.0016f, mh - 0.0016f, rd.y);
    if (al > 0) { float rg = fbm(az * 34.0f + rd.y * 60.0f, rd.y * 90.0f), sn = sstep(0.070f, 0.115f, rd.y + 0.035f * (rg - 0.5f)) * sstep(0.25f, 0.55f, rg + 0.6f * sstep(0.10f, 0.16f, rd.y));
        col = mix3(col, mix3(mul(V(0.028f, 0.030f, 0.052f), 0.7f + 0.6f * rg), V(0.115f, 0.135f, 0.20f), sn), al * 0.95f); }
    {   float fh = 0.010f + 0.040f * fbm(az * 4.1f + 11, 3.3f) + 0.010f * fbm(az * 17.0f, 6.1f);                                    // devant, des collines dans la brume
        col = mix3(col, add(V(0.030f, 0.026f, 0.040f), mul(cityfog(rd), 0.35f)), 0.85f * sstep(fh + 0.0016f, fh - 0.0016f, rd.y)); }
    return mix3(col, cityfog(rd), 1 - expf(-0.09f * expf(-fmaxf(ro.y, 0) / 40.0f) / fmaxf(rd.y, 0.002f)));
}
static v3 clight(v3 p, v3 n) {
    v3 L = add(mul(V(0.022f, 0.030f, 0.060f), 0.5f + 0.5f * n.y), mul(V(0.11f, 0.055f, 0.035f), (0.5f - 0.5f * n.y) * expf(-p.y / 12.0f)));
    L = add(L, mul(V(0.07f, 0.09f, 0.14f), fmaxf(dot(n, MOON), 0)));
    L = add(L, mul(V(0.35f, 0.16f, 0.10f), DAWN * (0.3f + 0.7f * fmaxf(-n.z, 0))));
    float ax = fabsf(p.x);
    if (p.z < SQ0 + 6 && ax < 24 && p.y < 26) {     // the lamps of the avenue, the shop windows
        float iz0 = floorf(p.z / CL - 0.5f);
        for (int k = 0; k <= 1; k++) { float zl = (iz0 + k + 0.5f) * CL; if (zl > SQ0 || zl < -236.0f) continue;
            for (int s = -1; s <= 1; s += 2) { v3 lv = sub(V(5.9f * s, 5.35f, zl), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
                L = add(L, mul(V(1.0f, 0.60f, 0.28f), 16.0f / (1 + 0.8f * d2) * fmaxf(dot(n, lv) * il, 0))); } }
        if (ax < FX + 0.2f && p.y < 5.0f) { float sg = p.x > 0 ? 1.0f : -1.0f, iz = roundf(p.z / CL), lz = p.z - iz * CL;
            if (iz * CL < 49) L = add(L, mul(mix3(V(1.0f, 0.72f, 0.38f), shopcol(bh(iz, sg)), 0.45f), (shoplit(bh(iz, sg), shoptext(bh(iz, sg), iz, sg)) ? 1.7f : 0.35f) * expf(-(FX - ax) * 0.28f) * sstep(5.9f, 4.4f, fabsf(lz)) * sstep(5.0f, 2.0f, p.y) * fmaxf(0.25f + 0.75f * (sg * n.x * 0.8f + n.y * 0.6f), 0))); }
    }
    if (p.z > SQ0 - 40) {       // the square: its candelabra, the neon of the palaces, the great sign on the roofs
        for (int i = 0; i < 4; i++) { v3 lv = sub(V((i & 1) ? 20.0f : -20.0f, 7.0f, (i & 2) ? 100.0f : 70.0f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(V(1.0f, 0.66f, 0.34f), 30.0f / (1 + 0.25f * d2) * fmaxf(dot(n, lv) * il, 0))); }
        static const v3 SP[6] = {{0, 13.5f, 109.0f}, {-33.2f, 10.1f, 85.0f}, {33.4f, 8.6f, 85.0f}, {0, GY, GZ - 1.2f}, {33.2f, 4.2f, 85.0f}, {-33.4f, 2.6f, 85.0f}};
        static const v3 SC[6] = {{0.45f, 0.85f, 1.0f}, {1.0f, 0.12f, 0.45f}, {1.0f, 0.62f, 0.10f}, {1.0f, 0.42f, 0.06f}, {1.0f, 0.60f, 0.26f}, {1.0f, 0.52f, 0.16f}};
        static const float SI[6] = {26, 30, 20, 55, 60, 30}, SK[6] = {0.03f, 0.04f, 0.04f, 0.02f, 0.02f, 0.035f};
        for (int i = 0; i < 6; i++) { v3 lv = sub(SP[i], p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(SC[i], SI[i] / (1 + SK[i] * d2) * fmaxf(dot(n, lv) * il, 0) * 0.12f)); }
        if (p.z > 105) { v3 lv = sub(V(clampf(p.x, -20, 20), 1.3f, 110.0f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);       // the columns, lit from their feet
            L = add(L, mul(V(1.0f, 0.75f, 0.45f), 3.2f / (1 + 0.12f * d2) * fmaxf(dot(n, lv) * il, 0))); }
    }
    if (ax > 26.0f || p.z > 137.0f || p.z < -238.0f) {      // dans les rues de la ville : un réverbère à chaque carrefour, ou presque
        float cx = (roundf(p.x / BS - 0.5f) + 0.5f) * BS, cz = (roundf(p.z / BS - 0.5f) + 0.5f) * BS;
        if (hash2(cx * 0.13f + 1.7f, cz * 0.17f + 4.1f) < 0.8f) { v3 lv = sub(V(cx, 7.5f, cz), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(V(1.0f, 0.58f, 0.26f), 8.0f / (1 + 0.12f * d2) * fmaxf(dot(n, lv) * il, 0))); } }
    { v3 lv = sub(V(0, 2.3f, TRZ), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2); if (d2 < 900) L = add(L, mul(V(1.0f, 0.75f, 0.45f), 9.0f / (1 + 0.25f * d2) * fmaxf(dot(n, lv) * il, 0))); }
    if (CPX < 1e8f && p.x > 9.3f && p.x < 11.5f && p.y < 3.1f && fabsf(p.z - 11.2f) < 4.7f) {      // dans la charcuterie : trois globes au plafond
        L = add(L, V(0.09f, 0.06f, 0.035f));
        for (int j = 0; j < 3; j++) { v3 lv = sub(V(10.65f, 2.62f, 7.2f + 3.0f * j), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2); L = add(L, mul(V(1.0f, 0.74f, 0.42f), 1.9f / (1 + 1.1f * d2) * (0.25f + 0.75f * fmaxf(dot(n, lv) * il, 0)))); } }
    if (CPX < 1e8f) { v3 lv = sub(V(CPX + 0.6f, 1.9f, CPZ - 0.4f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2); L = add(L, mul(V(1.0f, 0.72f, 0.40f), 3.0f / (1 + 1.5f * d2) * fmaxf(dot(n, lv) * il, 0))); }
    return L;
}
S float cglow(v3 p) {       // how much lamp there is around a point of the air: the halos in the snow
    float g = 0;
    if (p.z < -238.0f) { }
    else if (p.z < SQ0 + 4) { float zl = p.z - (roundf(p.z / CL - 0.5f) + 0.5f) * CL, dx = fabsf(p.x) - 5.9f, dy = p.y - 5.35f; g += 1.0f / (1 + 14.0f * (dx * dx + dy * dy + zl * zl)); }
    else { float dx = fabsf(p.x) - 20.0f, dz = fabsf(p.z - 85.0f) - 15.0f, dy = p.y - 7.0f; g += 1.6f / (1 + 7.0f * (dx * dx + dy * dy + dz * dz)); }
    { float dx = fabsf(p.x) - 0.62f, dy = p.y - 1.08f, dz = p.z - TRZ - TDIR * 6.9f; g += 0.9f / (1 + 9.0f * (dx * dx + dy * dy + dz * dz)); }
    if (SPARK > 0) { float dy = p.y - 5.72f, dz = p.z - TRZ - 1.0f; g += SPARK * 3.0f / (1 + 20.0f * (p.x * p.x + dy * dy + dz * dz)); }
    return g;
}
static float ctrace(v3 ro, v3 rd, int *m, float tmax, int steps, float *glow) {
    float t = 0.05f, g = 0;
    for (int i = 0; i < steps && t < tmax; i++) { v3 p = add(ro, mul(rd, t)); float d = cmap(p, m);
        if (d < 0.001f * (1 + t * 0.5f)) { *glow = g; return t; }
        g += cglow(p) * fminf(d, 0.9f); t += d; }
    *glow = g; return -1;
}
static float flakes(v3 ro, v3 rd, float tmax, float c, float far, float fall, float size) {       // the snow that falls: one flake per cell of air
    float acc = 0, t = 0.2f; v3 o = add(ro, V(T * 0.45f * fall, T * fall, T * 0.2f * fall)); tmax = fminf(tmax, far);
    for (int i = 0; i < 26 && t < tmax; i++) {
        v3 q = add(o, mul(rd, t)); float ix = floorf(q.x / c), iy = floorf(q.y / c), iz = floorf(q.z / c), h = hash2(ix + iy * 57.3f, iz + iy * 13.7f);
        v3 oc = sub(V((ix + 0.2f + 0.6f * fract(h * 17.3f)) * c, (iy + 0.2f + 0.6f * fract(h * 39.1f)) * c, (iz + 0.2f + 0.6f * fract(h * 71.7f)) * c), o);
        float tc = dot(oc, rd);
        if (tc > 0.15f && tc < tmax) { float d2 = dot(oc, oc) - tc * tc, r = size * (0.7f + 0.6f * h), R = r + tc * 0.0020f; acc += sstep(R * R, R * R * 0.15f, d2) * fminf(r / R * 1.5f, 1.0f) * (1 - tc / far); }
        float tx = rd.x > 1e-5f ? ((ix + 1) * c - q.x) / rd.x : rd.x < -1e-5f ? (ix * c - q.x) / rd.x : 1e9f;
        float ty = rd.y > 1e-5f ? ((iy + 1) * c - q.y) / rd.y : rd.y < -1e-5f ? (iy * c - q.y) / rd.y : 1e9f;
        float tz = rd.z > 1e-5f ? ((iz + 1) * c - q.z) / rd.z : rd.z < -1e-5f ? (iz * c - q.z) / rd.z : 1e9f;
        t += fminf(tx, fminf(ty, tz)) + 0.01f;
    }
    return fminf(acc, 1);
}
static v3 cshade(v3 ro, v3 rd, int depth, float *zout) {
    int hi = ro.y > 30.0f; float glow; int m; float t = ctrace(ro, rd, &m, depth ? 160.0f : hi ? 1250.0f : 650.0f, depth ? 80 : hi ? 300 : 220, &glow); v3 col;
    if (t < 0) { col = citysky(ro, rd); *zout = 1e4f; }
    else {
        v3 p = add(ro, mul(rd, t)), n = normal(p, t); mat_t M; cmaterial(p, n, m, t, &M);
        float ao = depth ? 0.85f : ambocc(p, n), f5 = powf(1 - fmaxf(-dot(rd, n), 0), 5);
        v3 L = clight(p, n);
        if (M.sss > 0) L = add(L, mul(mulv(clight(p, mul(n, -1.0f)), V(1.0f, 0.45f, 0.35f)), 0.30f * M.sss));
        col = add(mulv(M.alb, mul(L, 0.35f + 0.65f * ao)), M.emis);
        if (M.env > 0.9f) {     // chrome, gold: nothing but what they reflect
            v3 rdir = refl(rd, n), rc; float z2;
            if (depth == 0) rc = cshade(add(p, mul(n, 0.03f)), rdir, 1, &z2); else rc = add(cityfog(rdir), mul(V(0.5f, 0.3f, 0.25f), 0.25f));
            if (m == M_TITLE) { v3 nb = fabsf(n.z) > 0.5f ? nrm(V(n.x, n.y + 1.5f * ((p.y - TYB) / 10.0f - 0.5f), n.z)) : n; rdir = refl(rd, nb); float y = rdir.y;
                v3 st = y > 0 ? mix3(V(1.9f, 1.7f, 1.3f), V(0.10f, 0.20f, 0.55f), sstep(0.0f, 0.55f, y)) : mix3(V(1.1f, 0.55f, 0.18f), V(0.12f, 0.05f, 0.02f), sstep(0.0f, -0.45f, y));
                rc = add(mul(rc, 0.35f), st); }
            float hz = expf(-fabsf(rdir.y) * 7.0f);       // the old trick: a bright band where the horizon would be
            rc = add(rc, add(mul(V(0.9f, 0.55f, 0.35f), 0.30f * hz), mul(V(0.10f, 0.14f, 0.30f), 0.5f * fmaxf(rdir.y, 0))));
            col = add(add(mulv(M.alb, rc), M.emis), mul(mulv(M.alb, L), 0.10f));
            col = add(col, mul(mulv(M.alb, V(1.0f, 0.75f, 0.5f)), 0.5f * powf(fmaxf(dot(rdir, nrm(V(0.3f, 0.5f, -0.8f))), 0), M.gloss)));
        } else if (M.env > 0.02f && depth == 0) {      // wet stone, glass: a mirror, more of one as the look grazes
            v3 nn = n;
            if (m == M_GROUND) { float w1 = vnoise(p.x * 2.6f, p.z * 2.6f) - 0.5f, w2 = vnoise(p.x * 2.6f + 31, p.z * 2.6f + 17) - 0.5f; nn = nrm(V(w1 * 0.018f, 1, w2 * 0.018f)); }
            float z2, F = M.env + (1 - M.env) * f5 * (m == M_GROUND ? 0.45f + 1.2f * M.env : 1.0f); if (F > 1) F = 1; v3 rc = cshade(add(p, mul(n, 0.03f)), refl(rd, nn), 1, &z2);
            col = add(mul(col, 1 - F * 0.85f), mul(rc, F));
        }
        col = add(col, mul(mulv(M.alb, clight(p, refl(rd, n))), M.spec * 0.10f * (0.3f + 0.7f * f5)));
        col = mix3(col, cityfog(rd), 1 - expf(-fogod(ro, rd, t)));
        *zout = t;
    }
    if (depth == 0 && CPX < 1e8f && rd.x > 0.02f && ro.x < FX) {       // la vitre de la charcuterie : on y voit la rue, et ceux qui regardent
        float tg = (FX + 0.02f - ro.x) / rd.x; v3 pg = add(ro, mul(rd, tg));
        if ((t < 0 || tg < t) && pg.y > 0.5f && pg.y < 2.95f && pg.z > 7.1f && pg.z < 15.3f) { float z2, F = 0.08f + 0.6f * powf(1 - rd.x, 5);
            NOCZ = 1; v3 rc = cshade(V(FX - 0.02f, pg.y, pg.z), V(-rd.x, rd.y, rd.z), 1, &z2); NOCZ = 0;
            rc = V(fmaxf(rc.x - 0.16f, 0), fmaxf(rc.y - 0.16f, 0), fmaxf(rc.z - 0.16f, 0));       // seuls les néons, les lampes et les visages éclairés s'y reflètent
            col = add(mul(col, 1 - 0.3f * F), mul(rc, 0.9f * F)); } }
    if (depth == 0) for (int i = 0; i < NPL; i++) { float od = cloud(ro, rd, t > 0 ? t : 1e9f, V(PLM[i][0], PLM[i][1], PLM[i][2]), V(PLM[i][3], PLM[i][4], PLM[i][3]), 2.2f, hash2(rd.x * 811.3f + T, rd.y * 433.7f)) * PLM[i][5];
        if (od > 0) col = mix3(col, V(PLM[i][6], PLM[i][7], PLM[i][8]), 1 - expf(-od)); }
    col = add(col, mul(V(1.0f, 0.62f, 0.30f), glow * 0.55f));
    if (depth == 0) {
        float tm = t > 0 ? t : 1e9f, br = 0.30f + 1.6f * fminf(glow, 1.0f) + 0.35f * expf(-fmaxf(ro.y, 0) / 14.0f);
        float f = flakes(ro, rd, tm, 1.3f, 16.0f, 1.5f, 0.022f) + 0.6f * flakes(ro, rd, tm, 3.7f, 46.0f, 1.3f, 0.03f);
        col = mix3(col, mul(V(0.78f, 0.80f, 0.90f), br), fminf(f, 1) * 0.85f);
    }
    return col;
}
static v3 shade_city(v3 ro, v3 rd, float *zout) { return cshade(ro, rd, 0, zout); }
static void crk(const float *K, int nk, float t, v3 *pos, v3 *tar) {       // camera keys (t, pos, target), Catmull-Rom
    int i = 0; while (i < nk - 2 && t > K[(i + 1) * 7]) i++;
    float u = clampf((t - K[i * 7]) / (K[(i + 1) * 7] - K[i * 7]), 0, 1), u2 = u * u, u3 = u2 * u; float out[6];
    int i0 = i > 0 ? i - 1 : 0, i3 = i + 2 < nk ? i + 2 : nk - 1;
    for (int c = 0; c < 6; c++) { float p0 = K[i0 * 7 + 1 + c], p1 = K[i * 7 + 1 + c], p2 = K[(i + 1) * 7 + 1 + c], p3 = K[i3 * 7 + 1 + c];
        out[c] = 0.5f * (2 * p1 + (p2 - p0) * u + (2 * p0 - 5 * p1 + 4 * p2 - p3) * u2 + (3 * p1 - p0 - 3 * p2 + p3) * u3); }
    *pos = V(out[0], out[1], out[2]); *tar = V(out[3], out[4], out[5]);
}
static const float KC1[] = {        // from the sky, through the O of the title, down to the rails behind the tram, up to the square
     0.0f,  40.0f, 78.0f, -215.0f,   24.0f, 38.0f, -70.0f,
     3.2f,  22.0f, 58.0f, -150.0f,   12.0f, 38.0f, -70.0f,
     5.6f,   3.0f, 42.5f, -104.0f,    0.0f, 38.2f, -70.0f,
     7.0f,   0.0f, 38.2f,  -71.0f,    0.0f, 20.0f, -20.0f,
     8.2f,  -0.8f, 20.0f,  -50.0f,    0.0f,  6.0f,   0.0f,
     9.6f,  -2.2f,  6.5f,  -30.0f,    0.0f,  3.0f,  14.0f,
    11.5f,  -2.6f,  2.7f,  -12.0f,    0.5f,  2.5f,  22.0f,
    15.0f,   3.4f,  2.3f,   10.0f,    7.5f,  2.2f,  34.0f,
    19.0f,   2.2f,  2.7f,   37.0f,    0.0f,  5.0f,  85.0f };
static void setup3(float s) {       // Douzi City (time 0 = its first frame)
    SHOT = 20; PARTY = 0; NOPIGS = 1; STAR = 0.7f; MOON = nrm(V(0.35f, 0.5f, 0.79f)); EXPO = 1.25f;
    TDIR = 1; TRZ = fminf(-5.0f + 5.4f * (s - 9.0f), 44.0f); SPARK = (TRZ < 43.9f && hash2(floorf(T * 14), 3.3f) > 0.78f) ? 1.0f : 0.0f;
    v3 pos, tar; NPL = 0; people_pose(s);
    if (s < 19) { crk(KC1, 9, s, &pos, &tar); cam(pos, tar, 1.45f); TITLEON = s < 9 ? 1.0f : 0.0f; if (s > 10) { FOCUS = 14; APERT = 0.010f; }
        NPL = 2; { static const float P0[2][9] = {{4.6f, 1.6f, 31.0f, 1.3f, 2.4f, 0.55f, 0.42f, 0.30f, 0.26f}, {-4.8f, 1.5f, 3.0f, 1.2f, 2.2f, 0.5f, 0.40f, 0.26f, 0.24f}}; memcpy(PLM, P0, sizeof(P0)); } }
    else if (s < 29) { float u = (s - 19) / 10, a = -2.6f + 2.05f * u, r = 17.0f - 4.0f * u;       // round the chrome pig
        cam(V(r * cosf(a), 2.4f + 2.2f * u, 85.0f + r * sinf(a)), V(0, 5.2f, 85.0f), 1.35f); FOCUS = r - 1.5f; APERT = 0.012f; }
    else if (s < 32) { float u = (s - 29) / 3; CPX = 8.9f; CPZ = 12.9f;                              // la charcuterie : une dame et un petit devant la vitre
        cam(V(7.70f + 0.15f * u, 1.30f, 9.35f + 0.50f * u), V(9.15f, 1.26f, 13.4f), 2.25f); FOCUS = 3.6f; APERT = 0.010f; }
    else if (s < 35.5f) { float u = (s - 32) / 3.5f;                                                // TOUJOURS PLUS, sur son portique
        cam(V(-7.4f - 1.0f * u, 14.5f + 3.0f * u, 25.0f + 7.0f * u), V(0.8f, 22.4f, 55.4f), 1.55f);
        NPL = 3; { static const float P1[3][9] = {{13.4f, 23.5f, 41.0f, 2.2f, 5.0f, 0.34f, 0.40f, 0.30f, 0.30f}, {-13.4f, 22.5f, 51.5f, 2.0f, 4.5f, 0.30f, 0.36f, 0.28f, 0.30f}, {18.6f, 22.0f, 51.5f, 1.8f, 4.0f, 0.28f, 0.44f, 0.32f, 0.30f}}; memcpy(PLM, P1, sizeof(P1)); } }
    else { float u = ease((s - 35.5f) / 6.0f); DAWN = u; TDIR = -1; TRZ = 44.0f - 4.5f * fmaxf(s - 36.0f, 0) * fmaxf(s - 36.0f, 0) * 0.5f; SPARK = hash2(floorf(T * 14), 3.3f) > 0.78f ? 1.0f : 0.0f;
        cam(mix3(V(3.0f, 5.0f, 102.0f), V(8.0f, 68.0f, 96.0f), u), mix3(V(0, 6.0f, 60.0f), V(0, 18.0f, -70.0f), u), 1.4f); }
}

