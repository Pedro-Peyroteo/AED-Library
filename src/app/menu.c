
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
	void menuLivros(DB *db)
	{
		int opcao;

		do
		{
			printf("\n=== MENU LIVROS ===\n");
			printf("1. Inserir novo livro\n");
			printf("2. Editar livro\n");
			printf("3. Eliminar livro\n");
			printf("4. Pesquisar livros\n");
			printf("5. Duplicar livro\n");
			printf("6. Listar todos os livros\n");
			printf("0. Voltar ao menu principal\n");
			printf("Escolha uma opcao: ");

			if (scanf("%d", &opcao) != 1)
			{
				printf("Opcao invalida.\n");
				opcao = -1;
			}
			while (getchar() != '\n')
				;

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
				printf("A sair do menu de livros.\n");
				break;
			default:
				printf("Opcao invalida. Tente novamente.\n");
				break;
			}
		} while (opcao != 0);
	}
			break;
		}
	} while (opcao != 0);
>>>>>>> nadia-dev
}

void menuEmprestimos(DB *db)
{
	printf("\n=== MENU EMPRESTIMOS ===\n");
	loan_list_all(db);
}

void menuSugestoes(DB *db)
{
	(void)db;
	printf("\n=== MENU SUGESTOES ===\n");
	printf("[sugestoes] Funcionalidade em desenvolvimento.\n");
}
