#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/books.h"
#include "lib/dlist/dlist.h"

#include "fs/books_file.h"

#define BOOK_LINE_MAX 512

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

DList *file_load_books(const char *path)
{
	FILE *f = fopen(path, "r");

	if (!f)
	{
		/* If the file does not exist, treat as empty list. */
		DList *empty = dlist_create(false, NULL);
		return empty;
	}

	DList *list = dlist_create(false, NULL);
	if (!list)
	{
		fclose(f);
		return NULL;
	}

	char line[BOOK_LINE_MAX];

	/* Skip optional header line if present */
	if (fgets(line, sizeof line, f))
	{
		trim_newline(line);
		if (strncmp(line, "id;", 3) != 0)
		{
			/* Not a header, process this line as data */
			/* fall through into normal loop by handling it manually */
			Book *first = malloc(sizeof *first);
			if (first && book_from_csv(first, line) == 0)
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
			continue; /* skip empty lines */

		Book *b = malloc(sizeof *b);
		if (!b)
			break;

		if (!book_from_csv(b, line))
		{
			/* malformed line, discard this book and continue */
			free(b);
			continue;
		}

		dlist_push_back(list, b);
	}

	fclose(f);
	return list;
}

int file_save_books(const char *path, const DList *books)
{
	FILE *f = fopen(path, "w");
	if (!f)
		return -1;

	char buffer[BOOK_LINE_MAX];

	/* Write header */
	fprintf(f, "id;title;author;year;available\n");

	DLIST_FOREACH(books, node)
	{
		Book *b = (Book *)node->data;

		book_to_csv(b, buffer, sizeof buffer);
		fprintf(f, "%s\n", buffer);
	}

	fclose(f);
	return 0;
}

