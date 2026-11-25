# ADR 0051 — Se détacher de KDE au maximum

- **Statut** : accepté pour la direction ; **chaque étape est validée séparément** (maquette ou
  proposition, puis ADR d'étape)
- **Date** : 2026-11-25

## Contexte

VinX remplace déjà l'interface de Plasma (barre, dock, panneaux, Paramètres, Fichiers, Store, Terminal,
verrouillage, connexion, extinction, bulles, agent polkit : ADR 0009, 0025, 0028). Mais il repose encore
sur KDE pour démarrer et fonctionner. L'utilisateur souhaite **s'en détacher au maximum** : moins de
paquets, moins de comportements KDE qui ressurgissent (Breeze, clavier virtuel, raccourcis, réglages
écrits par Plasma), une identité propre.

## Ce qui dépend encore de KDE (inventaire au 25/11/2026)

| Élément | Rôle aujourd'hui | Remplacement envisagé | Difficulté |
|---|---|---|---|
| Applications : Ark, KCalc, Okular, Spectacle, Skanlite, Konsole (secours), KWalletManager | Utilitaires installés par défaut | Équivalents VinX (calculatrice, lecteur PDF…) ou applications Qt/GTK neutres ; Spectacle déjà remplacé par la Capture VinX | facile à moyenne |
| `plasma-workspace` (`startplasma-wayland`, kded, ksmserver) | Démarre la session, services d'arrière-plan | **Session VinX propre** : unité systemd `vinx-session.target` qui lance KWin puis les services VinX, sans startplasma ni plasmashell | moyenne |
| `powerdevil` | Veille, écran éteint, couvercle, luminosité | Service VinX : logind (couvercle, veille), protocole d'inactivité Wayland, luminosité par logind | moyenne |
| `plasma-pa` (kded audio), `plasma-nm` | Touches son, réseau | Déjà en partie VinX (wpctl, nmcli) ; finir les touches et le réseau | facile |
| `kscreen` | Configuration des écrans | Protocole de gestion des sorties du compositeur, piloté par Paramètres | moyenne, liée au compositeur |
| `kglobalaccel` | Raccourcis globaux | Raccourcis gérés par le compositeur ou par vinx-shell | liée au compositeur |
| `kscreenlocker` | Verrouillage sûr | Protocole `ext-session-lock` (écran de verrouillage VinX en client Wayland) | moyenne, liée au compositeur |
| `libtaskmanager` | Fenêtres ouvertes pour le dock | Protocole `foreign-toplevel` | moyenne, liée au compositeur |
| KIO | Fichiers : réseau, corbeille, MTP, SMB | Qt seul pour le local ; GIO/gvfs pour réseau et téléphones | moyenne à difficile |
| Breeze (curseurs, icônes, GTK) hérités | Formes non dessinées par VinX | Thèmes VinX complets, héritage vers Adwaita/hicolor | facile |
| **KWin** | Compositeur Wayland, fenêtres, effets, décoration | (a) **KWin seul**, sans Plasma ; (b) compositeur wlroots (labwc, Wayfire) ; (c) compositeur VinX (QtWaylandCompositor) | (a) moyenne · (b) très difficile · (c) des années |
| Session de secours (ADR 0049) | Bureau Plasma | Mode VinX minimal, quand Plasma ne sera plus installé | facile, en dernier |
| SDDM | Écran de connexion | Garder : projet indépendant de KDE (Qt), thème VinX | — |

## Décision

Détachement **progressif**, du plus rentable au plus risqué, chaque étape livrée et testée sur le portable
avant la suivante :

1. **Applications KDE** retirées de l'ISO et remplacées (VinX ou neutres).
2. **Session VinX autonome** : plus de `startplasma-wayland` ni de `plasma-workspace` ; KWin lancé
   directement avec les services VinX.
3. **Services** : énergie (à la place de powerdevil), son, réseau, touches, raccourcis.
4. **Fichiers sans KIO.**
5. **Compositeur** : décision à part (ADR dédié). Recommandation : garder **KWin seul** comme moteur,
   comme d'autres systèmes gardent un compositeur existant. Le remplacer coûterait des années pour peu
   de gain visible, et KWin seul n'apporte plus aucune interface KDE.

## Conséquences

- Moins de paquets, moins de surprises KDE, démarrage plus rapide.
- Plus de code VinX à maintenir (énergie, session, verrouillage) : chaque remplacement doit être au
  moins aussi fiable que l'original avant de le retirer.
- Pendant la transition, la session de secours reste le bureau Plasma.
