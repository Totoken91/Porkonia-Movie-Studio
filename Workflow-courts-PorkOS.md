# Workflow validé — courts métrages PorkOS

Validé par Kenny le 7 octobre 2026 : « le rendu est excellent ».

## Méthode à reprendre
1. Préparer le sujet et consulter les assets canoniques de Porkonia. Ne pas inventer des produits ou des visages lorsqu'une référence existe.
2. Créer l'image de référence avec la génération d'images de ChatGPT, en utilisant les références réelles. Pour Douzi Ambrée : bouteille brune, étiquette bordeaux et or, cochon couronné, texte Douzi Ambrée. Référence du test : public/tv/douzi-ambree-bouteille.jpg dans Totoken91/porkonia-os, branche porkos.
3. Garder une image source sans tracking incrusté : le traitement VHS est temporel et intervient après animation. Cadre 4:3, décor crédible, objets précis, lumière et texture de caméscope amateur. Éviter le rendu showroom ou publicité générique.
4. Importer l'image dans Higgsfield et la fournir comme start_image. Demander une animation limitée, conserver bouteilles, étiquettes, objets, composition. Décrire une seule action simple et son timing. Éviter zooms et mouvements inutiles.
5. Vérifier le coût avant de générer. Le test validé utilise Seedance 1.5 Pro, 4 secondes, 480p, 4:3, audio désactivé, coût annoncé 2,4 crédits. Ces prix et disponibilités doivent être revérifiés. Cinema Studio n'est pas requis : son test coûtait 14 crédits.
6. Télécharger le plan brut et extraire les images à 25 images/seconde.
7. Appliquer render-vhs.cjs : adaptation des étapes 2 à 4 d'EcranVhs.tsx et de lineOffset de vhs.ts de Channel Pork. Effet intégré aux pixels de la vidéo, pas un overlay de lecteur.
8. Exporter H.264, yuv420p, 25 fps, 768×576, CRF 18, faststart. Conserver le brut pour comparaison et pour refaire l'étalonnage. Vérifier quelques images et la durée.
9. Ajouter son et montage selon le film. Le test validé est silencieux ; le son n'est pas encore une recette validée.

## Effet VHS validé
Résolution de traitement 384×288 ; chrominance 64×288, étalée horizontalement et décalée de 4 pixels ; bavure supplémentaire à droite ; luminance contrastée avec léger flou ; halos et images fantômes décalées ; dominante chaude ; ondulation horizontale par ligne ; torsion du haut ; commutation des têtes dans les 10 dernières lignes ; bande de tracking mobile ; drop-outs blancs ; bruit et noirs délavés légèrement violacés. Pas de bombé CRT ni de scanlines artificielles : le traitement simule la cassette, pas un moniteur.

Pour ce test : bande de tracking entre 1,25 et 2,65 secondes, force 0,55. Ne pas répéter mécaniquement ce timing pour chaque plan du film : espacer les défauts et varier leur position sur la timeline complète.

## Réutiliser le script
Dépendances : ffmpeg ; Node.js avec @napi-rs/canvas.

```bash
mkdir -p frames vhs
ffmpeg -i plan-brut.mp4 -vf fps=25 frames/%04d.png
node render-vhs.cjs frames
ffmpeg -framerate 25 -i vhs/%04d.png -c:v libx264 -crf 18 -pix_fmt yuv420p -movflags +faststart plan-vhs.mp4
```

Le dossier vhs doit être frère du dossier frames. Partir de dossiers vides pour éviter les images d'un ancien plan. Le script emploie un bruit aléatoire : conserver l'export validé pour retrouver exactement le même résultat.

## Sources et limites
Dépôt : https://github.com/Totoken91/porkonia-os/tree/porkos
Révision consultée : 7ca5bd21c2c50650d401ec35c2e3caf2011288b2.
Sources : src/apps/channel-pork/EcranVhs.tsx et src/apps/channel-pork/vhs.ts.
Test final validé : Porkonia-test-VHS.mp4.
L'animation du test a déplacé le cadrage davantage que demandé : renforcer les contraintes de caméra et vérifier la conservation des accessoires pour les futurs plans.
Recherche native Higgsfield : preset « 2000's paparazzi » décrit comme VHS ; aucun filtre VHS général trouvé dans les outils consultés. Rechercher à nouveau si les outils évoluent ; ne pas remplacer automatiquement cette recette validée.

## Direction permanente
Pour les courts PorkOS : références créées ici → animation Higgsfield → traitement VHS de PorkOS. Ne pas repartir sur une vidéo texte seule avec des assets génériques. Utiliser les assets canoniques et garder les personnages, produits et lieux cohérents entre les plans.
