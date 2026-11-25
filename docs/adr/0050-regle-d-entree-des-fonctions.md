# ADR 0050 — Règle d'entrée des fonctions (remplace le gel de l'ADR 0039)

- **Statut** : accepté
- **Date** : 2026-11-25

## Contexte

L'ADR 0039 gelait les fonctions jusqu'à la 1.0. En pratique, le gel a été levé à chaque lot
(design, espaces, icônes, curseurs, bureau, énergie, filet de sécurité), parce que l'usage quotidien
sur le portable montrait de vrais besoins. La relecture des ADR du 25/11/2026 (`REVUE-2026-11.md`)
propose de remplacer le gel par la règle réellement suivie.

## Décision

1. **Une fonction entre** si l'usage sur le portable en montre le besoin, et, pour tout ce qui se voit,
   après **maquette ou capture avant/après validée** par l'utilisateur.
2. Le **critère de sortie 1.0** de l'ADR 0039 reste valable (recette, CI verte, ISO neuve mise à jour
   par le dépôt APT), ainsi que l'organisation des branches, complétée par le canal `stable` (ADR 0049).
3. **Firefox** : Firefox ESR reste le navigateur de la 1.0 (ADR 0006), à revoir ensuite. Il utilise la
   barre de titre de VinX (pastilles, ADR 0024) : `browser.tabs.inTitlebar = 0`, en préférence par défaut
   (paquet vinx-shell et politique de l'ISO), modifiable par l'utilisateur.
4. La session s'appelle « VinX OS » à l'écran de connexion (plus « expérimental »).
