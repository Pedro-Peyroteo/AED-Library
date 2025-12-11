#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "app/loan_controller.h"
#include "db/db.h"
#include "model/loans.h"
#include "model/user.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

static void clear_input_buffer(void);
static unsigned get_next_loan_id(const DB *db);
static Loan *loan_find_by_id(const DB *db, unsigned id);
static int has_active_loan(const DB *db, unsigned user_id, unsigned book_id);

static void clear_input_buffer(void)
{
	int ch;
	while ((ch = getchar()) != '\n' && ch != EOF)
	{
		/* discard */
	}
}

static unsigned get_next_loan_id(const DB *db)
{
	unsigned max_id = 0;
	DList *loans = db ? db_get_loans(db) : NULL;

	if (!loans || !loans->head)
		return 1;

	DLIST_FOREACH(loans, node)
	{
		Loan *l = (Loan *)node->data;
		if (l->id > max_id)
			max_id = l->id;
	}

	return max_id + 1;
}

static Loan *loan_find_by_id(const DB *db, unsigned id)
{
	if (!db)
		return NULL;

	return db_find_loan_by_id(db, id);
}

static int has_active_loan(const DB *db, unsigned user_id, unsigned book_id)
{
	if (!db)
		return 0;

	DList *loans = db_get_loans(db);
	if (!loans)
		return 0;

	DLIST_FOREACH(loans, node)
	{
		Loan *l = (Loan *)node->data;
		if (l->user_id == user_id && l->book_id == book_id && l->date_return == 0)
			return 1;
	}

	return 0;
}

void loan_list_all(const DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	DList *loans = db_get_loans(db);
	if (!loans || !loans->head)
	{
		printf("[loan] Nao existem emprestimos registados.\n");
		return;
	}

	printf("[loan] Lista de emprestimos:\n");
	DLIST_FOREACH(loans, node)
	{
		Loan *l = (Loan *)node->data;
		User *u = db_find_user_by_id(db, l->user_id);
		Book *b = db_find_book_by_id(db, l->book_id);

		printf("  id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u\n",
			   l->id, l->user_id, l->book_id, l->date_borrow, l->date_return);
		printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
		printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
	}
}

