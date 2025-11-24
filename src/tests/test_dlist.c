#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib/dlist/dlist.h"

// comparator for integers
int cmp_ints(void* a, void* b) {
    int ia = *(int*)a;
    int ib = *(int*)b;
    return (ia - ib);
}

int main() {
    printf("== DList Basic Test ==\n");

    // false = not priority mode
    DList* list = dlist_create(false, cmp_ints);
    if (!list) {
        printf("Failed to create list!\n");
        return 1;
    }

    // DATA MUST BE MALLOC'ED because the dlist does NOT copy data.
    int* a = malloc(sizeof(int));
    int* b = malloc(sizeof(int));
    int* c = malloc(sizeof(int));

    *a = 10;
    *b = 20;
    *c = 30;

    dlist_push_back(list, a);
    dlist_push_back(list, b);
    dlist_push_front(list, c);

    printf("List after push_front/back: ");
    DLIST_FOREACH(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    // find the node for 20
    int key = 20;
    DListNode* found = dlist_find(list, &key, cmp_ints);
    if (found) {
        printf("Found 20 → removing it\n");
        dlist_remove_node(list, found, free);  // frees the integer
    }

    printf("List after removing 20: ");
    DLIST_FOREACH(list, node) {
        printf("%d ", *(int*)node->data);
    }
    printf("\n");

    printf("Size = %lu\n", (unsigned long)dlist_size(list));

    // clear everything
    dlist_destroy(list, free); // free all remaining ints

    printf("OK!\n");
    return 0;
}
