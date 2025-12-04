#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "app/user_controller.h"
#include "db/db.h"
#include "model/user.h"
#include "lib/dlist/dlist.h"

static void remove_newline(char *s);
static void clear_input_buffer(void);
static unsigned get_next_user_id(const DB *db);
static int contains_case_insensitive(const char *text, const char *needle);
static User *user_find_by_id(const DB *db, unsigned id);

static void remove_newline(char *s)
{
	if (!s)
		return;

	size_t len = strlen(s);
	while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
	{
		s[--len] = '\0';
	}
}

static void clear_input_buffer(void)
{
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF)
	{
		/* discard */
	}
}

static unsigned get_next_user_id(const DB *db)
{
	unsigned max_id = 0;
	DList *users = db ? db_get_users(db) : NULL;

	if (!users || !users->head)
		return 1;

	DLIST_FOREACH(users, node)
	{
		User *u = (User *)node->data;
		if (u->id > max_id)
			max_id = u->id;
	}

	return max_id + 1;
}

static int contains_case_insensitive(const char *text, const char *needle)
{
	if (!text || !needle)
		return 0;

	size_t needle_len = strlen(needle);
	if (needle_len == 0)
		return 1;

	for (const char *p = text; *p; ++p)
	{
		size_t matched = 0;
		while (p[matched] && needle[matched] &&
			   tolower((unsigned char)p[matched]) == tolower((unsigned char)needle[matched]))
		{
			++matched;
		}

		if (matched == needle_len)
			return 1;
	}

	return 0;
}

static User *user_find_by_id(const DB *db, unsigned id)
{
	if (!db)
		return NULL;

	return db_find_user_by_id(db, id);
}

void user_list_all(const DB *db)
{
	if (!db)
	{
		printf("[user] DB invalida.\n");
		return;
	}

	DList *users = db_get_users(db);
	if (!users || !users->head)
	{
		printf("[user] Nao existem utilizadores registados.\n");
		return;
	}

	printf("[user] Lista de utilizadores:\n");
	DLIST_FOREACH(users, node)
	{
		User *u = (User *)node->data;
		printf("  id=%u, nome=%s, email=%s\n", u->id, u->name, u->email);
	}
}

void user_insert(DB *db)
{
	if (!db)
	{
		printf("[user] DB invalida.\n");
		return;
	}

	char name[128];
	char email[128];

	printf("\n--- INSERIR NOVO UTILIZADOR ---\n");

	unsigned new_id = get_next_user_id(db);

	printf("Nome: ");
	if (!fgets(name, sizeof name, stdin))
		return;
	remove_newline(name);

	if (strlen(name) == 0)
	{
		printf("[user] Nome nao pode estar vazio.\n");
		return;
	}

	printf("Email: ");
	if (!fgets(email, sizeof email, stdin))
		return;
	remove_newline(email);

	if (strlen(email) == 0)
	{
		printf("[user] Email nao pode estar vazio.\n");
		return;
	}

	User temp_user;
	user_init(&temp_user, new_id, name, email);

	if (db_add_user(db, &temp_user) == 0)
		printf("[user] Utilizador '%s' (ID=%u) inserido com sucesso.\n", temp_user.name, new_id);
	else
		printf("[user] Erro ao adicionar utilizador.\n");
}

