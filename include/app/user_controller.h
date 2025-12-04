#pragma once

#include "db/db.h"

/* Operacoes de negocio simples relacionadas com utilizadores. */

void user_list_all(const DB *db);
void user_insert(DB *db);
void user_edit(DB *db);
void user_delete(DB *db);
void user_search(DB *db);
