# ADR 0048 — Pictogrammes d'applications plus affirmés

- **Statut** : accepté
- **Date** : 2026-11-22

## Contexte

Retours du portable : les icônes Fichiers, Paramètres et Store paraissent ternes, « trop
simples » par rapport aux logos tiers (SendMessage : « plus de personnalité »). Ces
pictogrammes sont partagés par le shell (`Vinx.Controls.Icon`), l'agent (`AppIcon`,
`AppStyle.js`) et le thème d'icônes freedesktop généré (`branding/icon-theme/gen_icons.py`,
depuis les mêmes SVG de `branding/icons/`).

## Décision

Faire évoluer trois pictogrammes de `branding/icons/`, sans changer le style (toujours
monochrome `currentColor`, trait 1,8, cap/joins ronds, ADR 0018) :

- **Fichiers** (`folder.svg`) : le dossier gagne des lignes de contenu — il lit « des
  fichiers », pas juste un dossier.
- **Paramètres** (`sliders.svg`) : horizontales discrètes → console de mixage verticale, trois
  curseurs à des niveaux différents (nets en tuile comme en pictogramme seul).
- **Store** (`bag.svg`) : l'anse plonge plus dans le sac (jusqu'à mi-hauteur) et un emblème en
  carré arrondi rappelle la tuile VinX au centre du sac.

Le thème d'icônes suit automatiquement (génération depuis les mêmes SVG) : dock, épinglés,
menu d'applications et affichages KDE liés.

## Conséquences

- Aucun changement de code : les SVG sont consommés tels quels ; la modification est visible
  après une reconstruction du paquet (les ressources sont embarquées à la compilation).
- La nuance de personnalité reste modérée par construction (un seul trait, un seul ton) :
  ne pas introduire de couleurs dans les pictogrammes sans réviser l'ADR 0018.
- À valider visuellement sur le matériel (48 px du dock, 16 px de la barre, thème clair et sombre).
