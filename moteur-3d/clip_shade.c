#include "danseur_mat.h"
// ================================================================ LE CLIP : matières, ciels, lumières, personnages, danses, plans
S float bal_dots(v3 p) {       // les éclats de la boule à facettes sur la salle
    v3 d = nrm(sub(p, CBALLC)); float u = (atan2f(d.z, d.x) + T * 0.30f) * 12.0f, v = asinf(clampf(d.y, -1, 1)) * 12.0f;
    return hash2(floorf(u), floorf(v)) < 0.42f ? 0.55f * sstep(0.22f, 0.08f, len2(fract(u) - 0.5f, fract(v) - 0.5f)) : 0;
}
S float cl_tile(float ix, float iz, v3 *c) {       // une dalle de la piste : sa couleur, son éclat. Cinq motifs : hasard, anneaux, diagonales, damier, spirale
    float bi = floorf(CB), r = len2(ix + 0.5f, iz + 0.5f), on = 1; int ci;
    switch (CPAT) {
    case 1: { float w = r * 0.22f - CB * 0.5f; ci = (int)floorf(w + 64.0f); on = 0.2f + 0.8f * fract(w); break; }
    case 2: { float w = (ix + iz) * 0.16f - CB * 0.5f; ci = (int)floorf(w + 64.0f); on = 0.25f + 0.75f * fract(w); break; }
    case 3: { int o = ((int)(ix + iz + bi) + 256) & 1; ci = o ? (int)bi : (int)bi + 2; on = o ? 1.0f : 0.22f; break; }
    case 4: { float w = (atan2f(iz + 0.5f, ix + 0.5f) / 6.2832f * 3.0f + r * 0.16f - CB * 0.25f) * 2.0f; ci = (int)floorf(w + 64.0f); on = 0.2f + 0.8f * fract(w); break; }
    default: { float h = hash2(ix * 3.1f + bi * 1.7f, iz * 5.3f + bi * 0.9f); ci = (int)(h * 3.999f); on = fract(h * 7.7f) < 0.25f + 0.55f * CE ? 1.0f : 0.08f; } }
    *c = CPAL[ci & 3]; return on * (0.40f + 1.4f * CBT) * (0.25f + 0.75f * CE);
}
static void cl_window(float z, float y, float zl, float wy, float arch, mat_t *M) {       // une fenêtre du Bal, vue de la salle : la nuit, la neige qui tombe, la buée
    float kz = roundf((z - 1.0f) / 4.0f), sn = 0;
    for (int i = 0; i < 2; i++) { float s = i ? 0.17f : 0.10f, u = zl / s + kz * 7.3f + 0.6f * sinf(y * 1.3f + T * 0.5f + i), v = (y + T * (i ? 0.55f : 0.9f)) / s, h = hash2(floorf(u) + 17.0f * i, floorf(v));
        if (h < 0.22f) sn += sstep(0.40f, 0.12f, len2(fract(u) - 0.3f - 0.4f * fract(h * 31.0f), fract(v) - 0.3f - 0.4f * fract(h * 57.0f))) * (i ? 0.6f : 1.0f); }
    v3 out = add(mul(V(0.045f, 0.085f, 0.22f), 1.0f + 0.12f * (wy + 2.0f)), mul(V(1.0f, 0.62f, 0.25f), 0.55f * expf(-2.5f * len2(zl - 0.45f, wy + 0.9f))));
    out = add(out, mul(V(0.75f, 0.85f, 1.0f), 0.8f * sn));
    float fog = clampf(0.30f + 0.5f * sstep(0.45f, 0.0f, -arch) + 0.45f * sstep(0.6f, -1.9f, wy) * (0.5f + 0.8f * vnoise(zl * 3.0f + kz * 5.0f, y * 3.0f)), 0, 0.93f);
    if (CHEART > 0 && kz == 0) {       // un cœur tracé du doigt dans la buée
        float u = zl / 0.40f, v = (wy - 0.15f) / 0.40f, q = u * u + v * v - 1, f = q * q * q - u * u * v * v * v, fx = 6 * u * q * q - 2 * u * v * v * v, fy = 6 * v * q * q - 3 * u * u * v * v,
              ds = fabsf(f) / sqrtf(fx * fx + fy * fy + 1e-3f) * 0.40f, a = fract(atan2f(u, -v + 0.2f) / 6.2832f + 0.5f);
        if (a < CHEART) fog *= 1 - 0.97f * sstep(0.075f, 0.040f, ds); }
    M->alb = V(0.02f, 0.025f, 0.04f); M->emis = mul(mix3(out, V(0.36f, 0.42f, 0.54f), fog), 1.0f); M->spec = 1.5f; M->gloss = 120;
    if (fabsf(zl) < 0.035f || fabsf(c_rep(wy + 0.35f, 1.1f)) < 0.03f) { M->emis = V(0, 0, 0); M->alb = V(0.78f, 0.72f, 0.62f); }
}
static void clip_material(v3 p, v3 n, int m, float t, mat_t *M, float *snow, float *up) {
    float ax = fabsf(p.x);
    switch (m) {
    // ---- le Bal
    case W_CFLOOR:
        if (ax < 4.2f && p.z > -5.8f && p.z < 5.4f) { float ix = floorf(p.x / 0.7f), iz = floorf((p.z + 0.2f) / 0.7f), ed = fminf(0.5f - fabsf(fract(p.x / 0.7f) - 0.5f), 0.5f - fabsf(fract((p.z + 0.2f) / 0.7f) - 0.5f)) * 0.7f; v3 c; float on = cl_tile(ix, iz, &c);
            M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 1.2f; M->gloss = 90; M->env = 0.22f; if (ed > 0.03f) M->emis = mul(c, on * CSPOTK * (0.75f + 0.25f * sstep(0.03f, 0.3f, ed))); }
        else { float ck = fmodf(floorf(p.x) + floorf(p.z) + 400.0f, 2.0f); M->alb = ck > 0.5f ? V(0.74f, 0.70f, 0.62f) : V(0.035f, 0.03f, 0.04f); M->spec = 1.0f; M->gloss = 100; M->env = 0.36f;
            M->emis = mul(V(1.0f, 0.95f, 0.85f), 0.22f * bal_dots(p) * CSPOTK); }
        break;
    case W_CWALL: { float dots = bal_dots(p) * CSPOTK;
        M->alb = mul(V(0.10f, 0.032f, 0.055f), 0.85f + 0.3f * vnoise(p.z * 1.3f + p.x, p.y * 1.3f)); M->spec = 0.3f;
        if (p.y > BAH - 0.05f) { M->alb = V(0.02f, 0.02f, 0.03f); dots *= 0.7f; }
        else if (p.z > BAZ1 - 0.05f) { float f = sinf(p.x * 7.0f + 0.6f * sinf(p.y * 0.8f)); M->alb = mul(V(0.30f, 0.02f, 0.06f), 0.35f + 0.65f * f * f); M->spec = 0.5f; M->gloss = 20; }       // le rideau de scène
        else if (p.y < 1.0f) M->alb = mul(V(0.07f, 0.04f, 0.03f), 0.8f + 0.4f * vnoise(p.x * 0.5f + p.z * 0.5f, p.y * 14.0f));
        else if (fabsf(p.y - 1.06f) < 0.05f) { M->alb = V(1.0f, 0.74f, 0.30f); M->spec = 1.4f; M->gloss = 80; }
        else if (fabsf(c_rep((ax > BAX - 0.05f ? p.z : p.x) - 1.0f, 2.0f)) < 0.03f) M->alb = mul(M->alb, 0.5f);
        if (p.x < -BAX + 0.05f && p.z < 7.2f && p.z > -9.2f) { float zl = c_rep(p.z - 1.0f, 4.0f), wy = p.y - 3.0f, arch = wy > 1.2f ? len2(zl, wy - 1.2f) - 1.05f : fmaxf(fabsf(zl) - 1.05f, -1.9f - wy);
            if (arch < 0) { cl_window(p.z, p.y, zl, wy, arch, M); dots = 0; } else if (arch < 0.10f) { M->alb = V(0.78f, 0.72f, 0.62f); M->spec = 0.6f; } }
        M->emis = add(M->emis, mul(V(1.0f, 0.95f, 0.85f), 0.5f * dots)); break; }
    case W_CSTAGE: M->alb = V(0.05f, 0.03f, 0.035f); M->spec = 0.8f; M->gloss = 60;
        if (n.y > 0.5f) { M->alb = V(0.03f, 0.03f, 0.035f); M->env = 0.2f; }
        else if (n.z < -0.5f) { if (fabsf(p.y - 0.42f) < 0.035f) { M->alb = V(1.0f, 0.74f, 0.30f); M->spec = 1.5f; }
            if (len2(fract(p.x / 0.4f) - 0.5f, (p.y - 0.2f) / 0.4f) < 0.16f) M->emis = mul(CPAL[((int)floorf(p.x / 0.4f + 64.0f) + (int)floorf(CB * 2.0f)) & 3], 3.0f * (0.4f + 0.6f * CE)); }      // la guirlande : un chenillard
        break;
    case W_CCONS: M->alb = V(0.022f, 0.022f, 0.026f); M->spec = 0.8f; M->gloss = 60;
        if (n.z < -0.5f && ax < 1.35f) { float cu = p.x / 0.085f, lv = 0.15f + 0.8f * hash2(floorf(cu) * 2.3f, floorf(CTS / 0.165f) * 1.1f) * (0.35f + 0.65f * CBT), v = (p.y - STY - 0.08f) / 0.44f;
            if (fabsf(fract(cu) - 0.5f) < 0.36f && v > 0 && v < lv && fract(v * 11.0f) > 0.25f) M->emis = mul(v > 0.75f ? V(1.0f, 0.1f, 0.05f) : v > 0.5f ? V(1.0f, 0.7f, 0.05f) : V(0.1f, 1.0f, 0.25f), 2.2f); }
        break;
    case W_CVINYL: { float sx = ax - 0.38f, sz = p.z - 9.0f, rr = len2(sx, sz), a = atan2f(sz, sx) + (p.x > 0 ? 1.0f : -1.0f) * T * 3.49f + (p.x > 0 ? CSCR : 0);
        M->alb = V(0.012f, 0.012f, 0.014f); M->spec = 0.6f + 2.5f * powf(fabsf(sinf(a)), 24.0f); M->gloss = 30; M->alb = mul(M->alb, 1.0f + 6.0f * powf(fabsf(sinf(atan2f(sz, sx) + 0.7f)), 18.0f) + 0.8f * sinf(rr * 420.0f));
        if (rr < 0.085f) { M->alb = fabsf(fract(a / 6.2832f) - 0.5f) < 0.07f ? V(0.9f, 0.7f, 0.25f) : V(0.42f, 0.04f, 0.09f); M->spec = 0.3f; } if (rr < 0.012f || rr > 0.255f) { M->alb = V(0.5f, 0.5f, 0.52f); M->spec = 1.5f; } break; }
    case W_CMIX: { float u = p.x / 0.045f, v = p.z / 0.045f; M->alb = V(0.05f, 0.05f, 0.055f); M->spec = 0.6f;
        if (n.y > 0.5f) { if (len2(fract(u) - 0.5f, fract(v) - 0.5f) < 0.22f) M->alb = V(0.4f, 0.4f, 0.42f); if (hash2(floorf(u), floorf(v) + floorf(CB * 2.0f)) < 0.2f && len2(fract(u) - 0.5f, fract(v) - 0.5f) < 0.12f) M->emis = mul(CPAL[(int)(hash2(floorf(u), 3.0f) * 3.999f)], 2.5f); } break; }
    case W_CSPK: { float yy = p.y - STY; M->alb = V(0.018f, 0.018f, 0.02f); M->spec = 0.3f;
        if (n.z < -0.5f) { float cy = yy < 1.0f ? yy - 0.52f : yy < 1.95f ? yy - 1.47f : yy - 2.25f, R = yy < 1.95f ? 0.42f : 0.22f, rr = len2(ax - 3.7f, cy);
            if (rr < R) { M->alb = mul(V(0.07f, 0.07f, 0.075f), 0.7f + 0.6f * rr / R); if (rr < R * (0.30f + 0.12f * CBT)) M->alb = V(0.16f, 0.16f, 0.17f); M->spec = 0.9f; }
            else if (rr < R + 0.035f) { M->alb = V(0.45f, 0.45f, 0.48f); M->spec = 1.5f; M->emis = mul(CPAL[yy < 1.0f ? 0 : 1], 1.2f * CBT * CE); } } break; }
    case W_CNEON: { float sf = 6.2f / (TR[T_VITEAU].w - 80.0f), dt = text2d(T_VITEAU, p.x / sf + TR[T_VITEAU].w * 0.5f, TR[T_VITEAU].base - (p.y - 4.35f + 0.40f) / sf) * sf, fl = (0.80f + 0.12f * sinf(T * 37.0f) * sinf(T * 5.3f) + 0.35f * CBT) * (0.35f + 0.65f * CSPOTK);
        M->alb = V(0.02f, 0.015f, 0.03f); if (n.z < -0.5f) { M->emis = add(mul(V(7.0f, 0.8f, 3.6f), sstep(0.02f, -0.02f, dt) * fl), mul(V(0.9f, 0.08f, 0.5f), expf(-fmaxf(dt, 0) * 9.0f) * fl * 0.9f));
            if (fabsf(fabsf(p.y - 4.35f) - 0.58f) < 0.02f || fabsf(ax - 3.38f) < 0.02f) M->emis = mul(V(0.2f, 2.6f, 3.6f), fl); } break; }
    case W_CBALL: { v3 d = nrm(sub(p, CBALLC)); float u = (atan2f(d.z, d.x) + T * 0.30f) * 7.0f, v = asinf(clampf(d.y, -1, 1)) * 7.0f, h = hash2(floorf(u), floorf(v) + floorf(CB * 4.0f));
        M->alb = mul(V(0.55f, 0.56f, 0.6f), 0.5f + 0.5f * fract(h * 3.3f)); M->spec = 2.0f; M->gloss = 80; M->env = 0.25f;
        if (fminf(0.5f - fabsf(fract(u) - 0.5f), 0.5f - fabsf(fract(v) - 0.5f)) < 0.06f) { M->alb = V(0.02f, 0.02f, 0.02f); M->env = 0; } else if (h < 0.14f) M->emis = mul(CPAL[(int)(fract(h * 31.0f) * 3.999f)], 4.0f * CE + 1.5f); break; }
    case W_CBAR: M->alb = mul(V(0.16f, 0.07f, 0.04f), 0.8f + 0.4f * vnoise(p.z * 0.7f, p.y * 9.0f)); M->spec = 0.6f; M->gloss = 50;
        if (fabsf(c_rep(p.z, 0.9f)) < 0.02f) M->alb = mul(M->alb, 0.4f);
        if (fabsf(p.y - 0.95f) < 0.03f) { M->alb = V(1.0f, 0.74f, 0.30f); M->spec = 1.5f; }
        if (p.y < 0.10f && n.x < -0.5f) M->emis = mul(CPAL[1], 2.5f * (0.5f + 0.5f * CBT)); break;
    case W_CBARTOP: M->alb = V(0.03f, 0.025f, 0.03f); M->spec = 1.5f; M->gloss = 150; M->env = 0.35f; break;
    case W_CSHELF: M->alb = V(0.05f, 0.03f, 0.03f);
        if (n.x < -0.5f) { float u = (p.z + 2.0f) / 0.17f, row = floorf((p.y - 1.4f) / 0.62f), v = fract((p.y - 1.4f) / 0.62f), h = hash2(floorf(u), row * 3.1f), fu = fabsf(fract(u) - 0.5f), hb = 0.45f + 0.30f * h;
            v3 back = mul(CPAL[((int)row + (int)floorf(CB * 0.5f) + 8) & 3], (1.2f + 1.8f * CBT * CE) * (0.22f + 0.78f * CSPOTK)); int bottle = v < hb ? (fu < 0.34f) : v < hb + 0.2f ? (fu < 0.12f) : 0;
            if (v > 0.93f || v < 0.06f) M->alb = V(0.10f, 0.06f, 0.04f);
            else if (bottle) { v3 bc = h < 0.33f ? V(0.02f, 0.12f, 0.05f) : h < 0.66f ? V(0.22f, 0.09f, 0.01f) : V(0.13f, 0.13f, 0.15f); M->alb = bc; M->emis = mul(mulv(back, bc), 1.3f); M->spec = 1.5f; M->gloss = 90; }
            else M->emis = back; }
        break;
    case W_CSTOOL: if (p.y > 0.68f) { M->alb = V(0.36f, 0.03f, 0.07f); M->spec = 0.9f; M->gloss = 50; } else { M->alb = V(0.93f, 0.95f, 1.0f); M->env = 1; M->spec = 1.6f; M->gloss = 200; } break;
    case W_CTABLE: if (p.y > 0.70f) { int ck = (fract(p.x * 5.0f) > 0.5f) ^ (fract(p.z * 5.0f) > 0.5f); M->alb = ck ? V(0.80f, 0.77f, 0.70f) : V(0.50f, 0.08f, 0.12f); M->sss = 0.2f; } else { M->alb = V(0.93f, 0.95f, 1.0f); M->env = 1; M->spec = 1.6f; M->gloss = 200; } break;
    case W_CBEER: { float yy = p.y - 0.762f - CGLY; M->alb = V(0.80f, 0.38f, 0.04f); M->emis = mul(V(0.9f, 0.40f, 0.03f), 0.30f + 0.30f * CBT); M->spec = 1.6f; M->gloss = 160; M->env = 0.12f;
        if (yy > 0.150f + 0.006f * sinf(atan2f(p.z + 3.74f, p.x + 6.16f) * 3.0f + CB * 6.2832f) * CBT) { M->alb = V(0.95f, 0.92f, 0.84f); M->emis = V(0.10f, 0.09f, 0.07f); M->env = 0; M->spec = 0.2f; M->sss = 0.4f; } break; }
    case W_CGRILL: { float qy = p.y, qz = p.z - 6.6f; M->alb = V(0.03f, 0.03f, 0.035f); M->spec = 0.9f; M->gloss = 50;
        if (n.y > 0.5f && qy > 0.7f) { float em = vnoise(p.x * 9.0f + T * 0.7f, qz * 9.0f - T * 0.5f); M->emis = mul(V(1.0f, 0.26f, 0.03f), (0.5f + 2.4f * em * em) * (0.7f + 0.9f * CBT)); if (fabsf(c_rep(qz, 0.07f)) < 0.012f) { M->emis = V(0, 0, 0); M->alb = V(0.02f, 0.02f, 0.02f); } } break; }
    case W_CSAUS: M->alb = mul(V(0.46f, 0.13f, 0.06f), 0.8f + 0.4f * vnoise(p.x * 30.0f, p.z * 12.0f)); if (fabsf(c_rep(p.z - 6.57f, 0.09f)) < 0.014f) M->alb = mul(M->alb, 0.3f);
        M->spec = 1.3f; M->gloss = 60; M->sss = 0.2f; M->emis = mul(V(0.30f, 0.06f, 0.0f), fmaxf(-n.y, 0) * (0.6f + 0.6f * CBT)); break;
    case W_CMARB: { int kb = 0; float db = 1e9f; for (int i = 0; i < 6; i++) { float e = len(sub(p, CMB[i])); if (e < db) { db = e; kb = i; } }
        M->alb = mul(CPALS[0][kb & 3], 0.8f); M->emis = mul(CPALS[0][kb & 3], 0.35f); M->spec = 2.0f; M->gloss = 200; M->env = 0.35f; break; }
    case W_CCHROME: M->alb = V(0.93f, 0.95f, 1.0f); M->env = 1; M->spec = 1.6f; M->gloss = 200; break;
    case W_CSPOT: M->alb = V(0.02f, 0.02f, 0.022f); M->spec = 0.8f; if (n.y < -0.5f) M->emis = mul(CPAL[((int)roundf(p.x / 2.4f) + 8 + (int)floorf(CB * 0.5f)) & 3], 6.0f * CBEAMK * (0.3f + 0.7f * CBT)); break;
    // ---- la ville
    case W_CSNOW: { float rut = sstep(0.30f, 0.10f, fabsf(ax - 1.15f)) * sstep(6.0f, 4.5f, ax);
        M->alb = mul(WSNW, (0.84f + 0.16f * fbm(p.x * 0.4f, p.z * 0.4f)) * (1 - 0.22f * rut) * (ax < 5.0f ? 0.93f : 1.0f)); M->spec = 0.25f; M->gloss = 20; M->sss = 0.3f;
        if (hash2(floorf(p.x * 36.0f), floorf(p.z * 36.0f)) > 0.994f) M->emis = mul(V(0.8f, 0.9f, 1.0f), 0.5f * sstep(60.0f, 10.0f, t)); break; }
    case W_CBLDG: case W_CROOF: { float kx = roundf(p.x / 12.0f), kz = roundf(p.z / 11.0f), h = vil_h(kx, kz), hh = hash2(kx * 1.7f, kz * 2.3f); int fx = fabsf(n.x) > 0.5f;       // les maisons de Douzi City : crépis de couleur, colombages, volets, vitrines
        static const v3 FC[5] = {{0.50f, 0.43f, 0.32f}, {0.47f, 0.29f, 0.27f}, {0.50f, 0.36f, 0.17f}, {0.26f, 0.32f, 0.40f}, {0.38f, 0.17f, 0.13f}};
        static const v3 SH[4] = {{0.30f, 0.03f, 0.06f}, {0.05f, 0.20f, 0.12f}, {0.06f, 0.10f, 0.26f}, {0.34f, 0.22f, 0.06f}};
        v3 fc = mul(FC[(int)(hh * 4.999f)], 0.62f); int timber = fract(hh * 13.0f) < 0.45f; *snow = 1;
        M->alb = mul(fc, 0.85f + 0.3f * vnoise(p.x * 0.8f + p.z * 0.8f, p.y * 0.8f)); M->spec = 0.15f;
        if (m == W_CROOF && n.y > 0.2f) { M->alb = mul(WSNW, 0.86f + 0.14f * fbm(p.x * 0.6f, p.z * 0.6f)); M->spec = 0.25f; M->gloss = 20; M->sss = 0.3f; if (hash2(floorf(p.x * 30.0f), floorf(p.z * 30.0f)) > 0.994f) M->emis = V(0.4f, 0.45f, 0.5f); break; }
        if (n.y > 0.5f) { M->alb = mul(WSNW, 0.9f); break; }
        if (p.y > h) { M->alb = mul(V(0.30f, 0.14f, 0.10f), 0.7f + 0.3f * (fract(p.y * 3.0f) > 0.12f)); break; }       // la cheminée, en brique
        if (n.y < 0.5f && m == W_CBLDG) { float uu = (fx ? p.z : p.x), u = uu / 1.7f, v = p.y / 1.6f, wh = hash2(floorf(u) + kx * 13.0f + (fx ? 0 : 57.0f), floorf(v) + kz * 7.0f), fu = fabsf(fract(u) - 0.5f), fv = fract(v) - 0.45f;
            if (p.y < 3.0f) {       // le rez-de-chaussée : une vitrine éclairée par boutique, un store rayé au-dessus, un soubassement de pierre
                float us = uu / 3.4f, fs = fabsf(fract(us) - 0.5f), sh = hash2(floorf(us) + kx * 5.0f, kz * 3.0f + (fx ? 0 : 9.0f));
                if (p.y < 0.45f) M->alb = V(0.16f, 0.15f, 0.15f);
                else if (p.y > 2.45f) { M->alb = fract(uu * 2.2f) < 0.5f ? SH[(int)(sh * 3.999f)] : V(0.62f, 0.58f, 0.50f); }
                else if (fs < 0.40f) { M->alb = V(0.03f, 0.03f, 0.04f); M->spec = 1.2f; M->gloss = 100;
                    if (sh < 0.8f) { v3 gl = sh < 0.3f ? V(1.0f, 0.70f, 0.32f) : sh < 0.55f ? V(1.0f, 0.50f, 0.22f) : V(0.55f, 0.85f, 1.0f);
                        M->emis = mul(gl, (1.7f + 0.9f * vnoise(uu * 2.0f, p.y * 2.0f)) * (fabsf(fs - 0.20f) < 0.012f || fabsf(p.y - 1.75f) < 0.02f ? 0.15f : 1.0f)); } }
                else if (fs < 0.44f) M->alb = V(0.07f, 0.045f, 0.03f);
            } else if (p.y < h - 0.5f) {
                if (fu < 0.27f && fabsf(fv) < 0.30f) { int lit = wh < CLITK * 2.3f;
                    if (CVU > 0) lit = v * 1.6f / h < (0.2f + 0.8f * hash2(floorf(u) + kx * 13.0f + (fx ? 0 : 57.0f), floorf(CTS / 0.165f))) * (0.35f + 0.65f * CBT);
                    M->alb = V(0.02f, 0.025f, 0.04f); M->spec = 1.2f; M->gloss = 100;
                    if (fu < 0.018f || fabsf(fv) < 0.015f || fu > 0.235f || fabsf(fv) > 0.262f) { M->alb = V(0.50f, 0.47f, 0.42f); M->spec = 0.2f; }       // le cadre et les croisillons
                    else if (lit) M->emis = mul(CVU > 0 ? CPAL[(int)(fract(wh * 7.0f) * 3.999f)] : (fract(wh * 31.0f) < 0.15f ? V(0.35f, 0.5f, 1.0f) : V(1.0f, 0.62f, 0.25f)), (1.5f + 0.5f * fract(wh * 17.0f)) * (fv < -0.05f - 0.2f * fract(wh * 5.0f) ? 0.45f : 1.0f)); }
                else if (fu < 0.40f && fabsf(fv) < 0.30f) M->alb = mul(SH[(int)(fract(hh * 7.0f) * 3.999f)], 0.75f + 0.25f * (fract(v * 9.0f) > 0.2f));       // les volets
                else if (fu < 0.31f && fv < -0.30f && fv > -0.37f) M->alb = mul(WSNW, 0.95f);       // la neige sur l'appui
                else if (timber && (fract(u) < 0.07f || fract(v) > 0.93f || (fabsf(fv) > 0.30f && fabsf(fabsf(fract(u) - 0.5f) * 1.2f - fabsf(fv + (fv > 0 ? -0.55f : 0.55f))) < 0.035f))) M->alb = V(0.085f, 0.05f, 0.035f);       // les colombages
            } }
        break; }
    case W_CHALL: *snow = 1; M->spec = 0.25f;       // LE BAL DES NEIGES (refonte du 16 nov. 2026) : une tour d'angle à dôme de cuivre pour l'entrée, une aile de brique sous un toit d'ardoise, des travées toutes différentes
        { float bu = (fabsf(n.x) > 0.5f ? p.z : p.x) / 0.50f, bv = p.y / 0.22f; bu += 0.5f * ((int)floorf(bv) & 1); int mort = fract(bu) < 0.06f || fract(bv) < 0.12f, tower = p.z < -7.95f;
            const v3 CREAM = V(0.74f, 0.68f, 0.58f); float cu = (fabsf(n.x) > 0.5f ? p.z : p.x) / 0.45f, yb = tower ? 11.58f : 6.58f;
            M->alb = mort ? V(0.30f, 0.26f, 0.22f) : mul(V(0.34f, 0.085f, 0.075f), 0.75f + 0.5f * hash2(floorf(bu), floorf(bv)));
            if (p.x > 6.45f) {
                if (tower && p.y > 12.17f) { float an = atan2f(p.z + 12.0f, p.x - 11.0f); M->alb = mul(V(0.10f, 0.33f, 0.27f), 0.8f + 0.3f * vnoise(an * 6.0f, p.y * 2.0f)); M->spec = 0.7f; M->gloss = 30;       // le dôme : du cuivre vert-de-gris, des nervures dorées
                    if (fabsf(fract(an / 6.2832f * 12.0f) - 0.5f) > 0.44f) { M->alb = V(0.85f, 0.60f, 0.18f); M->spec = 1.2f; M->gloss = 60; } }
                else if (!tower && p.y > 7.17f) { float su = (fabsf(n.x) > 0.5f ? p.z : p.x) / 0.36f, sv = p.y / 0.26f; su += 0.5f * ((int)floorf(sv) & 1);       // le toit : des ardoises en écailles, des œils-de-bœuf éclairés
                    M->alb = mul(V(0.085f, 0.095f, 0.13f), 0.7f + 0.5f * hash2(floorf(su), floorf(sv))) ; if (len2(fract(su) - 0.5f, fract(sv) - 0.2f) > 0.62f) M->alb = mul(M->alb, 0.5f); M->spec = 0.5f; M->gloss = 30;
                    if (fabsf(n.y) < 0.5f) { float zo = c_rep((fabsf(n.x) > 0.5f ? p.z : p.x) - 1.0f, 4.0f), ro = len2(zo, p.y - 7.95f); if (ro < 0.42f) { M->alb = V(0.02f, 0.02f, 0.03f); M->emis = mul(V(1.0f, 0.60f, 0.22f), fabsf(zo) < 0.03f || fabsf(p.y - 7.95f) < 0.03f ? 0.1f : 1.3f); } else if (ro < 0.52f) M->alb = CREAM; } }
                else if (fabsf(p.y - (tower ? 12.0f : 7.0f)) < 0.175f) M->alb = CREAM;       // les corniches
                else if (fabsf(n.y) < 0.5f && fabsf(p.y - yb) < 0.24f) { M->alb = CREAM;       // sous la corniche : une rampe d'ampoules qui court
                    if (len2(fract(cu) - 0.5f, (p.y - yb) / 0.45f) < 0.26f) { int on = (((int)floorf(cu) + (int)floorf(CB * 4.0f)) & 3) < 2; M->alb = V(0.1f, 0.08f, 0.05f); M->emis = mul(V(1.0f, 0.72f, 0.30f), on ? 5.5f : 0.5f); } } } }
        if (p.x < 6.42f && n.y < -0.5f) { if (len2(fract(p.x / 0.3f) - 0.5f, fract(p.z / 0.3f) - 0.5f) < 0.2f) M->emis = mul(V(1.0f, 0.75f, 0.35f), 2.5f + 2.0f * CBT); }      // sous la marquise : des ampoules
        else if (p.x < 6.42f) { M->alb = V(0.36f, 0.03f, 0.07f); if (fabsf(n.y) < 0.5f) M->emis = mul(V(1.0f, 0.75f, 0.35f), 0.8f * (fract(p.z / 0.3f + p.x / 0.3f + floorf(CB * 2.0f) * 0.25f) < 0.5f)); }
        else if (n.x < -0.5f && p.y < (p.z < -7.95f ? 11.3f : 6.3f)) {       // la façade sur la rue
            static const int BT[6] = {2, 1, 3, 2, 1, 3}; float bay = clampf(roundf((p.z - 1.0f) / 4.0f), -2, 3), zl = p.z - 1.0f - 4.0f * bay, wy = p.y - 2.6f, dz = p.z + 12.0f; int tower = p.z < -7.95f, ty = tower ? 0 : BT[(int)bay + 2];
            const v3 CREAM = V(0.78f, 0.72f, 0.62f), GOLD = V(0.85f, 0.60f, 0.18f);
            if (p.y < 0.9f) M->alb = V(0.13f, 0.12f, 0.12f);       // un soubassement de pierre
            if (!tower && fabsf(fabsf(zl) - 2.0f) < 0.20f) M->alb = mul(CREAM, 0.9f + 0.1f * (fract(p.y / 0.9f) > 0.06f));       // un pilastre entre deux travées
            if (ty == 1) { float arch = wy > 0.6f ? len2(zl, wy - 0.6f) - 0.80f : fmaxf(fabsf(zl) - 0.80f, -1.3f - wy);       // une fenêtre en plein cintre, à taille de cochon : on devine ceux qui dansent
                if (arch < 0) { int ci = ((int)bay + 8 + (int)floorf(CB)) & 3; v3 c = mul(CPAL[ci], 0.55f + 1.3f * CBT * CE); float zq = zl / 0.50f, wq = (wy + 1.3f) / 0.50f - 1.9f;
// une vraie profondeur derrière la vitre : le rayon de la caméra continue dans la salle. Au fond, le mur et ses lumières floues ; devant, trois rangs de danseurs
                    // à contre-jour, de plus en plus flous avec la distance. Quand la caméra bouge, les rangs glissent les uns devant les autres.
                    v3 rv = nrm(sub(p, RO)); if (rv.x < 0.08f) rv.x = 0.08f; const float YF = 1.50f;
                    v3 qb = add(p, mul(rv, 8.0f / rv.x)); float gy = qb.y, gz = qb.z; v3 bg;
                    if (gy < YF) { float tf = (YF - p.y) / fminf(rv.y, -1e-3f); v3 qf = add(p, mul(rv, tf)); bg = mul(V(0.55f, 0.20f, 0.10f), 0.35f + 0.25f * (((int)floorf(qf.x / 0.7f) + (int)floorf(qf.z / 0.7f)) & 1)); }       // le parquet
                    else { bg = mul(V(1.0f, 0.46f, 0.14f), 0.55f + 0.30f * sstep(6.0f, 1.0f, gy));                                                                  // le mur du fond, chaud
                        for (int a = 0; a < 2; a++) { float cs = 1.3f + 0.7f * a, cz2 = floorf(gz / cs), cy2 = floorf(gy / cs), hb = hash2(cz2 * 3.3f + a * 9.0f, cy2 * 5.1f + 1.0f);       // des lumières de fête, hors foyer : de gros disques doux
                            float rb = len2(gz - (cz2 + 0.3f + 0.4f * hb) * cs, gy - (cy2 + 0.3f + 0.4f * fract(hb * 7.0f)) * cs) / cs;
                            bg = add(bg, mul(CPAL[(int)(hb * 3.999f + floorf(CB * 0.5f)) & 3], (hb > 0.45f ? 0.80f : 0.0f) * sstep(0.30f, 0.16f, rb) * (0.35f + 0.65f * (fract(hb * 13.0f + CB * 0.25f) < 0.5f ? CBT : CBO)))); } }
                    float tr = 1.0f;
                    for (int kk = 2; kk >= 0; kk--) { float dp = 1.1f + 1.7f * kk; v3 q3 = add(p, mul(rv, dp / rv.x)); float off = 0.55f * kk + 0.3f, cz3 = roundf((q3.z - off) / 1.75f), hk = hash2(cz3 * 3.1f + kk * 7.7f, 2.3f + kk);
                        float ph = CB * PI + hk * 6.0f, bo = 0.07f * fabsf(sinf(ph)), fx2 = (q3.z - off - 1.75f * cz3 - 0.18f * sinf(ph * 0.5f)) / 0.95f, fy = (q3.y - YF - bo) / 0.95f, ax2 = fabsf(fx2);
                        float sd = (len2(fx2 / 0.46f, (fy - 1.02f) / 0.37f) - 1.0f) * 0.37f;                                                 // la tête
                        sd = fminf(sd, (len2((ax2 - 0.37f) / 0.15f, (fy - 1.30f - 0.25f * (ax2 - 0.37f)) / 0.105f) - 1.0f) * 0.105f);          // les oreilles
                        sd = fminf(sd, (len2(fx2 / 0.36f, (fy - 0.48f) / 0.40f) - 1.0f) * 0.36f);                                             // le corps
                        sd = fminf(sd, sdCap(V(ax2, fy, 0), V(0.16f, 0.0f, 0), V(0.16f, 0.30f, 0), 0.10f));                                   // les pattes
                        { float up = 0.5f + 0.5f * sinf(ph + (fx2 > 0 ? 0 : PI)) * (hk > 0.3f); sd = fminf(sd, sdCap(V(ax2, fy, 0), V(0.31f, 0.62f, 0), V(0.50f + 0.05f * up, 0.50f + 0.75f * up, 0), 0.085f)); }
                        sd *= 0.95f; float bl = 0.06f + 0.07f * kk; tr *= 1.0f - (0.96f - 0.10f * kk) * sstep(bl, -bl, sd);
                        if (q3.y < YF) tr *= 1.0f; }
                    float fog = 0.22f + 0.10f * vnoise(p.z * 3.0f, p.y * 3.0f) + 0.16f * sstep(0.0f, -1.3f, wy);       // la buée, plus épaisse en bas
                    M->alb = V(0.02f, 0.02f, 0.03f); M->emis = add(mul(bg, tr * (1.0f - fog) * (0.85f + 0.25f * CBT * CE)), mul(V(1.0f, 0.55f, 0.20f), 0.55f * fog)); M->spec = 1.4f; M->gloss = 120; *snow = 0; (void)c; (void)zq; (void)wq;
                    if (fabsf(zl) < 0.03f || fabsf(wy - 0.6f) < 0.03f || fabsf(wy + 0.35f) < 0.025f) { M->emis = V(0, 0, 0); M->alb = CREAM; } }
                else if (arch < 0.10f) M->alb = CREAM;
                else if (fabsf(zl) < 0.95f && wy < -1.42f && wy > -1.52f) M->alb = mul(WSNW, 0.95f); }
            if (ty == 2) { float fx3 = fabsf(zl) - 1.15f, fy3 = fabsf(wy - 0.15f) - 1.35f, fr = fmaxf(fx3, fy3);       // une affiche lumineuse dans son cadre doré : DJ VITEAU, SOUS LA NEIGE
                if (fr < 0) { int id = bay < 0 ? T_VITEAU : T_NEIGE; float sf = 1.95f / (TR[id].w - 80.0f), dt = text2d(id, (-zl) / sf + TR[id].w * 0.5f, TR[id].base - (wy - 0.55f) / sf) * sf, ra = atan2f(wy + 0.55f, zl), rr = len2(zl, wy + 0.55f);
                    M->alb = V(0.03f, 0.01f, 0.02f); M->spec = 0.9f; M->gloss = 80; *snow = 0;
                    M->emis = mul(bay < 0 ? V(0.55f, 0.04f, 0.16f) : V(0.05f, 0.14f, 0.50f), 0.55f + 0.45f * (fract(ra / 6.2832f * 16.0f + CB * 0.25f) < 0.5f));       // des rayons qui tournent
                    if (rr < 0.52f) M->emis = mul(V(1.0f, 0.72f, 0.25f), 1.6f + 0.8f * CBT);                                                                              // un disque d'or, comme un vinyle
                    if (rr < 0.52f && (rr < 0.07f || fabsf(fract(rr * 14.0f) - 0.5f) > 0.42f)) M->emis = mul(M->emis, 0.35f);
                    M->emis = add(M->emis, mul(V(1.0f, 0.86f, 0.55f), 3.2f * sstep(0.02f, -0.02f, dt)));
                    if (fr > -0.10f) { M->emis = V(0, 0, 0); M->alb = GOLD; M->spec = 1.2f; M->gloss = 60; if (len2(fract((fx3 > fy3 ? wy : zl) / 0.22f) - 0.5f, 0) < 0.22f) M->emis = mul(V(1.0f, 0.75f, 0.35f), (((int)floorf((fx3 > fy3 ? wy : zl) / 0.22f) + (int)floorf(CB * 2.0f)) & 1) ? 4.0f : 0.6f); } } }
            if (ty == 3) { float fx3 = fabsf(fabsf(zl) - 0.58f) - 0.34f, fy3 = fabsf(wy + 0.05f) - 1.05f, fr = fmaxf(fx3, fy3);       // deux fenêtres étroites à rideaux, leurs volets, la neige sur l'appui ; au-dessus, un vitrail rond
                if (fr < 0) { float fold = 0.75f + 0.25f * sinf((fabsf(zl) - 0.58f) * 60.0f); M->alb = V(0.02f, 0.02f, 0.03f); M->spec = 1.2f; M->gloss = 100; *snow = 0;
                    M->emis = mul(bay > 2 ? V(1.0f, 0.42f, 0.30f) : V(1.0f, 0.62f, 0.22f), (fabsf(fabsf(zl) - 0.58f) > 0.10f + 0.12f * sstep(1.0f, -1.0f, wy) ? 0.55f * fold : 1.25f)); if (fr > -0.035f || fabsf(wy + 0.05f) < 0.02f) { M->emis = V(0, 0, 0); M->alb = CREAM; } }
                else if (fabsf(zl) < 1.25f && fabsf(wy + 0.05f) < 1.05f && fabsf(zl) > 0.92f) M->alb = mul(V(0.05f, 0.20f, 0.12f), 0.75f + 0.25f * (fract(wy * 7.0f) > 0.2f));
                else if (fabsf(zl) < 1.3f && wy < -1.12f && wy > -1.22f) M->alb = mul(WSNW, 0.95f); }
            if (ty == 3 && bay > 2.5f) { float ro = len2(zl, p.y - 5.35f); if (ro < 0.62f) { float an = atan2f(p.y - 5.35f, zl); int w8 = (int)floorf((an / 6.2832f + 0.5f) * 8.0f); M->alb = V(0.02f, 0.02f, 0.03f); *snow = 0;
                    M->emis = mul(CPAL[(w8 + (int)floorf(CB)) & 3], 0.9f + 0.9f * CBT); if (fabsf(fract((an / 6.2832f + 0.5f) * 8.0f) - 0.5f) > 0.44f || ro < 0.10f || ro > 0.56f) { M->emis = V(0, 0, 0); M->alb = V(0.10f, 0.09f, 0.08f); } } else if (ro < 0.72f) M->alb = CREAM; }
            if (tower) {       // la tour : la porte sous sa marquise, une grande rosace de néon, des ampoules qui grimpent le long des arêtes
                if (fabsf(dz) > 3.45f) { M->alb = CREAM; if (len2((fabsf(dz) - 3.72f) / 0.45f, fract(p.y / 0.45f) - 0.5f) < 0.26f && p.y > 1.0f) { int on = (((int)floorf(p.y / 0.45f) - (int)floorf(CB * 4.0f)) & 3) < 2; M->alb = V(0.1f, 0.08f, 0.05f); M->emis = mul(V(1.0f, 0.72f, 0.30f), on ? 5.5f : 0.5f); } }
                else if (fabsf(p.y - 5.6f) < 0.22f) M->alb = CREAM;
                else { float rr = len2(dz, p.y - 8.5f), an = atan2f(p.y - 8.5f, dz);
                    if (rr < 2.0f) { M->alb = V(0.03f, 0.02f, 0.04f); M->spec = 1.2f; M->gloss = 90; *snow = 0; float sp = fabsf(fract(an / 6.2832f * 12.0f) - 0.5f), ring = fminf(fminf(fabsf(rr - 1.85f), fabsf(rr - 1.25f)), fabsf(rr - 0.55f));
                        int ph = (int)floorf(CB * 2.0f); v3 nc = CPAL[((int)floorf(rr / 0.65f) + ph) & 3];
                        M->emis = add(mul(nc, 4.5f * sstep(0.035f, 0.0f, ring) + 0.8f * expf(-ring * 9.0f)), mul(V(1.0f, 0.72f, 0.3f), (rr > 0.6f && rr < 1.8f ? 3.0f * sstep(0.47f, 0.495f, sp) : 0) * (0.5f + 0.5f * CBT)));
                        if (rr < 0.50f) M->emis = mul(V(1.0f, 0.70f, 0.25f), 1.5f + 2.0f * CBT); }
                    else if (rr < 2.15f) M->alb = GOLD; } }
            if (fabsf(dz) < 1.2f && p.y < 2.95f) { M->alb = V(0.02f, 0.02f, 0.02f); M->emis = mul(mix3(V(1.0f, 0.6f, 0.25f), CPAL[(int)floorf(CB) & 3], 0.6f), 1.2f + 2.6f * CBT * CE); if (fabsf(dz) < 0.03f || fabsf(dz) > 1.12f || p.y > 2.86f) { M->emis = V(0, 0, 0); M->alb = V(0.80f, 0.60f, 0.25f); } }
            if (p.z > -8.4f && p.y > 4.55f && p.y < 6.3f && fabsf(p.z - 4.0f) < 6.0f) { float sf = 9.6f / (TR[T_BAL].w - 80.0f), dt = text2d(T_BAL, (4.0f - p.z) / sf + TR[T_BAL].w * 0.5f, TR[T_BAL].base - (p.y - 5.05f) / sf) * sf, fl = 0.85f + 0.12f * sinf(T * 37.0f) * sinf(T * 5.3f) + 0.3f * CBT;
                M->emis = add(M->emis, add(mul(V(7.0f, 0.8f, 3.6f), sstep(0.03f, -0.03f, dt) * fl), mul(V(0.9f, 0.08f, 0.5f), expf(-fmaxf(dt, 0) * 5.0f) * fl * 0.8f))); } }
        break;
    case D_SKIN: case D_SNOUT: case D_EYE: case D_SHIRT: case D_PANTS: case D_BOOT: case D_GOLD: case D_HAIR: case D_WHITE: dn_material(p, m, M); break;
    case W_CGARL: { static const v3 GC[4] = {{1.0f, 0.16f, 0.12f}, {1.0f, 0.70f, 0.15f}, {0.20f, 1.0f, 0.35f}, {0.25f, 0.50f, 1.0f}}; float id = roundf(p.x / 0.65f) + 3.0f * roundf(p.z / 5.5f);       // les ampoules : quatre couleurs, une sur deux respire avec le temps
        M->alb = V(0, 0, 0); M->emis = mul(GC[((int)id % 4 + 4) % 4], 5.0f * (((int)id & 1) ? 0.45f + 0.55f * CBT : 0.45f + 0.55f * CBO)); break; }
    case W_CPROJ: { int i = (int)clampf(roundf((p.z + 3.0f) / 5.0f), 0, 3); v3 b0, bd; cl_spot(i, &b0, &bd); M->alb = V(0.035f, 0.035f, 0.04f); M->spec = 0.9f; M->gloss = 50; *snow = 1;       // un projecteur : un fût de métal noir, une lentille qui éblouit
        if (dot(n, bd) > 0.55f && len(sub(p, b0)) < 0.6f) { M->alb = V(0, 0, 0); M->emis = mul(add(mul(CPAL[(i + (int)floorf(CB * 0.5f)) & 3], 0.6f), V(0.5f, 0.5f, 0.5f)), (9.0f + 9.0f * CBT) * CBEAMK * 2.0f + 1.0f); *snow = 0; } break; }
    case W_CLAMP: M->alb = V(0.03f, 0.035f, 0.04f); M->spec = 0.8f; break;
    case W_CLANT: M->alb = V(0, 0, 0); M->emis = mul(V(1.0f, 0.62f, 0.28f), 7.0f * (1 - 0.85f * CDAWN)); *snow = 0.9f; *up = sstep(0.75f, 0.95f, n.y); break;
    case W_CTRAIN: { float xo = c_rep(p.x - CTRX, 10.2f), yy = p.y - 10.2f; M->alb = V(0.30f, 0.035f, 0.07f); M->spec = 0.9f; M->gloss = 60; *snow = 1;
        if (fabsf(yy + 0.45f) < 0.06f) M->alb = V(1.0f, 0.74f, 0.30f);
        if (fabsf(n.z) > 0.5f && yy > -0.05f && yy < 0.85f && fabsf(c_rep(xo, 1.5f)) < 0.52f && fabsf(xo) < 4.3f) { M->alb = V(0.02f, 0.02f, 0.02f); M->emis = V(2.8f, 1.8f, 0.8f); *snow = 0; } break; }
    case W_CVIAD: M->alb = mul(V(0.12f, 0.12f, 0.15f), 0.8f + 0.4f * vnoise(p.x * 0.9f, p.y * 1.7f)); if (fabsf(c_rep(p.y, 0.8f)) < 0.03f) M->alb = mul(M->alb, 0.6f); *snow = 1; break;
    case W_CSMAN: { v3 q = V((p.x + 3.2f - (p.z + 1.5f)) * 0.7071f, p.y, (p.x + 3.2f + p.z + 1.5f) * 0.7071f); M->alb = mul(WSNW, 0.96f); M->sss = 0.4f; M->spec = 0.3f;
        if (len(V(fabsf(q.x) - 0.12f, q.y - 1.99f, q.z + 0.30f)) < 0.05f || (q.z < 0 && fabsf(q.x) < 0.06f && fabsf(c_rep(q.y - 1.30f, 0.2f)) < 0.05f && q.y > 1.0f && q.y < 1.55f)) { M->alb = V(0.02f, 0.02f, 0.02f); M->sss = 0; } break; }
    case W_CSNOUT: if (p.y < 1.76f) { M->alb = V(0.46f, 0.04f, 0.10f); M->sss = 0.4f; } else { v3 q = V((p.x + 3.2f - (p.z + 1.5f)) * 0.7071f, p.y, (p.x + 3.2f + p.z + 1.5f) * 0.7071f); M->alb = len2(fabsf(q.x) - 0.045f, q.y - 1.88f) < 0.022f ? V(0.45f, 0.12f, 0.16f) : V(0.95f, 0.47f, 0.52f); M->sss = 0.25f; } break;
    // ---- l'espace des vitrines
    case W_CCHECK: { float ck = fmodf(floorf(p.x / 2.0f) + floorf(p.z / 2.0f) + 2000.0f, 2.0f), r = len2(p.x, p.z), w = fract(r * 0.045f - CB * 0.5f);
        M->alb = ck > 0.5f ? V(0.74f, 0.70f, 0.62f) : V(0.03f, 0.026f, 0.035f); M->spec = 1.0f; M->gloss = 110; M->env = 0.40f;
        if (ck > 0.5f && CHV != 4) M->emis = mul(CPAL[(int)floorf(r * 0.045f - CB * 0.5f + 64.0f) & 3], 0.30f * w * w * CE * sstep(90.0f, 20.0f, r)); break; }
    case W_CDISC: { float rr = len2(p.x, p.z), a = atan2f(p.z, p.x) - CDA, sa = 6.2831853f / CNRING; M->alb = V(0.012f, 0.012f, 0.014f); M->spec = 0.8f; M->gloss = 40; M->env = 0.14f;
        M->alb = mul(M->alb, 1.0f + 5.0f * powf(fabsf(sinf(atan2f(p.z, p.x) + 0.7f)), 22.0f) + 0.8f * sinf(rr * 60.0f));
        if (rr < 2.2f) { M->env = 0; M->alb = fabsf(fract(a / 6.2832f * 3.0f) - 0.5f) < 0.08f || fabsf(rr - 2.05f) < 0.06f ? V(0.9f, 0.7f, 0.25f) : V(0.42f, 0.04f, 0.09f); }
        else if (rr > 5.85f) { M->alb = V(0.5f, 0.5f, 0.52f); M->spec = 1.5f; }
        else if (fabsf(rr - 4.3f) < 0.62f && n.y > 0.5f) { float da = c_rep(a, sa) * rr; if (len2(rr - 4.3f, da) < 0.60f && len2(rr - 4.3f, da) > 0.50f) M->emis = mul(CPAL[((int)roundf(a / sa) + 64 + (int)floorf(CB)) & 3], 0.8f + 3.0f * CBT); }
        break; }
    case W_CRING: M->alb = V(0.02f, 0.02f, 0.02f); M->emis = mul(CPAL[((int)roundf(p.z / 3.0f) + (int)floorf(CB * 2.0f) + 4096) & 3], 2.2f + 4.5f * CBT); break;
    case W_CSPKW: { float u = p.x / 1.6f, v = (p.y - 0.1f) / 1.6f, iu = floorf(u), iv = floorf(v), rr = len2(fract(u) - 0.5f, fract(v) - 0.5f) * 1.6f, bt = expf(-4.0f * fract(CB - len2(iu + 0.5f, iv - 0.5f) * 0.125f));
        M->alb = V(0.02f, 0.02f, 0.022f); M->spec = 0.4f;
        if (rr < 0.62f) { M->alb = mul(V(0.07f, 0.07f, 0.075f), 0.6f + 0.7f * rr / 0.62f); if (rr < 0.62f * (0.30f + 0.14f * bt)) M->alb = V(0.17f, 0.17f, 0.18f); M->spec = 0.9f; M->emis = mul(CPAL[((int)(iu + iv) + 64) & 3], 0.10f * bt); }
        else if (rr < 0.69f) M->emis = mul(CPAL[((int)(iu + iv) + 64) & 3], 0.6f + 4.0f * bt);
        break; }
    case W_CGOLDL: M->alb = V(1.0f, 0.70f, 0.24f); M->env = 0.15f; M->spec = 1.8f; M->gloss = 120; M->emis = mul(V(1.0f, 0.62f, 0.18f), 0.10f + 0.25f * CBT + 0.5f * fmaxf(-n.z, 0) * 0.3f); break;
    case W_CLENS: M->alb = V(0.012f, 0.012f, 0.016f); M->spec = 2.2f; M->gloss = 220; M->env = 0.30f; break;
    case W_CSTUDIO: if (CPOST) { M->alb = V(0.010f, 0.010f, 0.018f); M->spec = 1.5f; M->gloss = 160; M->env = 0.80f;       // la scène de l'affiche : laque noire, cadres de néon au sol
        float d1 = fabsf(fmaxf(fabsf(p.x) - 3.3f, fabsf(p.z + 0.4f) - 2.3f)), d2 = fabsf(fmaxf(fabsf(p.x) - 5.0f, fabsf(p.z + 0.4f) - 3.6f)), d3 = fabsf(fmaxf(fabsf(p.x) - 1.5f, fabsf(p.z + 0.4f) - 5.2f));
        M->emis = add(add(mul(V(0.10f, 0.85f, 1.0f), 2.4f * sstep(0.045f, 0.02f, d1) + 0.05f / (d1 * 6.0f + 0.25f)), mul(V(1.0f, 0.10f, 0.50f), 2.4f * sstep(0.045f, 0.02f, d2) + 0.05f / (d2 * 6.0f + 0.25f))), mul(V(0.55f, 0.20f, 1.0f), 2.0f * sstep(0.04f, 0.02f, d3))); break; }
    { float gr = vnoise(p.x * 0.22f + 0.6f * vnoise(p.x * 0.05f, p.z * 0.4f), p.z * 3.4f), pl = fabsf(c_rep(p.z, 2.2f)) < 0.012f ? 0.55f : 1.0f;       // un comptoir de chêne verni
        M->alb = mul(mix3(V(0.11f, 0.045f, 0.022f), V(0.25f, 0.12f, 0.05f), gr), pl * (0.85f + 0.3f * vnoise(p.x * 3.0f, p.z * 40.0f))); M->spec = 1.2f; M->gloss = 110; M->env = 0.16f; break; }
    case W_CPAD: M->alb = fabsf(c_rep(p.y - 0.42f, 0.4f)) < 0.03f ? V(1.0f, 0.74f, 0.30f) : V(0.36f, 0.03f, 0.07f); M->spec = 0.7f; M->gloss = 30; M->sss = 0.3f; break;
    default: break;
    }
}
static int clip_czmat(v3 p, int k, int m, cmat_t *cm) {       // les danseurs qui ne sont pas placés un par un : la piste, la ronde, la foule sans fin, le rang
    v3 l; int mm, f = -1;
    if (WN > 0 && k == CDJ && m == C_TOP && len2(p.x - WPX[k], p.z - WPZ[k]) < 0.80f * WCZ[k].sc) {       // la chemise hawaïenne de DJ Viteau
        cz_sdf(&WCZ[k], &WRG[k], &WPP[k], w_local(p, k), &mm); cz_mat(&WCZ[k], &WPP[k], m, cm);
        if (fabsf(cm->alb.x - WCZ[k].ctop.x) < 0.01f && fabsf(cm->alb.y - WCZ[k].ctop.y) < 0.01f) { float v = (CZ_Q.y + 2.0f) / 0.115f, row = floorf(v), ua = atan2f(CZ_Q.x, CZ_Q.z) / 6.2832f * 7.0f + 0.5f * fmodf(row, 2.0f),
                fu = fract(ua) - 0.5f, fv = fract(v) - 0.5f, rr = len2(fu, fv), h = hash2(floorf(ua) + 3.0f, row), pet = 0.25f + 0.17f * fabsf(cosf(2.5f * atan2f(fv, fu) + h * 6.0f));
            if (rr < 0.075f) cm->alb = V(1.0f, 0.78f, 0.08f); else if (rr < pet) cm->alb = h < 0.55f ? V(1.0f, 0.20f, 0.42f) : V(0.96f, 0.93f, 0.86f);
            else if (len2(fabsf(fu) - 0.46f, fv * 1.8f + 0.3f) < 0.17f) cm->alb = V(0.05f, 0.40f, 0.12f); }
        return 1; }
    if (WN > 0 && len2(p.x - WPX[k], p.z - WPZ[k]) < 0.80f * WCZ[k].sc) return 0;
    if (CSET == 0) f = bal_fan(p, &l); else if (CSET == 2 && CHV != 1 && CHV < 4) f = hyp_fan(p, &l);
    if (f < 0) return 0;
    cz_sdf(&CZC[f % 24], &CRR[f], &CRP[f], l, &mm); cz_mat(&CZC[f % 24], &CRP[f], m, cm);
    if (m == C_SKIN) { static const v3 TN[5] = {{0.43f, 0.59f, 0.37f}, {0.11f, 0.22f, 0.22f}, {1.04f, 2.16f, 1.89f}, {1.03f, 1.78f, 1.68f}, {0.67f, 0.81f, 0.58f}};       // des robes de toutes les couleurs dans la foule : brune, noire, crème, rose pâle, rousse
        float cs = 2.2f, hs = CSET == 0 ? hash2(CFIX * 5.1f + 2.0f, CFIZ * 3.3f + 7.0f) : hash2(floorf(p.x / cs) * 5.1f + 2.0f, floorf(p.z / cs) * 3.3f + 7.0f);
        if (hs > 0.42f) { v3 tn = TN[(int)((hs - 0.42f) / 0.58f * 4.999f)]; cm->alb = V(fminf(cm->alb.x * tn.x, 1), fminf(cm->alb.y * tn.y, 1), fminf(cm->alb.z * tn.z, 1)); } }
    return 1;
}
// ---------------------------------------------------------------- les ciels, la brume
static v3 vil_sky(v3 rd) {
    float y = fmaxf(rd.y, 0); v3 c = add(mix3(V(0.060f, 0.075f, 0.16f), V(0.010f, 0.014f, 0.045f), powf(y, 0.45f)), mul(V(0.42f, 0.20f, 0.12f), 0.30f * powf(1 - y, 9.0f)));       // une nuit de neige : la ville éclaire les nuages
    if (CDAWN > 0) { v3 dn = mix3(V(1.0f, 0.52f, 0.30f), V(0.24f, 0.30f, 0.62f), powf(y, 0.42f)); float cs = dot(rd, CSUND);
        if (cs > 0.9925f) { float yy = (rd.y - CSUND.y) / 0.123f, band = yy < 0.2f ? sstep(0.40f, 0.5f, fract((0.2f - yy) * 4.0f)) : 1.0f; dn = mix3(dn, mix3(V(3.0f, 0.75f, 0.55f), V(3.2f, 2.3f, 0.9f), clampf(yy * 0.5f + 0.5f, 0, 1)), band); }
        dn = add(dn, mul(V(1.0f, 0.45f, 0.20f), 0.55f * powf(fmaxf(cs, 0), 30.0f))); c = mix3(c, dn, CDAWN); }
    { float az = atan2f(rd.x, rd.z) * 38.0f, h = hash2(floorf(az), 7.0f), top = 0.015f + 0.075f * h * h + 0.02f * hash2(floorf(az * 0.25f), 3.0f);       // la ville, au loin
        if (rd.y < top) { float v = rd.y / 0.0085f, fu = fract(az); int win = fract(v) > 0.3f && fract(fu * 3.0f) > 0.3f, lit = win && hash2(floorf(az) * 3.0f + floorf(fu * 3.0f), floorf(v)) < 0.16f;
            if (CVU > 0) lit = win && rd.y / top < (0.2f + 0.8f * hash2(floorf(az), floorf(CTS / 0.165f))) * (0.35f + 0.65f * CBT);
            c = mix3(V(0.020f, 0.024f, 0.045f), V(0.07f, 0.055f, 0.07f), CDAWN); if (lit) c = add(c, CVU > 0 ? mul(CPAL[(int)(h * 3.999f)], 0.9f) : V(0.55f, 0.33f, 0.13f)); } }
    return c;
}
static v3 hyp_hor(void) { static const v3 H[4] = {{0.42f, 0.06f, 0.34f}, {0.08f, 0.14f, 0.44f}, {0.55f, 0.15f, 0.05f}, {0.40f, 0.035f, 0.10f}}; return H[CTH]; }
static v3 hyp_sky(v3 rd) {
    float y = rd.y; v3 c;
    if (CHV == 5 && CPOST) {       // la scène de l'affiche des Ranga Boyz : la nuit bleue, cinq arches de néon emboîtées, une couronne d'or, des tours de projecteurs, de la brume
        float iz = 1 / fmaxf(rd.z, 0.05f), u = rd.x * iz, v = rd.y * iz, au = fabsf(u); c = mix3(V(0.012f, 0.016f, 0.07f), V(0.0f, 0.0f, 0.012f), sstep(0.0f, 0.55f, v));
        if (rd.z < 0.05f) return V(0.004f, 0.004f, 0.02f);
        static const v3 NC[5] = {{1.0f, 0.10f, 0.50f}, {0.10f, 0.85f, 1.0f}, {0.60f, 0.20f, 1.0f}, {1.0f, 0.10f, 0.50f}, {0.10f, 0.85f, 1.0f}};
        for (int k = 0; k < 5; k++) { float w = 0.066f + 0.033f * k, v0 = 0.125f + 0.030f * k, d = v < v0 ? fabsf(au - w) : fabsf(len2(u, v - v0) - w); c = add(c, mul(NC[k], 1.9f * sstep(0.0036f, 0.0014f, d) + 0.00075f / (d + 0.0035f))); }
        { static const float CX[7] = {-0.034f, -0.034f, -0.017f, 0, 0.017f, 0.034f, 0.034f}, CY[7] = {0.092f, 0.128f, 0.106f, 0.138f, 0.106f, 0.128f, 0.092f}; float d = 1e9f;
            for (int k = 0; k < 7; k++) { int k2 = (k + 1) % 7; float ax = u - CX[k], ay = v - CY[k], bx = CX[k2] - CX[k], by = CY[k2] - CY[k], t = clampf((ax * bx + ay * by) / (bx * bx + by * by), 0, 1); d = fminf(d, len2(ax - bx * t, ay - by * t)); }
            c = add(c, mul(V(1.0f, 0.72f, 0.22f), 2.0f * sstep(0.0030f, 0.0012f, d) + 0.0007f / (d + 0.0035f))); }
        for (int k = 0; k < 2; k++) { float tu = au - (k ? 0.243f : 0.214f), tv = c_rep(v - 0.01f, k ? 0.058f : 0.046f), d2 = tu * tu + tv * tv, on = v > -0.02f && v < 0.60f;       // les tours de projecteurs
            c = add(c, mul(V(0.75f, 0.85f, 1.0f), on * (0.000030f / (d2 + 0.000022f) + 0.35f * sstep(0.0022f, 0.0f, fabsf(tv)) * sstep(0.022f, 0.0f, fabsf(tu))))); if (fabsf(tu + 0.006f) < 0.0022f) c = add(c, V(0.03f, 0.04f, 0.09f)); }
        c = add(c, mul(V(0.05f, 0.03f, 0.15f), expf(-au * 7.0f) * sstep(0.40f, -0.05f, v)));       // la brume éclairée
        return c; }
    if (CHV == 5) {       // le studio : un fond bordeaux qui file au noir, une boîte à lumière derrière, un réflecteur devant à gauche
        c = mix3(V(0.16f, 0.045f, 0.04f), V(0.022f, 0.008f, 0.014f), sstep(-0.05f, 0.75f, y)); float iz = 1 / (fabsf(rd.z) + 1e-3f);
        if (rd.z > 0.1f) c = add(c, mul(V(1.0f, 0.74f, 0.42f), 1.7f * sstep(0.24f, 0.05f, fabsf(rd.x * iz - 0.01f)) * sstep(0.26f, 0.05f, fabsf(rd.y * iz + 0.03f))));
        { v3 w = nrm(V(-0.78f, 0.10f, -0.62f)), wr = nrm(cross(V(0, 1, 0), w)), wu = cross(w, wr); float wd = dot(rd, w), u = dot(rd, wr) / (wd + 1e-3f), v = dot(rd, wu) / (wd + 1e-3f);
            if (wd > 0.3f && fabsf(u) < 0.24f && fabsf(v) < 0.50f && fabsf(u) > 0.016f && fabsf(fabsf(v) - 0.17f) > 0.016f) c = add(c, V(2.3f, 2.15f, 1.9f)); }       // une haute fenêtre à six carreaux, à hauteur d'œil : elle se reflète dans le verre
        return add(c, mul(V(0.9f, 0.5f, 0.3f), 0.25f * powf(fmaxf(dot(rd, nrm(V(-0.7f, 0.5f, -0.5f))), 0), 6.0f))); }
    if (CHV == 1) { float a = atan2f(rd.y, rd.x) * 20.0f, h = hash2(floorf(a), 3.0f), r = len2(rd.x, rd.y), st = fract(logf(r + 0.02f) * 1.5f + h * 7.0f + CTS * 1.2f);       // le tunnel : le noir, des étoiles qui filent
        return add(mul(CPAL[(int)(h * 3.999f)], 0.5f * sstep(0.16f, 0.0f, fabsf(fract(a) - 0.5f)) * sstep(0.7f, 1.0f, st) * (h < 0.5f)), V(0.004f, 0.003f, 0.010f)); }
    c = y < 0 ? hyp_hor() : mix3(hyp_hor(), V(0.012f, 0.008f, 0.04f), powf(clampf(y, 0, 1), 0.45f));
    { float u = atan2f(rd.z, rd.x) * 60.0f, v = y * 70.0f, h = hash2(floorf(u), floorf(v)); if (y > 0.05f && h > 0.985f) c = add(c, mul(V(0.9f, 0.9f, 1.0f), 0.9f * sstep(0.35f, 0.1f, len2(fract(u) - 0.5f, fract(v) - 0.5f)) * (0.6f + 0.4f * sinf(T * 3.0f + h * 90.0f)))); }
    if (CHSUN > 0) { float cs = dot(rd, CSUND);       // le soleil à bandes des vitrines
        if (cs > 0.955f) { float yy = (rd.y - CSUND.y) / 0.30f, band = yy < 0.25f ? sstep(0.42f, 0.5f, fract((0.25f - yy) * 5.0f + 0.25f * CB)) : 1.0f;
            c = mix3(c, mul(mix3(V(1.0f, 0.16f, 0.42f), V(1.0f, 0.78f, 0.22f), clampf(yy * 0.5f + 0.5f, 0, 1)), 0.85f + 0.35f * CBT), sstep(0.955f, 0.958f, cs) * band * CHSUN); }
        c = add(c, mul(V(1.0f, 0.35f, 0.40f), 0.20f * CHSUN * powf(fmaxf(cs, 0), 24.0f))); }
    return c;
}
static v3 clip_sky(v3 ro, v3 rd) { return CSET == 0 ? V(0.02f, 0.012f, 0.03f) : CSET == 1 ? vil_sky(rd) : hyp_sky(rd); }
static v3 clip_fogc(v3 rd) {
    if (CSET == 0) return add(V(0.030f, 0.018f, 0.045f), mul(CPAL[(int)floorf(CB) & 3], 0.022f * CBT * CE * CSPOTK));
    if (CSET == 1) return mix3(V(0.075f, 0.075f, 0.13f), V(0.85f, 0.50f, 0.33f), CDAWN);
    return CHV == 1 ? V(0.004f, 0.003f, 0.010f) : hyp_hor();
}
static float clip_fod(v3 ro, v3 rd, float t) { return (CSET == 0 ? 0.012f : CSET == 1 ? 0.0075f : CHV == 1 ? 0.022f : CHV == 5 ? 0.0f : 0.0065f) * t; }
// ---------------------------------------------------------------- les lumières propres au clip
S v3 clip_light(v3 p, v3 n) {
    v3 L = V(0, 0, 0);
    if (CSET == 0) {
        if (fabsf(p.x) < 4.6f && p.z > -6.2f && p.z < 5.8f) { int ci = ((int)floorf(p.x / 1.4f + 7.0f) + (int)floorf(p.z / 1.4f + 9.0f) + (int)floorf(CB)) & 3;       // la piste éclaire les danseurs par en dessous
            L = add(L, mul(CPAL[ci], (0.25f + 0.9f * CBT) * CE * CSPOTK * (0.55f - 0.45f * n.y) / (1 + 0.9f * fmaxf(p.y, 0)))); }
        if (CSPOTL > 0) { v3 lv = sub(V(0, 6.2f, 0.5f), p); float il = 1 / len(lv), cone = sstep(2.0f, 1.2f, len2(p.x, p.z - 0.5f) + 0.10f * p.y);       // la poursuite, sur le couple
            L = add(L, mul(V(1.0f, 0.93f, 0.80f), CSPOTL * 2.6f * cone * fmaxf(dot(n, lv) * il, 0.0f))); }
    } else if (CSET == 1) {
        float z0 = floorf((p.z - 5.5f) / 11.0f) * 11.0f + 5.5f;       // les réverbères, la porte du Bal, ses quatre fenêtres
        if (p.y < 14.0f && fabsf(p.x) < 16.0f) for (int j = 0; j <= 1; j++) for (int sg = -1; sg <= 1; sg += 2) { v3 lv = sub(V(5.2f * sg, 4.6f, z0 + 11.0f * j), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(V(1.0f, 0.60f, 0.28f), 4.5f * (1 - 0.9f * CDAWN) / (1 + 0.30f * d2) * fmaxf(dot(n, lv) * il, 0))); }
        { v3 lv = sub(V(5.2f, 1.6f, -12.0f), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2);
            L = add(L, mul(mix3(V(1.0f, 0.6f, 0.25f), CPAL[(int)floorf(CB) & 3], 0.6f), (2.0f + 5.0f * CBT * CE) / (1 + 0.12f * d2) * fmaxf(dot(n, lv) * il, 0))); }
        for (int i = 0; i < 4; i++) { v3 lv = sub(V(6.0f, 3.0f, -7.0f + 4.0f * i), p); float d2 = dot(lv, lv), il = 1 / sqrtf(d2); L = add(L, mul(CPAL[(i + (int)floorf(CB)) & 3], (0.5f + 2.0f * CBT * CE) / (1 + 0.25f * d2) * fmaxf(dot(n, lv) * il, 0))); }
    } else {
        if (CHV == 0 && len2(p.x, p.z) < 7.0f) L = add(L, mul(CPAL[(int)floorf(CB) & 3], (0.3f + 1.0f * CBT) * CE * (0.55f - 0.45f * n.y) / (1 + 0.7f * fmaxf(p.y, 0))));
        if (CHV == 1) L = add(L, mul(CPAL[((int)roundf(p.z / 3.0f) + (int)floorf(CB * 2.0f) + 4096) & 3], 0.8f + 1.6f * CBT));
        if (CHV == 3) L = add(L, mul(CPAL[(int)floorf(CB) & 3], (0.4f + 1.8f * CBT) * fmaxf(n.z, 0.0f)));       // le mur éclaire les danseurs à contre-jour
    }
    return L;
}
S float c_beam(v3 ro, v3 rd, float tmax, v3 b0, v3 bd, float r0, float spread, float jit) {       // un faisceau dans la brume de la salle (on peut être dedans)
    float acc = 0, dt = fminf(tmax, 24.0f) / 18.0f;
    for (int i = 0; i < 18; i++) { v3 v = sub(add(ro, mul(rd, (i + jit) * dt)), b0); float al = dot(v, bd); if (al < 0.05f) continue;
        v3 pr = sub(v, mul(bd, al)); float cr = r0 + al * spread; acc += expf(-2.0f * dot(pr, pr) / (cr * cr)) / (1 + 0.04f * al * al) * dt * (r0 / cr); }
    return acc;
}
S float c_beamA(v3 ro, v3 rd, float tmax, v3 b0, v3 bd, float r0, float spread) {       // un faisceau vu de loin : ce que le rayon en traverse, au plus près de son axe
    v3 w = sub(ro, b0); float b = dot(rd, bd), dq = dot(rd, w), e = dot(bd, w), den = 1 - b * b; if (den < 0.03f) return 0;
    float tr = (b * e - dq) / den, al = (e - b * dq) / den; if (tr < 0 || tr > tmax || al < 0.2f) return 0;
    v3 dv = sub(add(w, mul(rd, tr)), mul(bd, al)); float cr = r0 + al * spread; return expf(-2.0f * dot(dv, dv) / (cr * cr)) * 1.25f * r0 * r0 / (cr * sqrtf(den)) / (1 + 0.0002f * al * al);
}
static v3 clip_air(v3 ro, v3 rd, float t, v3 col) {
    float tm = t > 0 ? t : 300.0f, jit = hash2(rd.x * 811.3f + T, rd.y * 433.7f);
    if (CSET == 0) {
        if (CBEAMK > 0) for (int i = 0; i < 6; i++) { v3 b0 = V(-6.0f + 2.4f * i, 5.6f, i & 1 ? 5.0f : -3.0f), bd = nrm(V(0.8f * sinf(CB * 0.785f + i * 1.7f) - b0.x * 0.25f, -1, 0.9f * cosf(CB * 0.6f + i * 2.1f) - (b0.z - 1.0f) * 0.2f));
            col = add(col, mul(CPAL[(i + (int)floorf(CB * 0.5f)) & 3], 0.55f * c_beam(ro, rd, tm, b0, bd, 0.08f, 0.11f, jit) * (0.3f + 0.7f * CBT) * CBEAMK)); }
        if (CSPOTL > 0) col = add(col, mul(V(1.0f, 0.93f, 0.80f), 0.5f * CSPOTL * c_beam(ro, rd, tm, V(0, 6.2f, 0.5f), V(0, -1, 0), 0.25f, 0.24f, jit)));
    } else if (CSET == 1) {
        if (CBEAMK > 0) for (int i = 0; i < 4; i++) { v3 b0, bd; cl_spot(i, &b0, &bd);       // sur le toit du Bal : quatre faisceaux dans la neige qui tombe
            col = add(col, mul(CPAL[(i + (int)floorf(CB * 0.5f)) & 3], 1.1f * c_beamA(ro, rd, tm, b0, bd, 0.55f, 0.035f) * (0.45f + 0.55f * CBT) * CBEAMK)); }
    } else if (CHV == 0 && CBEAMK > 0) for (int i = 0; i < 6; i++) { float a = -1.3f * CDA + i * 1.0472f; v3 bd = nrm(V(cosf(a) * 0.9f, 1.0f + 0.5f * sinf(CB * 0.785f + i), sinf(a) * 0.9f));
            col = add(col, mul(CPAL[(i + (int)floorf(CB)) & 3], 0.9f * c_beamA(ro, rd, tm, V(0, 0.2f, 0), bd, 0.16f, 0.045f) * (0.3f + 0.7f * CBT) * CBEAMK)); }
    return col;
}
// ---------------------------------------------------------------- les personnages
static cz_t CLZ[10];
static void cl_hair(void) {       // (17 nov. 2026, Kenny : « ils ont trop de chapeaux, mets-leur des cheveux ») dans le clip, la foule passe en cheveux, de couleurs naturelles
    // (les couleurs vives restent aux Ranga Boyz) ; seuls gardent leur chapeau le notable (haut-de-forme), le garde (képi) et le serveur (canotier). Le barman, le client et le cuisinier gardent le leur (copiés avant).
    static const v3 HB = {0.035f, 0.028f, 0.028f}, HD = {0.15f, 0.075f, 0.035f}, HC = {0.36f, 0.17f, 0.06f}, HR = {0.48f, 0.13f, 0.05f}, HY = {0.82f, 0.64f, 0.32f}, HG = {0.62f, 0.60f, 0.58f};
    static const struct { int k, hat; v3 c; } T[9] = {{0, HAT_SHORT, HD}, {1, HAT_LONG, HR}, {2, HAT_PART, HC}, {4, HAT_SHORT, HB}, {5, HAT_BOWL, HY}, {7, HAT_PART, HG}, {8, HAT_LONG, HB}, {10, HAT_SHORT, HC}, {11, HAT_BUN, HY}};
    for (int i = 0; i < 9; i++) { cz_t *C = &CZA[T[i].k]; C->hat = T[i].hat; C->chat = T[i].c; C->chat2 = add(mul(T[i].c, 1.25f), V(0.03f, 0.03f, 0.03f)); }
    // (17 nov. 2026, « plus de variété », puis « fais des jeunes, pas que des vieux, c'est une boîte de nuit ») douze jeunes fêtards : t-shirts, jeans, vestes de satin
    // pailleté, robes courtes de couleurs vives, coupes du moment ; plus de moustache, de canne ni de cheveux gris. Les deux tiers de la piste sont des jeunes (bal_fan).
    for (int i = 0; i < 12; i++) CZC[i] = CZA[i];
    CZA[7].chat = HY; CZA[7].chat2 = add(mul(HY, 1.25f), V(0.03f, 0.03f, 0.03f)); CZC[7] = CZA[7];       // le cuisinier n'a plus les cheveux gris
    static const v3 DEN = {0.14f, 0.22f, 0.42f}, JBK = {0.04f, 0.04f, 0.05f}, WT = {0.92f, 0.91f, 0.88f};
    static const struct { int top, flags, hat; v3 ctop, cbot, cjack, cdress, hc; float fat, leg; } Y[12] = {
        {TOP_TEE, F_PANTS | F_JACKET, HAT_CURLY, {0.92f, 0.91f, 0.88f}, {0.14f, 0.22f, 0.42f}, {0.04f, 0.04f, 0.05f}, {0}, {0.035f, 0.028f, 0.028f}, -0.1f, 0.3f},      // bombers noir, t-shirt blanc, jean
        {TOP_TEE, F_DRESS | F_LASH, HAT_PONY, {0.95f, 0.20f, 0.55f}, {0}, {0}, {0.95f, 0.20f, 0.55f}, {0.82f, 0.64f, 0.32f}, -0.3f, 0.4f},                           // robe rose fluo, queue de cheval blonde
        {TOP_TEE, F_PANTS, HAT_SPIKES, {0.98f, 0.82f, 0.10f}, {0.14f, 0.22f, 0.42f}, {0}, {0}, {0.035f, 0.028f, 0.028f}, -0.2f, 0.3f},                                  // t-shirt jaune, piques noires (châtain, ça faisait une couronne)
        {TOP_TEE, F_PANTS | F_JACKET | F_SEQUIN, HAT_SLICK, {0.04f, 0.04f, 0.05f}, {0.04f, 0.04f, 0.05f}, {0.45f, 0.12f, 0.75f}, {0}, {0.035f, 0.028f, 0.028f}, 0.0f, 0.3f},      // veste pailletée violette, gominé
        {TOP_TEE, F_PANTS | F_GLASSES, HAT_BOWL, {0.85f, 0.10f, 0.14f}, {0.04f, 0.04f, 0.05f}, {0}, {0}, {0.15f, 0.075f, 0.035f}, -0.2f, 0.4f},                       // t-shirt rouge, mèche, lunettes
        {TOP_TEE, F_PANTS, HAT_SHORT, {0.10f, 0.70f, 0.75f}, {0.14f, 0.22f, 0.42f}, {0}, {0}, {0.15f, 0.075f, 0.035f}, 0.0f, 0.0f},
        {TOP_TEE, F_DRESS | F_LASH, HAT_LONG, {0.10f, 0.35f, 0.95f}, {0}, {0}, {0.10f, 0.35f, 0.95f}, {0.035f, 0.028f, 0.028f}, -0.3f, 0.4f},                         // robe bleu électrique, cheveux longs noirs
        {TOP_TEE, F_PANTS | F_JACKET, HAT_SHORT, {0.55f, 0.85f, 0.15f}, {0.04f, 0.04f, 0.05f}, {0.14f, 0.22f, 0.42f}, {0}, {0.82f, 0.64f, 0.32f}, -0.1f, 0.3f},      // veste en jean, t-shirt vert pomme
        {TOP_TEE, F_DRESS | F_LASH, HAT_BUN, {0.62f, 0.12f, 0.80f}, {0}, {0}, {0.62f, 0.12f, 0.80f}, {0.48f, 0.13f, 0.05f}, -0.3f, 0.4f},                             // robe violette, chignon auburn
        {TOP_TEE, F_PANTS | F_JACKET | F_SEQUIN, HAT_MOHAWK, {0.92f, 0.91f, 0.88f}, {0.04f, 0.04f, 0.05f}, {0.70f, 0.72f, 0.78f}, {0}, {0.035f, 0.028f, 0.028f}, 0.0f, 0.3f},      // veste argentée, crête noire
        {TOP_TEE, F_PANTS, HAT_AFRO, {1.0f, 0.50f, 0.08f}, {0.28f, 0.30f, 0.10f}, {0}, {0}, {0.035f, 0.028f, 0.028f}, 0.1f, 0.2f},                                   // t-shirt orange, treillis, afro
        {TOP_TEE, F_DRESS | F_LASH, HAT_CURLY, {0.98f, 0.82f, 0.10f}, {0}, {0}, {0.98f, 0.82f, 0.10f}, {0.48f, 0.13f, 0.05f}, -0.2f, 0.3f}};
    for (int i = 0; i < 12; i++) { cz_t *C = &CZC[12 + i]; cz_base(C); C->skin = CZA[i].skin; C->sc = CZA[i].sc; C->top = Y[i].top; C->flags = Y[i].flags; C->hat = Y[i].hat;
        C->ctop = Y[i].ctop; C->cbot = Y[i].cbot; C->cjack = Y[i].cjack; C->cdress = Y[i].cdress; C->cdress2 = Y[i].cdress; C->chat = Y[i].hc; C->chat2 = add(mul(Y[i].hc, 1.25f), V(0.03f, 0.03f, 0.03f));
        C->fat = Y[i].fat; C->leg = Y[i].leg; C->eye = 0.25f; C->cacc = JBK;
        { static const v3 SH[6] = {{0.93f, 0.92f, 0.89f}, {0.05f, 0.05f, 0.06f}, {0.85f, 0.12f, 0.15f}, {0.10f, 0.35f, 0.85f}, {0.95f, 0.75f, 0.10f}, {0.92f, 0.30f, 0.60f}}; C->flags |= F_SHOES; C->cshoe = SH[(i * 5) % 6]; }       /* les jeunes en baskets */ if (C->flags & F_SEQUIN) C->cbot = JBK; (void)DEN; (void)WT; }
}
static void cl_people(void) {
    const v3 IV = V(0.86f, 0.83f, 0.75f), BX = V(0.33f, 0.03f, 0.07f), BLK = V(0.035f, 0.035f, 0.045f), GOB = V(0.30f, 0.52f, 0.20f); cz_t *C;
    C = &CLZ[0]; cz_base(C); C->sc = 1.12f; C->fat = 0.5f; C->top = TOP_SHIRT; C->ctop = V(0.02f, 0.44f, 0.48f); C->flags = F_PANTS | F_GLASSES | F_BUTTONS; C->cbot = V(0.80f, 0.78f, 0.72f); C->hat = HAT_NONE;      // DJ Viteau : bling, lunettes de soleil, chemise hawaïenne (Kenny, 15 nov. 2026)
    C = &CLZ[1]; cz_base(C); C->flags = F_DRESS | F_LASH | F_BOW; C->pdress = PAT_DOTS; C->cdress = V(0.62f, 0.04f, 0.10f); C->cdress2 = IV; C->cacc = V(0.85f, 0.62f, 0.20f); C->eye = 0.5f; C->fat = -0.2f;      // celle qui chante
    C = &CLZ[2]; cz_base(C); C->sc = 1.06f; C->top = TOP_SHIRT; C->ctop = IV; C->flags = F_PANTS | F_BRACES | F_BOW; C->cbot = BLK; C->cacc = BX; C->hat = HAT_BOATER; C->chat = V(0.82f, 0.64f, 0.32f); C->chat2 = BX; C->fat = 0.2f;      // celui avec qui elle danse
    CLZ[3] = CZA[9]; CLZ[4] = CZA[4]; CLZ[8] = CZA[7]; CLZ[9] = CZA[0];      // le barman, le client, le cuisinier, un passant
    for (int i = 0; i < 3; i++) { C = &CLZ[5 + i]; cz_base(C); C->sc = 0.60f + 0.04f * i; C->skin = mul(GOB, 0.9f + 0.15f * i); C->ear = 1.6f; C->snout = -0.5f; C->eye = 0.8f; C->leg = -0.5f; C->head = 0.6f; C->top = TOP_TEE; C->ctop = V(0.20f, 0.12f, 0.07f); C->flags = F_PANTS; C->cbot = V(0.09f, 0.07f, 0.06f);
        C->hat = HAT_BEANIE; C->chat = i == 0 ? V(0.55f, 0.05f, 0.08f) : i == 1 ? V(0.70f, 0.42f, 0.06f) : V(0.05f, 0.12f, 0.40f); C->chat2 = IV; }      // trois gobelins
}
static pose_t *cl_new(int who, float seed) { cz_t *C = &WCZ[WN]; *C = CLZ[who]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, T, seed); return P; }
static void cl_put(float x, float y, float z, float yaw) { w_place(WN, x, y, z, yaw); WN++; }
static void cl_dance(pose_t *P, float ph, int style, float e) {       // un rebond par temps ; huit façons de danser
    float sn = sinf(ph * 0.5f), j = 0.5f - 0.5f * cosf(ph), s2 = sinf(ph * 0.25f);       // (16 nov. 2026) le rebond n'a plus de cassure, et plus aucun geste ne saute d'une position à l'autre
    P->bob += (0.055f * j - 0.02f) * e; P->foot[0].y += 0.035f * fmaxf(sn, 0) * e; P->foot[1].y += 0.035f * fmaxf(-sn, 0) * e;
    P->twist += 0.20f * sn * e; P->roll += 0.07f * sn * e; P->hpitch += 0.10f * j * e; P->hroll += 0.08f * sn * e; P->earflop[0] += 0.3f * j * e; P->earflop[1] += 0.3f * j * e; P->tail += 0.5f * sn * e;
    switch (style & 7) {
    case 0: P->hand[0] = mix3(P->hand[0], V(0.36f, 0.80f + 0.05f * j, 0.10f), e); P->hand[1] = mix3(P->hand[1], V(-0.36f, 0.80f + 0.05f * (1 - j), 0.10f), e); break;       // les deux bras en l'air
    case 1: P->hand[0] = mix3(P->hand[0], V(0.30f, 0.52f + 0.33f * j, 0.22f), e); break;                                                                                 // un poing qui pompe
    case 2: P->hand[0] = mix3(P->hand[0], V(0.30f, 0.40f, 0.25f + 0.09f * sn), e); P->hand[1] = mix3(P->hand[1], V(-0.30f, 0.40f, 0.25f - 0.09f * sn), e); P->sway += 0.05f * s2 * e; break;      // le zouk : les coudes au corps, les hanches
    case 3: P->hand[1] = mix3(P->hand[1], V(-0.34f, 0.58f + 0.27f * j, 0.15f), e); P->hand[0].z += 0.10f * sn * e; break;
    case 4: { float k = sstep(-0.45f, 0.45f, s2); P->hand[0] = mix3(P->hand[0], mix3(V(0.42f, 0.18f, 0.16f), V(0.46f, 0.84f, 0.02f), k), e); P->hand[1] = mix3(P->hand[1], mix3(V(-0.46f, 0.84f, 0.02f), V(-0.42f, 0.18f, 0.16f), k), e); P->twist += 0.25f * (k - 0.5f) * e; break; }      // le doigt en l'air, à la fièvre du samedi soir
    case 5: P->hand[0] = mix3(P->hand[0], V(0.10f + 0.22f * (1 - j), 0.84f, 0.16f), e); P->hand[1] = mix3(P->hand[1], V(-0.10f - 0.22f * (1 - j), 0.84f, 0.16f), e); break;      // il tape des mains au-dessus de la tête
    case 6: P->hand[0] = mix3(P->hand[0], V(0.36f + 0.14f * s2, 0.78f, 0.08f), e); P->hand[1] = mix3(P->hand[1], V(-0.36f + 0.14f * s2, 0.78f, 0.08f), e); P->roll += 0.10f * s2 * e; break;      // les essuie-glaces
    default: P->hand[0] = mix3(P->hand[0], V(0.34f, 0.50f + 0.20f * sstep(-0.85f, 0.85f, sn), 0.30f), e); P->hand[1] = mix3(P->hand[1], V(-0.34f, 0.50f + 0.20f * sstep(0.85f, -0.85f, sn), 0.30f), e); P->twist *= 0.3f; P->hroll *= 0.3f; break;      // le robot
    }
    P->smile = 1; P->open = 0.3f * j * e;
}
static void cl_couple(float x, float z, float ang, float e, float gaze) {       // deux qui dansent le zouk, face à face ; ils tournent doucement
    float c = cosf(ang), sn = sinf(ang), sw = sinf(CB * PI * 0.5f), j = fabsf(sinf(CB * PI)), g = 0.40f;
    for (int i = 0; i < 2; i++) { pose_t *P = cl_new(1 + i, 0.2f + 0.5f * i); float sc = WCZ[WN].sc, sd = i ? -1.0f : 1.0f;
        P->hand[i] = V(sd * 0.40f / sc, 0.74f / sc - 0.336f + 0.03f * sw, g / sc + 0.02f);                    // les mains jointes, sur le côté
        P->hand[1 - i] = i ? V(0.22f, 0.66f / sc - 0.336f, 2 * g / sc - 0.08f) : V(-0.20f, 0.92f / sc - 0.336f, 2 * g / sc - 0.10f);      // l'autre main : elle sur son épaule, lui à sa taille
        P->sway += sd * 0.045f * sw * e; P->roll += sd * 0.05f * sw * e; P->bob += 0.02f * j * e; P->twist += 0.06f * sw * e; P->foot[0].y += 0.02f * fmaxf(sw, 0) * e; P->foot[1].y += 0.02f * fmaxf(-sw, 0) * e;
        P->hyaw = 0.08f * sw; P->hpitch = i ? -0.06f : 0.04f; P->hroll = sd * 0.06f * sw; P->lookx = 0; P->looky = i ? 0.25f : 0.05f; P->smile = 0.85f + 0.15f * gaze; P->happy = gaze; P->blink = fmaxf(P->blink, 0.0f);
        cl_put(x - sd * g * c, 0, z - sd * g * sn, atan2f(sd * c, sd * sn)); }
}
static void cl_crowd(float e) {       // les vingt-quatre attitudes de la foule ; chacun son décalage (jusqu'à un cinquième de temps), un sur cinq danse à mi-tempo
    for (int k = 0; k < NCR; k++) { pose_t *P = &CRP[k]; int a = k % 12; cz_rest(&CZC[k % 24], P); cz_idle(P, T, 0.07f * k); cl_dance(P, (k % 5 == 3 ? 3.1416f : 6.2832f) * CB + 1.15f * sinf(k * 2.7f), k * 7 + k / 12 * 3, e * (0.65f + 0.35f * sinf(k * 1.9f))); P->hyaw += 0.2f * sinf(CB * 0.4f + k); cz_rig(&CZC[k % 24], P, &CRR[k]); }
}
static void cl_dj(float up) {       // DJ Viteau : une main sur chaque platine ; up : le bras qu'il lève
    pose_t *P = cl_new(0, 0.3f); float j = fabsf(sinf(CB * PI)), sc = WCZ[WN].sc, hy = 0.70f / sc - 0.336f, hz = 0.38f / sc;
    P->hand[1] = V(-0.38f / sc - 0.02f, hy, hz + 0.05f * CSCR); P->hand[0] = V(0.38f / sc - 0.05f, hy, hz - 0.03f);
    P->hand[0] = mix3(P->hand[0], V(0.36f, 0.86f + 0.04f * j, 0.12f), fminf(up, 1.0f)); if (up > 1.0f) P->hand[1] = mix3(P->hand[1], V(-0.36f, 0.86f + 0.04f * (1 - j), 0.12f), up - 1.0f);
    P->bob += 0.03f * j * (0.5f + 0.5f * CE); P->hpitch += 0.14f * j + 0.08f - 0.14f * fminf(up, 1.0f); P->twist += 0.08f * sinf(CB * PI); P->smile = 0.95f; P->looky = -0.1f + 0.3f * fminf(up, 1.0f); P->open = 0.4f * fminf(up, 1.0f);
    CDJ = WN; cl_put(0, STY, 9.38f, PI);
    { const rig_t *R = &WRG[CDJ]; int mm; float qy = 0.34f, z = 0;       // où pend la médaille : la première hauteur du ventre que la tête ne cache pas, puis un peu plus bas
        for (int pass = 0; pass < 2; pass++) { for (;;) { z = 0.7f;
                for (int it = 0; it < 48; it++) { float e = cz_sdf(&WCZ[CDJ], R, &WPP[CDJ], add(R->ot, add(mul(R->ty, qy), mul(R->tz, z))), &mm); if (e < 0.002f) break; z -= fmaxf(e, 0.004f); }
                if (pass || z < 0.30f || qy < 0.06f) break; qy -= 0.02f; }
            if (!pass) { DJT = qy + 0.14f; qy -= 0.03f; } }
        DJQ = qy; DJZ = z; if (getenv("DJDBG")) fprintf(stderr, "medaille qy=%.3f z=%.3f haut=%.3f\n", DJQ, DJZ, DJT); }
}
// ================================================================ LES RANGA BOYZ : Kevin Ranga et ses six danseurs (16 nov. 2026)
// Une chorégraphie de boys band : tout le monde fait le même geste sur le temps, net ; quatre phrases de huit temps (32 temps, 21 s), une formation par phrase.
static cz_t RBZ[7]; static int RBPAIL = 1;       // 1 : les Ranga Boyz (Porko citoyens) en satin pailleté, 0 : en costards gris
static void rb_people(void) {       // tenues accordées (veste grise, chemise noire, pantalon noir) et à peu près la même taille (Kenny, 16 nov. 2026) ; pas de robe tachetée
    const v3 BLK = V(0.030f, 0.030f, 0.038f), GREY = V(0.15f, 0.155f, 0.175f); cz_t *C;
    static const v3 SK[7] = {{0.30f, 0.16f, 0.10f}, {0.92f, 0.37f, 0.38f}, {0.085f, 0.07f, 0.075f}, {0.95f, 0.66f, 0.64f}, {0.93f, 0.42f, 0.44f}, {0.96f, 0.80f, 0.72f}, {0.82f, 0.50f, 0.50f}};
    for (int i = 0; i < 7; i++) { C = &RBZ[i]; cz_base(C); C->skin = SK[i]; C->top = TOP_SHIRT; C->ctop = V(0.90f, 0.89f, 0.86f); C->cacc = BLK; C->flags = F_PANTS | F_JACKET | F_BUTTONS | F_TIE; C->cbot = GREY; C->cjack = GREY; C->hat = HAT_NONE; C->sc = 1.0f; }       // en costard gris : veste et pantalon (Kenny, 16 nov. 2026)
    C = &RBZ[0]; C->sc = 1.04f; C->fat = 0.1f;                                                                                // Kevin Ranga, le chef : un marron un peu foncé
    C = &RBZ[1]; C->hat = HAT_QUIFF; C->chat = V(0.90f, 0.80f, 0.50f); C->chat2 = V(1.0f, 0.96f, 0.80f);                       // la banane platine
    C = &RBZ[2]; C->hat = HAT_AFRO; C->chat = V(0.75f, 0.36f, 0.05f); C->chat2 = V(0.95f, 0.62f, 0.15f); C->fat = 0.15f;       // la boule orange
    C = &RBZ[3]; C->hat = HAT_MOHAWK; C->chat = V(0.02f, 0.45f, 0.48f); C->chat2 = V(0.55f, 0.95f, 0.90f); C->fat = -0.1f;      // la crête verte (validée)
    C = &RBZ[4]; C->hat = HAT_BOWL; C->chat = V(0.36f, 0.10f, 0.55f); C->chat2 = V(0.62f, 0.30f, 0.80f); C->sc = 0.98f; C->flags |= F_GLASSES;      // le bol violet, les lunettes
    C = &RBZ[5]; C->hat = HAT_SPIKES; C->chat = V(0.05f, 0.16f, 0.55f); C->chat2 = V(0.35f, 0.65f, 1.0f); C->sc = 1.02f; C->fat = 0.2f;             // les piques bleues
    C = &RBZ[6]; C->hat = HAT_BUN; C->chat = V(0.50f, 0.04f, 0.09f); C->chat2 = V(0.85f, 0.20f, 0.25f); C->fat = -0.1f;        // le chignon bordeaux
}
typedef struct { float r[3], l[3], bob, sway, lean, twist, roll, hyaw, hpitch, f0[3], f1[3]; } rbk_t;       // une pose clé : les deux mains, le bassin, le buste, la tête, les deux pieds (écarts au repos)
enum { RB_REST, RB_STEPR, RB_STEPL, RB_PTR, RB_PTL, RB_CROSS, RB_OPEN, RB_LOW, RB_UP, RB_KICK, RB_GUN, RB_HIPR, RB_HIPL, RB_BOW, RB_CLAP, RB_FLEX, RB_KNEE, RB_FIN, RB_RS, RB_RSL, RB_NK };
static const rbk_t RBK[RB_NK] = {
    [RB_REST]  = {{0.365f, 0.112f, 0.075f}, {-0.365f, 0.112f, 0.075f}, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0}, {0, 0, 0}},
    [RB_STEPR] = {{0.52f, 0.44f, 0.10f}, {-0.14f, 0.48f, 0.27f}, -0.05f, 0.16f, 0.03f, 0.45f, 0.16f, 0.45f, 0, {0.20f, 0, 0}, {0.06f, 0.04f, 0}},
    [RB_STEPL] = {{0.14f, 0.48f, 0.27f}, {-0.52f, 0.44f, 0.10f}, -0.05f, -0.16f, 0.03f, -0.45f, -0.16f, -0.45f, 0, {-0.06f, 0.04f, 0}, {-0.20f, 0, 0}},
    [RB_PTR]   = {{0.54f, 0.88f, 0.04f}, {-0.30f, 0.30f, 0.12f}, 0.03f, 0.10f, -0.05f, 0.25f, -0.20f, 0.45f, -0.25f, {0.16f, 0, 0}, {0, 0, 0}},
    [RB_PTL]   = {{0.30f, 0.30f, 0.12f}, {-0.54f, 0.88f, 0.04f}, 0.03f, -0.10f, -0.05f, -0.25f, 0.20f, -0.45f, -0.25f, {0, 0, 0}, {-0.16f, 0, 0}},
    [RB_CROSS] = {{0.07f, 0.47f, 0.30f}, {-0.07f, 0.41f, 0.30f}, -0.02f, 0, 0.10f, 0, 0, 0, 0.35f, {-0.03f, 0, 0}, {0.03f, 0, 0}},
    [RB_OPEN]  = {{0.58f, 0.56f, 0.02f}, {-0.58f, 0.56f, 0.02f}, 0.02f, 0, -0.08f, 0, 0, 0, -0.25f, {0.08f, 0, 0}, {-0.08f, 0, 0}},
    [RB_LOW]   = {{0.34f, 0.10f, 0.30f}, {-0.34f, 0.10f, 0.30f}, -0.13f, 0, 0.16f, 0, 0, 0, 0.10f, {0.12f, 0, 0}, {-0.12f, 0, 0}},
    [RB_UP]    = {{0.24f, 0.90f, 0.06f}, {-0.24f, 0.90f, 0.06f}, 0.20f, 0, -0.06f, 0, 0, 0, -0.30f, {0.03f, 0.17f, 0}, {-0.03f, 0.17f, 0}},       // le saut
    [RB_KICK]  = {{0.44f, 0.30f, -0.14f}, {-0.44f, 0.30f, -0.14f}, 0.01f, -0.04f, -0.14f, 0, 0, 0, -0.10f, {0, 0.20f, 0.30f}, {0, 0, 0}},
    [RB_GUN]   = {{0.14f, 0.60f, 0.52f}, {-0.31f, 0.30f, 0.10f}, -0.02f, -0.04f, 0.06f, -0.30f, 0, 0.30f, 0.05f, {0.02f, 0, 0.10f}, {-0.06f, 0, -0.06f}},
    [RB_HIPR]  = {{0.31f, 0.30f, 0.10f}, {-0.31f, 0.30f, 0.10f}, -0.04f, 0.17f, 0, 0.15f, 0.22f, -0.30f, 0, {0.06f, 0, 0}, {-0.02f, 0, 0}},
    [RB_HIPL]  = {{0.31f, 0.30f, 0.10f}, {-0.31f, 0.30f, 0.10f}, -0.04f, -0.17f, 0, -0.15f, -0.22f, 0.30f, 0, {0.02f, 0, 0}, {-0.06f, 0, 0}},
    [RB_BOW]   = {{0.40f, 0.20f, -0.16f}, {-0.40f, 0.20f, -0.16f}, -0.05f, 0, 0.30f, 0, 0, 0, 0.50f, {0, 0, 0}, {0, 0, 0}},
    [RB_CLAP]  = {{0.05f, 0.86f, 0.14f}, {-0.05f, 0.86f, 0.14f}, 0.04f, 0, -0.03f, 0, 0, 0, -0.20f, {0, 0, 0}, {0, 0, 0}},
    [RB_FLEX]  = {{0.42f, 0.66f, 0.06f}, {-0.42f, 0.66f, 0.06f}, -0.04f, 0, -0.02f, 0, 0, 0, 0, {0.13f, 0, 0}, {-0.13f, 0, 0}},
    [RB_FIN]   = {{0.50f, 0.92f, 0.02f}, {-0.33f, 0.28f, 0.10f}, 0.0f, 0.10f, -0.03f, 0.20f, -0.16f, -0.50f, 0.42f, {0.20f, 0, 0}, {-0.04f, 0, 0}},       // le chef : un bras au ciel, la tête baissée
    [RB_RS]    = {{0.08f, 0.36f, 0.32f}, {-0.08f, 0.41f, 0.32f}, -0.08f, 0, 0.07f, 0, 0, 0, 0.05f, {0.16f, 0, 0}, {-0.16f, 0, 0}},       // le Ranga Style : au galop, les poignets croisés
    [RB_RSL]   = {{0.30f, 0.86f, 0.05f}, {-0.10f, 0.42f, 0.30f}, -0.08f, 0, 0.04f, 0.15f, 0, 0, -0.10f, {0.16f, 0, 0}, {-0.16f, 0, 0}},       // le Ranga Style : le lasso
    [RB_KNEE]  = {{0.54f, 0.62f, 0.10f}, {-0.30f, 0.16f, 0.22f}, -0.22f, 0.04f, 0.05f, 0.25f, -0.10f, 0.35f, -0.15f, {0.12f, 0, 0.14f}, {-0.10f, 0, -0.16f}},
};
static const unsigned char RBSEQ[32] = {RB_STEPR, RB_PTR, RB_STEPL, RB_PTL, RB_CROSS, RB_OPEN, RB_LOW, RB_UP,              // phrase 1, en V : l'unisson
                                        RB_CROSS, RB_LOW, RB_UP, RB_OPEN, RB_HIPR, RB_HIPL, RB_GUN, RB_GUN,                // phrase 2, en ligne : en canon, la vague passe d'un danseur à l'autre
                                        RB_KICK, RB_STEPR, RB_CLAP, RB_STEPL, RB_PTR, RB_PTL, RB_FLEX, RB_FLEX,            // phrase 3, sur deux rangs : les deux moitiés en miroir, puis un tour sur soi
                                        RB_RS, RB_RS, RB_RSL, RB_RSL, RB_OPEN, RB_BOW, RB_GUN, RB_GUN};                // phrase 4, en losange : la pose finale (Kevin vise la caméra, les autres un genou à terre)
