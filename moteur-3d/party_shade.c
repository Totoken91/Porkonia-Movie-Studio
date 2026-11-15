// ---------------------------------------------------------------- the party: its lights, its materials
static const v3 PCOL[4] = {{1.0f, 0.08f, 0.45f}, {0.08f, 0.70f, 1.0f}, {1.0f, 0.55f, 0.05f}, {0.55f, 0.15f, 1.0f}};
static v3 tilecol(float x, float z) {       // the lit dance floor: every tile takes a new colour on every beat
    float ix = roundf(x / CS), iz = roundf(z / CS - 0.5f), bt = floorf(T / BEAT), h = hash2(ix * 3.1f + bt * 1.3f, iz * 5.7f - bt * 0.7f);
    return mul(PCOL[(int)(h * 3.999f)], (fract(h * 9.7f) > 0.3f ? 1.0f : 0.12f) * (0.55f + 0.9f * expf(-fract(T / BEAT) * 3.5f)));
}
static void spot(int k, v3 *pos, v3 *dir) {
    *pos = V(-11 + 4.2f * k, 3.3f, (k & 1) ? 0.9f : -0.9f);
    *dir = nrm(sub(V(pos->x + 3 * sinf(T * 0.7f + k * 1.9f), FLY, 2.6f * sinf(T * 0.9f + k * 2.4f)), *pos));
}
static v3 plight(v3 p, v3 n) {
    float pulse = expf(-fract(T / BEAT) * 4.0f);
    if (PMODE == 1 || PMODE == 2) {
        v3 L = mul(V(1.0f, 0.74f, 0.45f), 2.4f * fmaxf(dot(n, nrm(V(-0.5f, 0.8f, -0.45f))), 0));
        L = add(L, mul(PCOL[(int)(floorf(T / BEAT)) & 3], (0.5f + 0.9f * pulse) * fmaxf(dot(n, nrm(V(0.6f, 0.3f, 0.75f))), 0)));
        return add(L, mul(V(0.10f, 0.06f, 0.16f), 0.6f + 0.4f * n.y));
    }
    if (PMODE == 3) {
        v3 lv = sub(V(0.2f, 2.95f, -1.9f), p), bd = nrm(V(cosf(T * 5), -0.3f, sinf(T * 5))); float d2 = dot(lv, lv), il = 1 / sqrtf(d2), c = -dot(lv, bd) * il;
        v3 L = mul(V(1.0f, 0.07f, 0.04f), (11.0f * sstep(0.62f, 0.96f, c) + 0.22f) / (1 + 0.25f * d2) * fmaxf(dot(n, lv) * il, 0));
        v3 l2 = sub(V(5.2f, 3.0f, 2.0f), p); float e2 = dot(l2, l2), i2 = 1 / sqrtf(e2);
        L = add(L, mul(V(0.30f, 0.50f, 0.55f), 5.5f / (1 + 0.2f * e2) * fmaxf(dot(n, l2) * i2, 0)));
        return add(L, V(0.02f, 0.025f, 0.03f));
    }
    v3 L = V(0.035f, 0.02f, 0.06f);
    for (int k = 0; k < 6; k++) { v3 lp, ld; spot(k, &lp, &ld); v3 lv = sub(lp, p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2), c = -dot(lv, ld) * il;
        if (c > 0.93f) L = add(L, mul(PCOL[k & 3], 9.0f * sstep(0.93f, 0.975f, c) * fmaxf(dot(n, lv) * il, 0) / (1 + 0.05f * d2))); }
    { float h = p.y - FLY; if (h < 4.0f) L = add(L, mul(tilecol(p.x, p.z), (0.55f - 0.45f * n.y) * expf(-h * 0.8f) * 0.9f)); }         // the floor lights them from below
    { v3 v = sub(p, V(2.0f, 2.7f, 0)); float dl = len(v), th = atan2f(v.z, v.x) + T * 0.5f, ph = asinf(clampf(v.y / dl, -1, 1));       // the dots of the mirror ball
      float fx = fract(th / 0.21f) - 0.5f, fy = fract(ph / 0.21f) - 0.5f, hh = hash2(floorf(th / 0.21f), floorf(ph / 0.21f));
      if (hh > 0.35f) L = add(L, mul(V(1, 1, 1), 1.7f * sstep(0.17f, 0.09f, len2(fx, fy)) * fmaxf(-dot(n, v) / dl, 0))); }
    if (HOLEX < 1e8f) { v3 lp = V(HOLEX - 1.2f, 3.5f, HOLEZ - 0.4f), lv = sub(lp, p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2), c = -dot(lv, nrm(V(1.2f, -3.9f, 0.4f))) * il;      // somebody put a follow spot on him
        L = add(L, mul(V(1.0f, 0.95f, 0.85f), 7.0f * sstep(0.955f, 0.985f, c) * fmaxf(dot(n, lv) * il, 0) / (1 + 0.05f * d2))); }
    if (p.x > 8.0f) {       // the stage: a warm key on Fatbass, the pink of his neon
        v3 lv = sub(V(9.3f, 3.0f, -0.6f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
        L = add(L, mul(V(1.0f, 0.80f, 0.55f), 9.0f / (1 + 0.5f * d2) * fmaxf(dot(n, lv) * il, 0)));
        lv = sub(V(14.5f, 1.6f, 0), p); d2 = dot(lv, lv); il = 1 / sqrtf(d2);
        L = add(L, mul(V(1.0f, 0.15f, 0.5f), (4.0f + 3.0f * pulse) / (1 + 0.4f * d2) * fmaxf(dot(n, lv) * il, 0)));
    }
    return add(L, mul(V(1, 1, 1), 0.12f * expf(-fract(T / (2 * BEAT)) * 25.0f) * KNOB));       // the strobe, once it is turned up
}
static void pmaterial(v3 p, v3 n, int m, float t, mat_t *M) {
    material(p, n, m, t, M);
    float pulse = expf(-fract(T / BEAT) * 4.0f);
    switch (m) {
    case M_FLOOR:
        if (PMODE == 3) { M->alb = mul(V(0.10f, 0.11f, 0.12f), 0.7f + 0.3f * sstep(0.1f, 0.2f, fabsf(fract(p.x * 4 + p.z * 4) - 0.5f))); M->spec = 0.6f; break; }
        { float e = fmaxf(fabsf(fract(p.x / CS + 0.5f) - 0.5f), fabsf(fract(p.z / CS) - 0.5f));
          M->alb = V(0.03f, 0.03f, 0.035f); M->emis = mul(tilecol(p.x, p.z), sstep(0.5f, 0.455f, e) * (0.7f + 0.3f * sstep(0.5f, 0.1f, e))); M->spec = 0.8f; M->gloss = 60; }
        break;
    case M_WALL: {
        M->alb = mul(V(0.40f, 0.38f, 0.34f), 0.8f + 0.2f * sstep(0.02f, 0.05f, fabsf(fract(p.x / 1.5f) - 0.5f))); M->spec = 0.3f;
        if (p.x > 14.9f) {      // behind the stage: his name in neon, and the number of the track
            float u = -p.z, sf = 4.4f / (TR[T_FAT].w - 80.0f), d = text2d(T_FAT, u / sf + TR[T_FAT].w * 0.5f, 134.0f - (p.y - 1.25f) / sf) * sf;
            M->emis = mul(V(1.0f, 0.12f, 0.50f), (0.75f + 0.25f * pulse) * (7.0f * sstep(0.035f, 0.0f, fabsf(d + 0.03f)) + 1.3f * expf(-fmaxf(d, 0) * 9)));
            int tx = TRACK == 48 ? T_P48 : T_P47;
            sf = 1.7f / (TR[tx].w - 80.0f); d = text2d(tx, u / sf + TR[tx].w * 0.5f, 134.0f - (p.y - 0.45f) / sf) * sf;
            M->emis = add(M->emis, mul(V(1.0f, 0.55f, 0.05f), 5.0f * sstep(0.01f, -0.01f, d) * sstep(0.42f, 0.25f, len2(fract(u * 22) - 0.5f, fract(p.y * 22) - 0.5f))));
            M->alb = V(0.05f, 0.04f, 0.05f);
        } else if (len2(p.y, p.z) > 3.6f && fabsf(p.y - 0.6f) < 1.0f) {      // portholes: black water, a gold ring
            float xi = clampf(roundf(p.x / 4), -3, 3) * 4, rr = len2(p.x - xi, p.y - 0.6f);
            if (rr < 0.76f) { M->alb = V(0.005f, 0.02f, 0.03f); M->spec = 1.0f; M->gloss = 80; }
            else if (rr < 0.92f) { M->alb = V(1.0f, 0.72f, 0.28f); M->spec = 1.0f; }
        }
        break; }
    case M_STAGE: M->alb = V(0.035f, 0.03f, 0.035f); M->spec = 0.6f; M->gloss = 60;
        if (p.x < 10.62f && p.y > -1.12f) { M->alb = V(1.0f, 0.72f, 0.28f); M->spec = 1; }
        else if (p.x < 10.62f) { float bx = fract(p.z * 2.5f); M->emis = mul(PCOL[((int)floorf(p.z * 2.5f + 40)) & 3], 1.5f * pulse * sstep(0.1f, 0.2f, bx) * sstep(0.9f, 0.8f, bx)); }
        break;
    case M_DESK: M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 0.7f; M->gloss = 70;
        if (p.x < 11.19f && p.y < -0.15f) {     // the front: bars of light that follow the bass
            float cz = floorf(p.z * 6), fz = fract(p.z * 6), lvl = 0.25f + 0.75f * pulse * (0.4f + 0.6f * hash2(cz, floorf(T / BEAT * 2))), hy = (p.y + 0.95f) / 0.75f;
            if (fz > 0.15f && fz < 0.85f && hy > 0 && hy < lvl && fract(hy * 9) > 0.25f) M->emis = hy > 0.75f ? V(4.0f, 0.3f, 0.2f) : hy > 0.5f ? V(4.0f, 2.2f, 0.2f) : V(0.4f, 3.5f, 0.6f);
        }
        break;
    case M_DISC: { float dz = fabsf(p.z) - 0.72f, dx = p.x - 11.5f, rr = len2(dx, dz), a = atan2f(dz, dx) + T * 3.5f * (p.z > 0 ? 1 : -1);
        M->alb = rr < 0.11f ? V(0.45f, 0.04f, 0.09f) : mul(V(0.02f, 0.02f, 0.02f), 1.0f + 1.5f * powf(fabsf(sinf(a)), 30) + 0.6f * sstep(0.3f, 0.7f, fract(rr * 40)));
        M->spec = 1.0f; M->gloss = 40; break; }
    case M_SPK: M->alb = V(0.025f, 0.025f, 0.03f); M->spec = 0.4f;
        if (p.x < 12.03f) { float cy = p.y > 0.1f ? 0.65f : -0.45f, rr = len2(p.y - cy, fabsf(p.z) - 2.75f);
            if (rr < 0.40f) { M->alb = rr < 0.12f ? V(0.25f, 0.25f, 0.27f) : V(0.05f, 0.05f, 0.055f);
                M->emis = mul(V(1.0f, 0.55f, 0.1f), (0.6f + 3.0f * pulse * (1 + KNOB)) * sstep(0.05f, 0.0f, fabsf(rr - 0.33f - 0.03f * pulse))); } }
        break;
    case M_SONAR: M->alb = V(0.20f, 0.25f, 0.22f); M->spec = 0.4f;
        if (p.x < 11.11f) { float rr = len2(p.y + 0.15f, p.z - 1.85f);
            if (rr < 0.25f) { M->alb = V(0.01f, 0.03f, 0.02f); M->spec = 1;
                float sw = expf(-fract(atan2f(p.y + 0.15f, p.z - 1.85f) / (2 * PI) + T * 0.8f) * 4.5f), rings = sstep(0.012f, 0.0f, fabsf(fract(rr * 12.5f) - 0.5f) / 12.5f);
                M->emis = mul(V(0.15f, 1.0f, 0.35f), SONARON * (0.25f + 3.5f * sw + 1.2f * rings + 5.0f * pulse * sstep(0.03f, 0.0f, fabsf(rr - 0.25f * fract(T / BEAT))))); }
            else if (rr < 0.29f) M->alb = V(0.55f, 0.5f, 0.4f); }
        break;
    case M_CABLE: M->alb = V(0.5f, 0.03f, 0.05f); M->spec = 0.6f; break;
    case M_BALL: { v3 v = sub(p, V(2.0f, 2.7f, 0)); float th = atan2f(v.z, v.x) + T * 0.5f, ph = asinf(clampf(v.y / 0.42f, -1, 1)), hh = hash2(floorf(th / 0.16f), floorf(ph / 0.16f));
        float ed = fmaxf(fabsf(fract(th / 0.16f) - 0.5f), fabsf(fract(ph / 0.16f) - 0.5f));
        M->alb = mul(V(0.5f, 0.5f, 0.55f), sstep(0.5f, 0.42f, ed)); M->spec = 1;
        M->emis = add(mul(PCOL[(int)(hh * 3.999f)], 0.5f * hh), mul(V(1, 1, 1), 9.0f * powf(fract(hh * 7.7f + T * 0.3f), 24))); break; }
    case M_PIPE: M->alb = V(0.50f, 0.27f, 0.13f); M->spec = 1.0f; M->gloss = 50; break;
    case M_ROBE: M->alb = fabsf(p.y - FLY - 1.03f) < 0.035f ? V(0.70f, 0.68f, 0.66f) : mul(V(0.92f, 0.90f, 0.86f), 0.9f + 0.1f * vnoise(p.x * 60, p.y * 60 + p.z * 60)); M->spec = 0.05f; break;
    case M_HSKIN: M->alb = V(0.80f, 0.60f, 0.50f); M->spec = 0.2f; break;
    case M_CHOP: M->alb = V(0.85f, 0.36f, 0.34f); M->spec = 0.5f; M->gloss = 40; break;
    case M_BONE: M->alb = V(0.92f, 0.88f, 0.78f); break;
    case M_HPHONE: M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 0.9f; M->gloss = 80; break;
    case M_GLASS: { float bub = sstep(0.93f, 1.0f, hash2(floorf(atan2f(p.z, p.x) * 14), floorf((p.y - T * 0.35f) * 22)));
        M->alb = mix3(V(0.80f, 0.36f, 0.03f), V(1.0f, 0.70f, 0.12f), clampf(p.y / 1.3f, 0, 1)); M->emis = mul(M->alb, 0.35f + 0.9f * bub); M->spec = 1.4f; M->gloss = 120; M->env = 0.25f; break; }
    case M_FOAMY: M->alb = mul(V(0.96f, 0.94f, 0.88f), 0.86f + 0.14f * vnoise(p.x * 26, p.z * 26 + p.y * 26)); M->spec = 0.25f; M->gloss = 20; M->sss = 3;
        if (PMODE == 0) M->emis = mul(tilecol(p.x, p.z), 0.09f);
        break;
    case M_BOTTLE: { float k = clampf(roundf(p.x / 0.62f), -2, 2), lvl = 1.42f * sstep(k * 0.45f + 0.9f, k * 0.45f + 2.6f, FOAMT);
        M->alb = V(0.16f, 0.07f, 0.02f); M->spec = 1.5f; M->gloss = 120; M->env = 0.3f; M->emis = V(0.05f, 0.02f, 0.005f);
        if (p.y < lvl) { float e = sstep(0.0f, 0.05f, lvl - p.y); M->alb = mix3(M->alb, V(0.75f, 0.62f, 0.42f), 0.75f * e); M->emis = mul(V(0.30f, 0.22f, 0.12f), e); }
        if (p.y > 0.36f && p.y < 0.68f) { M->alb = (p.y > 0.40f && p.y < 0.64f) ? V(0.86f, 0.82f, 0.72f) : V(0.40f, 0.04f, 0.09f); M->emis = V(0, 0, 0); M->spec = 0.2f; M->env = 0;
            if (fabsf(p.y - 0.52f) < 0.06f && fabsf(p.x - k * 0.62f) < 0.08f) M->alb = V(0.40f, 0.04f, 0.09f); }
        break; }
    case M_CAP: M->alb = V(1.0f, 0.72f, 0.28f); M->spec = 1.0f; M->gloss = 80; break;
    case M_COUNTER: M->alb = mul(mix3(V(0.16f, 0.07f, 0.035f), V(0.26f, 0.12f, 0.06f), vnoise((p.x * 0.9f + 0.4f * vnoise(p.x * 2, p.z * 9)) * 3, p.z * 1.3f)), 0.8f); M->spec = 1.0f; M->gloss = 90; M->env = 0.15f; break;
    case M_BOKEH: { float h = hash2(roundf(p.x / 2.4f) * 3.3f + 1, roundf(p.y / 2.0f) * 7.7f + 2);
        M->emis = mul(PCOL[(int)(fract(h * 3.7f + floorf(T / BEAT) * 0.37f) * 3.999f)], 5.0f * (0.5f + 0.5f * pulse)); M->alb = V(0, 0, 0); break; }
    case M_ENGINE: M->alb = mul(V(0.10f, 0.15f, 0.13f), 0.7f + 0.3f * vnoise(p.x * 3, p.y * 3 + p.z * 3)); M->spec = 0.5f; M->gloss = 30;
        if (fabsf(p.z + 0.9f) < 0.03f && p.y > 1.0f && p.y < 1.5f && len2(fract(p.x * 1.4f) - 0.5f, (p.y - 1.25f) * 1.4f) < 0.22f) { M->alb = V(0.8f, 0.78f, 0.7f); M->emis = V(0.25f, 0.24f, 0.18f); }
        break;
    case M_BEACON: M->emis = V(14.0f, 0.8f, 0.4f); break;
    default: break;
    }
}
static v3 shade_party(v3 ro, v3 rd, float *zout) {
    int m; float t = trace(ro, rd, &m, 60.0f, 170), tt = t > 0 ? t : 60.0f;
    v3 haze = PMODE == 0 ? V(0.035f, 0.015f, 0.05f) : PMODE == 3 ? V(0.02f, 0.004f, 0.004f) : V(0.012f, 0.008f, 0.02f), col = haze;
    if (t > 0) {
        v3 p = add(ro, mul(rd, t)), n = normal(p, t); mat_t M; pmaterial(p, n, m, t, &M);
        float ao = ambocc(p, n), f5 = powf(1 - fmaxf(-dot(rd, n), 0), 5);
        v3 L = plight(p, n), rdir = refl(rd, n);
        if (M.sss > 2.5f) L = add(mul(L, 0.75f), mul(plight(p, V(0, 1, 0)), 0.35f));       // foam: the light gets in from everywhere
        col = add(mulv(M.alb, mul(L, 0.3f + 0.7f * ao)), M.emis);
        col = add(col, mul(plight(p, rdir), M.spec * 0.12f * (0.3f + 0.7f * f5)));         // a cheap gloss: what the mirror direction receives
        if (M.env > 0) col = add(col, mul(PCOL[(int)floorf(T / BEAT) & 3], M.env * (0.15f + 0.85f * f5) * (0.4f + 0.6f * fmaxf(rdir.z, 0))));
        if (m >= M_SKIN && m <= M_BALLOON) col = add(col, mul(mulv(M.alb, V(0.6f, 0.4f, 0.7f)), 0.25f * f5));
        col = mix3(col, haze, 1 - expf(-t * (PMODE == 0 ? 0.022f : 0.012f)));
    }
    if (PMODE == 0) for (int k = 0; k < 6; k++) { v3 lp, ld; spot(k, &lp, &ld); col = add(col, mul(PCOL[k & 3], 0.20f * linebeam(ro, rd, tt, lp, ld, 0.08f, 0.085f, 0.06f, 7.5f))); }
    if (PMODE == 0 && HOLEX < 1e8f) col = add(col, mul(V(1.0f, 0.95f, 0.85f), 0.30f * linebeam(ro, rd, tt, V(HOLEX - 1.2f, 3.5f, HOLEZ - 0.4f), nrm(V(1.2f, -3.9f, 0.4f)), 0.10f, 0.11f, 0.02f, 5.5f)));
    if (PMODE == 3) col = add(col, mul(V(1.0f, 0.07f, 0.04f), 0.16f * linebeam(ro, rd, tt, V(0.2f, 2.95f, -1.9f), nrm(V(cosf(T * 5), -0.3f, sinf(T * 5))), 0.12f, 0.09f, 0.25f, 7.0f)));
    *zout = tt; return col;
}
static v3 sonar_rings(v3 ro, v3 rd, float tt) {     // every beat leaves the bow as a shell of sound
    v3 oc = sub(subw(V(19.5f, 0, 0)), ro); float b = dot(oc, rd), d2 = dot(oc, oc) - b * b, acc = 0;
    for (int k = 0; k < 7; k++) { float R = 13.0f * (fract(T / BEAT) + k) * BEAT, w = 0.35f + 0.035f * R; if (R * R <= d2) continue;
        float h = sqrtf(R * R - d2), I = expf(-R * 0.03f) * fminf(w * R / sqrtf(h * h + w * w) - w, 4 * w);
        for (int s = -1; s <= 1; s += 2) { float tc = b + s * h; if (tc > 0 && tc < tt) acc += I * expf(-tc * 0.03f); } }
    return mul(V(0.25f, 1.0f, 0.55f), acc * 0.055f * RINGS);
}
static void setup2(float s) {       // the shots of the second part (time 0 = its first frame)
    PARTY = 1; SHOT = 10; NOPIGS = 1; DOORON = 0;
    if (s < 8) { float u = ease(s / 8); cam(mix3(V(-13.2f, 2.5f, -0.7f), V(-3.5f, 1.0f, 0.5f), u), mix3(V(11.5f, 0.6f, 0), V(11.8f, 0.3f, 0), u), 1.15f); FOCUS = 12; APERT = 0.006f; }
    else if (s < 15) { float u = (s - 8) / 7; HOLEX = 3.3f; HOLEZ = 0.55f; cam(V(0.1f + 0.5f * u, 0.50f + 0.1f * u, -0.9f + 0.3f * u), V(3.3f, -0.25f, 0.55f), 1.75f); FOCUS = 3.4f; APERT = 0.03f; }
    else if (s < 18.8f) SHOT = 99;
    else if (s < 24.5f) { float u = (s - 18.8f) / 5.7f; PMODE = 1; FOAMG = sstep(0.05f, 0.95f, u); cam(V(0.9f - 0.5f * u, 1.75f - 0.2f * u, -3.4f + 0.5f * u), V(0.05f, 1.05f + 0.15f * u, 0), 2.0f); FOCUS = 3.4f - 0.4f * u; APERT = 0.035f; }
    else if (s < 28.5f) { float u = (s - 24.5f) / 4.0f; PMODE = 2; FOAMT = s - 24.5f; FOAMG = sstep(0.3f, 1.0f, u); cam(V(-1.6f + 1.0f * u, 1.25f, -3.0f), V(0, 0.8f, 0), 1.9f); FOCUS = 3.2f; APERT = 0.03f; }
    else if (s < 33) { float u = (s - 28.5f) / 4.5f; PMODE = 3; FOAMH = 0.15f + 0.85f * u; cam(V(-3.6f + 0.6f * u, 1.9f, 2.1f), V(2.0f, 1.1f, 0), 1.25f); }
    else if (s < 38.5f) { float u = (s - 33) / 5.5f; FOAMH = 0.45f + 0.2f * u; CLRX = -6.5f + 1.5f * u; CLRZ = 0.3f; CLRR = 1.75f; cam(V(CLRX, -0.35f, CLRZ), V(11, 0.0f, 0), 1.3f); FOCUS = 5; APERT = 0.012f; }
    else if (s < 45.5f) { float q = s - 38.5f, u = q / 7; FOAMH = 0.6f; PLUG = 1; SONARON = sstep(4.5f, 4.8f, q);
        cam(V(9.0f + 0.5f * u, 0.35f, -2.5f + 0.4f * u), V(11.9f, 0.05f, 0.5f), 1.55f); FOCUS = 3.6f; APERT = 0.02f; }
    else if (s < 51) { float q = s - 45.5f, u = q / 5.5f; SHOT = 5; PARTY = 0; SUBP = V(0, -46, 0); RINGS = 1; WHALE = 1; WHP = V(33 - 3.6f * q, -56.5f, -12); EXPO = 1.25f;
        cam(V(31 - 3 * u, -50.5f + u, -26), V(10, -45.5f, 6), 1.25f); }
    else if (s < 57.5f) { float q = s - 51, u = q / 6.5f; SHOT = 1; PARTY = 0; NIGHT = 1; NOPORT = 1; HOFF = 31; WLIT = 0.02f + 0.5f * sstep(0.5f, 5.5f, q); SUBP = V(0, -200, 0);
        MOON = nrm(V(0.15f, 0.24f, -0.95f)); SUN = MOON; SUNC = V(0.16f, 0.20f, 0.32f); EXPO = 1.5f;
        cam(V(30 - 5 * u, 7 - u, -28 - 6 * u), V(14, 15, -120), 1.7f); }
    else if (s < 66.5f) { float q = s - 57.5f, u = q / 9; FOAMH = 0.75f; PLUG = 1; SONARON = 1; MECH = 1; KNOB = sstep(3.6f, 4.3f, q); NOD = sstep(5.2f, 5.6f, q) * (1 - sstep(7.4f, 7.8f, q));
        cam(V(9.3f + 0.3f * u, 0.25f, 2.7f - 0.3f * u), V(11.6f, -0.05f, -0.2f), 1.5f); FOCUS = 3.4f; APERT = 0.02f; }
    else if (s < 72.5f) SHOT = 99;
    else { float u = (s - 72.5f) / 7; FOAMH = 0.95f; PLUG = 1; SONARON = 1; KNOB = 1; cam(V(13.6f, 1.2f + 1.3f * u, 1.4f - 0.6f * u), V(0, -0.9f, 0), 1.15f); FOCUS = 9; APERT = 0.008f; }
}

