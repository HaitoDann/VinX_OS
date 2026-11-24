# ADR 0048 — Icônes du bureau et menu « Nouveau »

- **Statut** : accepté
- **Date** : 2026-11-24

## Contexte

Retours : pas d'icônes sur le bureau, pas de « créer un fichier » au clic droit. Maquette 5 validée.

## Décision

- Le bureau affiche le contenu du dossier « Bureau » (XDG) en colonnes depuis le coin haut droit, sur le
  premier écran ; double-clic pour ouvrir. Renommer ou supprimer se fait dans Fichiers. Désactivable :
  Paramètres → Bureau → « Icônes sur le bureau » (`desktop/icons`, activé par défaut).
- Clic droit sur le bureau : **Nouveau** (dossier, document texte, Writer, Calc, Impress), « Ouvrir le
  Terminal ici ». Les documents sont des fichiers vides : LibreOffice ouvre un fichier vide comme un nouveau
  document du type de son extension.
- Fichiers propose les mêmes créations dans son menu et dans un dossier vide.

## Conséquences

Sans gestionnaire d'icônes complet (pas de glisser-déposer ni de positions libres) : volontairement simple
pendant la consolidation 1.0.
