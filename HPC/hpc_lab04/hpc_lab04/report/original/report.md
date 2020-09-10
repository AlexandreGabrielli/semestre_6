# laboratoire 4 - Perf & Valgrind
#### auteur : Alexandre Gabrielli
#### date : 

## matériel

j'effectue ce laboratoire sur un rasbery Pi 3 Model B Rev 1.2 donc voici les informations essentiels:
Hardware	: BCM2835
Revision	: a02082
Serial		: 0000000040be5365
le fabriquant est donc Sony Uk
et les informations sur le processeur: 
processor	: 1
model name	: ARMv7 Processor rev 4 (v7l)
BogoMIPS	: 38.40
Features	: half thumb fastmult vfp edsp neon vfpv3 tls vfpv4 idiva idivt vfpd32 lpae evtstrm crc32 
CPU implementer	: 0x41
CPU architecture: 7
CPU variant	: 0x0
CPU part	: 0xd03
CPU revision	: 4

## version de linux 
debian 10.3
## version du kernel
4.19.97-v7+

### installation 

perf sur debian pose quelque problème , après avoir installer linux tools j'ai due installer une autre version de perf car le package 4.19 n'as pas été écris sur debian et je n'ai pas envie de l'écrire moi même.
du coup j'ai installer linux-perf-4.9 et ensuite modifier le fichier /usr/bin/perf  et modifier la ligne exec "perf_4.$version" "$@" par exec "perf_4.9" "$@" cella implique que perf n'est pas a as 100% fonctionnel et peu nécessiter d'utilisé les paramètre  --no-demangle ou --call-graph=lbr works.


## perf

pour commencer a profiler nos deux fonctions nous allons commencer par utilisé perf en mesurant les events suivants:

- task-clock pour voir l'utilisation du cpu
- context-switches pour voir s'il y a beaucoup de changement de contexte
- page-faults afin de voir si on a bien géré nos data
- branches et branch-misses pour voir si on est limité par des branch mis-prediction

nous allons lancé perf sur ./sort array 100000 et ./sort list 100000

### array 

![array with perf first version](.\_array_perf_first_version.png)


### discutions array 

on voit que nous avons 0 % de cache miss mais notre programme prend beaucoup de temps, on utilise beaucoup de temps cpu et nous avons énormément de branches car nous utilisons un tri par insertion et nous devons donc effectuer beaucoup de comparaison.

en changeant la premières version de notre code (ci dessous): 

```c
/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len) {
    int i = 0, j = 0, tmp;
    for (i = 0; i < len; i++) {   // loop n times - 1 per element
        for (j = 0; j < len - i - 1; j++) { // last i elements are sorted already
            if (data[j] > data[j + 1]) {  // swop if order is broken
                tmp = data[j];
                data[j] = data[j + 1];
                data[j + 1] = tmp;
            }
        }
    }
}
```
par un tri nécessitant moins de comparaison comme un tri par sélection (code ci-dessous): 
```c

/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len) {
    /*tri par selection*/
       int passage = 0;
    bool permutation = true;
    int en_cours;
   
    while ( permutation) {
        permutation = false;
        passage ++;
        for (en_cours=0;en_cours<20-passage;en_cours++) {
            if (data[en_cours]>data[en_cours+1]){
                permutation = true;
                // on echange les deux elements
                int temp = data[en_cours];
                data[en_cours] = data[en_cours+1];
                data[en_cours+1] = temp;
            }
        }
    }

}
```
on obtiens de bien meilleur performance. 

![array with perf selection sort](.\_array_perf_selection.png)

