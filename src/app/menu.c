
#include <stdio.h>

#include "app/menu.h"
#include "app/user_controller.h"
#include "app/book_controller.h"
#include "app/loan_controller.h"

/*
	Menus simples que mostram o fluxo:
	menu -> controller (camada de aplicacao) -> DB.

	Cada menu aqui so chama uma funcao "listar" de exemplo
	para manter tudo muito simples para a equipa.
*/
void menuClientes(DB *db)
{
	printf("\n=== MENU CLIENTES ===\n");
	user_list_all(db);
}

void menuLivros(DB *db)
{
	printf("\n=== MENU LIVROS ===\n");
	book_list_all(db);
}

void menuEmprestimos(DB *db)
{
	printf("\n=== MENU EMPRESTIMOS ===\n");
	loan_list_all(db);
}

void menuSugestoes(DB *db)
{
	printf("\n=== MENU SUGESTOES ===\n");
	printf("[sugestoes] Funcionalidade de sugestoes ainda nao implementada.\n");
}
