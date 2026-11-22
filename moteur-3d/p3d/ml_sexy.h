// ================================================================ MARTIN PLAISIR : la danse devant l'anneau (22 nov. 2026, « une danse archi sexy »)
// Même méthode que ml_dance : rien de tenu, rien de sauté, tout en courbes continues ; les hanches mènent, le buste suit avec un retard,
// la tête avec un retard de plus. Suave, jamais vulgaire : tout passe par les hanches, les mains qui glissent, le regard par-dessus l'épaule.
// b : temps de musique depuis le début de la danse (26 temps). Côté 0 = +x. lookside : +1 ou -1, l'épaule par-dessus laquelle il regarde.
//   0-8   cercles de hanches lents ; la main 0 remonte le long du corps jusque derrière la tête, l'autre sur la hanche
//   8-12  ondulations (la vague part du bassin, passe au buste, finit à la tête), puis regard par-dessus l'épaule et clin d'œil
//   12-20 la main 0 se tend vers le ciel (vers l'anneau qui lévite), cercles de hanches ; descente lente, mains sur les cuisses, et remontée
//   20-26 les deux bras montent en serpent, dernière ondulation, dernier regard par-dessus l'épaule, clin d'œil
static void ml_sexy(float b, float lookside, dn_pose_t *P) {
    dn_rest(P);
    float w = PI * b;                                                    // un cercle de hanches tous les deux temps
    float roll = ml_win(b, 8.0f, 12.0f) + 0.8f * ml_win(b, 21.0f, 26.0f);                 // les ondulations
    float circ = 1.0f - 0.6f * roll;                                                      // les cercles de hanches
    float dsc = sinf(PI * clampf((b - 14.0f) / 5.0f, 0, 1)); dsc *= dsc;                  // la descente lente, puis la remontée
    float look = ml_win(b, 10.0f, 12.6f) + ml_win(b, 23.8f, 26.5f);                       // le regard par-dessus l'épaule
    // le bassin : un cercle (x, z), le déhanché suit, et la vague du corps
    float hx = 0.095f * circ * cosf(w), hz = 0.060f * circ * sinf(w);
    P->pel = V(hx, 1.0f - 0.03f - 0.02f * cosf(2 * w) - 0.19f * dsc, hz + 0.05f * roll * sinf(w));
    P->proll = -2.4f * hx; P->pyaw = 0.16f * sinf(w - 0.6f) * circ + 0.25f * look * lookside; P->ptilt = 1.6f * hz + 0.28f * roll * sinf(w) + 0.25f * dsc;
    float wc = w - 0.65f, wh = w - 1.3f;                                                  // le buste en retard, la tête encore plus
    P->chest = V(-0.07f * circ * cosf(wc), 0.36f, -0.04f * circ * sinf(wc) + 0.03f * roll * sinf(wc));
    P->croll = 0.17f * circ * cosf(wc); P->cyaw = -0.22f * circ * sinf(wc - 0.6f) + 0.45f * look * lookside; P->cpitch = -0.32f * roll * sinf(wc) + 0.18f * dsc;
    P->hroll = -0.14f * cosf(wh) * circ - 0.10f * look * lookside; P->hyaw = 0.95f * look * lookside; P->hpitch = 0.22f * roll * sinf(wh) - 0.06f - 0.12f * look;
    P->tail = b * 9.0f; P->smile = 0.7f + 0.3f * look;
    { float wk = sstep(0.18f, 0.0f, fabsf(b - 11.6f)) + sstep(0.18f, 0.0f, fabsf(b - 25.2f)); P->lid = 0.55f + 0.45f * wk; }      // paupières lourdes, et le clin d'œil
    // les pieds : le poids passe d'une jambe à l'autre avec la hanche, le talon libre se lève, les genoux rentrent ; un peu plus écartés pour descendre
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, fr = sstep(0.1f, 0.9f, -sg * cosf(w)) * circ * (1 - dsc);
        P->foot[s] = V(sg * (0.13f + 0.02f * dsc), 0, 0.05f * sg); P->heel[s] = 0.8f * fr; P->fyaw[s] = sg * (0.3f - 0.12f * fr + 0.15f * dsc);
        P->kpole[s] = V(sg * (0.3f - 0.4f * fr) - sg * 0.15f * dsc, 0, 1); }
    // les mains : des phrases fondues l'une dans l'autre
    float W5[5] = {ml_win(b, -2.0f, 8.0f), ml_win(b, 8.0f, 12.0f), ml_win(b, 12.0f, 14.0f), ml_win(b, 14.0f, 20.0f), ml_win(b, 20.0f, 30.0f)}, ws = 0;
    v3 Hd[5][2], Ep[5][2];
    v3 hip0 = V(P->pel.x + 0.225f, P->pel.y + 0.005f, P->pel.z + 0.065f), hip1 = V(P->pel.x - 0.225f, P->pel.y + 0.005f, P->pel.z + 0.065f);
    {   float u = ease(clampf(b / 5.0f, 0, 1)), v = 1 - u;                               // 1 : la main glisse de la hanche, le long du buste, derrière la tête
        v3 p0 = hip0, p1 = V(0.10f, 1.38f, 0.20f), p2 = V(0.09f, 1.80f, -0.07f);
        Hd[0][0] = add(add(mul(p0, v * v), mul(p1, 2 * u * v)), mul(p2, u * u)); Ep[0][0] = mix3(V(1, -0.3f, 0), V(1, 0.6f, -0.2f), u);
        Hd[0][1] = hip1; Ep[0][1] = V(-1, 0, -0.5f); }
    {   Hd[1][0] = V(0.08f, 1.80f + 0.03f * sinf(w), -0.06f); Ep[1][0] = V(1, 0.6f, -0.2f);    // 2 : la main reste derrière la tête, l'autre caresse la hanche
        Hd[1][1] = add(hip1, V(-0.02f, 0.06f * sinf(w), 0.03f * cosf(w))); Ep[1][1] = V(-1, 0, -0.5f); }
    {   Hd[2][0] = V(0.30f, 2.05f + 0.02f * sinf(w), 0.18f); Ep[2][0] = V(1, -0.2f, -0.3f);      // 3 : il tend la main vers le ciel, vers l'anneau
        Hd[2][1] = hip1; Ep[2][1] = V(-1, 0, -0.5f); }
    {   float k = dsc;                                                                    // 4 : la descente, les mains glissent sur les cuisses ; la main 0 garde le ciel au début
        float u0 = ease(clampf((b - 14.0f) / 1.5f, 0, 1));
        v3 th0 = V(P->pel.x + 0.20f, P->pel.y - 0.22f - 0.10f * k, P->pel.z + 0.12f), th1 = V(P->pel.x - 0.20f, P->pel.y - 0.22f - 0.10f * k, P->pel.z + 0.12f);
        Hd[3][0] = mix3(V(0.30f, 2.05f, 0.18f), th0, u0); Ep[3][0] = V(1, -0.3f, -0.6f);
        Hd[3][1] = mix3(hip1, th1, u0); Ep[3][1] = V(-1, -0.3f, -0.6f); }
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, ph = w + s * PI;           // 5 : les deux bras montent en serpent : l'épaule, puis le coude, puis la main
        float up = ease(clampf((b - 20.0f) / 2.0f, 0, 1));
        Hd[4][s] = mix3(V(sg * 0.28f, 1.10f, 0.15f), V(sg * (0.22f + 0.06f * cosf(ph)), 2.02f + 0.06f * sinf(ph), 0.02f + 0.05f * sinf(ph + 1.0f)), up);
        Ep[4][s] = V(sg * 0.4f, 0.8f * sinf(ph + 1.1f), -0.35f); }
    for (int k = 0; k < 5; k++) ws += W5[k];
    for (int s = 0; s < 2; s++) { v3 h = V(0, 0, 0), e = V(0, 0, 0);
        for (int k = 0; k < 5; k++) { h = add(h, mul(Hd[k][s], W5[k] / ws)); e = add(e, mul(Ep[k][s], W5[k] / ws)); }
        P->hand[s] = h; P->epole[s] = e; }
}
