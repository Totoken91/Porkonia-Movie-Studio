# Porkonia Movie Studio

Contexte partagé et outils pour produire les courts métrages Porkonia / PorkOS avec Claude ou Codex.

**Base retenue : scénario causal → storyboard et animatique → références digicam 4:3 validées → Seedance 2.0 Fast avec voix natives → mix sonore → VHS PorkOS.**

Décision de Kenny, 7 octobre 2026 : voix natives par défaut, Kevin inclus. ElevenLabs est conservé comme secours ciblé.

## Commencer ici

- [Guide de réalisation, storyboard, animatique et timing](docs/guide-realisation.md)
- [Workflow actuel et reprise dans une nouvelle conversation](docs/workflow-production.md)
- [Consignes communes aux agents](AGENTS.md) et [entrée Claude](CLAUDE.md)
- [Compétence portable de réalisation](skills/porkonia-video-direction/SKILL.md)
- [Tests, prompts exacts et avis de Kenny](docs/tests-qualite-prix.md)
- [Conseils de voix françaises et devis datés](skills/porkonia-video-direction/references/voix-francaises.md)
- [Recette VHS historique](docs/workflow-courts-porkos.md)
- [PorkOS et assets canoniques](https://github.com/Totoken91/porkonia-os/tree/porkos)

Le test précédent préférait Fast + ElevenLabs ; la décision actuelle privilégie le natif pour son naturel. Veo Lite et Mini sont écartés. Vérifier prix, accès et schémas avant chaque dépense. La version native de La Cible a été livrée ; ses prises restent à juger à l’écoute.

## Traitement VHS

Installer FFmpeg, Node.js et les dépendances avec `npm install`. Utiliser des dossiers frames/vhs vides par plan et attendre que le script finisse avant l'encodage.

```bash
mkdir -p frames vhs
ffmpeg -i plan-brut.mp4 -vf fps=25 frames/%04d.png
node scripts/render-vhs.cjs frames
ffmpeg -framerate 25 -i vhs/%04d.png -i plan-brut.mp4 -map 0:v:0 -map '1:a:0?' -c:v libx264 -crf 18 -pix_fmt yuv420p -c:a aac -b:a 192k -shortest -movflags +faststart plan-vhs.mp4
```

Cette commande conserve la piste audio du brut ; pour un mix final séparé, employer ce fichier audio comme seconde entrée. Le script traite en 384×288 et exporte en 768×576 à 25 fps, sans bombé CRT. Conserver l'export approuvé : le bruit VHS est aléatoire.

Le script historique à la racine reste disponible ; scripts/render-vhs.cjs est la copie organisée identique. La bible et les médias originaux ne sont pas inclus dans ce dépôt.
