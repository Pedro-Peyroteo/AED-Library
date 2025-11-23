#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/user.h"
#include "lib/dlist/dlist.h"

#include "fs/users_file.h"

#define USER_LINE_MAX 512

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

DList *file_load_users(const char *path)
{
    FILE *f = fopen(path, "r");

    if (!f)
    {
        DList *empty = dlist_create(false, NULL);
        return empty;
    }

    DList *list = dlist_create(false, NULL);
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
                dlist_push_back(list, first);
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

        dlist_push_back(list, u);
    }

    fclose(f);
    return list;
}

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
