#ifndef BOOKS_H
#define BOOKS_H

#include <stddef.h>

typedef struct {
    unsigned id;
    char title[128];
    char author[128];
    int year;
    int available;
} Book;

/* Constructor-like initializer */
void book_init(Book* b, unsigned id,
               const char* title,
               const char* author,
               int year,
               int available);

/* CSV helpers */
int  book_from_csv(Book* b, const char* line);         // parse "id;title;author;year;available"
void book_to_csv(const Book* b, char* out, size_t out_size);

#endif
