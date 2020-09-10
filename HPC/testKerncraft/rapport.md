# HPC TOOL PROJECT : kerncraft

> Auteur : Alexandre Gabrielli

**Introduction** 

L’outils kerncraft a pour d'effectuer une analyse statique d'un code noyau afin de donner un aperçu des goulots d'étranglement du noyaux et mémoire. Il permet de produire et récupérer les donner afin d'appliquer des modèles de performance.

Kerncraft propose donc des modèles d'analyse qui n'exécute pas réellement le code cible, il se base sur une description yaml de l'architecture cible afin de prédire les performances.

**Installation**

Lorsque l'on installe l'outils il nécessite IACA de Intel, pour ce faire un script python iaca_get est fourni malheureusement il n'est plus d'actualité. J'ai donc simplement réécrit rapidement le script en changeant les url de download des version d'IACA nécessaire (on peut voir dans la liste de commit git qu’ils sont en train d'essayer de fixer cella mais leur pipeline ne passe pas encore les tests).



**Modèle donnée utilisé par Kerncraft**

Kerncraft propose trois modèles d'analyse et de prédiction différente : ECM, Roofline et Benchmark. ECM et Roofline n'exécute pas le code à proprement parler tandis que Benchmark oui.

Roofline

Roofline est un modèle qui considéré l'intensité opérationnelle qui représente le nombre d'opérations par octet du trafic mémoire. Ce modèle permet de prédire dans un système donné ce chiffre grâce à des formule. Plus de détail dans la doc officiel.

**Exécution-Cache-Memory Model (ECM)**

Basé sur Roofline se modèle donne plus d'importance à la hiérarchie du cache et introduit d'autre mesure comme les opérations ponctuelles par seconde (FLOP/s) et le cycles par ligne de cache (cy/CL). (plus petite quantité de données transférable entre la mémoire principal et les niveaux de cache). 

**CLI** 

Kerncraft s'utilise principalement en ligne de commande 

kerncraft --machine MACHINE --pmodel PMODEL [-D KEY VALUE] [--verbose] [--asm-block BLOCK] KERNELFILE [KERNELFILE ...]

Ou machine est le chemin vers le yaml de description de la machine

KERNELFILE est le chemin vers le code noyau que l'on veut analyser 

-D permet de passer les paramètres nécessaires au code

PMODEL permet de passer le modèle de performance avec lequel on souhaite analyser le code il en existe plusieurs :

### ECM : combinaison d'ECMData   et d'ECMCPU

![ECM](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\testKerncraft\ECM.png)

cette outils nous permet de savoir a combien de cores théoriquement nous serons saturé. Et nous donne pour chaque nombre de cores une approximation du cy/CL(cycles par ligne de cache = plus petite quantité de données transférable entre la mémoire principal et les niveaux de cache). Cette donnée est très interessante car elle nous permet de savoir (sans avoir les CPU physiquement puisque tout ce fait en static) lorsque l'on cherche la scalabilité a combien de cores il faut théoriquement s'arrêter. 

### ECMData

![ECMData](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\testKerncraft\ECMData.png)

ECMData nous donne toujours les cy/CL mais ici en donnant par niveau de cache et memoire système.

### EMCPU : ECM d’Intel (via     IACA)

![ECMCPU](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\testKerncraft\ECMCPU.png)

il faut s'avoir que cette commande ne marche que sur les architectures intel (normal puisque IACA est fait pour et par intel), elle nous donne un T_COMP qui est une moyenne des niveau de cache supérieur a L1 et aussi un T_RegL1 qui est un resultat pour la cache L1

### Roofline : Roofline "custom"     de Kerncraft

![Roofline](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\testKerncraft\Roofline.png)

Cette outils nous permet de detecter les goulets d'étranglement au niveau de la mémoire, on voit ici que le  goulet d'étranglement ce situe au niveau de la cache L3  (FLOP/s = opérations ponctuelles par seconde) et nous donne l'intensité arithmetic en FLOP / Bytes.

### RooflineIACA : donne le résultat     d'un Roofline Intel avec iaca

![RooflineIACA](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\testKerncraft\RooflineIACA.png)

même chose que précedament mais cette commande ne marche que pour les architectures intel.

### Benchmark

Nous n'avons pas pu tester cette fonctionnalité a cause d'un BUG (voir section suivante)

**bug** 

Normalement ont généré la description de notre machine en utilisant likwid_bench_auto et ont rempli les champs manquant (REQUIRED_INFORMATION) mais bien sur ce script ne fonctionne pas. Essayer de débugger deviens très compliquer car on peut qualifier les scripts de "code spaghetti" tellement il est complexe de le lire. Nous allons donc prendre un yaml de description d'architecture et utilisé pour analyser avec les deux modèle ECM et Roofline qui n'exécute pas réellement le code. ici nous nous baserons uniquement sur des yaml fourni.b  

**Conclusion**

Kerncraft est un outil extrêmement intéressant combinant deux modèles d'HPC. Il permet grâce à sa description yaml de pouvoir prédire des résultats via une définition de la machine sans devoir possèder les CPUs. On peu donc l'utilisé pour le choix d'un processeur lorsque l'on a une opération spécifique a réalisé. On peu aussi grace au modèle ECM utilisé par kerncraft savoir combien de cores on doit acheter pour une scalabilité maximal. On voit aisément l'utilité de cette outils si on désire construire un système répartie (combien de cores sur chaque machine en prennant en compte en plus le transfert de donné entre machine).

 Malheureusement le code python (principal langage de Kerncraft) est un code spaghetti est on voit, lorsqu'on regarde les commit du repo git ,que son créateur a énormément de mal à le maintenir. Lorsque l'on veut l'utiliser on se retrouve souvent à devoir débugger des bouts de code ce qui rend son utilisation extrêmement complexe. 

De plus la documentation est inéxistante, la seul "doc" est la thése de master qui date de 2015 et qui n'est donc plus à jour. 

Kerncraft est un très bonne outils car il propose une méthode de calcul statique qu'on retrouve peu mais souffre d'un manque de rigueur au niveau de son code (kerncraft n'était pas le but premier de la thèse de master mais bien les calculs qui sont dessous) qui rend son maintient quasiment impossible.

 