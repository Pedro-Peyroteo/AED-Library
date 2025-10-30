#include "cutils.h"
#include <stdio.h>
#include <stdlib.h> // For malloc()...
#include <string.h> // For memcpy().

#define INITIAL_CAPACITY 4


/*
    Initialize the list to a safe empty state.
    No allocated memory.
*/ 
void arraylist_init(ArrayList *list, size_t elem_size) {
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->elem_size = elem_size;
}

/*  
    Ensures the list has enough capacity to hold at least min_capacity elements.
    If not enough, it will grow the internal array.
    Returns false if any memory allocation fails.
*/
static bool ensure_capacity(ArrayList *list, size_t min_capacity){
    if (list->capacity >= min_capacity) return true;

    // Calculate new capacity (double or start at INITIAL_CAPACITY).
    if (list->capacity > SIZE_MAX / 2) return false; // Overflow guard.
    size_t new_capacity = (list->capacity == 0) ? INITIAL_CAPACITY : list->capacity * 2;
    if (new_capacity < min_capacity) new_capacity = min_capacity;

    // Total bytes required.
    size_t new_size = new_capacity * list->elem_size;
    if (new_size / list->elem_size != new_capacity) return false; // Overflow guard.

    // Debug info.
    printf("[DEBUG] Reallocating: Old Cap=%zu, New Cap=%zu, Elem Size=%zu, Total Bytes=%zu\n",
           list->capacity, new_capacity, list->elem_size, new_size);

    // Reallocate memory.
    void *new_items = realloc(list->items, new_size);
    if (!new_items) {
        fprintf(stderr, "[ERROR] Memory allocation failed.\n");
        return false;
    }

    list->items = new_items;
    list->capacity = new_capacity;
    return true;
}

// Append value to list.
bool arraylist_append(ArrayList *list, const void *value) {
    // Checks if list has enough capacity to hold new value.
    if (!ensure_capacity(list, list->count + 1)) return false;

    // Append the value and increment the count.
    //list->items[list->count++] = value;
    void *dest = (char *)list->items + list->count * list->elem_size;

    // Copy the element into the array
    memcpy(dest, value, list->elem_size);
    list->count++;

    // Debug: since we don't know the element type, just print pointer & state
    printf("[DEBUG] Appended element | Count = %zu | Capacity = %zu\n",
           list->count, list->capacity);

    return true;
}

// Get pointer to element at index.
void *arraylist_get(ArrayList *list, size_t index) {
    if (index >= list->count) return NULL;
    return (char *)list->items + index * list->elem_size;
}


// Free any memory used by the list (if any) and reset it.
void arraylist_free(ArrayList *list) {
    // Free the allocated memory.
    free(list->items);

    // Debug: Print a message confirming the memory has been freed.
    printf("[DEBUG] Memory freed. Count = %zu, Capacity = %zu, Items = %p\n",
           list->count, list->capacity, (void *)list->items);

    // Reset the list to a safe empty state.
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->elem_size = 0;

    printf("[DEBUG] List items reset. Count = %zu, Capacity = %zu, Items = %p\n",
           list->count, list->capacity, (void *)list->items);
}