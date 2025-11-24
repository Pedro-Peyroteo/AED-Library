
#include <stdio.h>

#include "app/menu.h"

/*
	Basic menu stubs.

	These functions demonstrate how the UI layer receives a DB*
	and can later call db_add_* / db_find_* / db_remove_* as needed.
	For now they only print a placeholder message so the
	application links and runs end-to-end.
*/

void menuClientes(DB *db)
{
	(void)db; /* unused for now */
	printf("[menuClientes] Menu de clientes ainda nao implementado.\n");
}

void menuLivros(DB *db)
{
	(void)db;
	printf("[menuLivros] Menu de livros ainda nao implementado.\n");
}

void menuEmprestimos(DB *db)
{
	(void)db;
	printf("[menuEmprestimos] Menu de emprestimos ainda nao implementado.\n");
}

void menuSugestoes(DB *db)
{
	(void)db;
	printf("[menuSugestoes] Menu de sugestoes ainda nao implementado.\n");
}

