// Le cochon standard qui tourne sur lui-meme en trottinant, sur fond transparent (RGBA 256x256), pour le titrage.
#include "p3d_core.h"
static float ANG, PH;
static inline V roty(V p, float c, float s) { return VV(p.x * c - p.z * s, p.y, p.z * c + p.x * s); }
static float map(V p, int *m) {
    V q = roty(p, cosf(ANG), sinf(ANG)); q.z += 0.25f; q.y -= 0.10f * fabsf(sinf(PH));
    return pig(q, PH, M_PINK, m);
}
static V sky(V rd) { return mixv(VV(0.55f, 0.35f, 0.30f), VV(0.35f, 0.40f, 0.65f), clampf(rd.y * 0.5f + 0.5f, 0, 1)); }
static void material(int m, V p, V n, Mat *o) {}
static void scene_init(void) {
    LDIR = norm(VV(-0.5f, 0.75f, -0.6f)); SUNC = VV(1.7f, 1.45f, 1.2f);
    AMBU = VV(0.38f, 0.34f, 0.42f); AMBD = VV(0.18f, 0.10f, 0.10f); AMBC = VV(0.04f, 0.03f, 0.03f);
    RIMC = VV(0.55f, 0.40f, 0.35f); RIMD = norm(VV(0.6f, 0.3f, 1.0f));
    FOGC = VV(0, 0, 0); FOGD = 0.0f; MAXD = 30; STEPS = 120; EXPO = 1.15f; CONTRAST = 0.45f; METAL_LIT = 0.5f; METAL_ENV = 0.9f;
}
static void frame_setup(V *cam, V *tgt, float *focal) {}
static int hitp(V ro, V rd) {
    float t = 0.02f; int m;
    for (int i = 0; i < STEPS; i++) { float d = map(add(ro, mul(rd, t)), &m); if (d < 0.0012f * t + 0.0008f) return 1; t += d * 0.92f; if (t > MAXD) break; }
    return 0;
}
int main(int argc, char **argv) {
    int NFR = atoi(argv[1]);
    scene_init();
    V cam = VV(0, 2.5f, -6.4f), tgt = VV(0, 0.9f, 0); float focal = 2.3f;
    V fw = norm(sub(tgt, cam)), rt = norm(cross(VV(0, 1, 0), fw)), up = cross(fw, rt);
    unsigned char *buf = malloc(W * H * 4);
    for (int f = 0; f < NFR; f++) {
        ANG = 6.2832f * f / NFR; PH = 4 * ANG;
        #pragma omp parallel for schedule(dynamic, 4)
        for (int y = 0; y < H; y++)
            for (int x = 0; x < W; x++) {
                V acc = VV(0, 0, 0); float a = 0;
                for (int s = 0; s < 4; s++) {
                    float px = (x + 0.25f + 0.5f * (s & 1)) / W * 2 - 1, py = 1 - (y + 0.25f + 0.5f * (s >> 1)) / H * 2;
                    V rd = norm(add(mul(fw, focal), add(mul(rt, px), mul(up, py))));
                    if (!hitp(cam, rd)) continue;
                    V c = trace(cam, rd, 0); acc = add(acc, VV(tone(c.x), tone(c.y), tone(c.z))); a += 1;
                }
                unsigned char *o = buf + (y * W + x) * 4;
                float k = a > 0 ? 255.0f / a : 0;
                o[0] = (unsigned char)fminf(acc.x * k, 255); o[1] = (unsigned char)fminf(acc.y * k, 255); o[2] = (unsigned char)fminf(acc.z * k, 255);
                o[3] = (unsigned char)(a * 63.75f);
            }
        fwrite(buf, 1, W * H * 4, stdout);
    }
    return 0;
}
