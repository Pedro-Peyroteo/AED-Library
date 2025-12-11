#pragma once

#include "db/db.h"

/* Operacoes de negocio simples relacionadas com emprestimos. */

void loan_list_all(const DB *db);
void loan_create(DB *db);
void loan_return_book(DB *db);
void loan_search(DB *db);
void loan_list_active(const DB *db);
void loan_list_history(const DB *db);