on voit que nous avons plus de branch-misses (environ 1.4%) mais nous effectuons beaucoup moins de branches (environ 700'000 contre plus de 5'000'00'000) et nécessitons moins de temps cpu (12ms contre ~120'000 ms). 

le résultat est donc un programme beaucoup plus rapide malgré le fait que nous avons beaucoup plus de branch-misses.

essayons maintenant d'amélioré encore cella avec le tri rapide:

```c
void array_sort(int *tableau, int taille) {
    int mur, courant, pivot, tmp;
    if (taille < 2) return;
    // On prend comme pivot l element le plus a droite
    pivot = tableau[taille - 1];
    mur  = courant = 0;
    while (courant<taille) {
        if (tableau[courant] <= pivot) {
            if (mur != courant) {
                tmp=tableau[courant];
                tableau[courant]=tableau[mur];
                tableau[mur]=tmp;              
            }
            mur ++;
        }
        courant ++;
    }
    array_sort(tableau, mur - 1);
    array_sort(tableau + mur - 1, taille - mur + 1);
}

```

on obtiens 

![array with perf quick sort](.\_array_perf_rapide.png)

on voit que les deux derniers tri sont assez similaire, pour les séparer augmentons de manière significatif et de voir la différence entre les deux : 

###### selection sort

![array with perf selectionSort_100000000](.\_array_perf_selctionSort_100000000.png)

###### quick sort

![ array with perf quickSort_100000000](.\_array_perf_quickSort_100000000.png)

on peu voir que si nous augmentons la taille de l'array le quick sort fait beaucoup plus de page-faults surement due à la récursion. Nous préférerons donc utilisé le sélection sort

### list

![list with perf first version](.\_list_perf_first_version.png)

### discutions list

on peut voir que nous avons autant de branches que notre premier code de array mais en plus nous avons énormément de branch-misses. comme nous ne pouvons pas effectuer facilement un tri par sélection ou quick sort nous allons remplacé notre premier code par un tri par insertion que nous dont nous avons trouver <a url="https://www.geeksforgeeks.org/insertion-sort-for-singly-linked-list/">le code</a> sur internet 
```c
/* Arrange a list in increasing order of value */
void list_sort(struct list_element *start) {
    int swapped, i;
    struct list_element *ptr1;
    struct list_element *lptr = NULL;

    /* Checking for empty list */
    if (start == NULL)
        return;

    do {
        swapped = 0;
        ptr1 = start;

        while (ptr1->next != lptr) {
            if (ptr1->data > ptr1->next->data) {
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    } while (swapped);
}
```
deviens : 

```c
/ function to sort a singly linked list using insertion sort
void list_sort(struct list_element **head_ref)
{
    // Initialize sorted linked list
    struct list_element *sorted = NULL;

    // Traverse the given linked list and insert every
    // node to sorted
    struct list_element *current = *head_ref;
    while (current != NULL)
    {
        // Store next for next iteration
        struct list_element *next = current->next;

        // insert current in sorted linked list
        sortedInsert(&sorted, current);

        // Update current
        current = next;
    }

    // Update head_ref to point to sorted linked list
    *head_ref = sorted;
}

/* function to insert a new_node in a list. Note that this
  function expects a pointer to head_ref as this can modify the
  head of the input linked list (similar to push())*/
void sortedInsert(struct list_element** head_ref, struct list_element* new_node)
{
    struct list_element* current;
    /* Special case for the head end */
    if (*head_ref == NULL || (*head_ref)->data >= new_node->data)
    {
        new_node->next = *head_ref;
        *head_ref = new_node;
    }
    else
    {
        /* Locate the node before the point of insertion */
        current = *head_ref;
        while (current->next!=NULL &&
               current->next->data < new_node->data)
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}
```

on peut voir que le résultat est beaucoup plus satisfaisant: 

![list_insertionSort](.\list_insertionSort.png)

## valgrind 

## array

 ![valgrind_array](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\lab04\hpc_lab04\report\valgrind_array.png)

![valgrind_array2](.\valgrind_array2.png)



comme notre code d'array est plutôt simple on ne voit pas trop de problème qui apparait avec calgrind, il ne semble pas y avoir de goulet d'étranglement bien défini.

## List 

 ![valgrind_list_resume](.\valgrind_list_resume.png)

![valgrind_list_read](.\valgrind_list_read.png)

on voit très bien qu'il y a beaucoup de D1 misses dans ce programme et on voit très bien que le goulet d'étranglement se trouve a la ligne 

```c
if (*head_ref == NULL || (*head_ref)->data >= new_node->data)
```

je vais donc essayer de retenir directement l'adresse et change cette fonction comme suis :

```c
void sortedInsert(struct list_element** head_ref, struct list_element* new_node)
{
    struct list_element* current;
    struct list_element* tmp = *head_ref;
    /* Special case for the head end */
    if (tmp == NULL || (tmp)->data >= new_node->data)
    {
        new_node->next = tmp;
        tmp = new_node;
    }
    else
    {
        /* Locate the node before the point of insertion */
        current = tmp;
        while (current->next!=NULL &&
               current->next->data < new_node->data)
        {
            current = current->next;
        }
        new_node->next = current->next;
        current->next = new_node;
    }
}
```



![Nice_1](.\Nice_1.png)

![Nice_2](.\Nice_2.png)

on peut voir que le résultats est surprenant, la ligne n'est plus le goulet d'étranglement qu'il était avant et le nombre de miss dans la d1 a très très fortement diminuer, on vérifie avec perf pour voir si la différence de temps est significatif

 ![amazing](C:\Users\Alexandre\Documents\cours\semestre_6\HPC\lab04\hpc_lab04\report\amazing.png)

on voit bien que l'on a extrêmement réduit le temps cpu, le nombre de branches , même si on a gagner quelque branch-misses on est beaucoup plus rapide avec cette version.



## conclusion

J'ai pu grâce a ce laboratoire comprendre comment utilisé valgrind et perf pour detecter les goulets d'étranglement, grâce au piste donné par les deux outils j'ai pu dans un premier temps sélectionner un meilleur algorithme de tri et dans un seconde nettement amélioré mes performances en particulier pour la liste chainé. 

On peu voir que la dernière transformation de code effectuer sur la fonction `sortedInsert` qui semble de prime abord anodin a permis un énorme gain de performance, surement du au fait qu'a cause des pointeurs le compilateur ne pouvait pas prouver certaines choses pour pouvoir faire des raccourcis mais nous verrons cella plus avant dans le cours. 

J'ai pu remarquer aussi que c'était inutile de vouloir a tout pris réduire les branch-misses car cella peut rajouter beaucoup de branch et de task-clock et pas rentable en terme de performance, le but est donc d'avoir un bon ratio entre branches / branch-misses et task.clock. 



