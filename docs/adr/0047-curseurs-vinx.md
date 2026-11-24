# ADR 0047 — Thème de curseurs VinX

- **Statut** : accepté
- **Date** : 2026-11-24

## Contexte

Au lancement d'une application, KDE fait sautiller une petite icône de l'application à côté du curseur.
L'utilisateur la trouvait peu esthétique et a demandé « un vrai changement avec un curseur propre à VinX OS » ;
parmi les animations de chargement proposées, il a choisi la **grille 2 × 2**.

## Décision

- Thème `vinx-cursors` (installé dans `/usr/share/icons`), généré par `branding/cursors/gen_cursors.py` et
  versionné (la construction du paquet n'a pas besoin d'ImageMagick) :
  flèche **sans queue** aux coins arrondis (encre, liseré blanc) ; main = même flèche en **vert du logo** ;
  curseur texte arrondi ; attente = **grille 2 × 2** dont la case allumée tourne dans le sens des aiguilles
  d'une montre ; chargement = flèche + petite grille. Les autres formes héritent de Breeze.
- `vinx-session` impose le thème (`kcminputrc [Mouse] cursorTheme`, `XCURSOR_THEME`) et remplace l'icône qui
  sautille par le curseur d'attente (`klaunchrc`), le temps de la session.

## Conséquences

Identité visible en permanence. Le rendu réel (tailles, échelle HiDPI) reste à vérifier sur le portable.
