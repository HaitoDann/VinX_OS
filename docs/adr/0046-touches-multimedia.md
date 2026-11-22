# ADR 0046 — Volume et luminosité : touches multimédia VinX

- **Statut** : accepté
- **Date** : 2026-11-22

## Contexte

Sur le portable, les touches volume et luminosité du clavier ne faisaient rien en session VinX.
Dans une session Plasma complète, ces touches sont traitées par des modules KDE dédiés
(plasma-pa côté son, powerdevil côté énergie), qui se déclarent dans kglobalaccel et demandent
à plasmashell d'afficher l'OSD. La session VinX (ADR 0009) remplace plasmashell, et l'on veut
que le système fonctionne même quand ces modules KDE ne sont pas actifs.

Par ailleurs, le shell VinX publie déjà `org.kde.osdService` (ADR 0028) : n'importe quel
processus peut demander une bulle de volume ou de luminosité au bureau, comme le ferait KDE.

## Décision

- **vinx-shellctl** gère les actions multimédia en plus des panneaux du bureau :
  `volume-up`, `volume-down`, `volume-mute`, `brightness-up`, `brightness-down`.
  - Volume : `wpctl` (PipeWire, déjà requis par le singleton Audio).
  - Luminosité : `brightnessctl` (ajouté à l'ISO, `distro/live/config/package-lists/base.list.chroot`),
    qui passe par logind si les permissions directes manquent.
  - Après application, `vinx-shellctl` appelle `org.kde.osdService` pour la bulle ; sans shell
    VinX, et sans outils installés, l'action est ignorée silencieusement (return 0).
- **Raccourcis** : la session écrit les entrées `kglobalshortcutsrc [services]`
  (`org.vinx.shellctl.volume-up.desktop`… avec `XF86AudioRaiseVolume`, `XF86MonBrightness*`)
  desktop files installés à côté des autres (`shell/session/`), nettoyés à la fin de session,
  comme pour les panneaux (bureau, agenda…).
- Le module d'entrée de KWin reste éteint : rien ne remplace ces touches si KDE les
  déclarerait aussi (pas de conflit : kglobalaccel n'attribue un raccourci qu'une fois).

## Conséquences

- Les touches volume et luminosité fonctionnent sans plasmashell ni modules kded de KDE,
  avec l'OSD VinX existant (ADR 0028).
- La valeur de pas (5 %) et la limite d'amplification (100 %) sont celles du script ;
  le centre de contrôle garde ses propres réglages fins.
- À vérifier sur le matériel (portable de test) : touches réelles (XF86*), OSD affiché,
  luminosité via logind sur le noyau en place.