void user_edit(DB *db)
{
	if (!db)
	{
		printf("[user] DB invalida.\n");
		return;
	}

	unsigned id;
	printf("\n--- EDITAR UTILIZADOR ---\n");
	printf("Insira o ID do utilizador: ");
	if (scanf("%u", &id) != 1)
	{
		printf("ID invalido.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	User *user_to_edit = user_find_by_id(db, id);
	if (!user_to_edit)
	{
		printf("[user] Utilizador com ID %u nao encontrado.\n", id);
		return;
	}

	printf("Utilizador atual: id=%u, nome=%s, email=%s\n",
		   user_to_edit->id, user_to_edit->name, user_to_edit->email);

	char new_name[128];
	char new_email[128];

	printf("Novo nome (enter para manter): ");
	if (fgets(new_name, sizeof new_name, stdin))
	{
		remove_newline(new_name);
		if (strlen(new_name) > 0)
		{
			strncpy(user_to_edit->name, new_name, sizeof(user_to_edit->name) - 1);
			user_to_edit->name[sizeof(user_to_edit->name) - 1] = '\0';
		}
	}

	printf("Novo email (enter para manter): ");
	if (fgets(new_email, sizeof new_email, stdin))
	{
		remove_newline(new_email);
		if (strlen(new_email) > 0)
		{
			strncpy(user_to_edit->email, new_email, sizeof(user_to_edit->email) - 1);
			user_to_edit->email[sizeof(user_to_edit->email) - 1] = '\0';
		}
	}

	printf("[user] Utilizador com ID %u atualizado.\n", id);
}

void user_delete(DB *db)
{
	if (!db)
	{
		printf("[user] DB invalida.\n");
		return;
	}

	unsigned id;
	printf("\n--- ELIMINAR UTILIZADOR ---\n");
	printf("Insira o ID do utilizador: ");
	if (scanf("%u", &id) != 1)
	{
		printf("ID invalido.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	User *existing = db_find_user_by_id(db, id);
	if (!existing)
	{
		printf("[user] Utilizador com ID %u nao encontrado.\n", id);
		return;
	}

	// Verificar se o utilizador tem emprestimos ativos
	DList *loans = db_get_loans(db);
	if (loans)
	{
		DLIST_FOREACH(loans, node)
		{
			Loan *l = (Loan *)node->data;
			if (l->user_id == id && l->date_return == 0)
			{
				printf("[user] Nao e possivel eliminar utilizador com emprestimos ativos.\n");
				return;
			}
		}
	}

	if (db_remove_user(db, id) == 0)
		printf("[user] Utilizador removido.\n");
	else
		printf("[user] Erro ao remover utilizador.\n");
}

void user_search(DB *db)
{
	if (!db)
	{
		printf("[user] DB invalida.\n");
		return;
	}

	DList *users = db_get_users(db);
	if (!users || !users->head)
	{
		printf("[user] Nao existem utilizadores para pesquisar.\n");
		return;
	}

	int opcao;
	printf("\n--- PESQUISAR UTILIZADORES ---\n");
	printf("1. ID (correspondencia exata)\n");
	printf("2. Nome (termo parcial)\n");
	printf("3. Email (termo parcial)\n");
	printf("Escolha uma opcao: ");

	if (scanf("%d", &opcao) != 1)
	{
		printf("Opcao invalida.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	char term[128] = {0};
	unsigned id_search = 0;

	switch (opcao)
	{
	case 1:
		printf("ID: ");
		if (scanf("%u", &id_search) != 1)
		{
			printf("ID invalido.\n");
			clear_input_buffer();
			return;
		}
		clear_input_buffer();

		{
			User *found = user_find_by_id(db, id_search);
			if (found)
				printf("  id=%u, nome=%s, email=%s\n", found->id, found->name, found->email);
			else
				printf("[user] Utilizador com ID %u nao encontrado.\n", id_search);
		}
		break;

	case 2:
		printf("Nome (termo de pesquisa): ");
		if (!fgets(term, sizeof term, stdin))
			return;
		remove_newline(term);

		printf("[user] Resultados de pesquisa por nome:\n");
		{
			int count = 0;
			DLIST_FOREACH(users, node)
			{
				User *u = (User *)node->data;
				if (contains_case_insensitive(u->name, term))
				{
					printf("  id=%u, nome=%s, email=%s\n", u->id, u->name, u->email);
					count++;
				}
			}
			if (count == 0)
				printf("  (nenhum resultado)\n");
		}
		break;

	case 3:
		printf("Email (termo de pesquisa): ");
		if (!fgets(term, sizeof term, stdin))
			return;
		remove_newline(term);

		printf("[user] Resultados de pesquisa por email:\n");
		{
			int count = 0;
			DLIST_FOREACH(users, node)
			{
				User *u = (User *)node->data;
				if (contains_case_insensitive(u->email, term))
				{
					printf("  id=%u, nome=%s, email=%s\n", u->id, u->name, u->email);
					count++;
				}
			}
			if (count == 0)
				printf("  (nenhum resultado)\n");
		}
		break;

	default:
		printf("Opcao invalida.\n");
		break;
	}
}
