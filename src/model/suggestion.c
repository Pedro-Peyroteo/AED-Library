#include "model/suggestion.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void suggestion_init(Suggestion *s,
                     unsigned id,
                     const char *title,
                     const char *author,
                     const char *isbn,
                     const char *edition)
{
    if (!s)
        return;

    s->id = id;
    strncpy(s->title, title ? title : "", sizeof(s->title));
    s->title[sizeof(s->title) - 1] = '\0';

    strncpy(s->author, author ? author : "", sizeof(s->author));
    s->author[sizeof(s->author) - 1] = '\0';

    strncpy(s->isbn, isbn ? isbn : "", sizeof(s->isbn));
    s->isbn[sizeof(s->isbn) - 1] = '\0';

    strncpy(s->edition, edition ? edition : "", sizeof(s->edition));
    s->edition[sizeof(s->edition) - 1] = '\0';
}

int suggestion_from_csv(Suggestion *s, const char *line)
{
    if (!s || !line)
        return 0;

    char buffer[512];
    strncpy(buffer, line, sizeof(buffer));
    buffer[sizeof(buffer) - 1] = '\0';

    char *token = strtok(buffer, ";");
    if (!token)
        return 0;
    s->id = (unsigned)strtoul(token, NULL, 10);

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->title, token, sizeof(s->title));
    s->title[sizeof(s->title) - 1] = '\0';

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->author, token, sizeof(s->author));
    s->author[sizeof(s->author) - 1] = '\0';

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->isbn, token, sizeof(s->isbn));
    s->isbn[sizeof(s->isbn) - 1] = '\0';

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->edition, token, sizeof(s->edition));
    s->edition[sizeof(s->edition) - 1] = '\0';

    return 1;
}

void suggestion_to_csv(const Suggestion *s, char *out, size_t out_size)
{
    if (!s || !out || out_size == 0)
        return;

    /* Write CSV: id;title;author;isbn;edition */
    snprintf(out, out_size, "%u;%s;%s;%s;%s",
             s->id,
             s->title,
             s->author,
             s->isbn,
             s->edition);
}
