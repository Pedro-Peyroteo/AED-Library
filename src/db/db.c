/*
	Implementation of the DB (in-memory database) layer.

	This sits between the filesystem (CSV files) and the app layer.
	It owns three DLists with heap-allocated elements:
	- books: Book*
	- users: User*
	- loans: Loan*

	The lifecycle is:
		1) db_init:    load all data from disk into memory.
		2) CRUD ops:   manipulate the in-memory lists during the program.
		3) db_save:    write the current state back to disk.
		4) db_destroy: free all memory.
*/

#include "db/db.h"

#include <stdlib.h>

#include "fs/books_file.h"
#include "fs/users_file.h"
#include "fs/loans_file.h"
#include "fs/suggestions_file.h"

/* Small wrappers so dlist_destroy can free the stored elements. */
static void free_book(void *p)  { free(p); }
static void free_user(void *p)  { free(p); }
static void free_loan(void *p)  { free(p); }
static void free_suggestion(void *p) { free(p); }

/*
	Loads all data from the filesystem layer into the DB.

	On success, the three lists (books/users/loans) are ready to use.
	On error (e.g. out of memory), any partially created lists
	are destroyed and the function returns -1.
*/
int db_init(DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path,
			const char *suggestions_path)
{
	if (!db)
		return -1;

	db->books = file_load_books(books_path);
	db->users = file_load_users(users_path);
	db->loans = file_load_loans(loans_path);
	db->suggestions = file_load_suggestions(suggestions_path);

	if (!db->books || !db->users || !db->loans || !db->suggestions)
	{
		/* Clean up anything that was allocated before signaling failure. */
		db_destroy(db);
		return -1;
	}

	return 0;
}

/*
	Persists the current in-memory state of the DB to disk.
	Each list is written using the filesystem helpers, overwriting
	the corresponding .txt file with a header + all records.
*/
int db_save(const DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path,
			const char *suggestions_path)
{
	if (!db)
		return -1;

	int ok = 0;

	if (file_save_books(books_path, db->books) != 0)
		ok = -1;
	if (file_save_users(users_path, db->users) != 0)
		ok = -1;
	if (file_save_loans(loans_path, db->loans) != 0)
		ok = -1;
	if (file_save_suggestions(suggestions_path, db->suggestions) != 0)
		ok = -1;

	return ok;
}

/*
	Releases all memory owned by the DB.
	Each list is destroyed and every stored Book/User/Loan is freed.
*/
void db_destroy(DB *db)
{
	if (!db)
		return;

	if (db->books)
		dlist_destroy(db->books, free_book);
	if (db->users)
		dlist_destroy(db->users, free_user);
	if (db->loans)
		dlist_destroy(db->loans, free_loan);
	if (db->suggestions)
		dlist_destroy(db->suggestions, free_suggestion);

	db->books = NULL;
	db->users = NULL;
	db->loans = NULL;
	db->suggestions = NULL;
}

/*
	Linear search helpers.
	These are small convenience functions used by the app layer
	to locate a specific element by its id.
*/

Book *db_find_book_by_id(const DB *db, unsigned id)
{
	if (!db || !db->books)
		return NULL;

	DLIST_FOREACH(db->books, node)
	{
		Book *b = (Book *)node->data;
		if (b->id == id)
			return b;
	}

	return NULL;
}

User *db_find_user_by_id(const DB *db, unsigned id)
{
	if (!db || !db->users)
		return NULL;

	DLIST_FOREACH(db->users, node)
	{
		User *u = (User *)node->data;
		if (u->id == id)
			return u;
	}

	return NULL;
}

Loan *db_find_loan_by_id(const DB *db, unsigned id)
{
	if (!db || !db->loans)
		return NULL;

	DLIST_FOREACH(db->loans, node)
	{
		Loan *l = (Loan *)node->data;
		if (l->id == id)
			return l;
	}

	return NULL;
}

