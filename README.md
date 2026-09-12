# SM64 Source 35E v9 — memory fix

Cette version corrige les dépassements du linker Graph 35+E II observés en v8 :
- ROM : +293120 octets
- RAM : +30240 octets

## Ce qui reste original SM64
Les fichiers de physique/actions/caméra restent ceux de `n64decomp/sm64` dans `original/` (ou à la racine si tu as importé les sources ainsi).

## Adaptations mémoire Graph 35+E II
1. Les tables trigonométriques de `math_util.c` sont identiques en valeurs mais placées en ROM (`const`) au lieu de consommer la RAM.
2. Le sandbox embarque 59 animations SM64 exactes au lieu des 209, sélectionnées à partir des animations réellement atteintes sur la plateforme plate : déplacement, sommeil/réveil, sauts, long jump, ground pound, crouch/crawl, punch/kick, dive, skid, sideflip, etc.
3. Si une action de niveau/cutscene impossible dans ce sandbox demande une animation absente, le loader utilise une pose idle originale plutôt que de planter.

Le rendu 128x64, le clavier et la plateforme plate restent la couche spécifique Graph.

## Compilation
Crée `.github/workflows/build-g1a.yml`, colle le contenu du `build-g1a.yml` visible, puis lance `Compiler SM64 Source G1A` dans Actions.
