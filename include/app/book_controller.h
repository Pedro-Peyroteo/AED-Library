#pragma once

#include "db/db.h"

/* Operacoes de negocio simples relacionadas com livros. */

void book_list_all(const DB *db);

void book_insert(DB *db);
void book_edit(DB *db);
void book_delete(DB *db);
void book_search(DB *db);
void book_duplicate(DB *db);

unsigned db_get_next_book_id(const DB *db);
unsigned find_max_book_id(const DB *db);
