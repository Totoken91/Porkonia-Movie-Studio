#!/bin/sh
# Les tronçons du film entier, l'un après l'autre (ceux qui existent déjà sont sautés), puis le collage.
cd "$(dirname "$0")"
for K in 0 1 2 3 4 5; do [ -f film_$K.mp4 ] || python3 filmpost.py $K > film_$K.log 2>&1; done
while [ "$(stat -c %s fin.rgb)" -lt $((2500*497664)) ]; do sleep 10; done
[ -f film_6.mp4 ] || python3 filmpost.py 6 > film_6.log 2>&1
python3 filmpost.py audio > film_audio.log 2>&1
python3 filmpost.py join > film_join.log 2>&1
