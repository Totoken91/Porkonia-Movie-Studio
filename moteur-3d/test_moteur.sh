#!/bin/sh
# Batterie de tests du moteur (22 nov. 2026). À lancer avant et après toute modification :
#   ./test_moteur.sh ref     construit, vérifie, et photographie les 30 trames sondes (tests/ref/)
#   ./test_moteur.sh         construit, vérifie, et compare les sondes à la photo précédente
#   ASAN=1 ./test_moteur.sh  en plus : les sondes avec le détecteur de fautes mémoire et de calcul (lent : 3 min)
cd "$(dirname "$0")"; ok=0; ko=0
pass() { echo "  ok    $1"; ok=$((ok+1)); }
fail() { echo "  ÉCHEC $1"; ko=$((ko+1)); }
echo "== construction"
python3 tex.py > /dev/null && ./buildc.sh && ./buildclip.sh && [ -x filmc ] && [ -x filmclip ] && pass "filmc et filmclip construits" || { fail "construction"; exit 1; }
echo "== ligne de commande"
./filmc > /dev/null 2>&1; [ $? -eq 2 ] && pass "sans argument : message d'usage" || fail "sans argument"
./filmc 0 1 abc > /dev/null 2>&1; [ $? -eq 2 ] && pass "T0 illisible refusé" || fail "T0 illisible"
./filmc 0 1 nan > /dev/null 2>&1; [ $? -eq 2 ] && pass "T0 = nan refusé" || fail "T0 nan"
n=$(./filmc 5 2 300 2>/dev/null | wc -c); [ "$n" -eq 0 ] && pass "F0 > F1 : rien" || fail "F0 > F1"
n=$(cd /tmp && "$OLDPWD/filmc" 540 541 300 2>/dev/null | wc -c); [ "$n" -eq 497664 ] && pass "lancé depuis un autre dossier (txt.bin trouvé à côté du binaire)" || fail "autre dossier"
python3 - <<'P' && pass "caméra qui vise pile vers le bas : vraie image" || fail "caméra verticale"
import subprocess, os, numpy as np
r = subprocess.run(["./filmc", "540", "541", "300"], env=dict(os.environ, WCAM="0 30 0 0 0 0 1.5"), capture_output=True)
a = np.frombuffer(r.stdout, np.uint8).reshape(288, 576, 3).astype(float); raise SystemExit(0 if a.std() > 15 and a[:, :, 0].std(1).mean() > 5 else 1)
P
echo "== reprise et threads"
./filmc 540 543 700 > /tmp/tm_a.rgb 2>/dev/null; : > /tmp/tm_b.rgb; for f in 540 541 542; do ./filmc $f $((f+1)) 700 >> /tmp/tm_b.rgb 2>/dev/null; done
cmp -s /tmp/tm_a.rgb /tmp/tm_b.rgb && pass "rendu d'une traite = rendu trame par trame (reprise sûre)" || fail "reprise"
OMP_NUM_THREADS=1 ./filmc 540 541 700 2>/dev/null | cmp -s -n 497664 - /tmp/tm_a.rgb && pass "1 cœur = tous les cœurs" || fail "threads"
echo "== sondes"
if [ "$1" = "ref" ]; then python3 tests/sondes.py ref && pass "sondes photographiées dans tests/ref/"
elif [ -d tests/ref ]; then python3 tests/sondes.py cmp && pass "sondes identiques à la référence" || fail "sondes différentes (voir ci-dessus : normal si on a changé le rendu exprès)"
else echo "  (pas de référence : ./test_moteur.sh ref d'abord)"; fi
if [ -n "$ASAN" ]; then echo "== ASan / UBSan"
  for b in filmc filmclip; do gcc -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined -fopenmp -o tests/${b}_asan $b.c -lm || fail "construction ASan"; done
  mkdir -p tests/asan; for b in filmc filmclip; do cp tests/${b}_asan tests/asan/$b; done; ln -sf "$PWD/txt.bin" tests/asan/
  python3 - <<'P' && pass "aucune faute mémoire ni calcul indéfini" || fail "ASan/UBSan : voir tests/asan.log"
import subprocess, os, sys
sys.path.insert(0, "tests"); import sondes
bad = 0
for name, b, args, env in sondes.probes():
    r = subprocess.run([f"tests/asan/{b}"] + args, env=dict(os.environ, ASAN_OPTIONS="detect_leaks=0", **env), capture_output=True)
    e = r.stderr.decode(errors="replace")
    if r.returncode or "runtime error" in e or "AddressSanitizer" in e: bad += 1; print(name, e[-400:])
raise SystemExit(1 if bad else 0)
P
fi
echo "== bilan : $ok ok, $ko échec(s)"; [ $ko -eq 0 ]
