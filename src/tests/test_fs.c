#include <stdio.h>
#include <stdlib.h>

#include "model/books.h"
#include "model/user.h"
#include "model/loans.h"

#include "fs/books_file.h"
#include "fs/users_file.h"
#include "fs/loans_file.h"

#include "lib/dlist/dlist.h"

static void free_book(void *p)  { free(p); }
static void free_user(void *p)  { free(p); }
static void free_loan(void *p)  { free(p); }

static void print_book(const Book *b)
{
    printf("Book{id=%u, title='%s', author='%s', year=%d, available=%d}\n",
           b->id, b->title, b->author, b->year, b->available);
}

static void print_user(const User *u)
{
    printf("User{id=%u, name='%s', email='%s'}\n",
           u->id, u->name, u->email);
}

static void print_loan(const Loan *l)
{
    printf("Loan{id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u}\n",
           l->id, l->user_id, l->book_id, l->date_borrow, l->date_return);
}

int main(void)
{
    const char *books_path = "data/books_test.txt";
    const char *users_path = "data/users_test.txt";
    const char *loans_path = "data/loans_test.txt";

    /* Read-only test: just load existing data from disk */
    DList *books = file_load_books(books_path);
    DList *users = file_load_users(users_path);
    DList *loans = file_load_loans(loans_path);

    printf("Loaded %lu books, %lu users, %lu loans from fs layer.\n",
           (unsigned long)dlist_size(books),
           (unsigned long)dlist_size(users),
           (unsigned long)dlist_size(loans));

    printf("\nBooks loaded:\n");
    DLIST_FOREACH(books, bnode) {
        print_book((const Book *)bnode->data);
    }

    printf("\nUsers loaded:\n");
    DLIST_FOREACH(users, unode) {
        print_user((const User *)unode->data);
    }

    printf("\nLoans loaded:\n");
    DLIST_FOREACH(loans, lnode) {
        print_loan((const Loan *)lnode->data);
    }

    dlist_destroy(books, free_book);
    dlist_destroy(users, free_user);
    dlist_destroy(loans, free_loan);

    printf("FS layer test OK.\n");
    return 0;
}
