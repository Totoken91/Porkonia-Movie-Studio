// ============================ LE DANSEUR : un porc humanoïde, élancé, aux proportions exprès trop longues (16 nov. 2026, essai demandé par Kenny) ============================
// Fait pour danser souplement : un bassin, un buste et une tête qui tournent chacun pour soi (la vague du corps), deux bras et deux jambes à deux os,
// posés par cinématique inverse (on dit où vont les mains et les pieds, les coudes et les genoux suivent). Repère : debout sur y = 0, regarde vers +z. 1,9 m.
enum { D_SKIN = 500, D_SNOUT, D_EYE, D_SHIRT, D_PANTS, D_BOOT, D_GOLD, D_HAIR, D_WHITE };
enum { DC_NONE, DC_SPIKES, DC_MOHAWK, DC_QUIFF, DC_AFRO, DC_FRINGE, DC_BUN };
typedef struct { v3 skin, hair, hair2; int suit, cut; float bulk, head, leg, arm, sho; int idol; v3 cloth; int shape; float torso, limb; } dn_look_t;       // shape 1 bedaine, 2 boule, 3 poire ; torso : longueur du buste ; limb : épaisseur des membres (0 = 1)       // suit 2 : costume de scène en satin pailleté, de la couleur cloth
static int CPOST;       // 1 : la scène aux néons de l'affiche (plan 50)       // + la longueur des jambes, des bras, la largeur d'épaules (0 = 1) : les poses sont écrites pour les proportions de base et reportées       // l'allure : la robe, la coupe, la tenue (0 : satin et pattes d'éléphant ; 1 : costard gris, cravate), la carrure, la taille de la tête
typedef struct {
    v3 pel; float pyaw, ptilt, proll;       // le bassin : sa place, son lacet, sa bascule avant, son déhanché
    v3 chest; float cyaw, cpitch, croll;    // le buste : son centre par rapport au bassin, ses rotations (le lacet s'ajoute à celui du bassin)
    float hyaw, hpitch, hroll;              // la tête, par rapport au buste
    v3 hand[2], foot[2], epole[2], kpole[2]; float fyaw[2], heel[2];       // 0 : côté +x ; mains et semelles dans le repère du sol ; où pointent coudes et genoux ; talon levé
    float lid, smile, tail;
} dn_pose_t;
typedef struct { v3 pel, px, py, pz, ch, cx, cy, cz, nk, hc, hx, hy, hz, sh[2], el[2], ha[2], hip[2], kn[2], an[2], ft[2]; float fc[2], fs[2]; dn_pose_t P; dn_look_t L; } dn_rig_t;
static __thread v3 DN_C, DN_H, DN_P; static __thread int DN_SIDE;
#define DN_UA 0.31f
#define DN_FA 0.30f
#define DN_TH 0.47f
#define DN_SN 0.47f
static void dn_rest(dn_pose_t *P) {
    memset(P, 0, sizeof(*P)); P->pel = V(0, 1.035f, 0); P->chest = V(0, 0.36f, 0);
    P->hand[0] = V(0.27f, 0.83f, 0.04f); P->hand[1] = V(-0.27f, 0.83f, 0.04f); P->foot[0] = V(0.11f, 0, 0); P->foot[1] = V(-0.11f, 0, 0);
    P->epole[0] = V(0.5f, -0.2f, -1); P->epole[1] = V(-0.5f, -0.2f, -1); P->kpole[0] = V(0.25f, 0, 1); P->kpole[1] = V(-0.25f, 0, 1); P->fyaw[0] = 0.25f; P->fyaw[1] = -0.25f; P->smile = 0.6f; P->lid = 0.35f;
}
S v3 dn_ik(v3 a, v3 *t, float l1, float l2, v3 pole) {       // l'articulation du milieu ; la cible est ramenée à portée
    v3 dv = sub(*t, a); float dl = len(dv), dc = clampf(dl, fabsf(l1 - l2) + 0.02f, l1 + l2 - 0.004f); v3 dr = mul(dv, 1.0f / fmaxf(dl, 1e-5f)); *t = add(a, mul(dr, dc));
    float x = (l1 * l1 - l2 * l2 + dc * dc) / (2 * dc), h = sqrtf(fmaxf(l1 * l1 - x * x, 0)); v3 pp = sub(pole, mul(dr, dot(pole, dr))); float pl = len(pp); pp = pl > 1e-4f ? mul(pp, 1 / pl) : V(0, 0, 1);
    return add(a, add(mul(dr, x), mul(pp, h)));
}
static void dn_rig(const dn_pose_t *P, dn_rig_t *R) {       // R->L (l'allure) doit être rempli avant
    float lk = R->L.leg > 0 ? R->L.leg : 1, ak = R->L.arm > 0 ? R->L.arm : 1, sw = R->L.sho > 0 ? R->L.sho : 1, hk = R->L.head > 0 ? R->L.head : 1, dy = (1 - lk) * 0.955f;
    R->P = *P; R->pel = V(P->pel.x, P->pel.y - dy - (1 - lk) * (1.0f - P->pel.y) * 0.0f, P->pel.z); R->pel.y = P->pel.y * 1.0f - dy; if (lk < 1) R->pel.y = 0.955f * lk + (P->pel.y - 0.955f) * lk + 0.08f * (1 - lk);
    R->px = cz_rot(V(1, 0, 0), P->pyaw, P->ptilt, P->proll); R->py = cz_rot(V(0, 1, 0), P->pyaw, P->ptilt, P->proll); R->pz = cz_rot(V(0, 0, 1), P->pyaw, P->ptilt, P->proll);
    float cy = P->pyaw + P->cyaw; R->ch = add(R->pel, V(P->chest.x, P->chest.y * (R->L.torso > 0 ? R->L.torso : 1), P->chest.z)); R->cx = cz_rot(V(1, 0, 0), cy, P->cpitch, P->croll); R->cy = cz_rot(V(0, 1, 0), cy, P->cpitch, P->croll); R->cz = cz_rot(V(0, 0, 1), cy, P->cpitch, P->croll);
    float hy = cy + P->hyaw; R->hx = cz_rot(V(1, 0, 0), hy, P->cpitch * 0.5f + P->hpitch, P->croll * 0.5f + P->hroll); R->hy = cz_rot(V(0, 1, 0), hy, P->cpitch * 0.5f + P->hpitch, P->croll * 0.5f + P->hroll); R->hz = cz_rot(V(0, 0, 1), hy, P->cpitch * 0.5f + P->hpitch, P->croll * 0.5f + P->hroll);
    R->nk = add(R->ch, mul(R->cy, 0.175f)); R->hc = add(R->nk, add(mul(R->cy, 0.06f), mul(R->hy, 0.215f * hk)));
    v3 ch0 = add(P->pel, P->chest);       // le buste aux proportions de base : c'est de ses épaules que partent les gestes écrits
    for (int s = 0; s < 2; s++) { float sg = s ? -1.0f : 1.0f;
        v3 sh0 = add(ch0, add(mul(R->cx, sg * 0.185f), mul(R->cy, 0.095f)));
        R->sh[s] = add(R->ch, add(mul(R->cx, sg * 0.185f * sw), mul(R->cy, 0.095f))); R->ha[s] = add(R->sh[s], mul(sub(P->hand[s], sh0), ak)); R->el[s] = dn_ik(R->sh[s], &R->ha[s], DN_UA * ak, DN_FA * ak, P->epole[s]);
        R->hip[s] = add(R->pel, add(mul(R->px, sg * 0.095f * (0.5f + 0.5f * sw)), mul(R->py, -0.05f))); R->fc[s] = cosf(P->fyaw[s]); R->fs[s] = sinf(P->fyaw[s]); R->ft[s] = V(P->foot[s].x * (0.5f + 0.5f * sw), P->foot[s].y * lk, P->foot[s].z * (0.4f + 0.6f * lk));
        R->an[s] = add(R->ft[s], V(0, 0.105f + 0.09f * P->heel[s], 0)); R->kn[s] = dn_ik(R->hip[s], &R->an[s], DN_TH * lk, DN_SN * lk, P->kpole[s]); }
}
S float dn_cone(v3 p, v3 a, v3 b, float r1, float r2) { v3 pa = sub(p, a), ba = sub(b, a); float h = clampf(dot(pa, ba) / dot(ba, ba), 0, 1); return (len(sub(pa, mul(ba, h))) - mixf(r1, r2, h)) * 0.92f; }
static float dn_sdf(const dn_rig_t *R, v3 p, int *m) {       // p dans le repère du danseur
    float bd = len(sub(p, V(R->pel.x, 1.0f, R->pel.z))) - 1.55f; *m = D_SKIN; if (bd > 0.5f) return bd;
    float d = 1e9f, e, bk = R->L.bulk, hk = R->L.head, bkl = bk * (R->L.limb > 0 ? R->L.limb : 1); int shp = R->L.shape; v3 lp = cz_loc(sub(p, R->pel), R->px, R->py, R->pz), lc = cz_loc(sub(p, R->ch), R->cx, R->cy, R->cz), lh = mul(cz_loc(sub(p, R->hc), R->hx, R->hy, R->hz), 1 / hk), ah = V(fabsf(lh.x), lh.y, lh.z);
    { float sw = R->L.sho > 0 ? R->L.sho : 1; lp.x /= bk; lp.z /= bk; lc.x /= bk * (0.45f + 0.55f * sw); lc.z /= bk; }
    DN_P = lp; DN_C = lc; DN_H = lh;
    // le pantalon : le bassin, les cuisses, les pattes d'éléphant
    int idol = R->L.idol;       // idol : la silhouette de boys band (épaules carrées, taille fine, hanches étroites, longues jambes en pantalon ajusté)
    e = sdEll(lp, idol ? V(0.116f, 0.112f, 0.104f) : V(0.150f, 0.115f, 0.118f));
    if (idol) for (int s = 0; s < 2; s++) { v3 Hp = R->hip[s], K = R->kn[s], A = R->an[s], Tm = add(mix3(Hp, K, 0.38f), mul(R->pz, 0.020f * bk)), Cm = add(mix3(K, A, 0.30f), mul(R->pz, -0.024f * bk));       // de vraies jambes : la cuisse galbée, le genou, le mollet, la cheville fine, le revers du pantalon
        e = smin(e, dn_cone(p, Hp, Tm, 0.086f * bk, 0.091f * bk), 0.05f); e = smin(e, dn_cone(p, Tm, K, 0.091f * bk, 0.056f * bk), 0.03f);
        e = smin(e, dn_cone(p, K, Cm, 0.054f * bk, 0.064f * bk), 0.03f); e = smin(e, dn_cone(p, Cm, A, 0.064f * bk, 0.039f * bk), 0.03f);
        e = smin(e, dn_cone(p, add(A, V(0, 0.035f, 0)), add(A, V(0, -0.012f, 0)), 0.043f * bk, 0.049f * bk), 0.006f); }
    else for (int s = 0; s < 2; s++) { e = smin(e, dn_cone(p, R->hip[s], R->kn[s], (idol ? 0.088f : 0.090f) * bkl, (idol ? 0.058f : 0.064f) * bkl), 0.06f); e = smin(e, dn_cone(p, R->kn[s], add(R->an[s], V(0, idol ? 0.0f : -0.04f, 0)), (idol ? 0.056f : 0.060f) * bkl, idol ? 0.043f * bkl : R->L.suit ? 0.075f * bkl : 0.118f), 0.035f); }
    if (shp == 3) e = smin(e, sdEll(V(lp.x, lp.y + 0.01f, lp.z), V(0.215f, 0.150f, 0.180f)), 0.05f);       // la poire : de grosses hanches
    UN(e, D_PANTS)
    for (int s = 0; s < 2; s++) { v3 f = sub(p, R->ft[s]); f = V(f.x * R->fc[s] - f.z * R->fs[s], f.y, f.x * R->fs[s] + f.z * R->fc[s]); float hl = 0.09f * R->P.heel[s];       // les bottines à plateau
        UN(fminf(sdEll(V(f.x, f.y - 0.075f - hl * (0.5f - f.z * 4.0f), f.z - 0.055f), V(0.062f, 0.050f, 0.135f)), sdBox(V(f.x, f.y - 0.022f - hl * clampf(0.5f - f.z * 4.0f, 0, 1), f.z - 0.05f), V(0.052f, 0.022f, 0.118f)) - 0.008f), D_BOOT) }
    // la chemise : la taille fine, le buste, les bras
    if (idol) {       // une vraie veste : le buste en trapèze, les pectoraux, la taille cintrée, la basque ouverte devant, les épaules carrées, des bras galbés
        float tt = sstep(-0.21f, 0.09f, lc.y), w = mixf(0.100f, 0.196f, tt), dp = mixf(0.082f, 0.100f, tt);
        e = fmaxf((len2(lc.x / w, lc.z / dp) - 1) * fminf(w, dp), fabsf(lc.y + 0.045f) - 0.165f) - 0.022f;
        e = smin(e, sdEll(V(fabsf(lc.x) - 0.078f, lc.y - 0.035f, lc.z - 0.062f), V(0.082f, 0.062f, 0.050f)), 0.04f);
        e = smin(e, sdCap(p, add(R->pel, mul(R->py, 0.07f)), sub(R->ch, mul(R->cy, 0.10f)), 0.072f * bk), 0.07f);
        { float ht = sstep(0.17f, -0.02f, lp.y), hw = mixf(0.088f, 0.130f, ht), hz = mixf(0.078f, 0.112f, ht), hem = fmaxf((len2(lp.x / hw, lp.z / hz) - 1) * fminf(hw, hz), fmaxf(-0.035f - lp.y, lp.y - 0.18f)) - 0.006f;
            hem = fmaxf(hem, -fmaxf(fabsf(lp.x) - 0.50f * (0.050f - lp.y), -lp.z)); e = smin(e, hem, 0.03f); }
        for (int s = 0; s < 2; s++) { v3 Sh = R->sh[s], E = R->el[s], Wr = mix3(E, R->ha[s], 0.90f), Bm = mix3(Sh, E, 0.42f), Fm = mix3(E, R->ha[s], 0.28f);
            e = smin(e, len(sub(p, add(Sh, mul(R->cy, 0.010f)))) - 0.066f * bk, 0.03f);
            e = smin(e, dn_cone(p, Sh, Bm, 0.060f * bk, 0.064f * bk), 0.03f); e = smin(e, dn_cone(p, Bm, E, 0.064f * bk, 0.046f * bk), 0.03f);
            e = smin(e, dn_cone(p, E, Fm, 0.046f * bk, 0.053f * bk), 0.02f); e = smin(e, dn_cone(p, Fm, Wr, 0.053f * bk, 0.041f * bk), 0.02f); }
    } else {
    e = smin(sdCap(p, add(R->pel, mul(R->py, 0.07f)), sub(R->ch, mul(R->cy, 0.04f)), (idol ? 0.070f : 0.080f) * bk), sdEll(V(lc.x, lc.y - 0.02f, lc.z), idol ? V(0.186f, 0.150f, 0.100f) : V(0.180f, 0.150f, 0.112f)), idol ? 0.11f : 0.07f);
    if (idol) e = smin(e, sdEll(V(lp.x, lp.y - 0.075f, lp.z), V(0.118f, 0.080f, 0.104f)), 0.05f);       // le bas de la veste, sur les hanches
    if (idol) for (int s = 0; s < 2; s++) e = smin(e, len(sub(p, add(R->sh[s], mul(R->cy, 0.012f)))) - 0.060f * bk, 0.03f);       // la veste épaulée
    for (int s = 0; s < 2; s++) { e = smin(e, dn_cone(p, R->sh[s], R->el[s], 0.054f * bkl, 0.043f * bkl), 0.045f); e = smin(e, dn_cone(p, R->el[s], R->ha[s], 0.042f * bkl, R->L.suit ? 0.044f * bkl : 0.050f), 0.02f); }
    }
    if (shp == 1) e = smin(e, sdEll(V(lp.x, lp.y - 0.135f, lp.z - 0.045f), V(0.175f, 0.165f, 0.170f)), 0.06f);       // la bedaine
    if (shp == 2) e = smin(e, len(sub(p, mix3(R->pel, R->ch, 0.55f))) - 0.245f * bk, 0.04f);       // la boule
    if (shp == 3) e = smin(e, sdEll(V(lp.x, lp.y - 0.10f, lp.z), V(0.185f, 0.13f, 0.150f)), 0.08f);
    UN(e, D_SHIRT)
    if (idol) { e = sdCap(p, R->nk, add(R->nk, mul(R->cy, 0.040f)), 0.053f);       // le col de chemise, les poignets blancs qui dépassent
        for (int s = 0; s < 2; s++) e = fminf(e, dn_cone(p, mix3(R->el[s], R->ha[s], 0.86f), mix3(R->el[s], R->ha[s], 1.03f), 0.035f * bk, 0.037f * bk));
        UN(e, D_WHITE) }
    // la peau : le cou, les mains, la queue, la tête
    e = sdCap(p, R->nk, sub(R->hc, mul(R->hy, 0.12f)), 0.043f);
    for (int s = 0; s < 2; s++) { v3 dr = nrm(sub(R->ha[s], R->el[s])); e = fminf(e, sdEll(sub(p, add(R->ha[s], mul(dr, 0.055f))), V(0.050f, 0.056f, 0.050f))); }
    e = fminf(e, len2(len2(lp.y - 0.03f, lp.z + 0.165f + 0.02f * sinf(R->P.tail)) - 0.036f, lp.x - 0.02f * sinf(lp.y * 40.0f + R->P.tail)) - 0.013f);
    { float hd = sdEll(lh, V(0.220f, 0.195f, 0.200f));
        hd = smin(hd, sdEll(V(ah.x - 0.165f - 0.55f * (ah.y - 0.17f), ah.y - 0.215f, ah.z + 0.02f + 0.25f * (ah.y - 0.17f)), V(0.085f, 0.120f, 0.028f)), 0.03f);       // les oreilles, en pointe, un peu tombantes
        e = fminf(e, hd * hk); }
    UN(e, D_SKIN)
    UN(smin(cylZ(V(lh.x, lh.y + 0.035f, lh.z - 0.205f), 0.090f, 0.050f) - 0.012f, sdEll(lh, V(0.220f, 0.195f, 0.200f)) + 0.02f, 0.02f) * hk, D_SNOUT)
    UN((len(sub(ah, V(0.088f, 0.060f, 0.158f))) - 0.056f) * hk, D_EYE)
    if (R->L.cut) { v3 hq = add(mul(lh, 1 / 0.60f), V(0, 0.22f, 0.03f)), a = V(fabsf(hq.x), hq.y, hq.z); float hd = 1e9f;       // les coupes des Ranga Boyz, reprises du citoyen (sa tête est 1,67 fois plus grosse)
        switch (R->L.cut) {
        case DC_SPIKES: for (int k = 0; k < 3; k++) { float th = 0.50f * k, sx = sinf(th), cx = cosf(th); v3 b0 = V(0.15f * sx, 0.21f + 0.29f * cx, 0.0f), dr = V(sx, cx, 0.10f); float t = clampf(dot(sub(a, b0), dr), 0, 0.30f); hd = fminf(hd, len(sub(a, add(b0, mul(dr, t)))) - (0.070f * (1 - t / 0.30f) + 0.006f)); } break;
        case DC_MOHAWK: hd = sdEll(sub(a, V(0, 0.500f, -0.03f)), V(0.042f, 0.185f + 0.022f * sinf(hq.z * 44.0f), 0.27f)); break;
        case DC_QUIFF: hd = smin(sdEll(sub(a, V(0, 0.535f, 0.05f)), V(0.120f, 0.075f, 0.200f)), sdEll(sub(a, V(0, 0.640f, 0.200f)), V(0.130f, 0.120f, 0.105f)), 0.07f); break;
        case DC_AFRO: hd = fmaxf(len(sub(a, V(0, 0.580f, -0.02f))) - 0.215f - 0.012f * sinf(hq.x * 34.0f) * sinf(hq.y * 34.0f + 1.0f) * sinf(hq.z * 34.0f), 0.47f - a.y); break;
        case DC_FRINGE: hd = smin(sdEll(sub(hq, V(0.02f, 0.540f, 0.07f)), V(0.175f, 0.060f, 0.165f)), sdCap(hq, V(-0.10f, 0.545f, 0.20f), V(0.215f, 0.415f, 0.285f), 0.058f), 0.05f); break;
        default: hd = fminf(len(sub(a, V(0, 0.625f, -0.03f))) - 0.105f, cylY(sub(a, V(0, 0.525f, -0.03f)), 0.060f, 0.030f)); break; }
        UN(hd * 0.60f * hk, D_HAIR) }
    UN(len2(len2(lc.x, lc.z - 0.045f) - 0.100f, lc.y - 0.120f - 0.25f * (lc.z - 0.045f) * -1.0f) - 0.0085f, D_GOLD)       // la chaîne en or
    return d;
}
// les danseurs posés dans la scène (jusqu'à huit) ; DNLOOK : l'allure du prochain qu'on pose
#define DN_MAX 8
static dn_rig_t DNRS[DN_MAX]; static v3 DNPOSS[DN_MAX]; static float DNCS[DN_MAX], DNSS[DN_MAX], DNSCS[DN_MAX]; static int DNN, DNON;
static const dn_look_t DN_MARTIN = {{0.93f, 0.43f, 0.44f}, {0, 0, 0}, {0, 0, 0}, 0, DC_NONE, 1.0f, 1.0f}; static dn_look_t DNLOOK = {{0.93f, 0.43f, 0.44f}, {0, 0, 0}, {0, 0, 0}, 0, DC_NONE, 1.0f, 1.0f};
static void dn_place(const dn_pose_t *P, v3 pos, float yaw, float sc) { if (DNN >= DN_MAX) return; DNRS[DNN].L = DNLOOK; dn_rig(P, &DNRS[DNN]); DNPOSS[DNN] = pos; DNCS[DNN] = cosf(yaw); DNSS[DNN] = sinf(yaw); DNSCS[DNN] = sc; DNN++; DNON = 1; }
S v3 dn_local(v3 p, int k) { float dx = p.x - DNPOSS[k].x, dz = p.z - DNPOSS[k].z, sc = DNSCS[k]; return V((dx * DNCS[k] - dz * DNSS[k]) / sc, (p.y - DNPOSS[k].y) / sc, (dx * DNSS[k] + dz * DNCS[k]) / sc); }
S float dn_map(v3 p, int *m, float d) { for (int k = 0; k < DNN; k++) { int mm; float e = dn_sdf(&DNRS[k], dn_local(p, k), &mm) * DNSCS[k]; if (e < d) { d = e; *m = mm; } } return d; }
S int dn_who(v3 p) { int kb = 0; float db = 1e9f; for (int k = 0; k < DNN; k++) { v3 l = dn_local(p, k); float e = len2(l.x - DNRS[k].pel.x, l.z - DNRS[k].pel.z) * DNSCS[k]; if (e < db) { db = e; kb = k; } } return kb; }
