/*
	Filesystem helpers for the Book model.

	This module is responsible only for converting between:
		- text files on disk (CSV format with a header), and
		- in-memory DLists of Book pointers.

	It does not know anything about the DB or the UI.
	The DB layer simply calls file_load_books/file_save_books to
	load/save all books at once.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "model/books.h"
#include "lib/dlist/dlist.h"

#include "fs/books_file.h"

/* Maximum length of a single CSV line when reading/writing books. */
#define BOOK_LINE_MAX 512

/*
	Utility: remove trailing '\n' or '\r' characters from a buffer
	read with fgets, so that CSV parsing works as expected.
*/
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
	Reads all books from a CSV text file into a DList.

	Format expected:
		- optional header line starting with "id;"
		- one line per book, encoded with book_to_csv / book_from_csv

	Return value:
		- on success: a valid DList* (may be empty)
		- if the file does not exist: an empty list is returned
		- on allocation failure: NULL

	Each element in the list is a heap-allocated Book* owned by the caller.
*/
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

/*
	Writes all books from the given list to a CSV text file.

	The function always overwrites the target file and writes:
		- a header line: "id;title;author;year;available"
		- one CSV line per Book in the list.

	Return value:
		- 0 on success
		- -1 if the file could not be opened for writing
*/
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

