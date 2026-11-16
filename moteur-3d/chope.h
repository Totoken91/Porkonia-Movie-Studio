// ============================ LA CHOPE DE DOUZI AMBRÉE : la bière de Porkonia ============================
// Modèle standard n° 3 (16 nov. 2026 ; Kenny : « à chaque fois tu fais juste un cylindre moche »). C'est LA bière de Porkonia, partout :
// sur les tables, les comptoirs, dans les mains, en monument. Une chope de verre à fossettes (trois rangs de neuf, en quinconce), un pied épais,
// une anse, la Douzi Ambrée dedans, sa mousse qui déborde, une coulure, des bulles qui montent, de la buée, des dentelles de mousse quand on a bu.
// Repère : posée sur y = 0 (le sous-bock est dessous, de y = -0,004 à 0), axe y, l'anse vers +x. Taille réelle : 16,5 cm (un demi).
// Pour une autre taille : diviser le point par l'échelle, multiplier la distance (ch_hull) ; le tracé optique travaille en mètres du modèle.
//   chope_t : fill  0 vide → 1 pleine ; foam  0 → 1 hauteur de mousse (elle déborde au-dessus de 0,55 quand la chope est pleine) ;
//             cold  0 → 1 buée et gouttes ; lace  0 → 1 dentelles de mousse au-dessus du niveau ; drip  0 → 1 la coulure ; t  le temps (les bulles) ;
//             coaster  1 : avec le sous-bock.
// Deux étages : 1) ch_hull() : l'enveloppe opaque, pour le lancer de rayons du moteur (une seule matière) ;
//               2) chope_trace() : l'optique. Le rayon entre dans le verre (indice 1,5), traverse la bière (1,34, absorption de Beer-Lambert : mince elle est
//                  dorée, épaisse elle est ambrée), se réfléchit en totalité sous les angles rasants, ressort ; la mousse et le sous-bock sont opaques.
// Le moteur hôte fournit trois macros avant d'inclure ce fichier (tout est dans le repère de la chope) :
//   CH_ENV(o, d)   la couleur de la scène le long du rayon (sans la chope) ; CH_LIGHT(p, n)  l'éclairement reçu en p, de normale n ;
//   CH_SPEC(p, r)  les reflets des lumières dans la direction r ; et, s'il veut la marque imprimée en or sur le bandeau, CHOPE_ETCH(a, y) (0 à 1).
// Ne pas en changer la forme ni les couleurs sans que Kenny le demande : on règle le remplissage, la mousse, la taille, pas le modèle.
#define CH_H 0.165f
#define CH_RI 0.0405f
#define CH_BND 0.150f
#define CH_C V(0.014f, 0.085f, 0)
#ifndef CHOPE_T_DEFINED
typedef struct { float fill, foam, cold, lace, drip, t; int coaster; } chope_t;
#endif
enum { CHM_AIR, CHM_GLASS, CHM_BEER, CHM_FOAM, CHM_COASTER };
static const v3 CH_BEERC = {0.92f, 0.36f, 0.035f}, CH_FOAMC = {0.93f, 0.88f, 0.77f}, CH_SIGB = {5.5f, 30.0f, 95.0f}, CH_SIGG = {3.4f, 1.3f, 2.5f};
static const float CH_IOR[5] = {1.0f, 1.5f, 1.34f, 1.0f, 1.0f};
#ifndef CHOPE_ETCH
#define CHOPE_ETCH(a, y) 0.0f
#endif
S float ch_yb(const chope_t *c) { return 0.016f + 0.126f * c->fill; }                                              // le niveau de la bière
S float ch_yf(const chope_t *c) { return ch_yb(c) + (c->fill > 0.02f ? 0.004f + 0.044f * c->foam : 0.0f); }        // le dessus de la mousse
S float ch_ro(float y) { return y > 0.132f ? 0.0475f : y < 0.014f ? 0.0500f : 0.0465f + 0.0035f * sinf(3.14159265f * (y - 0.014f) / 0.118f); }
S float ch_outer(v3 p) {       // le verre plein : le fût à fossettes, le pied, la lèvre arrondie, l'anse
    float r = len2(p.x, p.z), d = box2(r - ch_ro(p.y) + 0.003f, fabsf(p.y - CH_H * 0.5f) - CH_H * 0.5f + 0.003f) - 0.003f;
    if (p.y > 0.018f && p.y < 0.128f && r > 0.034f) {
        float row = clampf(roundf((p.y - 0.040f) / 0.033f), 0, 2), sa = 6.2831853f / 9, a = atan2f(p.z, p.x) + (((int)row & 1) ? sa * 0.5f : 0), al = a - sa * roundf(a / sa), yr = 0.040f + 0.033f * row;
        float e = (len(V((r * cosf(al) - ch_ro(yr) - 0.016f) / 0.0195f, (p.y - yr) / 0.0235f, r * sinf(al) / 0.0195f)) - 1.0f) * 0.0195f;
        d = -smin(-d, e, 0.004f); }
    { float h = fmaxf(len2(len2(p.x - 0.049f, p.y - 0.083f) - 0.043f, p.z * 0.75f) - 0.0078f, 0.046f - p.x); d = smin(d, h, 0.008f); }
    return d;
}
S float ch_inner(v3 p) { return box2(len2(p.x, p.z) - CH_RI + 0.006f, 0.022f - p.y) - 0.006f; }                    // le creux : un cylindre à fond arrondi, ouvert en haut
S float ch_glass(v3 p) { return fmaxf(ch_outer(p), -ch_inner(p)); }
S float ch_foamout(const chope_t *c, v3 p) {       // la mousse qui déborde : un dôme bosselé au-dessus de la lèvre, et la coulure le long du verre
    float yf = ch_yf(c), d = 1e9f; if (yf <= CH_H + 0.002f) return d;
    float hh = yf - CH_H + 0.006f, r = len2(p.x, p.z), a = atan2f(p.z, p.x);
    d = (len(V(p.x / 0.0480f, (p.y - CH_H + 0.004f) / hh, p.z / 0.0480f)) - 1.0f) * fminf(hh, 0.0480f);
    d = smin(d, (len2(r - 0.0372f, p.y - CH_H - 0.0045f - 0.0022f * sinf(3.0f * a + 1.0f)) - 0.0128f - 0.0034f * cosf(7.0f * a + 0.5f) - 0.0012f * cosf(13.0f * a)) * 0.8f, 0.010f);       // la couronne : sept bosses qui débordent de la lèvre
    for (int i = 0; i < 3; i++) { float b = 0.9f + i * 2.1f; d = smin(d, len(sub(p, V(0.015f * cosf(b), CH_H + 0.62f * hh + 0.003f * i, 0.015f * sinf(b)))) - 0.0165f, 0.012f); }
    d = fmaxf(d, CH_H - 0.004f - p.y);
    if (c->drip > 0) { const float a = -2.95f; float ox = 0.0505f * cosf(a), oz = 0.0505f * sinf(a), L = 0.042f * c->drip;
        d = smin(d, fminf(sdCap(p, V(ox * 0.90f, CH_H + 0.004f, oz * 0.90f), V(ox, CH_H - L, oz), 0.0052f), len(sub(p, V(ox * 1.012f, CH_H - L - 0.002f, oz * 1.012f))) - 0.0074f), 0.006f); }
    return d;
}
S float ch_coaster(v3 p) { return box2(len2(p.x, p.z) - 0.060f, fabsf(p.y + 0.002f) - 0.002f); }
S float ch_hull(const chope_t *c, v3 p) {       // l'enveloppe : le verre, ce qu'il contient, la mousse qui dépasse, le sous-bock
    float d = fminf(ch_glass(p), ch_foamout(c, p)); if (c->fill > 0.02f) d = fminf(d, fmaxf(ch_inner(p), p.y - ch_yf(c)));
    return c->coaster ? fminf(d, ch_coaster(p)) : d;
}
S int ch_med(const chope_t *c, v3 p) {       // dans quoi est-on ?
    if (c->coaster && ch_coaster(p) < 0) return CHM_COASTER;
    if (ch_glass(p) < 0) return CHM_GLASS;
    if (ch_inner(p) < 0) return p.y < ch_yb(c) ? CHM_BEER : p.y < ch_yf(c) ? CHM_FOAM : CHM_AIR;
    return ch_foamout(c, p) < 0 ? CHM_FOAM : CHM_AIR;
}
S float ch_udist(const chope_t *c, v3 p) {       // la distance à la prochaine frontière, quelle qu'elle soit
    float u = fminf(fminf(fabsf(ch_glass(p)), fabsf(p.y - ch_yb(c))), fminf(fabsf(p.y - ch_yf(c)), fabsf(ch_foamout(c, p))));
    return c->coaster ? fminf(u, fabsf(ch_coaster(p))) : u;
}
S v3 ch_nglass(v3 p) { const float e = 0.0004f; return nrm(V(ch_glass(V(p.x + e, p.y, p.z)) - ch_glass(V(p.x - e, p.y, p.z)), ch_glass(V(p.x, p.y + e, p.z)) - ch_glass(V(p.x, p.y - e, p.z)), ch_glass(V(p.x, p.y, p.z + e)) - ch_glass(V(p.x, p.y, p.z - e)))); }
S v3 ch_nfoam(const chope_t *c, v3 p) { const float e = 0.0008f; if (ch_foamout(c, p) > 0.004f) return V(0, 1, 0);
    return nrm(V(ch_foamout(c, V(p.x + e, p.y, p.z)) - ch_foamout(c, V(p.x - e, p.y, p.z)), ch_foamout(c, V(p.x, p.y + e, p.z)) - ch_foamout(c, V(p.x, p.y - e, p.z)), ch_foamout(c, V(p.x, p.y, p.z + e)) - ch_foamout(c, V(p.x, p.y, p.z - e)))); }
