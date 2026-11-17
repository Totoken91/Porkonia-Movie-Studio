"""Rendu 3D des plans du clip, un fichier par plan (r/pNNN.rgb, trames de 576 x 288), dans l'ordre du montage.
Reprenable : un plan déjà complet est sauté, un plan entamé reprend à sa dernière image entière.
  python3 clip_render.py            tout le clip
  python3 clip_render.py 10 20      les plans 10 à 19
  python3 clip_render.py etat       où on en est"""
import sys, os, time, subprocess
sys.argv.insert(1, "stills"); import clip_post as cp; sys.argv.pop(1)       # (on n'emprunte à clip_post que la commande du moteur)
from clip_edl import SHOTS
NFLD, R, ENG = cp.NFLD, cp.R, cp.ENG
def done(sh):
    p = f'{R}/p{sh["i"]:03d}.rgb'; return os.path.getsize(p) // NFLD if os.path.exists(p) else 0
if len(sys.argv) > 1 and sys.argv[1] == "etat":
    tot = sum(s["n"] for s in SHOTS); ok = sum(min(done(s), s["n"]) for s in SHOTS)
    print(f"{ok} images sur {tot} ({100 * ok / tot:.1f} %) ; plans complets : {sum(done(s) >= s['n'] for s in SHOTS)} sur {len(SHOTS)}"); sys.exit(0)
a, b = (int(sys.argv[1]), int(sys.argv[2])) if len(sys.argv) > 2 else (0, len(SHOTS))
t0 = time.time(); made = 0
for sh in SHOTS[a:b]:
    n0 = min(done(sh), sh["n"]); p = f'{R}/p{sh["i"]:03d}.rgb'
    if n0 >= sh["n"]: continue
    if os.path.exists(p): os.truncate(p, n0 * NFLD)
    cmd, env = cp.render_cmd(sh, n0, sh["n"])
    with open(p, "ab") as f: subprocess.run(cmd, cwd=ENG, env=env, stdout=f, stderr=subprocess.DEVNULL, check=True)
    made += sh["n"] - n0
    print(f'plan {sh["i"]:3d} (K {sh["K"]:2d}) : {sh["n"] - n0} images ; total {made} en {time.time() - t0:.0f} s ({(time.time() - t0) / max(made, 1):.2f} s par image)', flush=True)
print("FINI", flush=True)
