/*
    Implementation of the small ArrayList helper type.

    This is intentionally simple and generic:
    - it stores elements in a contiguous heap-allocated buffer;
    - it grows geometrically when more capacity is needed;
    - it knows only the size of each element, not its semantics.
*/

#include "cutils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 4

/*
    Initialize the list to a safe empty state.
    No memory is allocated until the first append.
*/
void arraylist_init(ArrayList *list, size_t elem_size)
{
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->elem_size = elem_size;
}

/*
    Ensure the list has at least min_capacity slots available.
    If the current capacity is too small, the internal buffer
    is reallocated with a larger capacity.

    Returns false if any overflow is detected or if allocation fails.
*/
static bool ensure_capacity(ArrayList *list, size_t min_capacity)
{
    if (list->capacity >= min_capacity)
        return true;

    /* Calculate new capacity (double, or start at INITIAL_CAPACITY). */
    if (list->capacity > SIZE_MAX / 2)
        return false; /* overflow guard */

    size_t new_capacity = (list->capacity == 0)
        ? INITIAL_CAPACITY
        : list->capacity * 2;
    if (new_capacity < min_capacity)
        new_capacity = min_capacity;

    size_t new_size = new_capacity * list->elem_size;
    if (new_size / list->elem_size != new_capacity)
        return false; /* overflow guard */

    void *new_items = realloc(list->items, new_size);
    if (!new_items)
    {
        fprintf(stderr, "[ArrayList] Memory allocation failed.\n");
        return false;
    }

    list->items = new_items;
    list->capacity = new_capacity;
    return true;
}

/*
    Append a new element to the list, growing the buffer if needed.
*/
bool arraylist_append(ArrayList *list, const void *value)
{
    if (!ensure_capacity(list, list->count + 1))
        return false;

    void *dest = (char *)list->items + list->count * list->elem_size;
    memcpy(dest, value, list->elem_size);
    list->count++;
    return true;
}

/*
    Return a pointer to the element at the given index, or NULL
    if the index is out of bounds.
*/
void *arraylist_get(ArrayList *list, size_t index)
{
    if (index >= list->count)
        return NULL;
    return (char *)list->items + index * list->elem_size;
}

/*
    Free the backing buffer (if any) and reset the list to an
    empty, safe state.
*/
void arraylist_free(ArrayList *list)
{
    free(list->items);
    list->items = NULL;
    list->count = 0;
    list->capacity = 0;
    list->elem_size = 0;
}