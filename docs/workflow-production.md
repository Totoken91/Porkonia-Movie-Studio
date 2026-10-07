# Workflow de production retenu — 7 octobre 2026

## Choix de Kenny

Le meilleur des trois essais de dialogue est **Seedance 2.0 Fast avec référence audio ElevenLabs**. Fast natif est apprécié pour sa voix qui semble venir de la pièce, mais présente un léger accent sur « à ce point-là ». Veo Lite est rejeté ; Mini écarté. Le VHS PorkOS et les prises ElevenLabs sont déjà approuvés. La présence acoustique du mix hybride reste à améliorer et à faire valider ; la cohérence entre plusieurs plans n'est pas encore testée.

## Recette

1. Lire bible visuelle actuelle et page Porkopédia du personnage. Identifier références de visage, produits et décor. Ne pas inventer de marques : Douzi Ambrée lorsque requise.
2. Écrire une scène avec changement visible, deux ou trois actions causales, et des dialogues courts. Prévoir ambiance et bruitages dès le découpage.
3. Créer ici une référence digicam 2000s 4:3 à partir des assets canoniques. Obtenir la validation explicite de Kenny avant l'envoi à Higgsfield. Prévoir mains, bouche et espace de mouvement visibles.
4. Générer/choisir la prise ElevenLabs v4. Voix française de France ; une ou deux balises vocales, texte parlé seul. Faire écouter accent, timbre, jeu, rythme. Réutiliser la prise retenue plutôt que la régénérer.
5. Mesurer sa durée et préparer une référence audio couvrant le plan selon les contraintes du modèle. Garder séparément la prise originale. Vérifier le schéma et le devis avec les vrais médias.
6. Générer Seedance 2.0, mode fast, 480p, 4:3, une sortie, audio activé, image de départ et audio de référence. Test préféré : 6 secondes, 6 crédits Higgsfield au devis du 7 octobre. Ce prix est daté.
7. Évaluer actions, visage/mains, géographie, lèvres, mots, voix et ambiance. L'audio de référence conditionne le modèle ; il ne garantit pas une copie exacte de la prise. Ne pas remplacer automatiquement la piste générée avec l'original si le timing diffère.
8. Mixer dialogue, ambiance continue et bruitages en pistes séparées lorsque disponibles. Rechercher une voix intégrée à la pièce : réverbération courte/discrète et niveau cohérent, sans effet d'écho excessif. Cette correction doit encore être écoutée et validée.
9. Monter puis appliquer le VHS PorkOS via scripts/render-vhs.cjs. Conserver les bruts, prises et exports approuvés. Vérifier durée complète et audio final.

## Paramètres connecteur Higgsfield testés

```json
{
  "model": "seedance_2_0",
  "mode": "fast",
  "resolution": "480p",
  "duration": 6,
  "aspect_ratio": "4:3",
  "generate_audio": true,
  "count": 1,
  "medias": [
    {"role": "start_image", "value": "ID_IMAGE_VALIDEE"},
    {"role": "audio", "value": "ID_AUDIO_CONFIRME"}
  ]
}
```

Ajouter le prompt au même objet. Remplacer les placeholders par des IDs confirmés dans le compte actuel. Le connecteur testé mappe audio vers audio_references. Avec un autre client, lire son schéma et employer ses clés réelles.

## Kevin Ranga

Adulte, veste grise et chemise noire. Phrase : « À… à ce point-là ? ». Voix fébrile légèrement bégayante. Référence : téléphone, brasserie/serveurs RangaNet, tuyaux cuivre, curry et illustrations gacha de femmes adultes aux courbes généreuses.

Prise Kevin approuvée : voix Alex française standard, ID aiFobLbZNvpjmWZD7HBh, eleven_v4 ; texte `[nervous] [trembling voice] À… à ce point-là ?`. Technicien hors champ : Romain uVFllq4keiE8s89ziX8R ; « Kevin, le curry coule dans le serveur. ». Rechercher et vérifier ces voix dans le catalogue du compte avant de les utiliser.

Les prompts exacts, IDs des essais et observations figurent dans tests-qualite-prix.md. Les IDs média/jobs sont des traces, pas des fichiers portables. Les images et prises originales doivent être récupérées depuis les assets approuvés ; ne pas remplacer silencieusement une référence manquante par une image nouvelle.

## Reprise dans une nouvelle conversation

Demander à l'agent de lire AGENTS.md, ce fichier, la compétence portable et les tests. Puis consulter la bible actuelle et préciser la scène à produire. La compétence peut être lue directement par Claude/Codex ; sa présence dans GitHub ne l'installe pas automatiquement dans les interfaces de skills.
