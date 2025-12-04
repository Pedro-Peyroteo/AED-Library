#ifndef SUGGESTION_H
#define SUGGESTION_H

#include <stddef.h>

typedef struct
{
    unsigned id;
    char title[128];
    char author[128];
    char isbn[32];
    char edition[32];
} Suggestion;

void suggestion_init(Suggestion *s,
                     unsigned id,
                     const char *title,
                     const char *author,
                     const char *isbn,
                     const char *edition);

int suggestion_from_csv(Suggestion *s, const char *line);
void suggestion_to_csv(const Suggestion *s, char *out, size_t out_size);

#endif /* SUGGESTION_H */
