# ADR 0047 — Batterie progressive et tuiles pleines du dock

- **Statut** : accepté
- **Date** : 2026-11-22

## Contexte

Deux retours sur le portable :

- L'icône de batterie de la barre est un SVG fixe (une jauge figée à ~70 %), remplacé par un
  éclair quand la machine charge : le niveau réel n'est jamais visible, alors que le singleton
  `Vinx.System.Battery` (UPower) met déjà un pourcentage à jour à chaque changement.
- Le dock affiche les icônes sur une tuile neutre **bordée** (tuile + pictogramme au trait +
  point de couleur, ADR 0018) : à 48 px et plus, l'utilisateur trouve le rendu « vide » et
  souhaite des icônes pleines, sans bordure.

## Décision

- **Batterie** : nouveau composant `Vinx/Shell/BatteryLevel.qml` qui dessine la batterie
  (mêmes proportions que `branding/icons/battery.svg`, tracé 24×24 mis à l'échelle) avec un
  niveau intérieur proportionnel au pourcentage, animé (`Theme.durFast`). Couleur passée par
  l'appelant : texte normal, rouge `Theme.danger` sous 10 % hors charge. En charge, l'éclair
  reste affiché à côté (barre supérieure) ou dans la ligne du centre de contrôle. Utilisé par
  `TopBar.qml` et `ControlCenter.qml`.
- **Dock** : variante **pleine** du composant `AppIcon` (`filled: true`) réservée au dock :
  la tuile prend la couleur de l'application, sans bordure ni point, pictogramme posé en
  encre contrastée (calcul de luminance : encre foncée sur couleur claire, encre claire sur
  couleur foncée). Les applications tierces gardent leur logo officiel, sur la même tuile
  neutre ***sans bordure*** dans le dock seulement. Ailleurs (recherche, Store, notifications,
  galerie), l'icône reste tuile neutre bordée + point (ADR 0018).

## Conséquences

- La barre supérieure montre le vrai niveau de batterie en continu, sans changer d'icône ;
  sous 10 % hors charge elle passe au rouge, comme avant.
- L'identité visuelle ADR 0018 reste la règle partout sauf dans le dock, où le point n'a plus
  de sens sur une tuile colorée (la couleur devient la tuile elle-même). Ne pas généraliser
  la tuile pleine au-delà du dock sans nouvel ADR.
- À vérifier sur le matériel : lisibilité des tuiles pleines sombres en thème sombre
  (numéro d'application sans point), et le niveau de batterie réel aux extrêmes (0 %, 100 %).
