# Voix françaises, promptage et budget

Vérifié le 7 octobre 2026. Distinguer résultat écouté, documentation fournisseur et hypothèse à tester. Lire les guides et schémas actuels lorsque les modèles changent.

## ElevenLabs v4

Choisir d'abord une voix source française de France, écouter son aperçu et verrouiller son ID par personnage. Un nom ne suffit pas : la recherche Alex a retourné une voix française standard et une autre québécoise. V4 conserve l'accent source lorsque la langue source et générée sont identiques. Ne pas essayer de réparer une voix québécoise avec strong French accent, qui peut créer une caricature plutôt qu'un accent métropolitain.

Envoyer comme prompt uniquement le texte parlé et les balises auditives. Ne pas ajouter de description de scène ou de gestes. Commencer avec une ou deux indications de jeu cohérentes avec la voix, puis changer seulement celle qui échoue. Employer ponctuation naturelle et ellipses pour l'hésitation ; écrire la répétition voulue pour le bégaiement. Conserver la réplique exacte validée.

Kevin, prise déjà approuvée :

`[nervous] [trembling voice] À… à ce point-là ?`

Technicien, proposition sobre :

`[matter of fact] Kevin, le curry coule dans le serveur.`

Garder une prise par personnage avec son ID, texte et modèle. Pour une phrase très courte, produire plusieurs prises seulement si nécessaire et dans le budget ; ne pas ajouter de faux dialogue pour allonger artificiellement le prompt. Si du contexte parlé est généré puis coupé, vérifier l'intonation et la coupe, et comptabiliser ce texte supplémentaire.

V4 comprend l'IPA entre barres obliques pour les noms difficiles ; tester la prononciation avant de remplacer un nom canonique. Ne pas utiliser de phonétique de fortune partout. Préserver le texte canonique des sous-titres si le texte de synthèse contient une correction phonétique.

V4 expose stabilité et similarité dans la documentation fournisseur ; le schéma du connecteur actuel expose seulement voice_id (alias ancien) et language_code BCP-47, sans ces curseurs. Ne pas inventer de réglages. Lorsque disponibles, diminuer modérément la stabilité pour plus de variation et éviter la similarité maximale si elle nuit au naturel. Style, Speed et SSML ne sont pas disponibles sur v4. Les modèles v2 ne partagent pas la syntaxe de balises v4.

Ajouter au montage une acoustique légère adaptée à la pièce et un niveau cohérent ; ne pas dégrader la diction avec un effet cassette excessif. Garder dialogue, bruitages et ambiance séparés.

## Modèles vidéo : sélection provisoire

Veo 3.1 génère dialogue, bruitages et ambiance selon Google. Candidat à tester pour une voix française intégrée, pas une garantie de bon accent ni de stabilité du timbre entre plans. Formuler une réplique exacte, un seul locuteur clairement identifié, l'accent métropolitain et le jeu ; décrire les sons en phrases distinctes. Ne pas envoyer les balises ElevenLabs comme si elles étaient des paramètres Veo.

Kling VIDEO 3.0 : le guide officiel liste chinois, anglais, japonais, coréen et espagnol. Il indique que les autres langues de dialogue sont traduites en anglais. Écarter comme premier choix pour dialogue français natif ; ne pas confondre son produit de doublage avec le modèle vidéo.

Seedance 1.5 : voix rejetées par Kenny. Seedance 2.0 avec référence ElevenLabs : voix appréciée, mouvement rejeté. Aucun test contrôlé de sa voix native sans référence n'est validé. Mini/Fast sont des candidats économiques, sans preuve de même performance vocale.

Devis sans génération, 6 secondes, une sortie, audio natif activé, sans références téléchargées :

| Modèle Higgsfield | Réglages | Crédits Higgsfield | Format exposé |
| --- | --- | --- | --- |
| seedance_2_0_mini | 480p | 3 | 4:3 |
| seedance_2_0 | fast, 480p | 6 | 4:3 |
| veo3_1_lite | generate_audio=true | 9 | devis 16:9, 4:3 non explicite |
| veo3_1 | fast, basic | 24 | 16:9 ou 9:16 |

Ce sont des devis datés, pas des requêtes complètes validées. Prévoir recadrage 4:3 pour Veo, avec composition compatible et référence adaptée validée avant envoi. Ne pas supposer que auto garantit le 4:3.

