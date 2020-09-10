#include "array_util.h"
#include <stdbool.h>

/* Liberate array memory */
void array_clear(uint64_t *data) {
    free(data);
}


/* Allocate an array of size "len" and fill it
 * with random data.
 * Return the array pointer */
uint64_t *array_init(const size_t len) {
    srand((unsigned) 1991);
    uint64_t *ptr = (uint64_t *) malloc(len * sizeof(uint64_t));
    int i;
    for (i = 0; i < len; i++) {
        ptr[i] = rand();
    }
    return ptr;
}


/* Arrange a array in increasing order of value */
void array_sort(uint64_t *data, const size_t len) {
    /*tri par selection*/
    int passage = 0;
    bool permutation = true;
    int en_cours;

    while (permutation) {
        permutation = false;
        passage++;
        for (en_cours = 0; en_cours < 20 - passage; en_cours++) {
            if (data[en_cours] > data[en_cours + 1]) {
                permutation = true;
                // on echange les deux elements
                int temp = data[en_cours];
                data[en_cours] = data[en_cours + 1];
                data[en_cours + 1] = temp;
            }
        }
    }

}

/*this function print the array */
void print_array(uint64_t *data, const size_t len) {
    printf("[");
    int i = 0;
    do {
        printf("%llu", data[i]);
        i++;
    } while (i < len && printf(","));
    printf("]\n");

}
