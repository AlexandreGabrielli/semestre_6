#include <list_util.h>

/* A list_element must at least contain a link to the next
 * element, as well as a uint64_t data value */
struct list_element {
    uint64_t data;
    struct list_element *next;
};

/* Allocate "len" linked elements and initialize them
 * with random data.
 * Return list head */
struct list_element *list_init(size_t len) {
    srand((unsigned) 1991);
    struct list_element *head = (struct list_element *) malloc(sizeof(struct list_element));
    head->data = rand();
    head->next = NULL;
    int i;
    struct list_element *current = head;
    for (i = 0; i < len; i++) {
        current->next = (struct list_element *) malloc(sizeof(struct list_element));
        current = current->next;
        current->next = NULL;
        current->data = rand();
    }
    return head;
}

/* Liberate list memory */
void list_clear(struct list_element *head) {
    struct list_element *next = head;
    struct list_element *nextNext;
    while (next != NULL) {
        nextNext = next->next;
        free(next);
        next = nextNext;
    }
}

/*this function print the list*/
void print_list(struct list_element *head) {
    struct list_element *next = head;
    printf("{");
    do {
        printf("%llu", next->data);
        next = next->next;
    } while (next != NULL && printf(","));
    printf("}\n");
}

/* function to swap data of two nodes a and b*/
void swap(struct list_element *a, struct list_element *b) {
    int temp = a->data;
    a->data = b->data;
    b->data = temp;
}




// Function to insert a given node in a sorted linked list
void sortedInsert(struct list_element**, struct list_element*);

// function to sort a singly linked list using insertion sort
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

