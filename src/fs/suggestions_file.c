#include "fs/suggestions_file.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SUGGESTION_LINE_MAX 512

/* Reutilizamos o id como prioridade para ordenar sugestoes sem logica adicional. */
static int suggestion_priority(unsigned id)
{
    return (int)id;
}

static void trim_newline(char *s)
{
    if (!s)
        return;

    size_t len = strlen(s);
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
    {
        s[--len] = '\0';
    }
}

DList *file_load_suggestions(const char *path)
{
    FILE *f = fopen(path, "r");
    if (!f)
    {
        return dlist_create(true, NULL);
    }

    DList *list = dlist_create(true, NULL);
    if (!list)
    {
        fclose(f);
        return NULL;
    }

    char line[SUGGESTION_LINE_MAX];

    if (fgets(line, sizeof line, f))
    {
        trim_newline(line);
        if (strncmp(line, "id;", 3) != 0)
        {
            Suggestion *first = malloc(sizeof *first);
            if (first && suggestion_from_csv(first, line))
            {
                dlist_insert_priority(list, first, suggestion_priority(first->id));
            }
            else if (first)
            {
                free(first);
            }
        }
    }

    while (fgets(line, sizeof line, f))
    {
        trim_newline(line);
        if (line[0] == '\0')
            continue;

        Suggestion *s = malloc(sizeof *s);
        if (!s)
            break;

        if (!suggestion_from_csv(s, line))
        {
            free(s);
            continue;
        }

        dlist_insert_priority(list, s, suggestion_priority(s->id));
    }

    fclose(f);
    return list;
}

int file_save_suggestions(const char *path, const DList *suggestions)
{
    FILE *f = fopen(path, "w");
    if (!f)
        return -1;

    char buffer[SUGGESTION_LINE_MAX];
    fprintf(f, "id;title;author;isbn;edition\n");

    if (suggestions)
    {
        DLIST_FOREACH(suggestions, node)
        {
            Suggestion *s = (Suggestion *)node->data;
            suggestion_to_csv(s, buffer, sizeof buffer);
            fprintf(f, "%s\n", buffer);
        }
    }

    fclose(f);
    return 0;
}
