# ADR 0053 — Étape 2 sans KDE : session VinX autonome, sans Plasma

- **Statut** : accepté — **en essai** (session « VinX OS (essai sans Plasma) » à côté de « VinX OS »)
- **Date** : 2026-11-25

## Contexte

Étape 2 de l'ADR 0051. `vinx-session` lançait `startplasma-wayland`, qui démarre KWin, les services
de Plasma (ksmserver, kded6, kactivitymanagerd, baloo, plasmashell masqué…) et réécrit des réglages
KDE. Choix de l'utilisateur (25/11/2026) : essai **à côté** de la session actuelle, puis bascule ;
**secours = bureau Plasma** pendant la transition ; `XDG_CURRENT_DESKTOP=KDE` conservé pour l'instant.
L'inventaire réel du portable n'a pas encore été fait : la session part de ce que Plasma 6.3 démarre.

## Décision

- Nouvelle entrée de connexion `vinx-standalone.desktop` → `vinx-session --standalone`. Même lanceur et
  mêmes réglages que « VinX OS » (ADR 0049), mais au lieu de startplasma :
  `vinx-session-standalone` lance **KWin** (`kwin_wayland_wrapper --xwayland
  --exit-with-session=vinx-session-inner`).
- `vinx-session-inner` transmet l'environnement à systemd et D-Bus, démarre **`vinx-session.target`**,
  puis attend : quand la cible s'arrête, KWin et la session se terminent.
- `vinx-session.target` (systemd utilisateur) : `vinx-shell.service` (relance automatique ; après 5
  plantages en 2 min, bureau Plasma de secours), applications au démarrage (`xdg-desktop-autostart.target`,
  dont l'agent polkit VinX) et, **provisoirement**, les services KDE `plasma-kcminit`, `plasma-kded6`,
  `plasma-powerdevil`, `plasma-xdg-desktop-portal-kde`, `plasma-kglobalaccel` (ignorés s'ils manquent).
- Plus de ksmserver : la fermeture de session passe par logind (déjà le cas dans VinX) ; pas de
  restauration des applications de la session précédente.
- Pas démarrés : plasmashell, ksmserver, kactivitymanagerd, baloo, xembedsniproxy, gmenudbusmenuproxy.

## Conséquences et points à vérifier (docs/RECETTE.md, « Session sans Plasma »)

Démarrage plus court, plus de réglages KDE réécrits. Risques connus : icônes X11 de la zone de
notification (xembedsniproxy) et menus globaux des applications GTK (gmenudbusmenuproxy) absents ;
applications non restaurées à l'ouverture. Bascule par défaut seulement après la recette sur le portable.
