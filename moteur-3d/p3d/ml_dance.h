// ml_dance : la danse de Martin Plaisir, COPIE de clip_shade.c (garder les deux pareilles si on la retouche)
S float ml_win(float b, float a, float e) { return sstep(a - 0.7f, a + 0.7f, b) * sstep(e + 0.7f, e - 0.7f, b); }
static void ml_dance(float b, dn_pose_t *P, float *yaw) {
    dn_rest(P); float w = PI * b, s1 = sinf(w), s2 = sinf(2 * w), amp = 1.0f + 0.45f * sstep(15.3f, 16.7f, b), wk = clampf(b - 24.0f, 0, 4), za = 0.33f * (wk - 0.5f * (sinf(PI * clampf(wk, 0, 0.5f)) - sinf(PI * clampf(wk - 3.5f, 0, 0.5f))) * 0.0f);
    float we = sstep(23.6f, 24.6f, b), dr = sinf(PI * clampf((b - 28.0f) / 2.2f, 0, 1)); dr = 0.24f * dr * dr; float calm = 1.0f - 0.55f * sstep(29.5f, 31.0f, b);
    float hx = 0.085f * amp * s1 * calm * (1 - 0.5f * we);
    P->pel = V(hx, 1.0f - 0.030f - 0.028f * cosf(2 * w) * calm - dr, za + 0.030f * s2 * calm);
    P->proll = -2.3f * hx; P->pyaw = 0.20f * amp * sinf(w - 0.6f) * calm; P->ptilt = 0.09f * s2 * calm + 0.25f * dr;
    float wd = PI * (b - 0.20f); P->chest = V(-0.060f * amp * sinf(wd) * calm, 0.36f, 0.035f * sinf(2 * PI * (b - 0.25f)) * calm);
    P->croll = 0.16f * amp * sinf(wd) * calm; P->cyaw = -0.26f * amp * sinf(wd - 0.6f) * calm + 0.07f * sinf(4 * w) * ml_win(b, 16.5f, 19.5f); P->cpitch = 0.09f * sinf(2 * PI * (b - 0.12f)) * calm + 0.3f * dr;
    float wh = PI * (b - 0.42f); P->hroll = -0.13f * sinf(wh) * calm - 0.12f * sstep(29.5f, 31.0f, b); P->hyaw = 0.30f * sinf(PI * b / 4 + 0.5f) * (1 - sstep(23.0f, 25.0f, b)); P->hpitch = 0.07f * sinf(2 * PI * (b - 0.5f)) * calm - 0.05f - 0.5f * dr;
    P->tail = b * 9.0f; P->smile = 0.6f + 0.4f * sstep(28.0f, 30.0f, b); { float bl = fract(b / 5.3f + 0.2f) * 5.3f; P->lid = 0.42f + 0.58f * sstep(0.22f, 0.0f, fabsf(bl - 0.3f)) + 0.58f * sstep(0.22f, 0.0f, fabsf(b - 30.7f)); }
    // les pieds : celui qui ne porte pas se soulève sur la pointe, son genou rentre ; puis quatre pas vers la caméra, un pied devant l'autre
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, fr = sstep(0.15f, 0.95f, -sg * s1) * calm * (1 - we), z = 0, lift = 0;
        for (int i = 0; i < 4; i++) if ((i & 1) == s) { float tz = 0.33f * (i + 1) + 0.10f; if (wk >= i + 1) z = tz; else if (wk > i) { z = mixf(z, tz, ease(wk - i)); lift = 0.11f * sinf(PI * (wk - i)); } }
        P->foot[s] = V(sg * mixf(0.14f - 0.035f * fr, 0.045f, we), lift, z); P->heel[s] = 0.75f * fr + 4.0f * lift; P->fyaw[s] = sg * (0.28f - 0.15f * fr); P->kpole[s] = V(sg * (0.28f - 0.42f * fr), 0, 1); }
    // les bras : cinq phrases
    float W[5] = {ml_win(b, -2.0f, 8.0f), ml_win(b, 8.0f, 16.0f), ml_win(b, 16.0f, 24.0f), ml_win(b, 24.0f, 28.0f), ml_win(b, 28.0f, 40.0f)}, ws = W[0] + W[1] + W[2] + W[3] + W[4]; v3 H[5][2], E[5][2];
    v3 hipL = V(P->pel.x - 0.225f, P->pel.y + 0.005f, P->pel.z + 0.065f), hipR = V(P->pel.x + 0.225f, P->pel.y + 0.005f, P->pel.z + 0.065f);
    { float u = ease(clampf(b / 7.0f, 0, 1)), v = 1 - u; v3 p0 = V(0.31f, 0.96f, 0.10f), p1 = V(0.05f, 1.42f, 0.42f), p2 = V(0.27f, 2.02f, 0.03f);       // 1 : une main remonte le long du corps jusqu'au ciel, l'autre sur la hanche
        H[0][0] = add(add(mul(p0, v * v), mul(p1, 2 * u * v)), mul(p2, u * u)); H[0][0].x += 0.03f * sinf(w); H[0][0].z += 0.03f * cosf(w); E[0][0] = V(1, -0.3f, -0.2f); H[0][1] = hipL; E[0][1] = V(-1, 0, -0.5f); }
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, ph = w + s * PI;                                                              // 2 : les bras ouverts, en serpent : l'épaule, puis le coude, puis la main
        H[1][s] = V(sg * (0.60f + 0.05f * cosf(ph)), 1.40f + 0.24f * sinf(ph), za + 0.06f + 0.05f * sinf(ph + 1.0f)); E[1][s] = V(sg * 0.3f, 0.9f * sinf(ph + 1.1f), -0.35f);
        float r = 0.5f + 0.5f * sinf(w - 0.4f + s * PI);                                                                                     // 3 : une main monte quand l'autre descend, avec la hanche
        H[2][s] = V(sg * (0.30f + 0.22f * r), 1.05f + 0.90f * r, za + 0.16f - 0.10f * r); E[2][s] = V(sg, -0.2f, -0.4f);
        H[3][s] = V(sg * 0.30f, 0.93f + 0.03f * sinf(2 * w), za + 0.20f * sinf(w + s * PI)); E[3][s] = V(sg * 0.6f, -0.2f, -1); }           // 4 : la marche, les bras ballants, à contretemps des jambes
    { float u = ease(clampf((b - 28.6f) / 2.0f, 0, 1)); H[4][0] = mix3(V(0.12f, 1.36f, za + 0.20f), V(0.26f, 1.34f, za + 0.54f), u); E[4][0] = V(1, -0.6f, -0.2f); H[4][1] = hipL; E[4][1] = V(-1, 0, -0.5f); (void)hipR; }       // 5 : il vise la caméra, lentement
    for (int s = 0; s < 2; s++) { v3 h = V(0, 0, 0), e = V(0, 0, 0); for (int k = 0; k < 5; k++) { h = add(h, mul(H[k][s], W[k] / ws)); e = add(e, mul(E[k][s], W[k] / ws)); } P->hand[s] = h; P->epole[s] = e; }
    *yaw = 6.2831853f * ease(clampf((b - 20.0f) / 4.0f, 0, 1));       // un tour sur lui-même, sans se presser
}
