#include <stdio.h>
#include <stdlib.h>

#include "db/db.h"
#include "model/books.h"
#include "model/user.h"
#include "model/loans.h"

/* Simple DB integration test.
   - loads existing data from data/x.txt through fs+db
   - does a few CRUD operations
   - saves back to disk
*/

static void print_db_summary(const DB *db)
{
    printf("DB summary: %lu books, %lu users, %lu loans\n",
           (unsigned long)dlist_size(db->books),
           (unsigned long)dlist_size(db->users),
           (unsigned long)dlist_size(db->loans));
}

int main(void)
{
    const char *books_path = "data/books_test.txt";
    const char *users_path = "data/users_test.txt";
    const char *loans_path = "data/loans_test.txt";
    const char *suggestions_path = "data/suggestions_test.txt";

    DB db;
    if (db_init(&db, books_path, users_path, loans_path, suggestions_path) != 0)
    {
        printf("db_init failed\n");
        return 1;
    }

    printf("Loaded initial data from fs via DB layer.\n");
    print_db_summary(&db);

    /* Example: add a new book */
    Book new_book;
    book_init(&new_book, 9999, "Test Book", "Test Author", 2025, 1);
    if (db_add_book(&db, &new_book) == 0)
        printf("Added book with id=%u via db_add_book.\n", new_book.id);

    /* Example: look it up */
    Book *found = db_find_book_by_id(&db, 9999);
    if (found)
        printf("Found book by id=9999: title='%s'\n", found->title);

    /* Example: remove it again */
    if (db_remove_book(&db, 9999) == 0)
        printf("Removed book with id=9999 via db_remove_book.\n");

    print_db_summary(&db);

    /* Persist any changes back to disk */
    if (db_save(&db, books_path, users_path, loans_path, suggestions_path) != 0)
        printf("db_save reported an error.\n");
    else
        printf("db_save completed successfully.\n");

    db_destroy(&db);
    printf("DB integration test finished.\n");

    return 0;
}
