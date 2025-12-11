#include "model/books.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void book_init(Book* b, unsigned id,
               const char* title,
               const char* author,
               int year,
               const char* edition,
               int available) 
{
    b->id = id;
    strncpy(b->title, title, sizeof(b->title));
    strncpy(b->author, author, sizeof(b->author));
    b->year = year;
    strncpy(b->edition, edition ? edition : "", sizeof(b->edition));
    b->edition[sizeof(b->edition) - 1] = '\0';
    b->available = available;
}

int book_from_csv(Book* b, const char* line) {
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    /* Manual parsing to handle empty fields (edition can be empty) */
    char *ptr = tmp;
    char *fields[6];
    int field_count = 0;
    
    /* Split by semicolon, preserving empty fields */
    fields[field_count++] = ptr;
    while (*ptr && field_count < 6) {
        if (*ptr == ';') {
            *ptr = '\0';
            fields[field_count++] = ptr + 1;
        }
        ptr++;
    }
    
    if (field_count < 6) return 0;
    
    b->id = atoi(fields[0]);
    strncpy(b->title, fields[1], sizeof(b->title));
    b->title[sizeof(b->title) - 1] = '\0';
    strncpy(b->author, fields[2], sizeof(b->author));
    b->author[sizeof(b->author) - 1] = '\0';
    b->year = atoi(fields[3]);
    strncpy(b->edition, fields[4], sizeof(b->edition));
    b->edition[sizeof(b->edition) - 1] = '\0';
    b->available = atoi(fields[5]);

    return 1;
}

void book_to_csv(const Book* b, char* out, size_t out_size) {
    snprintf(out, out_size, "%u;%s;%s;%d;%s;%d",
        b->id,
        b->title,
        b->author,
        b->year,
        b->edition,
        b->available
    );
}
