# Workflow de production retenu — 7 octobre 2026

## Choix de Kenny

Décision actuelle : **Seedance 2.0 Fast avec voix natives par défaut**, y compris Kevin et Bernis. Kenny les préfère pour leur naturel et leur présence dans la pièce. ElevenLabs reste un secours ciblé. Veo Lite et Mini restent écartés. Le VHS PorkOS est approuvé ; chaque nouvelle prise doit toujours être contrôlée. La version native de La Cible est livrée pour comparaison, sans retour final de Kenny à ce stade.

Lire [le guide de réalisation](guide-realisation.md) avant toute production. Les anciennes comparaisons sont conservées dans tests-qualite-prix.md.

## Recette

1. Lire bible visuelle actuelle et page Porkopédia du personnage. Identifier références de visage, produits et décor. Ne pas inventer de marques : Douzi Ambrée lorsque requise.
2. Écrire objectif, obstacle, décision, conséquence et conclusion. Définir géographie, regards et accessoires. Storyboarder les informations nouvelles et monter une animatique avec voix provisoires et sons essentiels ; faire valider avant vidéos, sauf délégation.
3. Créer ici les références individuelles digicam 2000s 4:3 à partir des assets canoniques. Faire valider en grand avant Higgsfield, sauf délégation explicite du contrôle. Prévoir mains, bouche et espace de mouvement visibles.
4. Écrire la réplique native exacte, le locuteur et un jeu sobre : français de France, mots intelligibles, intention cohérente. Pour Kevin : fébrile, petite hésitation, « À… à ce point-là ? ». Garder une description vocale commune entre ses plans et contrôler la continuité à l’écoute.
5. Mesurer une lecture provisoire ; distinguer durée montée et durée générée. Vérifier le schéma et le devis avec les vrais médias. Aucune référence audio par défaut.
6. Générer Seedance 2.0, mode fast, 480p, 4:3, une sortie, audio activé, image de départ. Derniers devis exécutés du 7 octobre : 4 s = 4 crédits, 6 s = 6 crédits ; revérifier, ce ne sont pas des tarifs garantis.
7. Évaluer les actions, identité, géographie, lèvres, mots, timbre et ambiance. Monter selon leurs timings réels. L’ASR ne valide pas l’accent. Si le natif échoue, diagnostiquer et corriger le texte/jeu ; envisager ElevenLabs seulement pour une réparation ciblée avec accès et devis vérifiés. Une référence vocale ne garantit pas une copie exacte.
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
    {"role": "start_image", "value": "ID_IMAGE_VALIDEE"}
  ]
}
```

Ajouter le prompt au même objet. Remplacer les placeholders par des IDs confirmés dans le compte actuel. Pour un secours ElevenLabs explicitement choisi, ajouter un média audio confirmé si le schéma le permet ; le connecteur testé le mappe vers audio_references. Avec un autre client, lire son schéma et employer ses clés réelles.

## Kevin Ranga

Adulte, veste grise et chemise noire. Phrase : « À… à ce point-là ? ». Voix fébrile légèrement bégayante. Référence : téléphone, brasserie/serveurs RangaNet, tuyaux cuivre, curry et illustrations gacha de femmes adultes aux courbes généreuses.

Ancienne prise Kevin approuvée, conservée comme secours : voix Alex française standard, ID aiFobLbZNvpjmWZD7HBh, eleven_v4 ; texte `[nervous] [trembling voice] À… à ce point-là ?`. Technicien hors champ : Romain uVFllq4keiE8s89ziX8R ; « Kevin, le curry coule dans le serveur. ». Rechercher et vérifier ces voix dans le catalogue du compte avant de les utiliser.

Les prompts exacts, IDs des essais et observations figurent dans tests-qualite-prix.md. Les IDs média/jobs sont des traces, pas des fichiers portables. Les images et prises originales doivent être récupérées depuis les assets approuvés ; ne pas remplacer silencieusement une référence manquante par une image nouvelle.

## Reprise dans une nouvelle conversation

Demander à l'agent de lire AGENTS.md, ce fichier, la compétence portable et les tests. Puis consulter la bible actuelle et préciser la scène à produire. La compétence peut être lue directement par Claude/Codex ; sa présence dans GitHub ne l'installe pas automatiquement dans les interfaces de skills.