static const float RBF[4][7][2] = {       // les formations ; 0 : Kevin, 1-2 ses voisins, 3-4, 5-6 les ailes (impairs à gauche)
    {{0, 0}, {-1.05f, 0.85f}, {1.05f, 0.85f}, {-2.1f, 1.7f}, {2.1f, 1.7f}, {-3.15f, 2.55f}, {3.15f, 2.55f}},
    {{0, 0.4f}, {-1.1f, 0.6f}, {1.1f, 0.6f}, {-2.2f, 0.6f}, {2.2f, 0.6f}, {-3.3f, 0.6f}, {3.3f, 0.6f}},
    {{0, -0.2f}, {-1.4f, 0}, {1.4f, 0}, {-0.7f, 1.4f}, {0.7f, 1.4f}, {-2.1f, 1.4f}, {2.1f, 1.4f}},
    {{0, -0.3f}, {-1.15f, 0.75f}, {1.15f, 0.75f}, {-2.3f, 1.8f}, {2.3f, 1.8f}, {-0.75f, 2.2f}, {0.75f, 2.2f}}};
static void rb_key(int k, int mir, rbk_t *o) {       // la pose k, ou son miroir
    *o = RBK[k]; if (!mir) return;
    for (int a = 0; a < 3; a++) { float t = o->r[a]; o->r[a] = o->l[a]; o->l[a] = t; t = o->f0[a]; o->f0[a] = o->f1[a]; o->f1[a] = t; }
    o->r[0] = -o->r[0]; o->l[0] = -o->l[0]; o->f0[0] = -o->f0[0]; o->f1[0] = -o->f1[0]; o->sway = -o->sway; o->twist = -o->twist; o->roll = -o->roll; o->hyaw = -o->hyaw;
}
// CHORÉGRAPHIE N° 2 (16 nov. 2026, après Martin Plaisir) : plus de poses tenues puis sautées. Un seul balancement pour les sept (une hanche par temps, le rebond sur le temps,
// la tête en retard), et par-dessus quatre phrases de bras fondues l'une dans l'autre : 1 le rebond (balancier, puis on frappe des mains en sautant), 2 la vague (accroupi puis
// debout, en canon d'un danseur à l'autre), 3 le miroir (un bras au ciel puis l'autre, les deux moitiés en miroir, un tour sur soi), 4 le Ranga Style (au galop, le lasso) et la pose.
S float rb_shp(float x) { return x / (0.42f + 0.58f * fabsf(x)); }       // un sinus qui « frappe » : il arrive plus vite, il reste un peu, sans jamais casser
S float rb_win(float b, float a, float e) { return sstep(a - 0.45f, a + 0.45f, b) * sstep(e + 0.45f, e - 0.45f, b); }
static void rb_dancer(int i, float b, float ox, float oz, float face) {       // le danseur i au temps b (en temps de la musique), le groupe posé en (ox, oz), tourné de face
    float bl = fmodf(b, 32.0f); if (bl < 0) bl += 32.0f; int ph = (int)(bl / 8.0f), left = (i & 1) && i > 0; float mg = left ? -1.0f : 1.0f;
    float bc = bl - 0.5f * ((i + 1) / 2) * rb_win(bl, 8.6f, 15.4f), w = PI * bc, s1 = rb_shp(sinf(w)), c2 = cosf(2 * w), fin = sstep(28.0f, 29.6f, bl), calm = 1.0f - 0.7f * fin;
    cz_t *C = &WCZ[WN]; *C = RBZ[i]; pose_t *P = &WPP[WN]; cz_rest(C, P);
    if (RBPAIL) { static const v3 RBC[7] = {{0.95f, 0.60f, 0.10f}, {0.95f, 0.14f, 0.42f}, {0.50f, 0.72f, 0.98f}, {0.08f, 0.72f, 0.42f}, {0.52f, 0.22f, 0.95f}, {0.90f, 0.10f, 0.13f}, {0.92f, 0.88f, 0.98f}}; C->cjack = C->cbot = RBC[i]; C->flags |= F_SEQUIN; }       // les costumes de scène pailletés
    P->blink = fract(b * 0.11f + i * 0.37f) < 0.03f;
    P->sway = 0.075f * s1 * calm; P->roll = 0.11f * s1 * calm; P->twist = 0.24f * sinf(w - 0.5f) * calm; P->bob = (-0.012f - 0.030f * c2) * calm; P->lean = 0.03f;
    P->hroll = -0.10f * rb_shp(sinf(w - 1.25f)) * calm; P->hyaw = 0.16f * sinf(w - 1.9f) * calm; P->hpitch = 0.06f * sinf(2 * w - 1.0f) * calm;
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, fr = sstep(0.2f, 0.95f, -sg * s1) * calm; P->foot[s].y += 0.05f * fr; P->footp[s] += 0.5f * fr; P->foot[s].x += sg * 0.03f; }
    v3 H[6][2]; float W[6] = {rb_win(bl, -2.0f, 8.0f), rb_win(bl, 8.0f, 16.0f), rb_win(bl, 16.0f, 24.0f), rb_win(bl, 24.0f, 26.0f), rb_win(bl, 26.0f, 28.0f), rb_win(bl, 28.0f, 40.0f)}, ws = 0;
    float clap = rb_win(bl, 6.3f, 7.7f), jump = sinf(PI * clampf(bl - 6.9f, 0, 1)); jump *= jump;       // 1 : le balancier des deux bras, puis les mains qui se rejoignent au-dessus de la tête, en sautant
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f; H[0][s] = mix3(V(sg * 0.34f + 0.17f * s1, 0.36f + 0.22f * (0.5f - 0.5f * c2), 0.17f), V(sg * 0.07f, 0.88f, 0.12f), clap); }
    P->bob += 0.17f * jump * (ph == 0); P->foot[0].y += 0.15f * jump * (ph == 0); P->foot[1].y += 0.15f * jump * (ph == 0);
    float u2 = 0.5f - 0.5f * cosf(PI * bc * 0.5f);       // 2 : la vague : on descend les mains aux genoux, on remonte les bras au ciel, chacun un demi-temps après son voisin
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f; H[1][s] = mix3(V(sg * 0.33f, 0.08f, 0.28f), V(sg * 0.26f, 0.90f, 0.05f), u2); }
    P->bob -= 0.11f * (1 - u2) * W[1]; P->lean += 0.15f * (1 - u2) * W[1]; P->hpitch += (0.15f - 0.40f * u2) * W[1];
    float r = 0.5f + 0.5f * mg * rb_shp(sinf(w - 0.4f));       // 3 : un bras au ciel, l'autre à la hanche, et on échange ; la moitié gauche fait l'inverse
    H[2][0] = mix3(V(0.30f, 0.30f, 0.12f), V(0.53f, 0.87f, 0.04f), r); H[2][1] = mix3(V(-0.30f, 0.30f, 0.12f), V(-0.53f, 0.87f, 0.04f), 1 - r);
    float g2 = 0.5f - 0.5f * cosf(4 * w), sg2 = sinf(2 * w);       // 4 : le Ranga Style : au galop, deux petits bonds par temps, les poignets croisés ; puis le lasso
    H[3][0] = V(0.08f, 0.37f + 0.05f * g2, 0.32f); H[3][1] = V(-0.08f, 0.41f + 0.05f * g2, 0.32f);
    H[4][0] = V(0.30f + 0.13f * cosf(4 * w), 0.86f, 0.05f + 0.13f * sinf(4 * w)); H[4][1] = V(-0.10f, 0.42f + 0.04f * g2, 0.30f);
    { float gk = W[3] + W[4]; P->bob += (0.055f * g2 - 0.05f) * gk; P->foot[0].y += 0.085f * sstep(0.1f, 0.9f, sg2) * gk; P->foot[1].y += 0.085f * sstep(0.1f, 0.9f, -sg2) * gk; P->foot[0].x += 0.035f * gk; P->foot[1].x -= 0.035f * gk; P->lean += 0.05f * gk; }
    if (i == 0) { H[5][0] = V(0.50f, 0.92f, 0.02f); H[5][1] = V(-0.33f, 0.28f, 0.10f); P->hpitch -= 0.22f * fin; P->hyaw += 0.30f * fin; P->roll -= 0.10f * fin; P->sway += 0.06f * fin; }       // la pose : le chef un bras au ciel, les autres un genou à terre, tournés vers lui
    else { H[5][0] = V(mg * 0.54f, 0.62f, 0.10f); H[5][1] = V(-mg * 0.30f, 0.16f, 0.22f); if (left) { v3 t = H[5][0]; H[5][0] = H[5][1]; H[5][1] = t; }
        P->bob -= 0.21f * fin; P->twist += mg * 0.25f * fin; P->hyaw += mg * 0.35f * fin; P->hpitch -= 0.15f * fin; P->foot[left ? 1 : 0].z += 0.14f * fin; P->foot[left ? 0 : 1].z -= 0.16f * fin; }
    for (int k = 0; k < 6; k++) ws += W[k];
    for (int s = 0; s < 2; s++) { v3 h = V(0, 0, 0); for (int k = 0; k < 6; k++) h = add(h, mul(H[k][s], W[k] / ws)); P->hand[s] = h; }
    P->earflop[0] += 0.30f * (0.5f + 0.5f * c2) * calm; P->earflop[1] += 0.30f * (0.5f + 0.5f * c2) * calm; P->tail = 0.6f * sinf(w); P->smile = 0.85f; P->open = 0.05f + 0.30f * fmaxf(clap, fmaxf(u2 * W[1], fin)); P->brow = 0.2f;
    // la place : la formation de la phrase ; on glisse vers la suivante pendant ses deux derniers temps, en marchant
    int pn = (ph + 1) & 3; float g = ease(clampf((bl - ph * 8.0f - 6.0f) / 2.0f, 0, 1)); float x = mixf(RBF[ph][i][0], RBF[pn][i][0], g), z = mixf(RBF[ph][i][1], RBF[pn][i][1], g);
    float yaw = 6.2831853f * ease(clampf((bl - 21.6f) / 2.2f, 0, 1)) * mg * (ph == 2);       // le tour sur soi, à la fin de la phrase 3
    if (g > 0 && g < 1) cz_walk(P, bl * PI * 2.0f, 0.5f * sinf(PI * g));
    float cf = cosf(face), sf = sinf(face); w_place(WN, ox + x * cf - z * sf, 0, oz - x * sf - z * cf, face + yaw); WN++;       // z de la formation : derrière le chef
}
static void rb_group(float b, float ox, float oz, float face) { for (int i = 0; i < 7; i++) rb_dancer(i, b, ox, oz, face); }
static cz_t CLV[10];       // dix habitants de plus (16 nov. 2026 : « plus de diversité chez les Porko citoyens ») : robes, tailles, coiffures, tenues d'hiver
static void cv_people(void) {
    const v3 IV = V(0.88f, 0.86f, 0.80f), BLK = V(0.035f, 0.035f, 0.045f), BX = V(0.36f, 0.03f, 0.08f); cz_t *C;
    for (int i = 0; i < 10; i++) { C = &CLV[i]; cz_base(C); C->hat = HAT_NONE; C->top = TOP_SHIRT; C->flags = F_PANTS; }
    C = &CLV[0]; C->skin = V(0.085f, 0.07f, 0.075f); C->hat = HAT_USHANKA; C->chat = V(0.30f, 0.17f, 0.09f); C->chat2 = IV; C->flags |= F_COAT | F_SCARF; C->cjack = BX; C->cacc = V(0.85f, 0.62f, 0.20f); C->sc = 1.05f;
    C = &CLV[1]; C->skin = V(0.95f, 0.70f, 0.68f); C->flags = F_DRESS | F_LASH | F_SCARF; C->pdress = PAT_DOTS; C->cdress = V(0.05f, 0.25f, 0.40f); C->cdress2 = IV; C->hat = HAT_EARMUFF; C->chat = V(0.75f, 0.06f, 0.10f); C->chat2 = BLK; C->cacc = V(0.75f, 0.06f, 0.10f); C->eye = 0.5f;
    C = &CLV[2]; C->skin = V(0.42f, 0.23f, 0.14f); C->hat = HAT_BEANIE; C->chat = V(0.10f, 0.34f, 0.16f); C->chat2 = IV; C->flags |= F_COAT; C->cjack = V(0.10f, 0.26f, 0.14f); C->sc = 0.92f; C->fat = 0.3f;
    C = &CLV[3]; C->skin = V(0.96f, 0.80f, 0.72f); C->hat = HAT_QUIFF; C->chat = V(0.70f, 0.26f, 0.05f); C->chat2 = V(0.95f, 0.55f, 0.15f); C->flags |= F_COAT | F_STACHE; C->cjack = V(0.05f, 0.09f, 0.26f); C->sc = 1.14f; C->fat = 0.9f;
    C = &CLV[4]; C->sc = 0.70f; C->head = 0.5f; C->eye = 0.7f; C->leg = -0.3f; C->hat = HAT_EARMUFF; C->chat = V(0.90f, 0.70f, 0.10f); C->chat2 = IV; C->flags |= F_COAT | F_SCARF; C->cjack = V(0.70f, 0.07f, 0.09f); C->cacc = IV;       // un petit
    C = &CLV[5]; C->skin = V(0.78f, 0.52f, 0.52f); C->hat = HAT_BERET; C->chat = BLK; C->flags |= F_COAT | F_GLASSES | F_STACHE | F_SCARF; C->cjack = V(0.30f, 0.30f, 0.32f); C->cacc = BX; C->prop = PROP_CANE; C->sc = 0.96f; C->fat = 0.2f;       // un ancien
    C = &CLV[6]; C->skin = V(0.50f, 0.42f, 0.42f); C->flags |= F_JACKET; C->hat = HAT_MOHAWK; C->chat = V(0.85f, 0.10f, 0.45f); C->chat2 = V(1.0f, 0.60f, 0.80f); C->cjack = BLK; C->top = TOP_TEE; C->ctop = V(0.55f, 0.55f, 0.58f); C->fat = -0.3f;
    C = &CLV[7]; C->leg = 0.6f; C->fat = -0.4f; C->sc = 1.08f; C->hat = HAT_USHANKA; C->chat = BLK; C->chat2 = V(0.45f, 0.42f, 0.40f); C->flags |= F_COAT; C->cjack = V(0.62f, 0.42f, 0.20f); C->snout = 0.4f;
    C = &CLV[8]; C->skin = V(0.60f, 0.29f, 0.21f); C->flags = F_DRESS | F_LASH | F_BOW; C->cdress = V(0.36f, 0.10f, 0.42f); C->cdress2 = V(0.85f, 0.62f, 0.20f); C->pdress = PAT_VSTRIPE; C->hat = HAT_BOWL; C->chat = V(0.05f, 0.04f, 0.04f); C->chat2 = V(0.30f, 0.10f, 0.40f); C->cacc = V(0.85f, 0.62f, 0.20f); C->eye = 0.4f; C->sc = 1.02f;
    C = &CLV[9]; C->skin = V(0.80f, 0.24f, 0.22f); C->fat = 1.0f; C->sc = 1.10f; C->hat = HAT_HEADBAND; C->chat = V(0.90f, 0.86f, 0.80f); C->top = TOP_TEE; C->ctop = V(0.90f, 0.70f, 0.10f); C->cbot = V(0.05f, 0.12f, 0.35f); C->ear = 0.5f;
}
// ================================================================ MARTIN PLAISIR (le Danseur) : une danse souple, 32 temps (16 nov. 2026, essai)
// Le contraire des Ranga Boyz : aucune pose tenue, aucun saut d'une pose à l'autre. Tout est fait de courbes continues. Les hanches mènent (un côté par temps,
// le poids passe d'une jambe à l'autre), le buste répond avec un retard, la tête avec un retard de plus : c'est ce décalage qui fait la vague.
// Les bras suivent cinq phrases fondues l'une dans l'autre sur plus d'un temps.
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
#define CHBIG 2.4f       // la taille des Douzi Ambrées au Bal : d'énormes pintes
// ================================================================ LES RANGA BOYZ SUR LE SQUELETTE DU DANSEUR (essais de morphologie, 17 nov. 2026)
// La même chorégraphie que rb_dancer (rebond, vague, miroir, Ranga Style, pose), écrite pour de vrais bras et de vraies jambes. var 1 : élancés ; var 2 : trapus (gros buste, grosse tête, jambes courtes, longs bras).
// LA BIBLIOTHÈQUE DE CORPS du squelette du Danseur (17 nov. 2026 ; Kenny : « j'aime bien toutes les morpho, on les réutilisera pour d'autres persos »). 4 = armoire : le corps de JOHN PORK.
//                                    bulk   head   leg    arm    sho    shape torso  limb   échelle
static const float DNBODY[8][9] = {{1.50f, 1.38f, 0.80f, 0.92f, 1.22f, 0, 1.00f, 1.00f, 0.92f},       // 0 compact
                                   {1.15f, 1.80f, 0.88f, 0.90f, 0.95f, 0, 0.95f, 0.90f, 0.86f},       // 1 grosse tête
                                   {1.30f, 1.40f, 0.82f, 1.00f, 1.00f, 2, 0.80f, 0.55f, 0.95f},       // 2 boule à tuyaux
                                   {1.40f, 1.30f, 0.78f, 0.95f, 1.05f, 1, 1.00f, 0.95f, 0.95f},       // 3 bedaine
                                   {1.55f, 1.05f, 0.72f, 1.02f, 1.75f, 0, 1.15f, 1.10f, 0.98f},       // 4 armoire (John Pork)
                                   {1.45f, 1.95f, 0.55f, 0.72f, 1.00f, 0, 0.80f, 1.00f, 0.90f},       // 5 mini
                                   {1.35f, 1.30f, 0.70f, 0.95f, 0.78f, 3, 1.00f, 0.95f, 0.98f},       // 6 poire
                                   {1.20f, 1.30f, 0.58f, 1.10f, 0.90f, 0, 1.55f, 0.85f, 0.95f}};      // 7 teckel
