#include "model/books.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void book_init(Book* b, unsigned id,
               const char* title,
               const char* author,
               int year,
               int available) 
{
    b->id = id;
    strncpy(b->title, title, sizeof(b->title));
    strncpy(b->author, author, sizeof(b->author));
    b->year = year;
    b->available = available;
}

int book_from_csv(Book* b, const char* line) {
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* token = strtok(tmp, ";");
    if (!token) return 0;
    b->id = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(b->title, token, sizeof(b->title));

    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(b->author, token, sizeof(b->author));

    token = strtok(NULL, ";");
    if (!token) return 0;
    b->year = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    b->available = atoi(token);

    return 1;
}

void book_to_csv(const Book* b, char* out, size_t out_size) {
    snprintf(out, out_size, "%u;%s;%s;%d;%d",
        b->id,
        b->title,
        b->author,
        b->year,
        b->available
    );
}
