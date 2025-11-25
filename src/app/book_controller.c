#include <stdio.h>

#include "app/book_controller.h"
#include "db/db.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

void book_list_all(const DB *db)
{
    if (!db) {
        printf("[book] DB invalida.\n");
        return;
    }

    DList *books = db_get_books(db);
    if (!books || !books->head) {
        printf("[book] Nao existem livros registados.\n");
        return;
    }

    printf("[book] Lista de livros:\n");
    DLIST_FOREACH(books, node) {
        Book *b = (Book *)node->data;
        printf("  id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
               b->id, b->title, b->author, b->year, b->available);
    }
}
