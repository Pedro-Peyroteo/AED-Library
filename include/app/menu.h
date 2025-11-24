#pragma once

#include "db/db.h"

/* Basic menu functions that receive the shared DB pointer. */

void menuClientes(DB *db);
void menuLivros(DB *db);
void menuEmprestimos(DB *db);
void menuSugestoes(DB *db);
