#ifndef BOOKS_FILE_H
#define BOOKS_FILE_H

#include <stdio.h>

// Your models
#include "model/books.h"

// Your dlist library (path may differ depending on your -I flags)
#include "lib/dlist/dlist.h"

/**
 * Loads all books from a CSV file into a DList.
 *
 * Format expected (one book per line, *no* header line, but you can add one if you want and skip it):
 *   id;title;author;year;available
 *
 * Return:
 *   - pointer to a DList containing Book* (heap-allocated)
 *   - or NULL on fatal error (e.g. out of memory)
 *
 * NOTE: If the file does not exist, you can choose to:
 *   - return an empty list (preferred for "first run" of the app)
 *   - or return NULL and treat as error in the caller
 */
DList *file_load_books(const char *path);

/**
 * Saves all books from the given DList to a CSV file.
 *
 * The list is assumed to store Book* as its data.
 *
 * Return:
 *   0 on success
 *  -1 on failure (e.g. fopen error, write error)
 */
int file_save_books(const char *path, const DList *books);

#endif // BOOKS_FILE_H