Suggestion *db_find_suggestion_by_id(const DB *db, unsigned id)
{
	if (!db || !db->suggestions)
		return NULL;

	DLIST_FOREACH(db->suggestions, node)
	{
		Suggestion *s = (Suggestion *)node->data;
		if (s->id == id)
			return s;
	}

	return NULL;
}

/* Simple accessors so UI code can iterate over lists. */

DList *db_get_books(const DB *db)
{
	return db ? db->books : NULL;
}

DList *db_get_users(const DB *db)
{
	return db ? db->users : NULL;
}

DList *db_get_loans(const DB *db)
{
	return db ? db->loans : NULL;
}

DList *db_get_suggestions(const DB *db)
{
	return db ? db->suggestions : NULL;
}

/*
	CRUD - Create helpers.

	Each "add" function:
		- validates the DB and source pointer,
		- allocates a new element on the heap,
		- copies the contents of the provided struct,
		- appends it to the appropriate list.

	The caller is responsible for picking an id (e.g. next free id).
*/

int db_add_book(DB *db, const Book *src)
{
	if (!db || !db->books || !src)
		return -1;

	Book *b = malloc(sizeof *b);
	if (!b)
		return -1;

	*b = *src;

	dlist_push_back(db->books, b);
	return 0;
}

int db_add_user(DB *db, const User *src)
{
	if (!db || !db->users || !src)
		return -1;

	User *u = malloc(sizeof *u);
	if (!u)
		return -1;

	*u = *src;

	dlist_push_back(db->users, u);
	return 0;
}

int db_add_loan(DB *db, const Loan *src)
{
	if (!db || !db->loans || !src)
		return -1;

	Loan *l = malloc(sizeof *l);
	if (!l)
		return -1;

	*l = *src;

	dlist_push_back(db->loans, l);
	return 0;
}

int db_add_suggestion(DB *db, const Suggestion *src)
{
	if (!db || !db->suggestions || !src)
		return -1;

	Suggestion *s = malloc(sizeof *s);
	if (!s)
		return -1;

	*s = *src;
	dlist_push_back(db->suggestions, s);
	return 0;
}

/*
	Internal helper for the "delete" operations.

	Given:
		- a list of elements (Book pointers, User pointers or Loan pointers)
		- the id we want to remove
		- a small callback that extracts the id from a void* element

	it walks the list, finds the first matching element, and
	removes plus frees it.
*/
static int db_remove_from_list(DList *list, unsigned id,
							   unsigned (*get_id)(const void *))
{
	if (!list || !get_id)
		return -1;

	DListNode *node = list->head;
	while (node)
	{
		void *data = node->data;
		if (get_id(data) == id)
		{
			dlist_remove_node(list, node, free);
			return 0;
		}
		node = node->next;
	}

	return -1;
}

/* id accessors used by db_remove_from_list */

static unsigned get_book_id(const void *p)
{
	const Book *b = (const Book *)p;
	return b->id;
}

static unsigned get_user_id(const void *p)
{
	const User *u = (const User *)p;
	return u->id;
}

static unsigned get_loan_id(const void *p)
{
	const Loan *l = (const Loan *)p;
	return l->id;
}

static unsigned get_suggestion_id(const void *p)
{
	const Suggestion *s = (const Suggestion *)p;
	return s->id;
}

int db_remove_book(DB *db, unsigned id)
{
	if (!db || !db->books)
		return -1;

	return db_remove_from_list(db->books, id, get_book_id);
}

int db_remove_user(DB *db, unsigned id)
{
	if (!db || !db->users)
		return -1;

	return db_remove_from_list(db->users, id, get_user_id);
}

int db_remove_loan(DB *db, unsigned id)
{
	if (!db || !db->loans)
		return -1;

	return db_remove_from_list(db->loans, id, get_loan_id);
}

int db_remove_suggestion(DB *db, unsigned id)
{
	if (!db || !db->suggestions)
		return -1;

	return db_remove_from_list(db->suggestions, id, get_suggestion_id);
}

