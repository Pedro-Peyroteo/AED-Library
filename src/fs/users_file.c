/*
    Filesystem helpers for the User model.

    Like books_file.c, this module only knows how to:
        - read users from a CSV text file into a DList
        - write users from a DList back to a CSV file

    The app and DB layers remain completely independent of
    the actual file format.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/user.h"
#include "lib/dlist/dlist.h"

#include "fs/users_file.h"

/* Maximum length of a single CSV line when reading/writing users. */
#define USER_LINE_MAX 512

/* Mantemos a mesma regra de prioridade por id para reutilizar a ordenacao automatica. */
static int user_priority(unsigned id)
{
    return (int)id;
}

/* Strip trailing newlines from a line read with fgets. */
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

/*
    Load all users from a CSV text file into a DList.

    Layout is analogous to the books file:
        - optional header line starting with "id;"
        - one user per following line encoded via user_to_csv.

    Return value semantics are the same as file_load_books.
*/
DList *file_load_users(const char *path)
{
    FILE *f = fopen(path, "r");

    if (!f)
    {
        DList *empty = dlist_create(true, NULL);
        return empty;
    }

    DList *list = dlist_create(true, NULL);
    if (!list)
    {
        fclose(f);
        return NULL;
    }

    char line[USER_LINE_MAX];

    /* Skip optional header line if present */
    if (fgets(line, sizeof line, f))
    {
        trim_newline(line);
        if (strncmp(line, "id;", 3) != 0)
        {
            User *first = malloc(sizeof *first);
            if (first && user_from_csv(first, line) == 0)
            {
                dlist_insert_priority(list, first, user_priority(first->id));
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

        User *u = malloc(sizeof *u);
        if (!u)
            break;

        if (!user_from_csv(u, line))
        {
            free(u);
            continue;
        }

        dlist_insert_priority(list, u, user_priority(u->id));
    }

    fclose(f);
    return list;
}

/*
    Write all users from the list to a CSV file.

    Overwrites the file and writes:
        - header: "id;name;email"
        - one line per User in the list.
*/
int file_save_users(const char *path, const DList *users)
{
    FILE *f = fopen(path, "w");
    if (!f)
        return -1;

    char buffer[USER_LINE_MAX];

    /* Write header */
    fprintf(f, "id;name;email\n");

    DLIST_FOREACH(users, node)
    {
        User *u = (User *)node->data;

        user_to_csv(u, buffer, sizeof buffer);
        fprintf(f, "%s\n", buffer);
    }

    fclose(f);
    return 0;
}