void loan_create(DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	unsigned user_id, book_id, date_borrow;

	printf("\n--- REGISTAR EMPRESTIMO ---\n");

	printf("ID do utilizador: ");
	if (scanf("%u", &user_id) != 1)
	{
		printf("ID invalido.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	/* Verificar se utilizador existe */
	User *user = db_find_user_by_id(db, user_id);
	if (!user)
	{
		printf("[loan] Utilizador com ID %u nao encontrado.\n", user_id);
		return;
	}

	printf("ID do livro: ");
	if (scanf("%u", &book_id) != 1)
	{
		printf("ID invalido.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	/* Verificar se livro existe */
	Book *book = db_find_book_by_id(db, book_id);
	if (!book)
	{
		printf("[loan] Livro com ID %u nao encontrado.\n", book_id);
		return;
	}

	/* Verificar se livro esta disponivel (tem copias disponiveis) */
	if (book->available <= 0)
	{
		printf("[loan] Livro '%s' nao esta disponivel para emprestimo.\n", book->title);
		return;
	}

	/* Verificar se ja existe emprestimo ativo deste livro para este utilizador */
	if (has_active_loan(db, user_id, book_id))
	{
		printf("[loan] Ja existe um emprestimo ativo deste livro para este utilizador.\n");
		return;
	}

	printf("Data do emprestimo (YYYYMMDD): ");
	if (scanf("%u", &date_borrow) != 1)
	{
		printf("Data invalida.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	/* Criar o emprestimo */
	unsigned new_id = get_next_loan_id(db);
	Loan temp_loan;
	loan_init(&temp_loan, new_id, user_id, book_id, date_borrow, 0);

	if (db_add_loan(db, &temp_loan) == 0)
	{
		/* Decrementar numero de copias disponiveis */
		book->available--;
		printf("[loan] Emprestimo registado (ID=%u).\n", new_id);
		printf("  -> Utilizador: %s\n", user->name);
		printf("  -> Livro: %s\n", book->title);
	}
	else
	{
		printf("[loan] Erro ao registar emprestimo.\n");
	}
}

void loan_return_book(DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	unsigned loan_id, date_return;

	printf("\n--- DEVOLVER LIVRO ---\n");
	printf("ID do emprestimo: ");
	if (scanf("%u", &loan_id) != 1)
	{
		printf("ID invalido.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	Loan *loan = loan_find_by_id(db, loan_id);
	if (!loan)
	{
		printf("[loan] Emprestimo com ID %u nao encontrado.\n", loan_id);
		return;
	}

	/* Verificar se emprestimo ja foi devolvido */
	if (loan->date_return != 0)
	{
		printf("[loan] Este emprestimo ja foi devolvido em %u.\n", loan->date_return);
		return;
	}

	printf("Data de devolucao (YYYYMMDD): ");
	if (scanf("%u", &date_return) != 1)
	{
		printf("Data invalida.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	/* Atualizar emprestimo */
	loan->date_return = date_return;

	/* Incrementar numero de copias disponiveis */
	Book *book = db_find_book_by_id(db, loan->book_id);
	if (book)
	{
		book->available++;
		printf("[loan] Livro devolvido com sucesso.\n");
		printf("  -> Livro: %s\n", book->title);
	}
	else
	{
		printf("[loan] Emprestimo atualizado, mas livro nao encontrado.\n");
	}
}

void loan_search(DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	DList *loans = db_get_loans(db);
	if (!loans || !loans->head)
	{
		printf("[loan] Nao existem emprestimos para pesquisar.\n");
		return;
	}

	int opcao;
	printf("\n--- PESQUISAR EMPRESTIMOS ---\n");
	printf("1. ID do emprestimo\n");
	printf("2. ID do utilizador\n");
	printf("3. ID do livro\n");
	printf("Escolha uma opcao: ");

	if (scanf("%d", &opcao) != 1)
	{
		printf("Opcao invalida.\n");
		clear_input_buffer();
		return;
	}
	clear_input_buffer();

	unsigned search_id = 0;

	switch (opcao)
	{
	case 1:
		printf("ID do emprestimo: ");
		if (scanf("%u", &search_id) != 1)
		{
			printf("ID invalido.\n");
			clear_input_buffer();
			return;
		}
		clear_input_buffer();

		{
			Loan *found = loan_find_by_id(db, search_id);
			if (found)
			{
				User *u = db_find_user_by_id(db, found->user_id);
				Book *b = db_find_book_by_id(db, found->book_id);
				printf("  id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u\n",
					   found->id, found->user_id, found->book_id,
					   found->date_borrow, found->date_return);
				printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
				printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
			}
			else
			{
				printf("[loan] Emprestimo com ID %u nao encontrado.\n", search_id);
			}
		}
		break;

	case 2:
		printf("ID do utilizador: ");
		if (scanf("%u", &search_id) != 1)
		{
			printf("ID invalido.\n");
			clear_input_buffer();
			return;
		}
		clear_input_buffer();

		printf("[loan] Emprestimos do utilizador %u:\n", search_id);
		{
			int count = 0;
			DLIST_FOREACH(loans, node)
			{
				Loan *l = (Loan *)node->data;
				if (l->user_id == search_id)
				{
					User *u = db_find_user_by_id(db, l->user_id);
					Book *b = db_find_book_by_id(db, l->book_id);
					printf("  id=%u, book_id=%u, borrow=%u, return=%u\n",
						   l->id, l->book_id, l->date_borrow, l->date_return);
					printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
					printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
					count++;
				}
			}
			if (count == 0)
				printf("  (nenhum resultado)\n");
		}
		break;

	case 3:
		printf("ID do livro: ");
		if (scanf("%u", &search_id) != 1)
		{
			printf("ID invalido.\n");
			clear_input_buffer();
			return;
		}
		clear_input_buffer();

		printf("[loan] Emprestimos do livro %u:\n", search_id);
		{
			int count = 0;
			DLIST_FOREACH(loans, node)
			{
				Loan *l = (Loan *)node->data;
				if (l->book_id == search_id)
				{
					User *u = db_find_user_by_id(db, l->user_id);
					Book *b = db_find_book_by_id(db, l->book_id);
					printf("  id=%u, user_id=%u, borrow=%u, return=%u\n",
						   l->id, l->user_id, l->date_borrow, l->date_return);
					printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
					printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
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

void loan_list_active(const DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	DList *loans = db_get_loans(db);
	if (!loans || !loans->head)
	{
		printf("[loan] Nao existem emprestimos registados.\n");
		return;
	}

	printf("[loan] Lista de emprestimos ativos:\n");
	int count = 0;
	DLIST_FOREACH(loans, node)
	{
		Loan *l = (Loan *)node->data;
		if (l->date_return == 0)
		{
			User *u = db_find_user_by_id(db, l->user_id);
			Book *b = db_find_book_by_id(db, l->book_id);

			printf("  id=%u, user_id=%u, book_id=%u, borrow=%u\n",
				   l->id, l->user_id, l->book_id, l->date_borrow);
			printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
			printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
			count++;
		}
	}

	if (count == 0)
		printf("  (nenhum emprestimo ativo)\n");
}

void loan_list_history(const DB *db)
{
	if (!db)
	{
		printf("[loan] DB invalida.\n");
		return;
	}

	DList *loans = db_get_loans(db);
	if (!loans || !loans->head)
	{
		printf("[loan] Nao existem emprestimos registados.\n");
		return;
	}

	printf("[loan] Historico de emprestimos devolvidos:\n");
	int count = 0;
	DLIST_FOREACH(loans, node)
	{
		Loan *l = (Loan *)node->data;
		if (l->date_return != 0)
		{
			User *u = db_find_user_by_id(db, l->user_id);
			Book *b = db_find_book_by_id(db, l->book_id);

			printf("  id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u\n",
				   l->id, l->user_id, l->book_id, l->date_borrow, l->date_return);
			printf("    -> user: %s\n", u ? u->name : "(nao encontrado)");
			printf("    -> book: %s\n", b ? b->title : "(nao encontrado)");
			count++;
		}
	}

	if (count == 0)
		printf("  (nenhum emprestimo no historico)\n");
}
