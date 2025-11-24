/*
    Small generic utilities library.

    Currently it provides a simple ArrayList type: a dynamic array
    of fixed-size elements (void* internally) that can grow as you
    append new items. It is independent from the rest of the project
    and can be reused by other modules if needed.
*/

#ifndef CUTILS_H
#define CUTILS_H

#include <stddef.h>  /* size_t */
#include <stdbool.h> /* bool   */

/*
    Generic array-backed list.

    - items:     pointer to the raw memory buffer.
    - count:     how many elements are currently stored.
    - capacity:  how many elements fit before a reallocation is needed.
    - elem_size: size (in bytes) of each element in the buffer.
*/
typedef struct {
    void   *items;
    size_t count;
    size_t capacity;
    size_t elem_size;
} ArrayList;

/*
    Initialize the list so it's safe to use.
    Starts empty with no memory allocated yet.
*/
void arraylist_init(ArrayList *list, size_t elem_size);

/*
    Free any memory used by the list and reset all fields.
    Does NOT free any objects that the caller might have
    stored inside the elements; it only frees the backing buffer.
*/
void arraylist_free(ArrayList *list);

/*
    Append one element to the end of the list.

    The value pointer must point to at least elem_size bytes.
    Returns false if any memory allocation fails, true otherwise.
*/
bool arraylist_append(ArrayList *list, const void *value);

/*
    Get a pointer to the element at the given index.

    Returns NULL if the index is out of bounds. The returned
    pointer is owned by the list and remains valid until the
    list is reallocated or freed.
*/
void *arraylist_get(ArrayList *list, size_t index);

#endif