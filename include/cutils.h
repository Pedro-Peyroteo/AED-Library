#ifndef CUTILS_H
#define CUTILS_H

#include <stddef.h>  // To use "size_t".
#include <stdbool.h> // To use bool type.

// A generic array-backed list.
typedef struct {
    void     *items;     // Pointer to the actual integers in memory.
    size_t  count;      // How many elements are currently stored.
    size_t  capacity;   // How many elements fit before the array must grow.
    size_t  elem_size;  // Size of each element in bytes.
} ArrayList;

// Initialize the list so it's safe to use (empty, no memory allocated yet).
void arraylist_init(ArrayList *list, size_t elem_size);

// Free any memory used by the list and reset it.
void arraylist_free(ArrayList *list);

// Append a value to the end. Returns false if any memory allocation fails.
bool arraylist_append(ArrayList *list, const void *value);

// Get pointer to element at index (returns NULL if out of bounds).
void *arraylist_get(ArrayList *list, size_t index);

#endif