Devis ElevenCreative pour une seule prise Kevin v4 avec le prompt ci-dessus : 46 crédits ElevenLabs, 0,46 centime USD (0,0046 dollar). Quatre prises coûteraient environ quatre fois ce devis. Crédits non comparables à Higgsfield. Le coût de synthèse de cette réplique est faible ; le lip sync séparé, les reprises vidéo et les forfaits peuvent dominer le coût du workflow. Réutiliser les prises approuvées existantes.

La page API publique affiche temporairement v4 à 0,022 dollar / 1000 caractères jusqu'au 12 octobre, puis prix affiché hors remise 0,08 dollar. Ne pas appliquer cette promotion au connecteur, dont le devis peut différer.

Comparer un plan identique avec la même image approuvée et des répliques courtes : accent, naturel, exactitude des mots, émotion, lèvres, mouvement, ambiance et coût par prise utilisable. Un seul succès ne valide pas la cohérence d'une voix sur un film. Ne lancer aucun test payant pendant une recherche seule.

## Sources officielles

- Accent : https://help.elevenlabs.io/hc/en-us/articles/19581255545873-How-do-I-select-the-language-and-accent
- Promptage, ponctuation, tags et IPA : https://elevenlabs.io/docs/overview/capabilities/text-to-speech/best-practices
- V4 et limites : https://elevenlabs.io/docs/fr/overview/capabilities/text-to-speech/eleven-v4
- Tarifs publics API : https://elevenlabs.io/pricing/api
- Veo audio natif : https://deepmind.google/models/veo/
- Veo promptage audio : https://cloud.google.com/vertex-ai/generative-ai/docs/video/video-gen-prompt-guide
- Kling langues : https://kling.ai/quickstart/klingai-video-3-model-user-guide
- Guides, catalogue, schémas et devis connectés ElevenLabs/Higgsfield consultés le 7 octobre 2026.

## Premiers essais réellement exécutés

Le 7 octobre 2026, Mini a refusé le lancement : Requires basic plan or higher. Son devis de 3 crédits ne prouve donc pas l'accès sur ce compte. Ne pas relancer sans changement d'accès.

Seedance Fast natif, 6 crédits, 6 s : action visible mais décor inversé entre gauche/droite dès le début. Veo Lite natif, 9 crédits, 6 s : action visible, puis lever et déplacement non demandés ; sortie 16:9 avec bandes latérales contenant l'image 4:3. Recadrage possible sans étirer l'image. Ces observations portent sur les images du clip, pas sur une écoute vocale.

Les deux fichiers ont une piste audio ; accent, réplique exacte, émotion et ambiance restent à valider par Kenny. L'analyse actuelle ne permet pas d'écouter directement l'audio. Ne jamais transformer un contrôle de présence de piste en preuve de qualité française.

## Retour de Kenny après écoute — 7 octobre 2026

Meilleur résultat de la série : Seedance Fast avec référence ElevenLabs. Retenir comme base pour les prochains plans, avec l'image validée avant envoi et les prises approuvées réutilisées. Cela valide la préférence pour ce plan, pas automatiquement la stabilité de toute une production.

Seedance Fast natif jugé très bon aussi, notamment parce que la voix paraît venir du personnage dans la pièce, plutôt que d'un doublage ajouté. Défaut entendu par Kenny : léger accent sur « à ce point-là », perçu comme « à ce point lo ? ». Ne pas qualifier cette voix d'accent métropolitain validé ; garder cette option secondaire si Kenny accepte une prise donnée.

Veo Lite rejeté par Kenny. Mini écarté à sa demande ; ne pas le relancer ou chercher à convaincre sur son tarif. Aucun nouveau test payant sans nouvelle instruction de test.

Prochaine amélioration sonore : conserver timbre et prononciation ElevenLabs approuvés, puis intégrer la voix au lieu avec ambiance continue, niveau cohérent et légère acoustique de pièce. Réverbération courte et discrète, à adapter au décor ; éviter écho de salle ou filtrage VHS excessif. Comparer à la voix native comme référence de présence acoustique. Ne pas promettre que le seul prompt de référence impose une copie exacte de la prise ou la réverbération voulue ; traiter au mix si nécessaire et faire écouter.
