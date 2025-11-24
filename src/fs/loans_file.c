/*
    Filesystem helpers for the Loan model.

    Responsibilities are the same pattern as the other fs modules:
        - file_load_loans: CSV text file -> DList of Loan*
        - file_save_loans: DList of Loan* -> CSV text file

    Loans reference books and users only by their ids; this layer
    does not resolve those relations, it simply mirrors the fields
    to and from the CSV representation.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/loans.h"
#include "lib/dlist/dlist.h"

#include "fs/loans_file.h"

/* Maximum length of a single CSV line when reading/writing loans. */
#define LOAN_LINE_MAX 512

/* Remove trailing newline characters from input lines. */
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
    Load all loans from a CSV file into a DList.

    File format:
        - optional header starting with "id;"
        - one line per loan encoded using loan_to_csv / loan_from_csv.

    Return value semantics follow the same rules as file_load_books.
*/
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

/*
    Write all loans to a CSV text file, overwriting the file.

    Header written:
        "id;user_id;book_id;date_borrow;date_return"
*/
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
