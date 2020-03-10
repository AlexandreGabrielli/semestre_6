# exercice logique

degrés de menace d'un autre joueur dans un jeu vidéo

## entrée 

distance entre soi et l'autre joueur

différence de niveau

## sortie

niveau de menace => prendre décision 

fuir - ignorer - attaque (reculer - rester sur place - avancer)

### règle 

- proche && niveau désavantageux => FUIR
- loin && différence de niveau négatif => IGNORER 
- proche && différence de niveau négatif => ATTAQUER