S float ch_bubbles(const chope_t *c, v3 p, v3 rd, float L) {       // les bulles : douze cheminées qui montent du fond ; ce que le rayon en croise
    float yb = ch_yb(c), acc = 0, hh = rd.x * rd.x + rd.z * rd.z;
    for (int i = 0; i < 12; i++) { float h = hash2(i * 3.7f + 1.1f, 5.3f), a = i * 2.399f, rr = 0.006f + 0.028f * sqrtf(fract(h * 7.3f)), ox = p.x - rr * cosf(a), oz = p.z - rr * sinf(a);
        float ts = hh > 1e-6f ? clampf(-(ox * rd.x + oz * rd.z) / hh, 0, L) : 0.5f * L, dx = ox + ts * rd.x, dz = oz + ts * rd.z, y = p.y + ts * rd.y;
        if (y < 0.019f || y > yb - 0.001f) continue;
        float s = 0.0120f, v = 0.055f * (0.7f + 0.6f * h), dy = (fract((y - v * c->t) / s + h * 9.0f) - 0.5f) * s, rb = 0.0005f + 0.0010f * (y - 0.016f) / (yb - 0.016f + 1e-4f), dd = sqrtf(dx * dx + dz * dz + dy * dy);
        acc += sstep(rb, rb * 0.3f, dd); }
    return fminf(acc, 1.5f);
}
S v3 ch_foamshade(const chope_t *c, v3 p, v3 n) {       // la mousse : crémeuse, de fines bulles, un peu d'ombre dans les creux
    float a = atan2f(p.z, p.x), tex = 1.0f - 0.07f * vnoise(a * 64.0f + p.y * 210.0f, p.y * 760.0f) - 0.06f * vnoise(a * 21.0f, p.y * 240.0f + a * 5.0f);
    v3 L = add(mul(CH_LIGHT(p, n), 0.62f), mul(CH_LIGHT(p, V(0, 1, 0)), 0.38f)); float occ = 0.72f + 0.28f * sstep(-0.004f, 0.010f, ch_foamout(c, add(p, mul(n, 0.012f))) );
    return mulv(mulv(mul(CH_FOAMC, tex), V(1.0f, 0.55f + 0.45f * occ, 0.35f + 0.65f * occ)), mul(L, (0.50f + 0.50f * occ) * 0.80f));
}
static v3 chope_trace(const chope_t *c, v3 ro, v3 rd, float t0) {       // rayon dans le repère de la chope ; t0 : où il touche l'enveloppe
    v3 p = add(ro, mul(rd, t0)), col = V(0, 0, 0), thr = V(1, 1, 1); int med = CHM_AIR, first = 1; float yb = ch_yb(c), yf = ch_yf(c);
    for (int ev = 0; ev < 16; ev++) {
        v3 pi = add(p, mul(rd, 0.0006f)); int nm = ch_med(c, pi);
        if (nm == CHM_COASTER) { float r = len2(p.x, p.z); v3 a = r > 0.057f ? V(0.80f, 0.60f, 0.22f) : (r > 0.041f && r < 0.053f) ? V(0.36f, 0.03f, 0.07f) : fabsf(r - 0.038f) < 0.0012f ? V(0.80f, 0.60f, 0.22f) : V(0.86f, 0.83f, 0.74f);
            return add(col, mulv(thr, mulv(a, CH_LIGHT(p, V(0, 1, 0))))); }
        if (nm == CHM_FOAM) { v3 n = med == CHM_GLASS ? nrm(V(p.x, 0.15f, p.z)) : ch_nfoam(c, pi); return add(col, mulv(thr, mul(ch_foamshade(c, p, n), med == CHM_GLASS ? 0.93f : 1.0f))); }
        if (nm != med) {
            v3 n = (med == CHM_GLASS || nm == CHM_GLASS) ? ch_nglass(p) : V(0, 1, 0); if (dot(n, rd) > 0) n = mul(n, -1);
            if (first) { first = 0;       // la première surface : la buée, les gouttes, la gravure, le reflet
                float a = atan2f(p.z, p.x), r = len2(p.x, p.z);
                if (nm == CHM_GLASS && c->cold > 0 && p.y > 0.016f && p.y < yb + 0.004f && r < 0.052f) {
                    float u = a * 0.048f / 0.0055f, v = p.y / 0.0055f, h = hash2(floorf(u), floorf(v)), fu = fract(u) - 0.5f - 0.25f * (fract(h * 13.0f) - 0.5f), fv = fract(v) - 0.5f - 0.25f * (fract(h * 29.0f) - 0.5f), rdp = 0.16f + 0.24f * fract(h * 71.0f);
                    if (h < 0.34f * c->cold && len2(fu, fv) < rdp) { v3 tg = nrm(V(-p.z, 0, p.x)); n = nrm(add(n, add(mul(tg, 1.6f * fu), V(0, 1.6f * fv, 0)))); }       // une goutte : une petite lentille
                    else { float al = c->cold * 0.20f * (0.55f + 0.45f * vnoise(a * 9.0f, p.y * 70.0f)); col = add(col, mulv(thr, mul(mulv(V(0.85f, 0.90f, 0.95f), CH_LIGHT(p, n)), al * 0.55f))); thr = mul(thr, 1 - al); } }
                if (nm == CHM_GLASS) { float e = CHOPE_ETCH(a, p.y); if (e > 0) { col = add(col, mulv(thr, mul(add(mulv(V(0.80f, 0.58f, 0.20f), CH_LIGHT(p, n)), mul(CH_SPEC(p, refl(rd, n)), 0.6f)), e))); thr = mul(thr, 1 - e); } }       // la marque, imprimée en or
                { float ci = fmaxf(-dot(n, rd), 0), F0 = nm == CHM_GLASS ? 0.04f : 0.021f, F = F0 + (1 - F0) * powf(1 - ci, 5); v3 rr = refl(rd, n);
                    col = add(col, mulv(thr, add(mul(CH_ENV(add(p, mul(n, 0.0015f)), rr), F), CH_SPEC(p, rr)))); thr = mul(thr, 1 - F); } }
            { float eta = CH_IOR[med] / CH_IOR[nm], ci = -dot(n, rd), k = 1 - eta * eta * (1 - ci * ci);
                if (k < 0) rd = refl(rd, n);       // réflexion totale : on reste dans le milieu
                else { rd = nrm(add(mul(rd, eta), mul(n, eta * ci - sqrtf(k))));
                    if (med == CHM_GLASS && nm == CHM_AIR && c->lace > 0 && ch_inner(pi) < 0 && p.y > yf && p.y < 0.134f) {       // les dentelles : la mousse restée sur le verre
                        float a = atan2f(p.z, p.x), al = c->lace * sstep(0.50f, 0.72f, vnoise(a * 7.0f + 3.0f * sinf(p.y * 60.0f), p.y * 95.0f) + 0.22f * sinf(p.y * 380.0f)) * 0.85f;
                        col = add(col, mulv(thr, mul(ch_foamshade(c, p, nrm(V(p.x, 0.2f, p.z))), al))); thr = mul(thr, 1 - al); }
                    med = nm; p = pi; } }
        } else p = pi;
        float t = 0, tp = 0; int hit = 0;       // traverser le milieu jusqu'à la prochaine frontière
        for (int i = 0; i < 64; i++) { tp = t; t += fmaxf(ch_udist(c, add(p, mul(rd, t))), 0.0007f); v3 q = add(p, mul(rd, t));
            if (ch_med(c, q) != med) { hit = 1; break; }
            if (len(sub(q, CH_C)) > CH_BND) break; }
        if (hit) { for (int i = 0; i < 7; i++) { float tm = 0.5f * (tp + t); if (ch_med(c, add(p, mul(rd, tm))) != med) t = tm; else tp = tm; } t = tp; }
        if (med == CHM_BEER) { float b = ch_bubbles(c, p, rd, t); v3 amb = CH_LIGHT(add(p, mul(rd, 0.5f * t)), V(0, 1, 0));
            col = add(col, mulv(thr, add(mul(mulv(CH_BEERC, amb), 0.36f * (1 - expf(-26.0f * t))), mul(mulv(V(1.0f, 0.96f, 0.85f), add(amb, V(0.25f, 0.25f, 0.25f))), 0.9f * b))));
            thr = mulv(thr, V(expf(-CH_SIGB.x * t), expf(-CH_SIGB.y * t), expf(-CH_SIGB.z * t))); }
        else if (med == CHM_GLASS) thr = mulv(thr, V(expf(-CH_SIGG.x * t), expf(-CH_SIGG.y * t), expf(-CH_SIGG.z * t)));
        p = add(p, mul(rd, t));
        if (!hit) return add(col, mulv(thr, CH_ENV(p, rd)));
    }
    return add(col, mulv(thr, mul(CH_BEERC, 0.25f)));
}
