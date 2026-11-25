# ADR 0054 — Étape 4 sans KDE : Fichiers sans KIO

- **Statut** : accepté — à vérifier sur le portable ; réseau (SMB, SFTP, téléphones) dans un lot suivant
- **Date** : 2026-11-25

## Contexte

Étape 4 de l'ADR 0051, avancée avant la recette de l'étape 2 car indépendante de la session.
Fichiers avait deux moteurs : KIO (KDE, compilé seulement sous Debian) et un moteur Qt local, incomplet
(pas de corbeille, copies qui figeaient la fenêtre, pas de clés USB non montées).

## Décision

- **Moteur Qt par défaut** (`-DVINX_FILES_KIO=OFF`) ; `-DVINX_FILES_KIO=ON` reprend KIO en cas de besoin.
- **Corbeille** selon la norme freedesktop (`trashspec.h`, testé) : parcourir (`trash:/`), restaurer (à sa
  place, ou « nom (2) » si elle est prise), supprimer, vider. Même corbeille que KDE, GNOME et Qt.
- **Copies et déplacements** dans un fil séparé, avec progression par élément.
- **Clés USB et disques amovibles** (`devices.h`, testé) : listés par `lsblk`, montés au clic par
  `udisksctl` (sans mot de passe pour la session active), bouton « Éjecter » sur les volumes montés.
  Sans Plasma, personne d'autre ne monte les clés : nécessaire aussi pour la session de l'ADR 0053.
- Pas encore repris : **réseau** (partages Windows, SFTP, téléphones MTP) : lot suivant, avec gvfs
  (`gio mount`) et une fenêtre de mot de passe VinX ; corbeille des clés USB (`.Trash-UID`) ;
  miniatures des images.

## Conséquences

Fichiers ne dépend plus de KIO. Régression connue tant que le lot réseau n'est pas fait : l'entrée
« Réseau » disparaît.
