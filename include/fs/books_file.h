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

/**
 * @brief Lê todas as sugestões do ficheiro 'suggestions.txt' para uma DList.
 * * @param path O caminho para o ficheiro suggestions.txt.
 * @return DList* com ponteiros para Suggestion, ou NULL em caso de erro de memória.
 */
DList *file_load_suggestions(const char *path);

/**
 * @brief Escreve a DList de sugestões para o ficheiro 'suggestions.txt'.
 * * O ficheiro existente é sobrescrito.
 * * @param path O caminho para o ficheiro suggestions.txt.
 * @param suggestions A lista de sugestões a guardar.
 * @return 0 em caso de sucesso, -1 em caso de falha na abertura do ficheiro.
 */
int file_save_suggestions(const char *path, const DList *suggestions);

#endif // BOOKS_FILE_H
