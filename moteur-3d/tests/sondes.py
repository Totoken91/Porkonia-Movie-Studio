"""Les trames sondes du moteur : 18 trames du film (deux par séquence) et 12 plans du clip, rendues, chronométrées, comparées.
  python3 sondes.py ref [BIN_DIR]        photographie l'état actuel dans tests/ref/ (à faire AVANT de toucher au moteur)
  python3 sondes.py cmp [BIN_DIR]        rend dans tests/new/ et compare à tests/ref/ : pixels changés, écart max, PSNR, vitesse
  python3 sondes.py planche              planche contact de tests/ref/ (sondes.png)
Variables : ONLY="film300 clip42" pour un sous-ensemble ; CLIPK="0 42" pour choisir les plans du clip."""
import sys, os, time, subprocess, json, numpy as np
HERE = os.path.dirname(os.path.abspath(__file__)); ENG = os.path.dirname(HERE)
sys.path.insert(0, os.path.join(ENG, "clip"))
from clip_edl import SHOTS
FILM = [(t0, f) for t0, n in [(0, 2400), (100, 4025), (200, 2075), (300, 1800), (400, 1800), (500, 1800), (600, 1800), (700, 1800), (800, 2500)]
        for f in (int(n * 0.3) // 2 * 2, int(n * 0.72) // 2 * 2 + 1)]
FIRST = {}
for s in SHOTS: FIRST.setdefault(s["K"], s)
CLIPK = [int(k) for k in os.environ.get("CLIPK", "0 1 4 10 13 20 23 28 33 38 42 45").split()]


def probes():
    for t0, f in FILM: yield f"film{t0}_{f}", "filmc", [str(f), str(f + 1), str(t0)], {}
    for K in CLIPK:
        sh = FIRST[K]; f = sh["n"] // 2
        env = {"CLIP": f'{sh["t0"]:.4f} {sh["D"]:.4f} {sh["e"]} {sh["th"]} {sh["pat"]} {sh["var"]} {sh["mult"]} {sh["s0"]}'}
        yield f"clip{K}", "filmclip", [str(f), str(f + 1), f'{900 + 100 * K + 1:.2f}'], env


def render(bindir, out):
    os.makedirs(out, exist_ok=True); res = {}; only = os.environ.get("ONLY", "").split()
    for name, b, args, env in probes():
        if only and not any(name.startswith(o) for o in only): continue
        t = time.time()
        r = subprocess.run([os.path.join(bindir, b)] + args, cwd=ENG, env=dict(os.environ, **env), capture_output=True)
        res[name] = time.time() - t
        if r.returncode or len(r.stdout) != 576 * 288 * 3: print(f"{name} : ÉCHEC rc={r.returncode} {r.stderr.decode()[-300:]}"); continue
        open(f"{out}/{name}.rgb", "wb").write(r.stdout)
        print(f"{name:14s} {res[name]:5.2f} s", flush=True)
    json.dump(res, open(f"{out}/temps.json", "w"))


def compare(ref, new):
    rt, nt = json.load(open(f"{ref}/temps.json")), json.load(open(f"{new}/temps.json"))
    same = 0; tr = tn = 0
    for name in nt:
        if not os.path.exists(f"{ref}/{name}.rgb") or not os.path.exists(f"{new}/{name}.rgb"): continue
        a = np.fromfile(f"{ref}/{name}.rgb", np.uint8).astype(int); b = np.fromfile(f"{new}/{name}.rgb", np.uint8).astype(int)
        d = np.abs(a - b).reshape(-1, 3).max(1); tr += rt[name]; tn += nt[name]
        if d.max() == 0: same += 1; tag = "identique"
        else:
            psnr = 10 * np.log10(255 ** 2 / np.mean((a - b) ** 2)); tag = f"{(d > 0).sum():6d} px changés, {(d > 8).sum():5d} visibles, max {d.max():3d}, PSNR {psnr:4.1f}"
        print(f"{name:14s} {rt[name]:5.2f} -> {nt[name]:5.2f} s   {tag}")
    print(f"\n{same} sondes identiques à l'octet sur {len(nt)} ; temps total {tr:.1f} s -> {tn:.1f} s (x{tr / max(tn, 1e-9):.2f})")
    return same == len(nt)


def sheet(d):
    from PIL import Image, ImageDraw
    fs = sorted(f for f in os.listdir(d) if f.endswith(".rgb")); cols = 6
    sh = Image.new("RGB", (cols * 288, (len(fs) + cols - 1) // cols * 162))
    for i, f in enumerate(fs):
        im = Image.fromarray(np.fromfile(f"{d}/{f}", np.uint8).reshape(288, 576, 3)).resize((288, 162))
        ImageDraw.Draw(im).text((3, 3), f[:-4], fill=(255, 255, 0)); sh.paste(im, ((i % cols) * 288, (i // cols) * 162))
    sh.save(os.path.join(HERE, "sondes.png")); print("sondes.png")


if __name__ == "__main__":
    cmd = sys.argv[1] if len(sys.argv) > 1 else "cmp"; bindir = os.path.abspath(sys.argv[2]) if len(sys.argv) > 2 else ENG
    if cmd == "ref": render(bindir, f"{HERE}/ref")
    elif cmd == "planche": sheet(f"{HERE}/ref")
    else: render(bindir, f"{HERE}/new"); sys.exit(0 if compare(f"{HERE}/ref", f"{HERE}/new") else 1)