static float RBE;       // 0 à 1 : la chorégraphie en plus énergique (rebonds sautés, hanches, bras et tête plus amples)
static void rbd_group(float b, float ox, float oz, float face, int var) {       // var 1 élancés, 2 trapus, 3 idols, 10 + n : le corps n de DNBODY
    static const v3 SK[7] = {{0.30f, 0.16f, 0.10f}, {0.92f, 0.37f, 0.38f}, {0.085f, 0.07f, 0.075f}, {0.95f, 0.66f, 0.64f}, {0.93f, 0.42f, 0.44f}, {0.96f, 0.80f, 0.72f}, {0.82f, 0.50f, 0.50f}};
    static const v3 HC[7][2] = {{{0, 0, 0}, {0, 0, 0}}, {{0.90f, 0.80f, 0.50f}, {1.0f, 0.96f, 0.80f}}, {{0.75f, 0.36f, 0.05f}, {0.95f, 0.62f, 0.15f}}, {{0.02f, 0.45f, 0.48f}, {0.55f, 0.95f, 0.90f}}, {{0.36f, 0.10f, 0.55f}, {0.62f, 0.30f, 0.80f}}, {{0.05f, 0.16f, 0.55f}, {0.35f, 0.65f, 1.0f}}, {{0.50f, 0.04f, 0.09f}, {0.85f, 0.20f, 0.25f}}};
    static const int CT[7] = {DC_NONE, DC_QUIFF, DC_AFRO, DC_MOHAWK, DC_FRINGE, DC_SPIKES, DC_BUN};
    static const v3 RBC[7] = {{0.95f, 0.60f, 0.10f}, {0.95f, 0.14f, 0.42f}, {0.50f, 0.72f, 0.98f}, {0.08f, 0.72f, 0.42f}, {0.52f, 0.22f, 0.95f}, {0.90f, 0.10f, 0.13f}, {0.92f, 0.88f, 0.98f}};
    float bl = fmodf(b, 32.0f); if (bl < 0) bl += 32.0f; int ph = (int)(bl / 8.0f); float fsp = var == 2 ? 1.22f : var >= 10 ? 1.18f : 1.05f, scl = var == 2 ? 1.0f : var >= 10 ? DNBODY[var - 10][8] * 0.92f : 0.80f;
    for (int i = 0; i < 7; i++) { int left = (i & 1) && i > 0; float mg = left ? -1.0f : 1.0f;
        float bc = bl - 0.5f * ((i + 1) / 2) * rb_win(bl, 8.6f, 15.4f), w = PI * bc, s1 = rb_shp(sinf(w)), c2 = cosf(2 * w), fin = sstep(28.0f, 29.6f, bl), calm = 1.0f - 0.7f * fin, wd = PI * (bc - 0.2f), wh = PI * (bc - 0.42f);
        dn_pose_t P; dn_rest(&P); float hx = (0.075f + 0.05f * RBE) * s1 * calm;
        P.pel = V(hx, 1.0f - 0.03f - (0.03f + 0.035f * RBE) * c2 * calm, 0); P.proll = -2.0f * hx; P.pyaw = (0.18f + 0.14f * RBE) * sinf(w - 0.6f) * calm; P.ptilt = 0.06f * sinf(2 * w) * calm;
        P.chest = V(-0.05f * sinf(wd) * calm, 0.36f, 0.03f * sinf(2 * PI * (bc - 0.25f)) * calm); P.croll = (0.14f + 0.10f * RBE) * sinf(wd) * calm; P.cyaw = -(0.22f + 0.14f * RBE) * sinf(wd - 0.6f) * calm;
        P.hroll = -0.12f * sinf(wh) * calm; P.hpitch = (0.06f + 0.08f * RBE) * sinf(2 * w - 1.0f) * calm - 0.04f; P.smile = 0.85f; P.lid = 0.25f + 0.75f * (fract(b * 0.11f + i * 0.37f) < 0.03f); P.tail = b * 9.0f;
        for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f, fr = sstep(0.15f, 0.95f, -sg * s1) * calm; P.foot[s] = V(sg * (0.15f - 0.03f * fr), 0, 0); P.heel[s] = 0.7f * fr; P.kpole[s] = V(sg * (0.28f - 0.40f * fr), 0, 1); }
        v3 H[6][2], E[6][2]; float W[6] = {rb_win(bl, -2.0f, 8.0f), rb_win(bl, 8.0f, 16.0f), rb_win(bl, 16.0f, 24.0f), rb_win(bl, 24.0f, 26.0f), rb_win(bl, 26.0f, 28.0f), rb_win(bl, 28.0f, 40.0f)}, ws = 0;
        float clap = rb_win(bl, 6.3f, 7.7f), jump = sinf(PI * clampf(bl - 6.9f, 0, 1)); jump *= jump * (ph == 0);
        for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f; H[0][s] = mix3(V(sg * 0.36f + (0.22f + 0.14f * RBE) * s1, 1.05f + (0.25f + 0.40f * RBE) * (0.5f - 0.5f * c2), 0.22f + 0.12f * RBE * c2), V(sg * 0.06f, 2.0f, 0.10f), clap); E[0][s] = V(sg, -0.3f, -0.5f); }       // 1 : le rebond
        P.pel.y += 0.16f * jump; P.foot[0].y += 0.14f * jump; P.foot[1].y += 0.14f * jump;
        { float sw2 = sinf(w), hop = 0.11f * RBE * sw2 * sw2 * calm * (1 - W[1]) * clampf(1 - W[3] - W[4], 0, 1); P.pel.y += hop; P.foot[0].y += 0.8f * hop; P.foot[1].y += 0.8f * hop; }       // énergique : un petit saut entre chaque temps
        float u2 = 0.5f - 0.5f * cosf(PI * bc * 0.5f);
        for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f; H[1][s] = mix3(V(sg * 0.22f, 0.50f, 0.30f), V(sg * 0.30f, 2.02f, 0.05f), u2); E[1][s] = V(sg, 0, -0.4f); }       // 2 : la vague
        P.pel.y -= 0.24f * (1 - u2) * W[1]; P.ptilt += 0.20f * (1 - u2) * W[1]; P.cpitch += 0.30f * (1 - u2) * W[1]; P.hpitch += (0.10f - 0.30f * u2) * W[1];
        float r = 0.5f + 0.5f * mg * rb_shp(sinf(w - 0.4f));       // 3 : le miroir
        H[2][0] = mix3(V(0.24f + hx, 0.99f, 0.07f), V(0.55f, 1.95f, 0.05f), r); H[2][1] = mix3(V(-0.24f + hx, 0.99f, 0.07f), V(-0.55f, 1.95f, 0.05f), 1 - r); E[2][0] = V(1, 0, -0.5f); E[2][1] = V(-1, 0, -0.5f);
        float g2 = 0.5f - 0.5f * cosf(4 * w), sg2 = sinf(2 * w), gk = W[3] + W[4];       // 4 : le Ranga Style
        H[3][0] = V(0.07f, 1.10f + 0.06f * g2, 0.36f); H[3][1] = V(-0.07f, 1.16f + 0.06f * g2, 0.36f); E[3][0] = V(1, -0.5f, 0); E[3][1] = V(-1, -0.5f, 0);
        H[4][0] = V(0.25f + 0.15f * cosf(4 * w), 2.0f, 0.05f + 0.15f * sinf(4 * w)); H[4][1] = V(-0.09f, 1.14f + 0.05f * g2, 0.36f); E[4][0] = V(1, 0, -0.3f); E[4][1] = V(-1, -0.5f, 0);
        P.pel.y += ((0.06f + 0.06f * RBE) * g2 - 0.10f) * gk; P.foot[0].y += (0.13f + 0.10f * RBE) * sstep(0.1f, 0.9f, sg2) * gk; P.foot[1].y += (0.13f + 0.10f * RBE) * sstep(0.1f, 0.9f, -sg2) * gk; P.foot[0].x += 0.07f * gk; P.foot[1].x -= 0.07f * gk; P.kpole[0].x += 0.5f * gk; P.kpole[1].x -= 0.5f * gk;
        if (i == 0) { H[5][0] = V(0.42f, 2.0f, 0.03f); H[5][1] = V(-0.24f + hx, 0.99f, 0.07f); E[5][0] = V(1, 0, -0.4f); E[5][1] = V(-1, 0, -0.5f); P.hpitch -= 0.20f * fin; P.hyaw += 0.30f * fin; P.pel.x += 0.06f * fin; P.proll -= 0.14f * fin; }       // la pose
        else { H[5][0] = V(mg * 0.62f, 1.45f, 0.15f); H[5][1] = V(-mg * 0.26f, 0.62f, 0.26f); E[5][0] = V(mg, -0.4f, -0.2f); E[5][1] = V(-mg, 0, -0.5f); if (left) { v3 t = H[5][0]; H[5][0] = H[5][1]; H[5][1] = t; t = E[5][0]; E[5][0] = E[5][1]; E[5][1] = t; }
            P.pel.y -= 0.40f * fin; P.pyaw += mg * 0.35f * fin; P.hyaw += mg * 0.30f * fin; P.hpitch -= 0.15f * fin; P.foot[left ? 0 : 1].z -= 0.42f * fin; P.heel[left ? 0 : 1] += 0.9f * fin; P.foot[left ? 1 : 0].z += 0.16f * fin; }
        for (int k = 0; k < 6; k++) ws += W[k];
        for (int s = 0; s < 2; s++) { v3 hh = V(0, 0, 0), ee = V(0, 0, 0); for (int k = 0; k < 6; k++) { hh = add(hh, mul(H[k][s], W[k] / ws)); ee = add(ee, mul(E[k][s], W[k] / ws)); } P.hand[s] = hh; P.epole[s] = ee; }
        int pn = (ph + 1) & 3; float g = ease(clampf((bl - ph * 8.0f - 6.0f) / 2.0f, 0, 1)), x = mixf(RBF[ph][i][0], RBF[pn][i][0], g) * fsp, z = mixf(RBF[ph][i][1], RBF[pn][i][1], g) * fsp;
        if (g > 0 && g < 1) { float wa = sinf(PI * g), st = sinf(w * 2.0f); P.foot[0].y += 0.07f * wa * fmaxf(st, 0); P.foot[1].y += 0.07f * wa * fmaxf(-st, 0); }
        float yaw = 6.2831853f * ease(clampf((bl - 21.6f) / 2.2f, 0, 1)) * mg * (ph == 2), cf = cosf(face), sf = sinf(face);
        if (var >= 10) { const float *q = DNBODY[var - 10]; DNLOOK = (dn_look_t){SK[i], HC[i][0], HC[i][1], 2, CT[i], q[0], q[1], q[2], q[3], q[4], 0, RBC[i], (int)q[5], q[6], q[7]}; } else
        DNLOOK = var == 3 ? (dn_look_t){SK[i], HC[i][0], HC[i][1], 1, CT[i], 1.30f, 1.16f, 1.10f, 1.0f, 1.62f, 1} : var == 2 ? (dn_look_t){SK[i], HC[i][0], HC[i][1], 1, CT[i], 1.75f, 1.50f, 0.62f, 1.0f, 1.45f} : (dn_look_t){SK[i], HC[i][0], HC[i][1], 2, CT[i], 1.28f, 1.16f, 1, 1, 1, 0, RBC[i]};       // var 1 : les élancés en satin pailleté, la tenue validée pour le clip (17 nov. 2026)
        dn_place(&P, V(ox + x * cf - z * sf, 0, oz - x * sf - z * cf), face + yaw, i ? scl : scl * 1.05f); }
}
static void bal_lights(float k, float warm) {       // la salle : quatre pinceaux de couleur qui se promènent, le néon, la console, le bar, la braise
    WSUN = V(0, 1, 0); WSUNC = V(0, 0, 0); WAMBU = mul(mix3(V(0.075f, 0.07f, 0.11f), V(0.13f, 0.09f, 0.07f), warm), 0.35f + 0.65f * k); WAMBD = mul(V(0.05f, 0.04f, 0.055f), 0.35f + 0.65f * k);
    WNL = 9; for (int i = 0; i < 4; i++) { int ci = (i + (int)floorf(CB * 0.5f)) & 3; float g[8] = {5.5f * sinf(CB * 0.6f + i * 1.7f), 2.6f, -3.5f + 3.0f * i + 2.0f * cosf(CB * 0.46f + i * 2.1f), CPAL[ci].x, CPAL[ci].y, CPAL[ci].z, (1.6f + 2.4f * CBT) * k * (0.4f + 0.6f * CE), 0.10f}; memcpy(WLT[i], g, sizeof(g)); }
    { float g[8] = {0, 3.6f, 10.6f, 1.0f, 0.15f, 0.55f, 3.2f * (0.35f + 0.65f * k), 0.08f}; memcpy(WLT[4], g, sizeof(g)); }
    { float g[8] = {0, STY + 1.5f, 8.0f, 0.2f, 0.8f, 1.0f, 1.1f, 0.5f}; memcpy(WLT[5], g, sizeof(g)); }
    { float g[8] = {6.2f, 2.3f, -2.0f, 1.0f, 0.62f, 0.30f, 2.6f * (0.4f + 0.6f * k), 0.12f}; memcpy(WLT[6], g, sizeof(g)); }
    { float g[8] = {-7.0f, 1.25f, 6.6f, 1.0f, 0.35f, 0.08f, 1.6f + 1.2f * CBT, 0.35f}; memcpy(WLT[7], g, sizeof(g)); }
    { float g[8] = {0, 2.4f, 6.6f, 1.0f, 0.90f, 0.78f, 2.4f * k, 0.06f}; memcpy(WLT[8], g, sizeof(g)); }       // une face chaude, de la salle vers l'estrade
    { float g[8] = {0, 3.4f, -8.6f, 1.0f, 0.92f, 0.84f, 2.2f * k, 0.05f}; memcpy(WLT[9], g, sizeof(g)); WNL = 10; }       // et une autre, de l'entrée : les dos ne sont pas noirs
}
// ---------------------------------------------------------------- les plans. K : 0 à 19 le Bal, 20 à 29 la ville, 30 à 39 l'espace des vitrines
static void clip_setup(int K, float s) {
    static int init; float ts = 0, th = 0, pat = 0, var = 0, mult = 1, s0 = 0; CD = 8; CE = 0.6f;
    { const char *e = getenv("CLIP"); if (e) sscanf(e, "%f %f %f %f %f %f %f %f", &ts, &CD, &CE, &th, &pat, &var, &mult, &s0); }
    { float s1 = (s - 1.0f) * mult; T += s1 - s; s = s1; }       // le plan commence à 1 s ; mult : 2 pour 25 images par seconde, 4 pour 12,5
    CK = K; CTS = ts + s; CB = (CTS - 0.191f) / 0.6602f; CBT = expf(-4.0f * fract(CB)); CBO = expf(-6.0f * fract(CB + 0.5f)); CTH = (int)th & 3; CPAT = (int)pat; CVAR = (int)var;
    if (!init) { init = 1; cl_people(); rb_people(); cv_people(); cl_hair(); }
    float sl = s + s0, u = clampf(sl / CD, 0, 1), eu = ease(u), j = fabsf(sinf(CB * PI));       // s0 : on reprend un mouvement de caméra en route
    CSET = (K < 20 || K == 42 || K == 45) ? 0 : (K < 30 || K == 46) ? 1 : 2; CDJ = -1; CHV = 0; CPIGS = 1; CCR = 2.6f; DNON = 0; DNN = 0; DNLOOK = DN_MARTIN; CCPL = 0; CCHEN = 0; CGIANT = 0; CTRON = 0; CMBON = 0; CDENS = 0.78f; CHEART = 0; CSPOTK = 1; CSPOTL = 0; CDAWN = 0; CVU = 0; CSCR = 0; CLITK = 0.22f; CBEAMK = CE; CHSUN = 1;
    CPOST = 0; RBE = 0; WCONF = (CVAR & 8) ? 0.9f : 0; CGLY = 0.010f * CBT; CJA = 0.085f * sstep(0.72f, 0.95f, CE); { float sj = sinf(PI * CB); CJ = CJA * sj * sj; }       // le saut de la foule : une sinusoïde, plus un coup sec CBALLC = V(0, 5.0f, 0); CDA = CTS * 0.35f; CGA = CTS * 0.8f; NPL = 0; APERT = 0; WSNOW = 0; EXPO = 1.0f; WMTN = 1.55f; STAR = 0.5f;
    for (int k = 0; k < 3; k++) { float a = CTS * (0.5f + 0.13f * k) + 2.1f * k; CSPH[k] = V(8.5f * cosf(a), 1.6f + 0.9f * k + 0.5f * fabsf(sinf(CB * PI * 0.5f + k)), 8.5f * sinf(a)); }
    if (CSET == 0) {       // ================= le Bal des Neiges
        WFAR = 44.0f; cl_crowd(0.35f + 0.65f * CE); bal_lights(1.0f, 0.0f);
        if (K == 1) CSCR = 0.9f * sinf(CB * PI * 2.0f) * sstep(0.2f, 0.6f, sinf(CB * PI * 0.25f)) * ((CVAR & 1) != 0);
        cl_dj(K == 2 ? ((CVAR & 1) ? sstep(0.0f, 0.3f, j) : 0) : K == 18 ? 1.0f + sstep(0.2f, 0.5f, j) : 0);
        ch_put(V(-6.16f, 0.766f + CGLY, -3.86f), CHBIG, 2.2f, 0, 1.0f, 0.85f, 0.5f, 0, 1, 1);       // la chope standard, en énormes pintes (2,4 : Kenny, 16 nov. 2026) : une sur la table, trois sur le bar
        ch_put(V(6.72f, 1.141f, -0.55f), CHBIG, 0.6f, 0, 1.0f, 0.7f, 0.4f, 0, 0, 1); ch_put(V(6.70f, 1.141f, -3.1f), CHBIG, 4.0f, 0, 0.45f, 0.3f, 0.3f, 1, 0, 1); ch_put(V(6.76f, 1.141f, -4.6f), CHBIG, 1.4f, 0, 1.0f, 1.0f, 0.5f, 0, 1, 1);
        switch (K) {
        case 1: cam(V(2.3f - 4.6f * eu, STY + 1.50f, 6.85f + 0.35f * sinf(u * PI)), V(0, STY + 1.22f, 9.3f), 1.45f); FOCUS = 2.7f; APERT = 0.006f; break;      // DJ Viteau aux platines
        case 2: cam(V(1.5f - 0.8f * u, STY + 2.45f + 0.2f * u, 11.2f), V(-0.2f, 0.7f, 0.0f), 1.25f); FOCUS = 9; APERT = 0.004f; break;      // par-dessus son épaule : la piste saute
        case 3: { float a = 0.8f + 1.4f * u; CBALLC = V(0, 40.0f, 0); cam(V(2.6f * sinf(a), 5.35f, -0.2f + 2.6f * cosf(a)), V(0, 0, -0.2f), 0.95f); break; }      // la piste, vue du plafond : elle tourne
        case 4: cam(V(-3.6f + 7.2f * u, 0.20f, -6.5f), V(-1.6f + 3.2f * u, 0.80f, 1.5f), 1.25f); FOCUS = 5; APERT = 0.01f; break;      // au ras du damier
        case 5: case 6: case 15: case 17: { float ang = CB * 0.10f + 0.6f; CCPL = 1;      // le couple
            if (K == 15) { CDENS = 0; CSPOTK = 0.12f; CSPOTL = 1; CBEAMK = 0; bal_lights(0.10f, 0); }
            if (K == 17) { CDENS = 0.16f; CSPOTL = 0.6f; CBEAMK = 0.25f; bal_lights(0.55f, 1.0f); cl_crowd(0.25f); }
            cl_couple(0, 0.5f, ang, K == 15 ? 0.6f : 1.0f, K == 6 ? 1.0f : 0.3f);
            if (K == 5) { float a = ang - 0.45f + 0.9f * u; cam(V(2.25f * sinf(a), 1.18f, 0.5f - 2.25f * cosf(a)), V(0, 0.90f, 0.5f), 1.25f); FOCUS = 2.25f; APERT = 0.012f; }
            else if (K == 6) { float c = cosf(ang), sn = sinf(ang), dr = (CVAR & 1 ? -1.0f : 1.0f) * (0.25f - 0.5f * u); v3 eye = V(-1.55f * sn + dr * c, 1.16f, 0.5f + 1.55f * c + dr * sn), hd = V(0, 1.07f, 0.5f); cam(eye, hd, 2.2f); FOCUS = 1.55f; APERT = 0.02f; }      // tes yeux
            else if (K == 15) { float a = ang - 0.35f + 0.7f * u; cam(V(5.6f * sinf(a), 2.3f - 0.5f * u, 0.5f - 5.6f * cosf(a)), V(0, 1.0f, 0.5f), 1.5f); }      // breakdown : seuls sous la poursuite
            else cam(V(-7.4f + 0.8f * u, 4.5f, -8.4f), V(1.0f, 0.5f, 2.2f), 1.15f);      // la salle se vide
            break; }
        case 7: CHEART = u * 1.25f; cam(V(-5.2f - 0.9f * eu, 2.75f, -0.4f + 0.7f * eu), V(-9.0f, 3.2f, 1.0f), 1.75f); break;      // la buée sur les carreaux
        case 8: { pose_t *P = cl_new(4, 0.6f); cz_cheers(P, 0.5f + 0.5f * sstep(0.2f, 0.5f, u)); P->hand[0].y += 0.25f * sstep(0.25f, 0.6f, u); P->hyaw = -0.5f; if (CVAR & 4) { P->hyaw = 0.15f; P->hand[0] = V(0.34f, 0.74f + 0.03f * sinf(CB * PI), 0.30f); P->open = 0.5f; cl_put(5.2f, 0, -3.0f, -2.46f); } else cl_put(5.55f, 0, -1.3f, 0.75f);       // variante 4 : il trinque, face à nous
            { int k = WN - 1; const rig_t *R = &WRG[k]; float sc = WCZ[k].sc, cy = WPC[k], sy = WPS[k]; v3 hl = add(R->ot, add(add(mul(R->tx, R->ha[0].x), mul(R->ty, R->ha[0].y)), mul(R->tz, R->ha[0].z + 0.03f)));       // sa main, dans le monde
                v3 hw = V(WPX[k] + sc * (hl.x * cy + hl.z * sy), WPY[k] + sc * hl.y, WPZ[k] + sc * (-hl.x * sy + hl.z * cy)); float ym = atan2f(cy, -sy), tl = -0.16f - 0.10f * sstep(0.25f, 0.6f, u);
                ch_put(V(0, 0, 0), CHBIG, ym, tl, 0.9f, 0.6f, 0.4f, 0, 0.6f, 0); chi_t *I = &CHI[NCH - 1]; I->pos = sub(hw, mul(add(mul(I->ex, 0.090f), mul(I->ey, 0.083f)), CHBIG)); if (getenv("DJDBG")) fprintf(stderr, "main %.2f %.2f %.2f  chope %.2f %.2f %.2f  NCH %d  ha %.2f %.2f %.2f\n", hw.x, hw.y, hw.z, I->pos.x, I->pos.y, I->pos.z, NCH, R->ha[0].x, R->ha[0].y, R->ha[0].z); }      // une autre tournée
            P = cl_new(3, 0.9f); P->hand[1] = V(-0.30f, 0.50f, 0.42f); P->hand[0] = V(0.32f, 0.46f, 0.36f + 0.1f * sinf(CB * PI)); P->smile = 0.8f;
            if (CVAR & 2) { P->lean = 0.75f; P->hpitch = 0.5f; P->hroll = 0.35f; P->blink = 1; P->smile = 0.3f; P->hand[0] = V(0.30f, 0.44f, 0.50f); P->hand[1] = V(-0.30f, 0.44f, 0.50f); P->bob = 0.012f * sinf(CTS * 1.5f); }
            cl_put(7.85f, 0.22f, -1.9f, -PI * 0.5f);
            if (CVAR & 4) { v3 e = V(4.05f - 0.1f * u, 1.22f, -4.35f), tg = V(5.02f, 1.02f, -3.22f); cam(e, tg, 2.5f); FOCUS = len(sub(e, tg)); APERT = 0.008f; }
            else { cam(V(3.5f + 0.5f * u, 1.45f, -5.0f + 1.2f * u), V(7.0f, 1.22f, -1.6f), 1.55f); FOCUS = 4.6f; APERT = 0.008f; } break; }
        case 9: { CMBON = 1; float tt = CTS * 1.4f;      // les gobelins jouent aux billes, près du bar
            for (int i = 0; i < 6; i++) { float a = i * 1.047f + 0.3f * sinf(tt * 0.3f + i), r = i == 0 ? 0.15f + 0.75f * fabsf(sinf(tt * 0.5f)) : 0.30f + 0.11f * i; CMB[i] = V(4.4f + r * cosf(a + (i == 0 ? tt * 0.2f : 0)), 0.05f, -7.4f + r * sinf(a + (i == 0 ? tt * 0.2f : 0))); }
            for (int i = 0; i < 3; i++) { float a = 0.3f + i * 2.09f; pose_t *P = cl_new(5 + i, 0.3f * i + 0.1f); P->lean = 0.35f; P->hpitch = 0.35f; P->looky = -0.5f; P->hand[i & 1] = V((i & 1 ? -0.20f : 0.20f), 0.10f, 0.42f + 0.06f * sinf(CB * PI + i)); if (i == 1 && !(CVAR & 2)) cl_dance(P, 6.2832f * CB, 5, 0.9f);
                if (CVAR & 2) { P->lean = 0.55f; P->hpitch = 0.6f; P->hroll = 0.3f * (i - 1); P->blink = 1; P->smile = 0.4f; P->bob = -0.03f + 0.012f * sinf(CTS * 1.6f + i); }
                cl_put(4.4f + 1.15f * cosf(a), 0, -7.4f + 1.15f * sinf(a), atan2f(-cosf(a), -sinf(a))); }
            { float g[8] = {4.0f, 1.3f, -8.3f, 1.0f, 0.80f, 0.55f, 2.6f, 0.25f}; memcpy(WLT[WNL++], g, sizeof(g)); }
            if (CVAR & 1) { cam(V(3.6f - 0.3f * u, 0.30f, -8.5f), V(4.4f, 0.12f, -7.4f), 1.9f); FOCUS = 1.4f; APERT = 0.02f; } else { cam(V(3.0f + 0.5f * u, 0.50f, -9.5f + 0.2f * u), V(4.4f, 0.38f, -7.4f), 1.6f); FOCUS = 2.5f; APERT = 0.012f; } break; }
        case 10: { v3 e = V(-5.50f - 0.08f * u, 0.97f + 0.02f * u, -4.50f + 0.20f * u), tg = V(-6.14f, 0.905f, -3.86f); cam(e, tg, 2.5f); FOCUS = len(sub(e, tg)); APERT = 0.02f; break; }      // un verre posé sur la table
        case 11: { pose_t *P = cl_new(8, 0.4f); P->hand[0] = V(0.22f, 0.50f + 0.05f * j, 0.42f); P->lean = 0.12f; P->looky = -0.4f; P->smile = 0.9f; cl_put(-7.0f, 0, 7.5f, PI);      // du porc qui grille
            NPL = 1; { float g[9] = {-7.0f, 2.0f, 6.6f, 0.45f, 1.1f, 0.5f, 0.42f, 0.36f, 0.33f}; memcpy(PLM[0], g, sizeof(g)); }
            cam(V(-4.9f + 0.4f * u, 1.38f, 5.0f + 0.2f * u), V(-7.0f, 0.92f, 6.7f), 1.7f); FOCUS = 2.6f; APERT = 0.012f; break; }
        case 12: CCHEN = 1; CCHX = -4.0f + 2.4f * sl; { float g[8] = {0.5f, 1.6f, -9.7f, 1.0f, 0.88f, 0.72f, 3.2f, 0.10f}; memcpy(WLT[WNL++], g, sizeof(g)); } cam(V(-1.2f + 2.4f * u, 0.50f, -9.6f), V(0.3f, 0.62f, -7.7f), 1.25f); FOCUS = 2.2f; APERT = 0.008f; break;      // la chenille des porcs de marche
        case 13: { float a = 0.6f + 1.2f * u; cam(V(2.0f * cosf(a), 4.25f, 2.0f * sinf(a)), V(0, 4.95f, 0), 1.6f); break; }      // la boule à facettes
        case 14: cam(V(-2.3f + 0.5f * u, STY + 0.45f, 7.3f), V(-3.7f, STY + 1.45f, 10.2f), 1.5f); break;      // les enceintes
        case 16: cam(V(-5.2f, 1.95f, -1.5f + 2.5f * u), V(1.5f, 1.25f, 1.6f), 1.3f); FOCUS = 5.5f; APERT = 0.008f; break;      // les bras en l'air
        case 18: cam(V(1.0f - 2.0f * u, 1.20f, 7.0f - 0.2f * u), V(0, STY + 1.50f, 9.38f), 1.45f); break;      // DJ Viteau, les bras levés, en contre-plongée
        case 45: { dn_pose_t P; float yw, v = CVAR, b = CB - (v < 2 ? 224.0f : 320.0f), ub = clampf(fract(b / 8.0f), 0, 1), eb2 = ease(ub); const float DZ = 0.5f;       // MARTIN PLAISIR au Bal
            CCPL = 1; if (v < 2) { CCR = 2.2f; CDENS = 0.16f; CSPOTL = 0.6f; CBEAMK = 0.25f; bal_lights(0.55f, 1.0f); cl_crowd(0.25f); } else CCR = 3.9f;       // au breakdown : la salle presque vide, la poursuite ; ensuite la piste s'écarte
            ml_dance(b, &P, &yw); dn_place(&P, V(0, 0, DZ), PI + yw, 1.0f);
            if (v == 0) cam(V(0.5f - 1.0f * ub, 0.50f, -3.1f + 0.9f * eb2), V(0, 1.10f, DZ), 2.4f);                                                   // on s'approche, au ras des dalles
            else if (v == 1) { float a = -0.75f + 1.5f * ub; cam(V(3.3f * sinf(a), 1.50f, DZ - 3.3f * cosf(a)), V(0, 1.28f, DZ), 2.3f); }           // on tourne autour de lui
            else if (v == 2) cam(V(-1.2f + 0.8f * ub, 2.8f - 1.6f * eb2, -2.9f), V(0, 1.15f, DZ), 2.2f);                                             // la grue descend sur lui
            else if (v == 3) { float a = 0.8f - 1.6f * ub; cam(V(2.8f * sinf(a), 1.20f, DZ - 2.8f * cosf(a)), V(0, 1.30f, DZ), 2.0f); }             // on tourne dans l'autre sens, plus près
            else if (v == 4) cam(V(-0.8f + 1.6f * ub, 0.90f, -3.0f), V(0, 1.06f, DZ), 2.3f);                                                        // en pied : le tour sur lui-même
            else { float z = sstep(0.45f, 1.0f, ub); cam(V(0.20f, 1.30f + 0.25f * z, -3.0f), V(0, mixf(1.10f, 1.66f, z), DZ - 0.33f * clampf(b - 24.0f, 0, 4)), mixf(2.1f, 3.4f, z * z)); }       // il vient vers nous
            FOCUS = 3.6f; break; }
        case 42: { CCPL = 1; CCR = 3.9f; rb_group(CB, 0, 0.6f, PI); int v = CVAR;       // la piste s'ouvre : les Ranga Boyz au milieu du Bal, DJ Viteau derrière eux, la foule en cercle
            if (v == 0) cam(V(0.6f - 1.2f * u, 0.45f + 0.4f * u, -3.2f + 0.8f * eu), V(0, 0.95f, 1.4f), 1.05f);                       // de face, bas, on avance
            else if (v == 1) { float a = -0.6f + 1.2f * u; cam(V(6.0f * sinf(a), 4.6f, 1.2f - 6.0f * cosf(a)), V(0, 0.5f, 1.6f), 1.5f); }      // à la grue, en arc
            else if (v == 2) cam(V(-1.9f + 3.8f * eu, 0.38f, -2.2f), V(0, 0.95f, 1.0f), 1.1f);       // (reste dans la clairière : à ±3,4 la caméra entrait dans un spectateur)                                         // travelling de côté, en contre-plongée
            else if (v == 3) cam(V(0.9f - 1.8f * u, 1.05f, -1.9f + 0.4f * u), V(0, 0.98f, 0.6f), 2.0f);                                 // serré sur Kevin
            else cam(V(0.01f, 4.35f, 1.6f), V(0, 0, 1.9f), 1.2f);                                                                     // à la verticale : les formations sur les dalles
            FOCUS = 4.5f; break; }
        case 19: cam(V(-1.6f + 3.2f * u, 0.36f, -6.8f), V(-1.2f + 2.4f * u, 0.10f, -5.0f), 2.3f); FOCUS = 1.9f; APERT = 0.012f; break;      // les sabots sur les dalles
        default: cam(V(0.7f * sinf(u * 2.6f), 2.5f - 0.7f * u, -9.4f + 5.0f * eu), V(0, 1.55f, 8.5f), 1.15f); FOCUS = 9; APERT = 0.004f; break;      // 0 : le Bal, en entier
        }
        return; }
    if (CSET == 1) {       // ================= Douzi City sous la neige
        WFAR = 320.0f; WSNOW = 1; WSUN = nrm(V(-0.3f, 0.5f, 0.8f)); WSUNC = V(0, 0, 0); WAMBU = V(0.11f, 0.14f, 0.26f); WAMBD = V(0.05f, 0.06f, 0.10f); WNL = 0; CBEAMK = CE; CSUND = nrm(V(0.0f, 0.085f, 1.0f));
        switch (K) {
        case 21: { pose_t *P = cl_new(9, 0.2f); float w = CTS * 6.0f; cz_walk(P, w, 1.0f); cl_put(4.3f, 0, -19.0f + 1.3f * sl, 0); cam(V(-4.3f, 1.6f + 0.5f * u, -20.5f + 4.5f * eu), V(6.5f, 3.9f, -6.5f), 1.22f); break; }      // la façade : l'enseigne, la porte, quelqu'un se dépêche
        case 22: cam(V(-32.0f + 13.0f * u, 21.5f - 1.5f * u, -34.0f + 10.0f * u), V(12.0f, 9.0f, 4.0f), 1.3f); break;      // les lumières glissent sur les toits
        case 23: if (CVAR == 3) cam(V(4.5f - 2.0f * u, 11.0f, -6.0f + 9.0f * u), V(14.0f, 8.9f, 3.0f + 2.0f * u), 1.5f);                                  // sur le toit : les projecteurs
            else if (CVAR == 1) cam(V(2.6f, 1.75f, -6.0f + 6.0f * u), V(6.5f, 2.7f, -3.0f), 1.7f);                                  // la fenêtre où l'on danse
            else if (CVAR == 2) cam(V(1.2f, 1.5f, -9.6f + 1.2f * u), V(6.5f, 2.8f, -7.0f), 1.6f);                                                  // l'affiche DJ VITEAU
            else cam(V(-4.6f + 1.2f * u, 0.55f + 3.2f * eu, -23.0f + 4.0f * u), V(8.5f, 5.2f + 1.8f * eu, -9.5f), 1.15f);                          // le Bal, de l'autre trottoir : la grue monte le long de la tour
            break;
        case 24: CTRON = !(CVAR & 1); CTRX = -24.0f + 13.0f * sl; CBEAMK = 0; cam(V(-2.2f, 1.9f, 33.0f + 3.0f * u), V(4.0f + 14.0f * u, 9.8f, 66.0f), 1.5f); break;      // le dernier train vient de partir
        case 25: CLITK = 0.035f; CBEAMK = 0; cam(V(-44.0f + 7.0f * u, 58.0f, -46.0f), V(7.0f, 0, 2.0f), 1.55f); break;      // Douzi City dort dehors
        case 26: { CDAWN = sstep(-0.1f, 0.7f, u); WSNOW = 0.5f; CBEAMK = 0; CLITK = 0.06f; WSUN = CSUND; WSUNC = mul(V(1.0f, 0.55f, 0.32f), 1.6f * CDAWN); WAMBU = mix3(WAMBU, V(0.30f, 0.26f, 0.34f), CDAWN); WAMBD = mix3(WAMBD, V(0.16f, 0.10f, 0.09f), CDAWN);      // le matin : on repart
            if (!(CVAR & 1)) for (int i = 0; i < 2; i++) { pose_t *P = cl_new(1 + i, 0.3f + 0.4f * i); float w = CTS * 5.2f + i * 0.4f; cz_walk(P, w, 0.9f); P->hand[i] = V((i ? -1.0f : 1.0f) * 0.36f, 0.10f, 0.06f); cl_put(i ? 0.36f : -0.36f, 0, -6.0f + 0.75f * sl, 0); }
            cam(V(0.9f - 0.5f * u, 1.45f + 0.3f * u, -13.5f + 1.0f * u), V(0, 2.9f, 30.0f), 1.45f); break; }
        case 27: CVU = 1; CBEAMK = 1; cam(V(-1.5f + 3.0f * u, 10.5f + 2.0f * u, -46.0f + 4.0f * u), V(0.0f, 8.0f, 30.0f), 1.15f); break;      // la ville en égaliseur
        case 46: { dn_pose_t P; float yw, b = CVAR == 1 ? CB - 380.0f + 16.0f : CVAR == 2 ? CB - 388.0f + 28.0f : CB - 280.0f + 8.0f; ml_dance(b, &P, &yw); dn_place(&P, V(0, 0, 6.0f), PI + yw, 1.0f);       // MARTIN PLAISIR dans la rue, les porcs de marche qui passent de chaque côté
            if (CVAR == 1) cam(V(-1.0f + 2.0f * u, 0.75f, 1.4f), V(0, 1.10f, 6.0f), 2.2f);                                                // au sommet final : les hanches, un tour sur lui-même
            else if (CVAR == 2) cam(V(0.20f, 1.52f, 1.2f), V(0, 1.64f, 4.68f), mixf(2.6f, 3.6f, eu));                                    // son regard, le clin d'œil
            else cam(V(0.5f - 1.0f * u, 0.60f + 0.3f * u, 1.8f + 1.0f * eu), V(0, 1.15f, 6.0f), 2.3f);                                   // au pont : les bras en serpent
            FOCUS = 4.5f; break; }
        case 29: { CPIGS = 0; rb_group(CB + 4.0f * CPAT, 0, 6.0f, PI); int v = CVAR;       // motif = décalage de la chorégraphie, par mesures int v = CVAR;       // les Ranga Boyz dansent dans la rue, sous les guirlandes
            if (v == 0) cam(V(0.6f - 1.2f * u, 0.55f + 0.5f * u, -1.2f + 1.6f * eu), V(0, 0.95f, 7.0f), 1.55f);                       // de face, au ras de la neige, on avance
            else if (v == 1) cam(V(-3.5f + 7.0f * u, 6.5f - 1.5f * u, -2.5f + 2.0f * u), V(0, 0.4f, 7.4f), 1.6f);                        // à la grue, pour les formations
            else if (v == 2) cam(V(-5.0f + 10.0f * eu, 0.45f, 3.6f), V(0, 0.95f, 7.0f), 1.35f);                                       // un travelling de côté, en contre-plongée
            else if (v == 4) { float a = -0.9f + 1.8f * u; cam(V(5.6f * sinf(a), 1.7f - 0.6f * u, 7.0f - 6.2f * cosf(a)), V(0, 0.9f, 7.2f), 1.4f); }      // on tourne autour d'eux
            else if (v == 5) cam(V(0.4f - 0.8f * u, 7.4f, 4.6f + 0.8f * u), V(0, 0, 7.6f), 1.55f);                                     // presque à la verticale
            else cam(V(1.3f - 2.6f * u, 1.05f, 3.6f + 0.6f * u), V(0, 0.95f, 6.0f), 2.3f);                                            // serré sur Kevin
            FOCUS = 7.5f; break; }
        case 28: cam(V(-4.4f + 0.4f * u, 1.25f, -6.1f + 0.4f * u), V(-3.2f, 1.42f, -1.5f), 1.65f); FOCUS = 4.6f; APERT = 0.012f; break;      // le bonhomme de neige
        default: if (CVAR & 1) { CDAWN = 0.8f + 0.2f * u; WSNOW = 0.5f; CBEAMK = 0; CLITK = 0.08f; WSUN = CSUND; WSUNC = mul(V(1.0f, 0.55f, 0.32f), 1.4f * CDAWN); WAMBU = mix3(WAMBU, V(0.30f, 0.26f, 0.34f), CDAWN); WAMBD = mix3(WAMBD, V(0.16f, 0.10f, 0.09f), CDAWN); eu = 1 - eu; }      // 20 : la ville, de haut ; on descend vers le Bal (variante 1 : au matin, on remonte)
            cam(mix3(V(-30.0f, 40.0f, -74.0f), V(-3.0f, 12.5f, -40.0f), eu), V(8.0f, 4.0f, -2.0f), 1.3f); break;
        }
        return; }
    // ================= l'espace des vitrines
    WFAR = 190.0f; WSUN = nrm(V(-0.4f, 0.8f, -0.45f)); WSUNC = V(0.85f, 0.80f, 0.90f); WAMBU = V(0.16f, 0.14f, 0.22f); WAMBD = V(0.10f, 0.07f, 0.09f); WNL = 0; CSUND = nrm(V(0.0f, 0.16f, 1.0f)); WSNOW = (CVAR & 2) ? 0.7f : 0;
    cl_crowd(0.4f + 0.6f * CE); CNRING = 10; CBALLC = V(0, 4.6f, 0);
    switch (K) {
    case 38: { float a = 0.5f + 1.1f * u; CGIANT = 2; ch_put(V(0, 0.836f, 0), 24.0f, 0.3f * CTS, 0, 1.0f, 1.0f, 0.4f, 0, 1, 1); cam(V(8.8f * sinf(a), 1.0f + 0.6f * u, -8.8f * cosf(a)), V(0, 2.9f, 0), 1.25f); break; }      // la chope géante, comme un monument
    case 39: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // la fiche du modèle : le studio
        static const float ST[4][5] = {{1.0f, 1.0f, 0.5f, 0, 1}, {0.62f, 0.32f, 0.35f, 1, 0.7f}, {0.28f, 0.16f, 0.2f, 1, 0.7f}, {0, 0, 0, 0.8f, 0}}; const float *q = ST[CPAT & 3]; int v = CVAR;
        ch_put(V(0, 0.04f, 0), 10.0f, v < 4 ? v * 1.5707963f : v == 4 ? 0.6f : -0.5f, 0, q[0], q[1], q[2], q[3], q[4], 1);
        if (v == 4) cam(V(-2.7f, 3.3f, -4.3f), V(0.05f, 0.95f, 0), 3.3f); else if (v == 5) cam(V(1.3f, 0.32f, -5.0f), V(0.12f, 1.02f, 0), 3.1f); else cam(V(0, 1.22f, -5.2f), V(0.10f, 0.94f, 0), 3.4f);
        break; }
    case 43: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // le Danseur au studio : sa planche
        dn_pose_t P; dn_rest(&P); int v = CVAR, q = CPAT;
        if (q == 1) { P.pel = V(0.07f, 0.955f, 0); P.proll = -0.17f; P.chest = V(-0.075f, 0.355f, 0.0f); P.croll = 0.14f; P.cyaw = -0.15f; P.hroll = -0.10f; P.hyaw = 0.30f; P.lid = 0.55f;       // le déhanché : une main sur la hanche
            P.hand[0] = V(0.235f, 0.985f, 0.05f); P.epole[0] = V(1, 0, -0.5f); P.hand[1] = V(-0.33f, 0.80f, 0.06f); P.foot[0] = V(0.10f, 0, -0.02f); P.foot[1] = V(-0.17f, 0, 0.12f); P.fyaw[1] = -0.6f; P.heel[1] = 0.6f; P.kpole[1] = V(-0.5f, 0, 1); }
        if (q == 2) { P.pel = V(-0.08f, 0.94f, 0); P.proll = 0.20f; P.pyaw = 0.25f; P.chest = V(0.09f, 0.35f, 0.02f); P.croll = -0.16f; P.cyaw = -0.45f; P.hyaw = 0.35f; P.hpitch = -0.15f; P.lid = 0.5f; P.smile = 1;       // un bras au ciel, l'autre sur la hanche
            P.hand[1] = V(-0.42f, 1.98f, 0.02f); P.epole[1] = V(-1, 0, -0.4f); P.hand[0] = V(0.21f, 0.97f, 0.08f); P.epole[0] = V(1, 0, -0.5f); P.foot[0] = V(0.20f, 0, 0.05f); P.foot[1] = V(-0.12f, 0, -0.03f); P.fyaw[0] = 0.7f; P.heel[0] = 0.8f; P.kpole[0] = V(0.7f, 0, 1); }
        dn_place(&P, V(0, 0, 0), PI + (v == 1 ? 0.75f : v == 2 ? 1.5708f : v == 3 ? PI : 0), 1.0f);
        if (v == 4) cam(V(0.25f, 1.62f, -1.25f), V(0, 1.60f, 0), 2.2f); else cam(V(0, 1.05f, -4.6f), V(0, 1.02f, 0), 3.6f);
        break; }
    case 44: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // Martin Plaisir danse, au studio (mesures 80 à 87)
        dn_pose_t P; float yw, b = CB - 320.0f, ub = clampf(fract(b / 8.0f), 0, 1), eb = ease(ub); ml_dance(b, &P, &yw); dn_place(&P, V(0, 0, 0), PI + yw, 1.0f);
        if (CVAR == 9) cam(V(0, 1.05f, -5.4f), V(0, 1.02f, -0.6f), 2.3f);
        else if (b < 8.0f) cam(V(0.5f - 1.0f * ub, 0.50f, -5.3f + 1.3f * eb), V(0, 1.10f, 0), 3.3f);                                              // on s'approche, au ras du sol
        else if (b < 16.0f) { float a = -0.75f + 1.5f * ub; cam(V(3.3f * sinf(a), 1.50f, -3.3f * cosf(a)), V(0, 1.28f, 0), 2.3f); }            // on tourne autour de lui, à mi-corps
        else if (b < 24.0f) cam(V(-0.9f + 1.8f * ub, 0.95f, -5.6f), V(0, 1.06f, 0), 3.1f);                                                    // en pied : le tour sur lui-même
        else { float z = sstep(0.45f, 1.0f, ub); cam(V(0.20f, 1.30f + 0.25f * z, -4.7f), V(0, mixf(1.10f, 1.66f, z), -0.33f * clampf(b - 24.0f, 0, 4)), mixf(2.9f, 5.2f, z * z)); }       // il vient vers nous ; on finit sur son regard
        break; }
    case 47: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // LA DÉMO des Ranga Boyz, au studio : la chorégraphie entière (le refrain 1)
        float b = CB - 96.0f, ub = clampf(fract(b / 8.0f), 0, 1), eb2 = ease(ub); rb_group(b, 0, 0, PI); WCONF = 0.9f * sstep(27.6f, 28.6f, b);       // des confettis pour la pose finale
        if (CVAR == 9) cam(V(1.2f - 0.6f * u, 1.25f, -8.0f + 0.8f * eu), V(0, 0.68f, 0.9f), 3.1f);                                                                             // caméra fixe, comme une vidéo de répétition
        else if (b < 8.0f) cam(V(0.4f - 0.8f * ub, 1.25f, -8.4f + 1.0f * eb2), V(0, 0.72f, 1.0f), 3.2f);                                           // 1 : de face, large, on avance un peu
        else if (b < 16.0f) cam(V(-4.2f + 8.4f * eb2, 0.95f, -6.6f), V(0, 0.70f, 0.6f), 2.5f);                                                   // 2 : on longe la ligne : la vague passe
        else if (b < 24.0f) cam(V(0.6f - 1.2f * ub, 2.6f, -6.8f), V(0, 0.55f, 0.8f), 3.3f);                                                      // 3 : un peu de haut, pour voir les deux rangs et le tour
        else if (b < 28.0f) cam(V(-0.5f + 1.0f * ub, 0.42f, -4.6f), V(0, 0.78f, 0.4f), 1.9f);                                                    // 4 : le Ranga Style, au ras du sol
        else cam(V(0.3f, 0.60f + 1.2f * eb2, -5.6f + 1.2f * eb2), V(0, 0.80f, 0.9f), mixf(2.1f, 2.7f, eb2));                                     // la pose finale : la grue monte
        break; }
    case 48: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // ESSAI : les Ranga Boyz dans une autre morphologie (le squelette du Danseur, plus trapu que Martin Plaisir)
        static const v3 SK[7] = {{0.30f, 0.16f, 0.10f}, {0.92f, 0.37f, 0.38f}, {0.085f, 0.07f, 0.075f}, {0.95f, 0.66f, 0.64f}, {0.93f, 0.42f, 0.44f}, {0.96f, 0.80f, 0.72f}, {0.82f, 0.50f, 0.50f}};
        static const v3 HC[7][2] = {{{0, 0, 0}, {0, 0, 0}}, {{0.90f, 0.80f, 0.50f}, {1.0f, 0.96f, 0.80f}}, {{0.75f, 0.36f, 0.05f}, {0.95f, 0.62f, 0.15f}}, {{0.02f, 0.45f, 0.48f}, {0.55f, 0.95f, 0.90f}}, {{0.36f, 0.10f, 0.55f}, {0.62f, 0.30f, 0.80f}}, {{0.05f, 0.16f, 0.55f}, {0.35f, 0.65f, 1.0f}}, {{0.50f, 0.04f, 0.09f}, {0.85f, 0.20f, 0.25f}}};
        static const int CT[7] = {DC_NONE, DC_QUIFF, DC_AFRO, DC_MOHAWK, DC_FRINGE, DC_SPIKES, DC_BUN}, O[7] = {5, 3, 1, 0, 2, 4, 6};
        if (CVAR == 12) {       // les baskets : six paires, de trois quarts (motif 0), de profil (1), de face (2)
            for (int i = 0; i < 6; i++) { cz_t *C = &WCZ[WN]; *C = CZC[12 + (i * 2 + 1) % 12]; C->flags |= F_SHOES; { static const v3 SH[6] = {{0.93f, 0.92f, 0.89f}, {0.05f, 0.05f, 0.06f}, {0.85f, 0.12f, 0.15f}, {0.10f, 0.35f, 0.85f}, {0.95f, 0.75f, 0.10f}, {0.92f, 0.30f, 0.60f}}; C->cshoe = SH[i]; }
                pose_t *P = &WPP[WN]; cz_rest(C, P); if (i & 1) { P->foot[0].z += 0.10f; P->footp[0] = -0.25f; }
                w_place(WN, -2.25f + 0.9f * i, 0, 0, PI + (CPAT % 3 == 0 ? 0.6f : CPAT % 3 == 1 ? 1.5708f : 0)); WN++; }
            { float x0 = CPAT >= 3 ? 1.35f : -1.35f; cam(V(x0 + 0.2f, 0.16f, -2.6f), V(x0, 0.07f, 0), 4.2f); } break; }
        if (CVAR == 11) {       // la bouche : cinq expressions côte à côte (sourire, ouverture)
            static const float E[5][2] = {{0.3f, 0}, {1.0f, 0}, {0.5f, 0.3f}, {1.0f, 0.35f}, {1.0f, 0.7f}};
            for (int i = 0; i < 5; i++) { cz_t *C = &WCZ[WN]; *C = CZC[i == 2 ? 20 : i == 4 ? 23 : 12 + i]; pose_t *P = &WPP[WN]; cz_rest(C, P); P->smile = E[i][0]; P->open = E[i][1]; P->happy = 0.5f;
                w_place(WN, -1.9f + 0.95f * i, 0, 0, PI); WN++; }
            cam(V(0, 1.10f, -3.9f), V(0, 1.02f, 0), 2.3f); break; }
        if (CVAR == 10) {       // planches d'exemples : visages (motif 0), mains (1), pieds (2) ; six citoyens différents
            static const int K[6] = {12, 0, 18, 3, 22, 8};
            for (int i = 0; i < 6; i++) { cz_t *C = &WCZ[WN]; *C = CZC[K[i]]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, 0.3f, 0.3f * i); P->smile = 0.4f + 0.12f * i; P->open = (i % 3 == 2) ? 0.35f : 0; P->happy = (i & 1) ? 1.0f : 0.3f;
                if (CPAT % 3 == 1) { float sg = (i & 1) ? -1.0f : 1.0f; if (i % 3 == 0) cz_wave(P, 1.0f, 1.0f); else if (i % 3 == 1) { P->hand[0] = V(0.34f, 0.42f, 0.30f); P->hand[1] = V(-0.34f, 0.42f, 0.30f); } else { P->hand[0] = V(0.10f, 0.86f, 0.12f); P->hand[1] = V(-0.10f, 0.86f, 0.12f); } (void)sg; }
                if (CPAT % 3 == 2 && (i & 1)) { P->foot[0].z += 0.10f; P->footp[0] = -0.3f; }
                w_place(WN, -2.25f + 0.9f * i, 0, 0, PI + 0.25f * ((i & 1) ? 1.0f : -1.0f)); WN++; }
            { int ty = CPAT % 3, hf = CPAT / 3; float x0 = hf == 1 ? -1.35f : hf == 2 ? 1.35f : 0, k = hf ? 0.5f : 1.0f;       // motif + 3 : la moitié gauche, + 6 : la moitié droite, de plus près
            if (ty == 0) cam(V(x0, 1.10f, -4.6f * k - 0.4f * (1 - k)), V(x0, 1.02f, 0), 2.2f);
            else if (ty == 1) cam(V(x0, 0.95f, -4.8f * k - 0.5f * (1 - k)), V(x0, 0.80f, 0), 1.9f);
            else cam(V(x0, 0.30f, -4.2f * k - 0.3f * (1 - k)), V(x0, 0.13f, 0), 2.0f * (hf ? 1.15f : 1.0f)); }
            break; }
        if (CVAR == 9) {       // gros plans d'inspection : visages (motif 0 de face, 1 de trois quarts), mains (2), pieds (3)
            static const int K[3] = {12, 2, 19};
            for (int i = 0; i < 3; i++) { cz_t *C = &WCZ[WN]; *C = CZC[K[i]]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, 0.3f, 0.3f * i); P->smile = i == 1 ? 1.0f : 0.6f; P->open = i == 2 ? 0.35f : 0;
                if (CPAT == 2) { P->hand[0] = V(0.40f, 0.62f, 0.28f); P->hand[1] = V(-0.30f, 0.30f, 0.32f); }
                w_place(WN, -0.95f + 0.95f * i, 0, 0, PI + (CPAT == 1 ? 0.55f : 0)); WN++; }
            if (CPAT <= 1) cam(V(0, 1.12f, -3.4f), V(0, 1.02f, 0), 2.2f);
            else if (CPAT == 2) cam(V(0.2f, 0.95f, -2.6f), V(0, 0.80f, 0), 1.6f);
            else cam(V(0.3f, 0.30f, -2.4f), V(0, 0.12f, 0), 1.9f);
            break; }
        if (CVAR == 8) {       // planche : les dix tenues de la foule du Bal, avec leurs nouvelles coupes
            static const int K[10] = {0, 1, 2, 3, 4, 6, 7, 8, 9, 10};
            for (int i = 0; i < 10; i++) { cz_t *C = &WCZ[WN]; *C = CZC[K[i] + (CPAT >= 3 ? 12 : 0)]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, 0.3f, 0.2f * i); P->smile = 0.8f; P->hyaw = 0.35f * ((i & 1) ? 1.0f : -1.0f);
                w_place(WN, -4.05f + 0.9f * i, 0, (i & 1) ? 0.5f : 0.0f, PI); WN++; }
            { float x0 = CPAT == 1 || CPAT == 4 ? -2.3f : CPAT == 2 || CPAT == 5 ? 2.2f : 0; cam(V(x0, 1.05f, -6.6f), V(x0, 0.72f, 0), CPAT && CPAT != 3 ? 3.3f : 1.95f); } break; }
        if (CVAR == 6 || CVAR == 7) {       // ESSAIS DE CORPS (17 nov. 2026) : deux photos de cinq, un repère à gauche (Martin Plaisir, puis l'élancé actuel) et quatre essais
            static const v3 RBC[7] = {{0.95f, 0.60f, 0.10f}, {0.95f, 0.14f, 0.42f}, {0.50f, 0.72f, 0.98f}, {0.08f, 0.72f, 0.42f}, {0.52f, 0.22f, 0.95f}, {0.90f, 0.10f, 0.13f}, {0.92f, 0.88f, 0.98f}};
            //                         bulk   head   leg    arm    sho    shape torso  limb   échelle
            static const float B[8][9] = {{1.50f, 1.38f, 0.80f, 0.92f, 1.22f, 0, 1.00f, 1.00f, 0.92f},       // 1 compact
                                          {1.15f, 1.80f, 0.88f, 0.90f, 0.95f, 0, 0.95f, 0.90f, 0.86f},       // 2 grosse tête
                                          {1.30f, 1.40f, 0.82f, 1.00f, 1.00f, 2, 0.80f, 0.55f, 0.95f},       // 3 boule à tuyaux
                                          {1.40f, 1.30f, 0.78f, 0.95f, 1.05f, 1, 1.00f, 0.95f, 0.95f},       // 4 bedaine
                                          {1.55f, 1.05f, 0.72f, 1.02f, 1.75f, 0, 1.15f, 1.10f, 0.98f},       // 5 armoire
                                          {1.45f, 1.95f, 0.55f, 0.72f, 1.00f, 0, 0.80f, 1.00f, 0.90f},       // 6 mini
                                          {1.35f, 1.30f, 0.70f, 0.95f, 0.78f, 3, 1.00f, 0.95f, 0.98f},       // 7 poire
                                          {1.20f, 1.30f, 0.58f, 1.10f, 0.90f, 0, 1.55f, 0.85f, 0.95f}};      // 8 teckel
            for (int i = 0; i < 5; i++) { dn_pose_t P; dn_rest(&P); float sg = (i & 1) ? -1.0f : 1.0f; int hs = sg > 0 ? 0 : 1; P.pel = V(0.06f * sg, 0.985f, 0); P.proll = -0.15f * sg; P.chest = V(-0.05f * sg, 0.355f, 0); P.croll = 0.11f * sg; P.hroll = -0.07f * sg; P.smile = 0.8f; P.lid = 0.4f;
                P.hand[hs] = V(sg * 0.27f + P.pel.x, 1.02f, 0.04f); P.epole[hs] = V(sg, 0, -0.7f); float sc = 1.0f;
                if (i == 0) { DNLOOK = CVAR == 6 ? DN_MARTIN : (dn_look_t){SK[0], HC[0][0], HC[0][1], 2, DC_NONE, 1.28f, 1.16f, 1, 1, 1, 0, RBC[0]}; sc = CVAR == 6 ? 1.0f : 0.84f; }
                else { int t = (CVAR - 6) * 4 + i - 1, k = (t + 1) % 7; const float *b = B[t]; DNLOOK = (dn_look_t){SK[k], HC[k][0], HC[k][1], 2, CT[k], b[0], b[1], b[2], b[3], b[4], 0, RBC[k], (int)b[5], b[6], b[7]}; sc = b[8]; }
                dn_place(&P, V(-2.9f + 1.45f * i, 0, 0), PI, sc); }
            cam(V(0, 1.0f, -7.8f), V(0, 0.90f, 0), 2.7f); break; }
        if (CVAR == 5) {       // comparaison : Martin Plaisir, Kevin élancé (actuel), Kevin « compact » (jambes plus courtes, tête plus grosse, veste carrée)
            for (int i = 0; i < 3; i++) { dn_pose_t P; dn_rest(&P); float sg = i == 1 ? -1.0f : 1.0f; P.pel = V(0.06f * sg, 0.985f, 0); P.proll = -0.15f * sg; P.chest = V(-0.05f * sg, 0.355f, 0); P.croll = 0.11f * sg; P.hroll = -0.07f * sg; P.smile = 0.8f; P.lid = 0.4f;
                P.hand[0] = V(0.27f + P.pel.x, 1.02f, 0.04f); P.epole[0] = V(1, 0, -0.7f);
                DNLOOK = i == 0 ? DN_MARTIN : i == 1 ? (dn_look_t){SK[0], HC[0][0], HC[0][1], 2, DC_NONE, 1.28f, 1.16f, 1, 1, 1, 0, {0.95f, 0.60f, 0.10f}} : (dn_look_t){SK[0], HC[0][0], HC[0][1], 2, DC_NONE, 1.50f, 1.38f, 0.80f, 0.92f, 1.22f, 0, {0.95f, 0.60f, 0.10f}};
                dn_place(&P, V(-1.5f + 1.5f * i, 0, 0), PI, i == 0 ? 1.0f : i == 1 ? 0.84f : 0.92f); }
            cam(V(0, 1.0f, -7.2f), V(0, 0.95f, 0), 2.6f); break; }
        for (int i = 0; i < 7; i++) { int k = O[i]; dn_pose_t P; dn_rest(&P); float sg = (i & 1) ? -1.0f : 1.0f, ph = T * 1.3f + i;
            if (CVAR == 1) { P.pel = V(0.07f * sg, 0.985f, 0); P.proll = -0.17f * sg; P.chest = V(-0.07f * sg, 0.355f, 0); P.croll = 0.13f * sg; P.hroll = -0.08f * sg; P.hand[0] = V(0.40f, 1.95f, 0.04f); P.epole[0] = V(1, 0, -0.4f); P.hand[1] = V(-0.24f + P.pel.x, 0.99f, 0.06f); P.epole[1] = V(-1, 0, -0.5f); P.smile = 1; }       // tous ensemble : un bras au ciel
            else { P.pel.x = 0.03f * sinf(ph); P.proll = -0.07f * sinf(ph); P.hroll = 0.05f * sinf(ph + 1.0f); P.hyaw = 0.12f * sinf(ph * 0.7f); }
            if (CVAR >= 3) {       // la photo de groupe façon boys band : en pointe, Kevin devant, chacun sa pose
                float a = fabsf(i - 3.0f); P.lid = 0.55f; for (int q = 0; q < 2; q++) { float qs = q ? -1.0f : 1.0f; P.kpole[q] = V(qs * 0.06f, 0, 1); P.foot[q] = V(qs * 0.105f, 0, qs * sg * 0.05f); P.fyaw[q] = 0; } P.smile = 0.55f; P.hpitch = -0.05f; P.hyaw = -0.10f * (i - 3.0f);
                if (k == 0) { P.pel = V(0.03f, 0.99f, 0); P.proll = -0.07f; P.chest = V(-0.02f, 0.36f, 0); P.hand[0] = V(0.105f, 1.27f, 0.17f); P.hand[1] = V(-0.105f, 1.27f, 0.17f); P.epole[0] = V(1, -0.5f, 0.1f); P.epole[1] = V(-1, -0.5f, 0.1f); P.hpitch = -0.12f; P.hroll = 0.06f; }       // Kevin : les deux mains aux revers
                else { P.pel = V(0.065f * sg, 0.985f, 0); P.proll = -0.16f * sg; P.chest = V(-0.06f * sg, 0.355f, 0); P.croll = 0.12f * sg; P.hroll = -0.09f * sg; P.pyaw = -0.22f * sg; int hs = sg > 0 ? 0 : 1, ls = 1 - hs;
                    P.hand[ls] = V(-sg * 0.205f + P.pel.x, 0.95f, 0.05f); P.epole[ls] = V(-sg, 0, -0.6f);       // une main dans la poche
                    if ((int)a == 1) { P.hand[hs] = V(sg * 0.02f, 1.31f, 0.17f); P.epole[hs] = V(sg, -0.6f, 0.1f); }       // l'autre au noeud de cravate
                    else if ((int)a == 2) { P.hand[hs] = V(sg * 0.30f, 1.80f, 0.05f); P.epole[hs] = V(sg, 0.2f, -0.4f); P.hpitch = 0.05f; }       // dans les cheveux
                    else { P.hand[hs] = V(sg * 0.27f + P.pel.x, 1.02f, 0.04f); P.epole[hs] = V(sg, 0, -0.7f); } }       // sur la hanche
                DNLOOK = (dn_look_t){SK[k], HC[k][0], HC[k][1], 1, CT[k], 1.30f, 1.16f, 1.10f, 1.0f, 1.62f, 1}; dn_place(&P, V(-2.85f + 0.95f * i, 0, 0.42f * a - 3.6f), PI, k ? 0.80f : 0.84f); continue; }
            DNLOOK = (dn_look_t){SK[k], HC[k][0], HC[k][1], 1, CT[k], 1.28f, 1.16f}; dn_place(&P, V(-3.3f + 1.1f * i, 0, 0), PI, k ? 0.80f : 0.84f); }
        if (CVAR == 4) cam(V(0.9f, 0.55f, -7.6f), V(0, 1.02f, -3.5f), 2.6f);       // plus près, en contre-plongée
        else if (CVAR == 3) cam(V(-3.4f, 0.70f, -11.6f), V(0, 0.95f, -3.0f), 3.0f);
        else cam(V(0, 0.95f, -9.6f), V(0, 0.80f, 0), 2.75f); break; }
    case 50: { CHV = 5; CPOST = 1; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.35f, 0.75f, -0.6f)); WSUNC = V(1.0f, 0.93f, 0.96f); WAMBU = V(0.12f, 0.12f, 0.26f); WAMBD = V(0.14f, 0.04f, 0.12f); CBEAMK = 0;      // L'AFFICHE DES RANGA BOYZ : les sept élancés, en costumes de satin pailleté, sur une scène aux néons (image fixe, format portrait)
        static const v3 SK[7] = {{0.30f, 0.16f, 0.10f}, {0.92f, 0.37f, 0.38f}, {0.085f, 0.07f, 0.075f}, {0.95f, 0.66f, 0.64f}, {0.93f, 0.42f, 0.44f}, {0.96f, 0.80f, 0.72f}, {0.82f, 0.50f, 0.50f}};
        static const v3 HC[7][2] = {{{0, 0, 0}, {0, 0, 0}}, {{0.90f, 0.80f, 0.50f}, {1.0f, 0.96f, 0.80f}}, {{0.75f, 0.36f, 0.05f}, {0.95f, 0.62f, 0.15f}}, {{0.02f, 0.45f, 0.48f}, {0.55f, 0.95f, 0.90f}}, {{0.36f, 0.10f, 0.55f}, {0.62f, 0.30f, 0.80f}}, {{0.05f, 0.16f, 0.55f}, {0.35f, 0.65f, 1.0f}}, {{0.50f, 0.04f, 0.09f}, {0.85f, 0.20f, 0.25f}}};
        static const v3 RBCL[7] = {{0.95f, 0.60f, 0.10f}, {0.95f, 0.14f, 0.42f}, {0.50f, 0.72f, 0.98f}, {0.08f, 0.72f, 0.42f}, {0.52f, 0.22f, 0.95f}, {0.90f, 0.10f, 0.13f}, {0.92f, 0.88f, 0.98f}};
        static const int CT[7] = {DC_NONE, DC_QUIFF, DC_AFRO, DC_MOHAWK, DC_FRINGE, DC_SPIKES, DC_BUN}, O[7] = {5, 3, 1, 0, 2, 4, 6};
        if (CVAR == 0) {       // les Ranga Boyz définitifs : les Porko citoyens en satin pailleté, en deux rangs, Kevin devant (var 1 : l'ancienne affiche des élancés)
            static const float XZ[7][2] = {{0, -0.60f}, {-1.46f, -0.25f}, {1.46f, -0.25f}, {-0.74f, 0.50f}, {0.74f, 0.50f}, {-2.22f, 0.40f}, {2.22f, 0.40f}};
            for (int k = 0; k < 7; k++) { cz_t *C = &WCZ[WN]; *C = RBZ[k]; pose_t *P = &WPP[WN]; cz_rest(C, P); C->cjack = C->cbot = RBCL[k]; C->flags |= F_SEQUIN; float sg = XZ[k][0] < 0 ? -1.0f : 1.0f;
                P->smile = 0.9f; P->open = 0.22f; P->brow = 0.2f; P->hyaw = -0.10f * XZ[k][0]; P->twist = -0.10f * XZ[k][0];
                if (k == 0) { P->hand[0] = V(0.50f, 0.92f, 0.02f); P->hand[1] = V(-0.33f, 0.28f, 0.10f); P->hpitch = -0.15f; P->roll = -0.08f; P->sway = 0.05f; }
                else if (k < 3) { int up = sg > 0 ? 0 : 1; P->hand[up] = V(sg * 0.54f, 0.70f, 0.10f); P->hand[1 - up] = V(-sg * 0.33f, 0.28f, 0.10f); P->roll = 0.08f * sg; P->hroll = -0.08f * sg; }
                else if (k < 5) { P->hand[0] = V(0.40f, 0.92f, 0.03f); P->hand[1] = V(-0.40f, 0.92f, 0.03f); P->hpitch = -0.10f; }
                else { P->hand[0] = V(0.33f, 0.28f, 0.10f); P->hand[1] = V(-0.33f, 0.28f, 0.10f); P->hroll = -0.10f * sg; P->roll = 0.06f * sg; }
                w_place(WN, XZ[k][0], 0, XZ[k][1], PI); WN++; }
            cam(V(0, 0.80f, -11.9f), V(0, 2.80f, 0), 3.0f); break; }
        for (int i = 0; i < 7; i++) { int k = O[i]; dn_pose_t P; dn_rest(&P); float sg = (i & 1) ? -1.0f : 1.0f, a = fabsf(i - 3.0f); P.lid = 0.5f; P.smile = 0.7f; P.hpitch = -0.05f; P.hyaw = -0.08f * (i - 3.0f);
            for (int q = 0; q < 2; q++) { float qs = q ? -1.0f : 1.0f; P.kpole[q] = V(qs * 0.10f, 0, 1); P.foot[q] = V(qs * 0.115f, 0, qs * sg * 0.05f); P.fyaw[q] = 0; }
            if (k == 0) { P.pel = V(0.03f, 0.99f, 0); P.proll = -0.07f; P.chest = V(-0.02f, 0.36f, 0); P.hand[0] = V(0.105f, 1.27f, 0.17f); P.hand[1] = V(-0.105f, 1.27f, 0.17f); P.epole[0] = V(1, -0.5f, 0.1f); P.epole[1] = V(-1, -0.5f, 0.1f); P.hpitch = -0.12f; P.hroll = 0.06f; }
            else { P.pel = V(0.065f * sg, 0.985f, 0); P.proll = -0.16f * sg; P.chest = V(-0.06f * sg, 0.355f, 0); P.croll = 0.12f * sg; P.hroll = -0.09f * sg; P.pyaw = -0.20f * sg; int hs = sg > 0 ? 0 : 1, ls = 1 - hs;
                P.hand[ls] = V(-sg * 0.205f + P.pel.x, 0.95f, 0.05f); P.epole[ls] = V(-sg, 0, -0.6f);
                if ((int)a == 1) { P.hand[hs] = V(sg * 0.02f, 1.31f, 0.17f); P.epole[hs] = V(sg, -0.6f, 0.1f); }
                else if ((int)a == 2) { P.hand[hs] = V(sg * 0.30f, 1.80f, 0.05f); P.epole[hs] = V(sg, 0.2f, -0.4f); P.hpitch = 0.05f; }
                else { P.hand[hs] = V(sg * 0.27f + P.pel.x, 1.02f, 0.04f); P.epole[hs] = V(sg, 0, -0.7f); } }
            DNLOOK = (dn_look_t){SK[k], HC[k][0], HC[k][1], 2, CT[k], 1.28f, 1.16f, 1, 1, 1, 0, RBCL[k]}; dn_place(&P, V(-2.22f + 0.74f * i, 0, (i & 1) ? 0.30f : -0.25f), PI, k ? 0.80f : 0.85f); }
        cam(V(0, 1.10f, -13.6f), V(0, 3.30f, 0), 3.0f); break; }
    case 49: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // la démo des Ranga Boyz sur le squelette du Danseur (var 1 élancés, 2 trapus), caméra de répétition
        float b = CB - 96.0f; RBE = CPAT ? 1.0f : 0.0f; rbd_group(b, 0, 0, PI, CVAR); WCONF = 0.9f * sstep(27.6f, 28.6f, b) * sstep(32.6f, 32.0f, b);       // motif 1 : la version énergique
        cam(V(1.2f - 0.6f * u, 1.40f, -8.0f + 0.8f * eu), V(0, 0.80f, 0.9f), CVAR == 2 ? 3.1f : CVAR >= 10 ? 2.7f : 3.4f); break; }
    case 41: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // les nouveaux habitants au studio, les porcs standards devant
        int h = CVAR & 1; for (int i = 0; i < 5; i++) { cz_t *C = &WCZ[WN]; *C = CLV[5 * h + i]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, T, 0.1f + 0.13f * i); if (C->prop == PROP_CANE) P->hand[1] = V(-0.36f, 0.16f, 0.16f); w_place(WN, -2.5f + 1.25f * i, 0, 0.6f, PI + 0.12f * (2 - i)); WN++; }
        cam(V(0, 3.6f, -9.6f), V(0, 0.35f, -1.1f), 2.75f); break; }
    case 40: { CHV = 5; CHSUN = 0; WSNOW = 0; WFAR = 80.0f; WSUN = nrm(V(-0.5f, 0.7f, -0.5f)); WSUNC = V(0.95f, 0.90f, 0.82f); WAMBU = V(0.17f, 0.14f, 0.13f); WAMBD = V(0.07f, 0.045f, 0.035f); CBEAMK = 0;      // les Ranga Boyz au studio : la planche
        int v = CVAR;
        if (v == 0) { for (int i = 0; i < 7; i++) { static const int O[7] = {5, 3, 1, 0, 2, 4, 6}; int k = O[i]; cz_t *C = &WCZ[WN]; *C = RBZ[k]; pose_t *P = &WPP[WN]; cz_rest(C, P); cz_idle(P, T, 0.1f + 0.13f * i); w_place(WN, -3.3f + 1.1f * i, 0, 0, PI); WN++; }
            cam(V(0, 0.95f, -9.6f), V(0, 0.70f, 0), 2.75f); }
        else { rb_group(CPAT + sl / 0.6602f, 0, 0, PI);
            if (v == 1) cam(V(0, 0.95f, -8.2f), V(0, 0.72f, 0.9f), 2.9f); else if (v == 2) cam(V(-2.2f, 0.28f, -5.6f), V(0.2f, 0.95f, 0.8f), 1.7f); else if (v == 3) cam(V(0, 9.0f, -3.5f), V(0, 0, 1.0f), 2.6f); else cam(V(1.1f, 1.0f, -3.4f), V(0, 0.85f, 0), 2.2f); }
        break; }
    case 31: { CHV = 1; WSUNC = V(0, 0, 0); WAMBU = V(0.05f, 0.05f, 0.08f); WSNOW = 0; float z = 13.632f * CTS, ph = CB * 0.39f; cam(V(2.7f * cosf(ph), 2.7f * sinf(ph), z), V(1.0f * cosf(ph + 0.8f), 1.0f * sinf(ph + 0.8f), z + 14.0f), 0.95f); break; }      // le tunnel de néons : trois anneaux par temps
    case 32: CHV = 3; CHSUN = 0; WSUNC = V(0, 0, 0); WAMBU = V(0.10f, 0.09f, 0.13f); cam(V(-1.3f + 2.6f * u, 0.85f, -0.4f + 0.5f * u), V(0, 1.30f, 3.4f), 1.25f); break;      // le mur d'enceintes
    case 33: { float a = 0.5f + 1.1f * u; CGIANT = 1; cam(V(8.2f * sinf(a), 1.5f + 0.5f * u, -8.2f * cosf(a)), V(0, 2.15f, 0), 1.45f); break; }      // le porc d'or, comme un trophée
    case 34: { CHV = 4; cam(mix3(V(-7.0f, 0.6f, -8.0f), V(3.0f, 2.8f, -14.0f), eu), V(0, 1.25f, 0), 1.35f); break; }      // DJ VITEAU, en lettres d'or
    case 35: { float a = 0.9f * CB * 0.25f; CGIANT = CVAR & 1; if (!CGIANT) CBALLC = V(0, 60.0f, 0); cam(V(1.2f * sinf(a), 15.0f - 2.0f * u, 1.2f * cosf(a)), V(0, 0, 0), 1.25f); break; }      // la ronde, vue du ciel : un mandala
    case 36: { CHV = 2; float z = 7.0f * CTS; cam(V(0.8f * sinf(CB * 0.39f), 3.6f - 0.8f * u, z), V(0, 1.3f, z + 15.0f), 1.15f); break; }      // la foule sans fin
    case 37: { float a = 2.4f + 0.8f * u; cam(V(9.0f * sinf(a), 0.55f, -9.0f * cosf(a)), V(0, 2.9f, 0), 1.25f); break; }      // la boule, comme une planète
    default: { float a = 0.3f + 1.0f * u + (CVAR & 4 ? 3.1f : 0); CGIANT = CVAR & 1; cam(V(11.5f * sinf(a), 2.7f + 1.0f * sinf(u * PI), -11.5f * cosf(a)), V(0, 1.5f, 0), 1.3f); break; }      // 30 : la ronde sur le disque géant
    }
}
