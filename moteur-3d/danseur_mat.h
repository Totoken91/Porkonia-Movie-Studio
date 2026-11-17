// Le Danseur : ses matières (à inclure après la définition de mat_t)
static void dn_sequin(v3 p, v3 c, mat_t *M) {       // le satin pailleté : un reflet de soie, et des paillettes qui accrochent la lumière ici et là
    v3 q = mul(p, 85.0f); float hs = hash2(floorf(q.x) + 17.0f * floorf(q.z), floorf(q.y)), dd = len(V(fract(q.x) - 0.5f, fract(q.y) - 0.5f, fract(q.z) - 0.5f));
    M->alb = mul(c, 0.80f + 0.25f * hs); if (hs > 0.93f && dd < 0.40f) { float k = (hs - 0.93f) * 14.0f; M->alb = mix3(c, V(1, 1, 1), 0.6f); M->emis = mul(add(mul(c, 0.6f), V(0.5f, 0.5f, 0.5f)), 0.5f + 2.2f * k * k); M->spec = 2.5f; M->gloss = 200; }
}
static void dn_material(v3 p, int m, mat_t *M) { int mm, kk = dn_who(p); const dn_rig_t *RR = &DNRS[kk]; dn_sdf(RR, dn_local(p, kk), &mm); v3 lh = DN_H, lc = DN_C, lp = DN_P, SKC = RR->L.skin; float ax = fabsf(lh.x); int suit = RR->L.suit;
    M->spec = 0.2f; M->gloss = 20; M->sss = 0; M->emis = V(0, 0, 0);
    switch (m) {
    case D_HAIR: M->alb = mix3(RR->L.hair, RR->L.hair2, 0.8f * sstep(0.10f, 0.26f, lh.y)); M->spec = 0.5f; M->gloss = 36; break;
    case D_SKIN: { v3 c = SKC; M->sss = 1; M->spec = 0.30f; M->gloss = 26;
        c = mix3(c, V(0.98f, 0.20f, 0.26f), (0.15f + 0.85f * sstep(0.08f, 0.40f, (SKC.x + SKC.y + SKC.z) / 3)) * 0.5f * sstep(0.075f, 0.02f, len2(ax - 0.150f, lh.y + 0.025f)) * sstep(0.02f, 0.10f, lh.z));       // les pommettes
        if (lh.z > 0.10f && lh.y < -0.075f && lh.y > -0.17f) { float sm = RR->P.smile, yy = -0.118f - 0.030f * sm + 0.9f * sm * ax * ax * 6.0f; if (fabsf(lh.y - yy) < 0.0075f && ax < 0.085f + 0.03f * sm) c = V(0.25f, 0.05f, 0.07f); }       // la bouche : un sourire en coin
        if (lh.z > 0.12f && ax > 0.035f && ax < 0.150f && fabsf(lh.y - 0.135f - 0.10f * (ax - 0.09f) + 0.02f * RR->P.lid) < 0.008f) c = V(0.30f, 0.10f, 0.10f);                                  // les sourcils
        M->alb = c; break; }
    case D_SNOUT: M->alb = mulv(SKC, V(1.03f, 0.84f, 0.95f)); M->sss = 1; M->spec = 0.35f; M->gloss = 30; if (lh.z > 0.24f && len2((ax - 0.034f) / 0.016f, (lh.y + 0.035f) / 0.026f) < 1.0f) M->alb = V(0.30f, 0.06f, 0.10f); break;
    case D_EYE: { float lid = RR->P.lid, ey = lh.y - 0.060f; M->alb = V(0.95f, 0.94f, 0.90f); M->spec = 1.0f; M->gloss = 120;
        if (len2(ax - 0.092f, ey + 0.004f) < 0.026f) M->alb = V(0.03f, 0.02f, 0.02f);                                                               // la pupille
        if (len2(ax - 0.100f, ey - 0.008f) < 0.007f) M->alb = V(1, 1, 1);
        if (ey > 0.050f - 0.085f * lid) { M->alb = mulv(SKC, V(0.86f, 0.77f, 0.91f)); M->spec = 0.3f; M->gloss = 26; M->sss = 1; if (ey < 0.056f - 0.085f * lid) M->alb = V(0.20f, 0.05f, 0.08f); }       // la paupière lourde, ses cils
        break; }
    case D_SHIRT: { v3 c = V(0.27f, 0.012f, 0.045f); M->spec = 0.9f; M->gloss = 45;                                                                // du satin bordeaux, le col ouvert sur la poitrine
        float vv = lc.y - 0.165f, open = 0.012f + 0.52f * clampf(vv + 0.24f, 0, 0.24f);       // le col s'ouvre en V : large en haut, fermé au nombril
        if (suit) { c = V(0.15f, 0.155f, 0.175f); M->spec = 0.25f; M->gloss = 16; if (suit == 2) { c = RR->L.cloth; M->spec = 1.3f; M->gloss = 34; }       // le costard gris : la chemise blanche dans le V, la cravate noire, deux boutons dorés
            if (RR->L.idol && lc.z > 0.02f && vv < 0.035f && vv > -0.25f && fabsf(lc.x) >= open * 0.55f && fabsf(lc.x) < open * 0.55f + 0.040f) { c = V(0.025f, 0.025f, 0.03f); M->spec = 1.1f; M->gloss = 60; }       // les revers en satin noir
            else if (suit == 2 && lc.z > 0.02f && vv < 0.035f && vv > -0.25f && fabsf(lc.x) >= open * 0.55f && fabsf(lc.x) < open * 0.55f + 0.030f) c = mul(c, 0.55f);       // les revers, plus sombres
            else if (lc.z > 0.02f && vv < 0.0f && vv > -0.24f && fabsf(lc.x) < open * 0.55f) { c = fabsf(lc.x) < 0.016f + 0.010f * sstep(-0.02f, -0.20f, vv) ? V(0.03f, 0.03f, 0.035f) : V(0.90f, 0.89f, 0.86f); }
            else if (lc.z > 0.02f && vv < -0.27f && vv > -0.36f && len2(lc.x - 0.012f, (fract((vv + 0.27f) / 0.045f) - 0.5f) * 0.045f) < 0.009f) { c = V(1.0f, 0.70f, 0.24f); M->spec = 1.2f; M->gloss = 60; }
            if (suit == 2 && c.x == RR->L.cloth.x && c.y == RR->L.cloth.y) dn_sequin(p, c, M);
            M->alb = suit == 2 && c.x == RR->L.cloth.x && c.y == RR->L.cloth.y ? M->alb : c; break; }
        if (lc.z > 0.02f && vv < 0.0f && vv > -0.24f && fabsf(lc.x) < open * 0.55f) { c = SKC; M->sss = 1; M->spec = 0.3f; M->gloss = 26; }
        else if (lc.z > 0.02f && vv < 0.03f && vv > -0.25f && fabsf(lc.x) < open * 0.55f + 0.035f) c = V(0.40f, 0.03f, 0.07f);                      // le revers du col
        M->alb = c; break; }
    case D_PANTS: M->alb = suit ? V(0.15f, 0.155f, 0.175f) : V(0.86f, 0.83f, 0.76f); M->spec = 0.25f; M->gloss = 18; if (suit == 2) { M->spec = 1.3f; M->gloss = 34; dn_sequin(p, RR->L.cloth, M); } if (lp.y > 0.075f && lp.y < 0.105f) { M->alb = V(0.05f, 0.04f, 0.04f); if (lp.z > 0 && fabsf(lp.x) < 0.035f) { M->alb = V(1.0f, 0.70f, 0.24f); M->spec = 1.3f; M->gloss = 70; } } break;
    case D_WHITE: M->alb = V(0.90f, 0.89f, 0.86f); M->spec = 0.3f; M->gloss = 20; break;
    case D_BOOT: M->alb = V(0.05f, 0.04f, 0.045f); M->spec = 1.0f; M->gloss = 80; break;
    default: M->alb = V(1.0f, 0.70f, 0.24f); M->spec = 1.4f; M->gloss = 80; M->env = 0.6f; break; }
}
