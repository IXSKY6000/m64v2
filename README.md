# SM64 Source 35E v6

Cette version compile directement le code C de `sm64-master.zip` fourni, au lieu de recréer la physique.

## Ce qui est maintenant directement le source SM64, inchangé

- `mario.c`
- `mario_actions_stationary.c`
- `mario_actions_moving.c`
- `mario_actions_airborne.c`
- `mario_actions_object.c`
- `mario_actions_submerged.c`
- `mario_actions_cutscene.c`
- `mario_actions_automatic.c`
- `mario_step.c`
- `camera.c`
- `math_util.c`
- `graph_node.c`

Les cinq fichiers `behaviors/*.inc.c` inclus par `camera.c` sont également copiés tels quels.
`SOURCE_EXACT_SHA256.txt` contient leurs hashes.

## Ce qui est spécifique à la Graph 35+E II

Le rendu N64 n'est pas utilisé. `port/renderer.c` transforme le modèle low-poly en wireframe 128x64.

La sandbox contient seulement une immense plateforme plate, donc les systèmes qui n'existent pas dans cette scène (audio, sauvegarde, vrais objets/niveaux, menus) ont une couche de compatibilité minimale. Les fichiers d'actions Mario eux-mêmes ne sont pas remplacés par des clones.

Les 209 animations du source sont placées dans une table statique pour remplacer le chargement DMA N64. Le modèle low-poly vient du source et est converti en arêtes pour l'écran monochrome.

## Contrôles

- flèches : stick N64
- F1 : A
- F2 : B
- F3 : Z
- F5 / F6 : C gauche / C droite
- F4 : reset de la sandbox
- EXIT : quitter

## Compilation GitHub

Comme pour les autres projets :

1. Mets les fichiers du ZIP dans ton dépôt.
2. Sur GitHub, crée exactement `.github/workflows/build-g1a.yml` si le dossier caché n'a pas été envoyé.
3. Copie dedans le contenu du `build-g1a.yml` visible à la racine.
4. Va dans **Actions**.
5. Lance **Compiler SM64 Source G1A**.
6. Télécharge l'artifact **SM64SRC-g1a**.
7. Dedans : `SM64SRC.g1a`.

## Vérification faite avant ce ZIP

J'ai fait un link complet local avec tous les fichiers ci-dessus et une couche Gint factice : PASS.
J'ai ensuite exécuté 100 frames simulées (marche, saut, punch, caméra, animation et renderer) : PASS.

Le vrai test restant est le cross-link SH de fxSDK sur GitHub, notamment la taille finale en Flash/RAM de la Graph 35+E II.
