#include <stdio.h>
#include <errno.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <sys/stat.h>
#include <sys/types.h>
#endif

#include "app/app.h"
#include "app/menu.h"
#include "db/db.h"

/*
	Application layer implementation.

	This module owns the global DB instance and glues together
	the menus (UI) with the persistence and data logic.

	main.c only calls app_init/app_run/app_shutdown.
*/
static DB db; /* shared in-memory database for the whole app */

/* Pequeno helper cross-platform para criar 'data/' se ainda nao existir. */
static int ensure_data_directory(void)
{
	const char *path = "data";

#ifdef _WIN32
	if (_mkdir(path) == 0)
		return 0;
#else
	if (mkdir(path, 0777) == 0)
		return 0;
#endif

	if (errno == EEXIST)
		return 0;

	return -1;
}

void app_init(void)
{
	printf("App iniciada.\n");

	if (ensure_data_directory() != 0)
		printf("Aviso: nao foi possivel criar o diretorio 'data/'.\n");

	if (db_init(&db,
		   "data/books.txt",
		   "data/users.txt",
		   "data/loans.txt",
		   "data/suggestions.txt") != 0)
	{
		printf("Erro ao carregar dados da base de dados.\n");
		/*
			If needed, you could exit here. For now we
			just continue with empty lists if loading failed.
		*/
	}
}

void app_run(void)
{
	int option;

	while (1)
	{
		printf("Menu Principal\n");
		printf("1 - Clientes\n");
		printf("2 - Livros\n");
		printf("3 - Emprestimos\n");
		printf("4 - Sugestoes\n");
		printf("0 - Sair\n");
		printf("Escolha uma opcao: ");
		if (scanf("%d", &option) != 1)
		{
			/* input error, clear and continue */
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF) {}
			printf("Entrada inválida.\n");
			continue;
		}

		switch (option)
		{
			case 1: menuClientes(&db);    break;
			case 2: menuLivros(&db);      break;
			case 3: menuEmprestimos(&db); break;
			case 4: menuSugestoes(&db);   break;
			case 0:
				printf("A encerrar a aplicação...\n");
				return;
			default:
				printf("Opção inválida!\n");
		}
	}
}

void app_shutdown(void)
{
	if (db_save(&db,
		  "data/books.txt",
		  "data/users.txt",
		  "data/loans.txt",
		  "data/suggestions.txt") != 0)
	{
		printf("Aviso: erro ao guardar dados.\n");
	}

	db_destroy(&db);
	printf("App encerrada com sucesso.\n");
}