#include "db/db.h"

#include <stdlib.h>

#include "fs/books_file.h"
#include "fs/users_file.h"
#include "fs/loans_file.h"

static void free_book(void *p)  { free(p); }
static void free_user(void *p)  { free(p); }
static void free_loan(void *p)  { free(p); }

int db_init(DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path)
{
	if (!db)
		return -1;

	db->books = file_load_books(books_path);
	db->users = file_load_users(users_path);
	db->loans = file_load_loans(loans_path);

	if (!db->books || !db->users || !db->loans)
	{
		db_destroy(db);
		return -1;
	}

	return 0;
}

int db_save(const DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path)
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

	return ok;
}

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

	db->books = NULL;
	db->users = NULL;
	db->loans = NULL;
}

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

int db_add_book(DB *db, const Book *src)
{
	if (!db || !db->books || !src)
		return -1;

	Book *b = malloc(sizeof *b);
	if (!b)
		return -1;

	/*
		Simple struct copy. If you prefer a constructor-like
		approach, you can call book_init here instead.
	*/
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

