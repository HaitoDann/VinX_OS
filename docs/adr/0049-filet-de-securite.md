# ADR 0049 — Filet de sécurité : session de secours, retour arrière, canal « stable »

- **Statut** : accepté
- **Date** : 2026-11-25

## Contexte

La 0.30.0 a été installée avec des marqueurs de conflit de fusion dans `vinx-session` : la session
s'arrêtait aussitôt et ramenait à l'écran de connexion. L'utilisateur ne pouvait plus se connecter
sans passer par une console. Rien ne protégeait d'une mise à jour cassée, et le retour arrière
n'existait qu'en ligne de commande (`vinx-dev rollback`). Le test de démarrage réel se fait sur le
portable (choix de l'utilisateur) : pas de test de session en machine virtuelle.

## Décision

1. **Lanceur minimal** : `/usr/bin/vinx-session` ne contient plus que la logique de démarrage ; les
   réglages (qui changent souvent) sont dans `/usr/lib/vinx/vinx-session-setup`. Le lanceur vérifie
   leur syntaxe (`sh -n`) avant de les charger.
2. **Session de secours** : réglages illisibles, ou deux sessions VinX arrêtées en moins de 30 s avec
   une erreur → bureau Plasma standard, avec une bulle qui explique comment revenir en arrière
   (`vinx-safe-notice`). Si `vinx-shell` plante 5 fois en 2 minutes, le bureau Plasma prend le relais
   dans la session.
3. **Versions gardées** : un hook APT (`81vinx-keep-shell`) copie chaque paquet vinx-shell installé dans
   `/var/lib/vinx/versions` (3 gardés), qu'il vienne du dépôt APT ou de `vinx-dev`.
4. **Retour arrière visuel** : « Mises à jour » → « Revenir à la version précédente »
   (`vinx-admin shell-rollback VERSION`, mot de passe administrateur). `vinx-dev rollback` reste.
5. **Canal stable** : le dépôt APT est publié quand la branche `stable` avance. `main` = essai
   (`vinx-dev`, portable de l'utilisateur) ; `stable` = version validée, proposée à tous par
   « Mises à jour ».
6. Test `session-fallback` (faux startplasma) et `shell-scripts-syntax` (syntaxe des scripts,
   aucun marqueur de conflit dans le dépôt).

## Conséquences

On peut toujours se connecter, même après une mise à jour cassée. La session de secours est le bureau
Plasma de KDE : moins joli, mais fonctionnel le temps de revenir en arrière.
