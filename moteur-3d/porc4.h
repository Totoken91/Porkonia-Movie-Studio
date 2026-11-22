// ============================ LE PORC DE MARCHE : le cochon à quatre pattes de Porkonia ============================
// Modèle standard n° 2, FIGÉ (seule retouche : la couronne, redessinée à la demande de Kenny le 22 nov. 2026). C'est le cochon de la Grande Parade du film « 24 heures à Porkonia », recopié tel quel
// (Kenny, 10 nov. 2026 : « ne les change pas, ils sont très mignons et cool, tout simples à 4 pattes »).
// Il sert de représentation générale de Porkonia : parades, foules, monuments, emblèmes en volume. Ne retoucher ni sa forme ni ses couleurs ;
// pour un personnage qui joue, parle ou porte des habits, prendre le citoyen (citizen.h).
// Repère : pattes sur y = 0, regarde vers +z, 2,6 de long, 1,8 de haut avec la couronne. Pour le changer de taille : p4_sdf(p / s, ...) * s.
//   ph    phase des pattes (elles battent en sin(ph)) ; amp : 1 = il marche, 0 = à l'arrêt
//   skin  matière du corps ; black : matière de l'œil ; gold : matière de la couronne (numéros de matière du moteur qui l'emploie)
// La marche de la parade : le corps monte de 0,06 * |sin(ph)|, avec ph = pi * T / 0,5625 (un pas par temps de la musique).
// Dans la parade : le meneur est chromé, 6 % des cochons sont en or, 14 % en chrome, les autres en rose.
// Matières officielles : rose = couleur (0,95 0,47 0,52), reflet 5 %, brillance 40 ; œil = noir (0,02) très brillant ;
//                        or = métal (1,0 0,74 0,30) ; chrome = métal (0,92 0,93 0,96).
static float p4_sdf(v3 p, float ph, float amp, int skin, int black, int gold, int *m) {
    *m = skin;
    float bd = len(sub(p, V(0, 0.95f, 0.25f))) - 1.8f;
    if (bd > 0.9f) return bd;
    float sg = p.x > 0 ? 1.0f : -1.0f;
    p.x = fabsf(p.x);
    float d = sdEll(sub(p, V(0, 0.9f, 0)), V(0.62f, 0.6f, 0.95f));
    d = smin(d, len(sub(p, V(0, 1.05f, 0.95f))) - 0.48f, 0.15f);
    d = smin(d, cylZ(sub(p, V(0, 0.98f, 1.42f)), 0.19f, 0.11f) - 0.035f, 0.06f);
    d = fmaxf(d, -(len(sub(p, V(0.075f, 0.99f, 1.585f))) - 0.045f));
    v3 q = sub(p, V(0.31f, 1.5f, 0.8f));
    float c = 0.8776f, s = 0.4794f;
    d = smin(d, sdEll(V(c * q.x + s * q.y, -s * q.x + c * q.y, q.z), V(0.13f, 0.23f, 0.06f)), 0.06f);
    float sw = 0.2f * amp * sinf(ph) * sg;
    d = smin(d, sdCap(p, V(0.32f, 0.6f, 0.55f), V(0.32f, 0.14f, 0.55f + sw), 0.14f), 0.08f);
    d = smin(d, sdCap(p, V(0.32f, 0.6f, -0.55f), V(0.32f, 0.14f, -0.55f - sw), 0.14f), 0.08f);
    q = sub(p, V(0, 1.12f, -0.99f));
    float tr = len2(q.y, q.z) - 0.12f;
    d = fminf(d, len2(tr, q.x) - 0.035f);
    float e = len(sub(p, V(0.2f, 1.22f, 1.34f))) - 0.055f;
    if (e < d) { d = e; *m = black; }
    q = sub(p, V(0, 1.53f, 0.9f));
    if (len(q) < 0.7f) {    // la couronne (redessinée le 22 nov. 2026, Kenny : « garde-les simples mais moins moches ») :
        // un bandeau plein et droit, un bourrelet en bas et un en haut, cinq pointes triangulaires pleines, une perle d'or à chaque pointe
        float r = len2(q.x, q.z), sec = 6.2831853f / 5, a = atan2f(q.z, q.x) - 1.5707963f;
        a -= sec * roundf(a / sec);
        float s = r * a, lx = r - 0.245f;
        float cr = box2(fabsf(lx) - 0.022f, fabsf(q.y + 0.005f) - 0.055f) - 0.006f;
        cr = fminf(cr, fminf(len2(lx, q.y + 0.065f) - 0.022f, len2(lx, q.y - 0.048f) - 0.015f));
        float y = q.y - 0.05f, w = 0.072f * (1 - y / 0.115f);
        float tri = fmaxf(fmaxf(fmaxf(fabsf(s) - w, -y), y - 0.115f) * 0.85f, fabsf(lx) - 0.016f) - 0.006f;
        cr = fminf(cr, fminf(tri, len(V(lx, q.y - 0.185f, s)) - 0.026f));
        if (cr < d) { d = cr; *m = gold; }
    }
    return d;
}
