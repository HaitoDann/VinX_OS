# ADR 0052 — Étape 1 sans KDE : applications remplacées

- **Statut** : accepté
- **Date** : 2026-11-25

## Contexte

Première étape de l'ADR 0051. Maquette du 25/11/2026 validée par l'utilisateur.

## Décision

| Application KDE | Remplacement |
|---|---|
| KCalc | **Calculatrice VinX** (`vinx-calculator`) : moteur de la recherche (Calc.js, sans eval), pourcentage « à la calculatrice » (200 + 10 % = 220), clavier et pavé numérique |
| Okular | **Lecteur PDF** de la Visionneuse VinX (`vinx-images` + `org.vinx.pdf.desktop`) : module PDF de Qt (`qt6-pdf-dev`), miniatures, défilement continu, zoom, recherche ; « Ouvrir dans Firefox » pour formulaires, annotations et impression |
| Ark | **Archives dans Fichiers** : « Extraire ici », double-clic (dossier du même nom, sans double niveau), « Compresser en .zip » ; `bsdtar` (`libarchive-tools`) ; Fichiers est associé aux archives |
| Skanlite | **Numériseur de documents** (`simple-scan`, GNOME) |
| Spectacle, Konsole, Gwenview | Retirés : Capture VinX, Terminal VinX (conservé), Images |
| kde-config-gtk-style | **vinx-shell écrit les réglages GTK** : `settings.ini` (GTK 3 et 4 : Adwaita, clair/sombre, IBM Plex Sans, curseurs et icônes VinX), `gtk.css` avec la couleur d'accent (jamais un gtk.css personnel), `gsettings` pour libadwaita |

Ces paquets sont retirés de l'ISO. Sur un système déjà installé, ils restent jusqu'à leur retrait manuel.

## Conséquences

- Le lecteur PDF ne gère pas encore les formulaires ni les annotations (Firefox en relais).
- Les applications GTK passent de Breeze à Adwaita, recolorées à l'accent VinX.
