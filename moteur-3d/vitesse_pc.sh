#!/bin/sh
# Mesure la vitesse du moteur sur cette machine (22 nov. 2026) et dit combien de temps prendraient les films.
#   ./vitesse_pc.sh          construit, rend les 30 trames sondes, compare au serveur de Claude (2 cœurs : 20,6 s pour les 30)
# Il faut gcc avec OpenMP et python3 (numpy). Sous Windows : dans WSL (Ubuntu). Sous macOS : gcc de Homebrew (CC=gcc-14 ./vitesse_pc.sh).
cd "$(dirname "$0")"
command -v ${CC:-gcc} >/dev/null || { echo "gcc introuvable (Ubuntu/WSL : sudo apt install build-essential ; macOS : brew install gcc puis CC=gcc-14)"; exit 1; }
python3 -c "import numpy" 2>/dev/null || { echo "numpy manquant : pip install numpy"; exit 1; }
if [ -n "$CC" ]; then sed -i.bak "s/^gcc /$CC /" buildc.sh buildclip.sh; fi
python3 tex.py > /dev/null && ./buildc.sh && ./buildclip.sh || exit 1
N=$(python3 -c "import os; print(os.cpu_count())"); echo "cœurs vus : $N"
python3 tests/sondes.py ref > /tmp/vitesse.log 2>&1
python3 - <<'P'
import json
t = sum(json.load(open("tests/ref/temps.json")).values()); k = 20.6 / t
print(f"30 trames sondes : {t:.1f} s (serveur de Claude, 2 cœurs : 20,6 s) -> {k:.1f} fois plus rapide")
per = 0.75 / k      # une trame moyenne du film, en s, sur cette machine
for nom, trames in [("une séquence de 36 s", 1800), ("le clip de DJ Viteau (4 min 28)", 13400), ("un documentaire de 20 min", 60000)]:
    m = trames * per / 60; print(f"  {nom:34s} ~ {m:5.0f} min" if m < 120 else f"  {nom:34s} ~ {m / 60:5.1f} h")
P
