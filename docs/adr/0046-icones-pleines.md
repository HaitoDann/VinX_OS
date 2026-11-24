# ADR 0046 — Icônes d'application pleines, sans bordure

- **Statut** : accepté (remplace la partie « icônes d'application » de l'ADR 0018)
- **Date** : 2026-11-24

## Contexte

Retour du portable : le dock paraissait terne, chaque icône étant une tuile claire bordée avec un point
de couleur ; l'utilisateur a demandé des icônes pleines, sans bordure, et des icônes Fichiers, Paramètres et
Store « avec plus de personnalité ». Maquettes présentées ; choix retenu : **1A** (couleur par application)
et **2** (dessins propres).

## Décision

- `AppIcon` dessine une tuile pleine en dégradé dans la couleur de l'application (`tint`), sans bordure ni
  point ; pictogramme blanc, à l'encre sur les couleurs claires, vert sur la tuile presque noire.
- Fichiers, Paramètres et Store utilisent un dessin complet (`branding/appicons/*.svg`, propriété `art`).
- Les logos tiers sont affichés tels quels, en pleine taille, sans tuile.
- Le thème d'icônes système (`gen_icons.py`) suit la même règle pour les applications ; dossiers et types de
  fichiers gardent la tuile neutre.

## Conséquences

Dock plus coloré et lisible d'un coup d'œil. Les couleurs d'application deviennent visibles en grand :
elles doivent rester distinctes entre elles.
