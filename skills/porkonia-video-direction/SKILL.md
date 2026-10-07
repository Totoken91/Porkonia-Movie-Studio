---
name: porkonia-video-direction
description: Préparer et évaluer les courts métrages Porkonia/PorkOS avec Higgsfield, Seedance, références digicam, dialogues ElevenLabs et finition VHS. Utiliser pour écrire les prompts vidéo, corriger les plans statiques, organiser les actions et sons, ou consolider ce workflow.
---

# Réalisation vidéo Porkonia

Lire [les sources et résultats](references/direction.md) avant de choisir la méthode. Relire la bible visuelle et les pages des personnages depuis les sources accessibles avant de créer de nouveaux assets. Ne pas inventer les éléments canoniques manquants.

Lire [voix françaises et budget](references/voix-francaises.md) pour prompter ElevenLabs v4, éviter les accents indésirables et choisir un candidat vidéo à audio natif. Comparer le coût total incluant lip sync et reprises, sans confondre crédits ElevenLabs et Higgsfield.

## Préparer le plan

1. Identifier le changement visible entre début et fin. Définir une action du corps ou une interaction avec un objet ; ne pas compter bouche, zoom ou VHS comme action narrative.
2. Décrire deux ou trois étapes causales compatibles avec la durée : déclencheur, réaction, conséquence. Mesurer la prise vocale et laisser une marge avant/après. Réduire les étapes plutôt que demander trop de gestes simultanés.
3. Vérifier que l'image permet ces actions : mains et accessoires visibles, espace pour bouger, bouche dégagée pendant la parole. Créer les références ici avec imagegen, en style digicam 2000s et 4:3. Obtenir la validation de Kenny AVANT transmission à Higgsfield ; une autorisation de vidéo ne remplace pas cette validation.
4. Conserver visage, vêtements et accessoires canoniques, mais autoriser poses et interactions à évoluer. Ne pas demander de préserver exactement tous les pixels, la composition et la posture.

## Écrire le prompt

Commencer par sujet et action observable, puis décor/lumière, cadrage/mouvement caméra, intention et son. Traiter les étapes temporelles comme indications, jamais comme garantie de synchronisation parfaite.

- Donner des verbes physiques : baisser le téléphone, pivoter, reculer la chaise, saisir le robinet. Remplacer « réaction subtile » par le geste voulu.
- Choisir un seul comportement caméra. Autoriser un plan fixe avec acteur actif ; ne pas confondre caméra fixe et absence d'action. Pour une captation amateur, demander une légère instabilité et un recadrage motivé, sans travelling glamour.
- Décrire le digicam en mouvement : balance des blancs imparfaite, hautes lumières brûlées, bruit capteur, éclairage domestique. Ne pas transformer « photo au flash » en pose figée. Appliquer le VHS déterministe après montage/lip sync.
- Décrire l'évolution du décor quand elle compte : filet de curry devenant flaque, voyants clignotants. Garder physique et géographie simples.
- Nommer explicitement ambiance continue et bruitages synchrones, leur niveau sous la voix, et la musique voulue. « No music » ne demande pas une ambiance.
- Définir voix et rôle du fichier audio séparément des actions. Ne pas laisser dix consignes de timbre dominer deux mots d'animation. Employer seulement les rôles et paramètres exposés ; ne pas copier les tags des tutoriels comme syntaxe API garantie.

## Voix, son et budget

Conserver les prises ElevenLabs approuvées comme originaux. Choisir une voix de France et écouter accent, jeu, bégaiement et rythme. Ne pas promettre qu'un audio de référence sera copié sans modification. Vérifier lèvres et voix obtenue.

Prévoir trois couches au mix final : dialogue, ambiance, bruitages. Si le modèle ignore les sons, les produire/monter séparément ; ne pas repayer une vidéo réussie uniquement pour son ambiance. Préserver la prise vocale approuvée en final lorsque nécessaire.

Vérifier modèle, schéma, durée autorisée et devis avant chaque génération. Pour tester le mouvement, choisir une résolution économique disponible et une seule variante. Pour tester audio/lip sync, garder l'audio : une prévisualisation muette ne valide pas ce point. Ne pas extrapoler un prix public au compte connecté.

## Évaluer

Regarder le clip entier et écouter sa bande son. Contrôler action réelle et changement d'état, continuité du visage et mains, lèvres, accent/voix, ambiance/bruitages, durée et 4:3. Faire valider par Kenny avant de qualifier le workflow de validé.

Conserver prompt exact, références, paramètres, coût et observations après un échec. Distinguer consignes contradictoires, capacité incertaine et limitation du connecteur. Modifier une variable importante à la fois. Ne pas lancer de nouveaux tests payants pour une demande de recherche ou documentation.
