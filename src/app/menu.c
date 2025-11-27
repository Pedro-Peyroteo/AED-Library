
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

// Em src/app/menu.c

void menuLivros(DB *db)
{
	int opcao;

	do
	{
		printf("\n=== MENU LIVROS ===\n");
		printf("1. Inserir novo livro\n");	   // Chama book_insert(db)
		printf("2. Editar livro existente\n"); // Chama book_edit(db)
		printf("3. Eliminar livro\n");		   // Chama book_delete(db)
		printf("4. Pesquisar livros\n");	   // Chama book_search(db)
		printf("5. Duplicar livro\n");		   // Chama book_duplicate(db)
		printf("6. Listar todos os livros\n"); // Chama book_list_all(db)
		printf("0. Voltar ao menu principal\n");
		printf("Escolha uma opção: ");

		if (scanf("%d", &opcao) != 1)
		{
			printf("Opção inválida.\n");
			opcao = -1;
		}
		while (getchar() != '\n')
			; // Limpa buffer após scanf

		switch (opcao)
		{
		case 1:
			book_insert(db);
			break;
		case 2:
			book_edit(db);
			break;
		case 3:
			book_delete(db);
			break;
		case 4:
			book_search(db);
			break;
		case 5:
			book_duplicate(db);
			break;
		case 6:
			book_list_all(db);
			break;
		case 0:
			printf("A sair do Menu Livros.\n");
			break;
		default:
			printf("Opção inválida. Tente novamente.\n");
			break;
		}
	} while (opcao != 0);
}

void menuEmprestimos(DB *db)
{
	printf("\n=== MENU EMPRESTIMOS ===\n");
	loan_list_all(db);
}

void menuSugestoes(DB *db)
{
	int opcao;

	do
	{
		printf("\n=== MENU SUGESTOES ===\n");
		printf("1. Sugerir aquisição de um livro\n");
		printf("2. Listar todas as sugestões\n");
		printf("0. Voltar ao menu principal\n");
		printf("Escolha uma opção: ");

		if (scanf("%d", &opcao) != 1)
		{
			printf(" Opção inválida.\n");
			opcao = -1;
		}
		while (getchar() != '\n')
			; // Limpa o buffer de input

		switch (opcao)
		{
		case 1:
			suggestion_register(db);
			break;
		case 2:
			suggestion_list_all(db);
			break;
		case 0:
			printf("A sair do Menu Sugestões.\n");
			break;
		default:
			printf("Opção inválida. Tente novamente.\n");
			break;
		}

	} while (opcao != 0);
}