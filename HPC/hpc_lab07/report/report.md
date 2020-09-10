# laboratoire 6 HCP 

> auteur : Alexandre Gabrielli

## performance de base 

on lance le benchmark sur le serveur sans modification de code et on obtiens les résultats suivant :

```

Building...
gcc -c sobel.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o sobel.o
gcc -c main.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o main.o
gcc -c image.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o image.o
gcc  sobel.o  main.o  image.o -lm  -o sobel
g++ -c gbench.cpp -O3 -g -Wall -I../../lib/stb -I../../lib/benchmark/include -DNDEBUG -o gbench.o
g++ gbench.o sobel.o image.o -L../../lib/benchmark/build/src -lbenchmark -lpthread -lm -o sobel-bm

Running...
2020-06-12 21:16:16
Running ./sobel-bm
Run on (8 X 2300 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 46080 KiB (x4)
Load Average: 0.33, 0.08, 0.03
-----------------------------------------------------------------------------------------------------------
Benchmark                                                                 Time             CPU   Iterations
-----------------------------------------------------------------------------------------------------------
RGB2GSFixture/rgb2gs_test/images/medalion.png:4593121                  4246 us         4246 us          125
RGB2GSFixture/rgb2gs_test/images/half-life.png:4593100                13766 us        13765 us           48
RGB2GSFixture/rgb2gs_test/images/stars.png:4593083                    98135 us        98132 us            6
FiltersFixture/gaussian_filter_test/images/medalion.png:4593121        9048 us         9048 us           79
FiltersFixture/gaussian_filter_test/images/half-life.png:4593100      95498 us        95497 us            7
FiltersFixture/gaussian_filter_test/images/stars.png:4593083        1175297 us      1175285 us            1
FiltersFixture/sobel_filter_test/images/medalion.png:4593121          15378 us        15377 us           43
FiltersFixture/sobel_filter_test/images/half-life.png:4593100        102200 us       102199 us            7
FiltersFixture/sobel_filter_test/images/stars.png:4593083           1320385 us      1320370 us            1


```

# amélioration 

## minimiser les duplications de calcul

Comme dans les filtres que l'on a vue en machine learning,  on a "un carré" qui se déplace et utilise le calcule précédent pour éviter de tout recalculer a chaque fois. 
Hors on vois que dans notre cas on recalcul a chaque fois qu'on se déplace le calcul du carré précédent. Hors nous pouvons réutilisé la précédente pour la majorité des pixels (excepter le dernier sur lequel nous tournons).

le but est donc de mémoriser un tableau 3 *3 qui va suivre le déploiement de notre filtre, au début on l'initialise simplement comme si nous effectuons les calculs pour le 1er carré: 

```c
    for (int i= 2 ; i >= 0 ;i--){
        tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
        tableu[i][1] = img->data[(i) * img->width + (1)];
        tableu[i][2] = 0;
    }   

```

puis a chaque fois on calculera les 3 derniers point les plus a droite: 

```c
    for (int i= 2 ; i >= 0 ;i--){
            tableu[i][2] = img->data[(y + i -1 ) * img->width + (x + 1)];
        }  
```

a chaque fois qu'on bouge d'un point vers la droite on déplace les pixels de notre tableau vers la gauche: 

```c
        for (int i= 2 ; i >= 0 ;i--){
            tableu[i][0] = tableu[1][1];
            tableu[i][1] = tableu[i][2];
        }   
```



et on oublie pas qu'en fin de ligne il faut mettre le tableau pour le début de ligne (on recalcule quelque points ici mais ce sera très compliquer de retenir chaque ligne et de la décaler de 1 vers le haut etc etc, je reset donc le tableau a la prochaine ligne) : 

```c
if (y !=0 && y != img->height){
            for (int i= 2 ; i >= 0 ;i--){
                tableu[i][0] = img->data[(i) * img->width + (1 - 1)];
                tableu[i][1] = img->data[(i) * img->width + (1)];
                tableu[i][2] = 0;
            } 
        }
```

On effectue cela pour les deux filtres et normalement on devrait voir une amélioration 

## résultat 

```
Building...
gcc -c sobel.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o sobel.o
gcc -c main.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o main.o
gcc -c image.c -O3 -g -Wall -I../../lib/stb/ -fno-inline -o image.o
gcc  sobel.o  main.o  image.o -lm  -o sobel
g++ -c gbench.cpp -O3 -g -Wall -I../../lib/stb -I../../lib/benchmark/include -DNDEBUG -o gbench.o
g++ gbench.o sobel.o image.o -L../../lib/benchmark/build/src -lbenchmark -lpthread -lm -o sobel-bm

Running...
2020-06-12 22:13:54
Running ./sobel-bm
Run on (8 X 2300 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 46080 KiB (x4)
Load Average: 0.15, 0.04, 0.01
-----------------------------------------------------------------------------------------------------------
Benchmark                                                                 Time             CPU   Iterations
-----------------------------------------------------------------------------------------------------------
RGB2GSFixture/rgb2gs_test/images/medalion.png:4593345                  6953 us         6953 us           86
RGB2GSFixture/rgb2gs_test/images/half-life.png:4593324                14967 us        14966 us           37
RGB2GSFixture/rgb2gs_test/images/stars.png:4593307                   146336 us       146333 us            6
FiltersFixture/gaussian_filter_test/images/medalion.png:4593345        7657 us         7657 us           92
FiltersFixture/gaussian_filter_test/images/half-life.png:4593324      23177 us        23177 us           28
FiltersFixture/gaussian_filter_test/images/stars.png:4593307         187988 us       187985 us            4
FiltersFixture/sobel_filter_test/images/medalion.png:4593345          12282 us        12282 us           57
FiltersFixture/sobel_filter_test/images/half-life.png:4593324         43918 us        43915 us           16
FiltersFixture/sobel_filter_test/images/stars.png:4593307            391307 us       391302 us            2
```



sur le filtre gaussian on a gagner en temps cpu: 

- 15.4% sur le médaillon
- 75.7 % sur half-life 
- 84 % sur les étoiles (résultat exactement pareil)

sur le filtre sobel on a gagner en temps cpu : 

- 20 % sur le médaillon
- 57 % sur half-life
- 70.4 % sur les étoiles 

On voit bien que les résultats dépendent de l'image, je n'ai pas investigué plus que ca pour comprendre pourquoi.

## conclusion

dans ce laboratoire il nous a fallut bien comprendre ce que fessait le code pour pouvoir amélioré ces performances, quand nous avons compris qu'il s'agissait d'un simple filtre qu'on peu faire déplacer de gauche a droite de l'image et réutilisé donc les donné précédemment calculer il a été très facile de créer notre carré 3x3 et de l'utilisé dans ce design .  