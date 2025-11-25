#include <stdio.h>

#include "app/loan_controller.h"
#include "db/db.h"
#include "model/loans.h"
#include "model/user.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

void loan_list_all(const DB *db)
{
    if (!db) {
        printf("[loan] DB invalida.\n");
        return;
    }

    DList *loans = db_get_loans(db);
    if (!loans || !loans->head) {
        printf("[loan] Nao existem emprestimos registados.\n");
        return;
    }

    printf("[loan] Lista de emprestimos (com relacoes):\n");
    DLIST_FOREACH(loans, node) {
        Loan *l = (Loan *)node->data;
        User *u = db_find_user_by_id(db, l->user_id);
        Book *b = db_find_book_by_id(db, l->book_id);

        printf("  id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u\n",
               l->id, l->user_id, l->book_id, l->date_borrow, l->date_return);
        printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
        printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
    }
}
