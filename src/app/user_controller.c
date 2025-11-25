#include <stdio.h>

#include "app/user_controller.h"
#include "db/db.h"
#include "model/user.h"
#include "lib/dlist/dlist.h"

void user_list_all(const DB *db)
{
    if (!db) {
        printf("[user] DB invalida.\n");
        return;
    }

    DList *users = db_get_users(db);
    if (!users || !users->head) {
        printf("[user] Nao existem utilizadores registados.\n");
        return;
    }

    printf("[user] Lista de utilizadores:\n");
    DLIST_FOREACH(users, node) {
        User *u = (User *)node->data;
        printf("  id=%u, nome=%s, email=%s\n", u->id, u->name, u->email);
    }
}
