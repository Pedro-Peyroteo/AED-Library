#ifndef DB_H
#define DB_H

#include "lib/dlist/dlist.h"
#include "model/books.h"
#include "model/user.h"
#include "model/loans.h"

/*
	Central in-memory database for the library.
	Owns three lists:
	- books: DList of Book*
	- users: DList of User*
	- loans: DList of Loan*

	The DB layer:
	- Loads all data from the filesystem layer on startup.
	- Provides a single place for the app layer to query/update data.
	- Saves everything back to disk on shutdown.
*/
typedef struct DB {
	DList *books;
	DList *users;
	DList *loans;
} DB;

/*
	Initializes a DB instance by loading data from the filesystem.

	books_path/users_path/loans_path:
		Paths to the .txt files (CSV with header) managed by the fs layer.

	Returns:
		0 on success
	   -1 on error (e.g. out of memory). In that case, all lists are set to NULL.
*/
int db_init(DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path);

/*
	Saves the current contents of the DB back to disk.

	Returns:
		0 on success (all saves succeeded)
	   -1 if any save fails.
*/
int db_save(const DB *db,
			const char *books_path,
			const char *users_path,
			const char *loans_path);

/*
	Frees all memory owned by the DB.
	Destroys the three lists and all Book/User/Loan elements.
*/
void db_destroy(DB *db);

/*
	Example query helpers.
	Simple linear searches for now; can be optimized or expanded later.
*/
Book *db_find_book_by_id(const DB *db, unsigned id);
User *db_find_user_by_id(const DB *db, unsigned id);
Loan *db_find_loan_by_id(const DB *db, unsigned id);

/* Give read-only access to the internal lists (for iteration). */
DList *db_get_books(const DB *db);
DList *db_get_users(const DB *db);
DList *db_get_loans(const DB *db);

/*
	Basic CRUD helpers.

	The "add" functions:
		- allocate a new element on the heap,
		- copy the contents from the provided struct,
		- append it to the corresponding list.

	The caller is responsible for choosing a suitable 'id' value
	(e.g., by tracking the next id somewhere in the app layer).

	Return:
		0 on success
	   -1 on allocation failure or invalid DB pointer.
*/
int db_add_book(DB *db, const Book *src);
int db_add_user(DB *db, const User *src);
int db_add_loan(DB *db, const Loan *src);

/*
	Remove an element by id.

	These functions:
		- search the corresponding list,
		- free the matching element,
		- unlink its node from the list.

	Return:
		0 if an element was removed
	   -1 if no element with that id exists or DB is invalid.
*/
int db_remove_book(DB *db, unsigned id);
int db_remove_user(DB *db, unsigned id);
int db_remove_loan(DB *db, unsigned id);

#endif /* DB_H */

