#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/loans.h"
#include "lib/dlist/dlist.h"

#include "fs/loans_file.h"

#define LOAN_LINE_MAX 512

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

DList *file_load_loans(const char *path)
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

    char line[LOAN_LINE_MAX];

    /* Skip optional header line if present */
    if (fgets(line, sizeof line, f))
    {
        trim_newline(line);
        if (strncmp(line, "id;", 3) != 0)
        {
            Loan *first = malloc(sizeof *first);
            if (first && loan_from_csv(first, line) == 0)
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

        Loan *l = malloc(sizeof *l);
        if (!l)
            break;

        if (!loan_from_csv(l, line))
        {
            free(l);
            continue;
        }

        dlist_push_back(list, l);
    }

    fclose(f);
    return list;
}

int file_save_loans(const char *path, const DList *loans)
{
    FILE *f = fopen(path, "w");
    if (!f)
        return -1;

    char buffer[LOAN_LINE_MAX];

    /* Write header */
    fprintf(f, "id;user_id;book_id;date_borrow;date_return\n");

    DLIST_FOREACH(loans, node)
    {
        Loan *l = (Loan *)node->data;

        loan_to_csv(l, buffer, sizeof buffer);
        fprintf(f, "%s\n", buffer);
    }

    fclose(f);
    return 0;
}
