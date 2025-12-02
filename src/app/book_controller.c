#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "app/book_controller.h"
#include "db/db.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

static void remove_newline(char *s);
static void clear_input_buffer(void);
static unsigned get_next_book_id(const DB *db);
static int contains_case_insensitive(const char *text, const char *needle);
static Book *book_find_by_id(const DB *db, unsigned id);

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

static unsigned get_next_book_id(const DB *db)
{
    unsigned max_id = 0;
    DList *books = db ? db_get_books(db) : NULL;

    if (!books || !books->head)
        return 1;

    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        if (b->id > max_id)
            max_id = b->id;
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

static Book *book_find_by_id(const DB *db, unsigned id)
{
    if (!db)
        return NULL;

    return db_find_book_by_id(db, id);
}

void book_list_all(const DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    DList *books = db_get_books(db);
    if (!books || !books->head)
    {
        printf("[book] Nao existem livros registados.\n");
        return;
    }

    printf("[book] Lista de livros:\n");
    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        printf("  id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
               b->id, b->title, b->author, b->year, b->available);
    }
}

void book_insert(DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    char title[128];
    char author[128];
    int year;
    int stock;

    printf("\n--- INSERIR NOVO LIVRO ---\n");

    unsigned new_id = get_next_book_id(db);

    printf("Titulo: ");
    if (!fgets(title, sizeof title, stdin))
        return;
    remove_newline(title);

    printf("Autor(es): ");
    if (!fgets(author, sizeof author, stdin))
        return;
    remove_newline(author);

    printf("Ano de publicacao: ");
    if (scanf("%d", &year) != 1)
    {
        printf("Ano invalido.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    printf("Numero de exemplares disponiveis: ");
    if (scanf("%d", &stock) != 1)
    {
        printf("Valor invalido.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    Book temp_book;
    book_init(&temp_book, new_id, title, author, year, stock);

    if (db_add_book(db, &temp_book) == 0)
        printf("[book] Livro '%s' (ID=%u) inserido com sucesso.\n", temp_book.title, new_id);
    else
        printf("[book] Erro ao adicionar livro.\n");
}

void book_edit(DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    unsigned id;
    printf("\n--- EDITAR LIVRO ---\n");
    printf("Insira o ID do livro: ");
    if (scanf("%u", &id) != 1)
    {
        printf("ID invalido.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    Book *book_to_edit = book_find_by_id(db, id);
    if (!book_to_edit)
    {
        printf("[book] Livro com ID %u nao encontrado.\n", id);
        return;
    }

    printf("Livro atual: id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
           book_to_edit->id, book_to_edit->title, book_to_edit->author,
           book_to_edit->year, book_to_edit->available);

    char new_title[128];
    char new_author[128];
    int new_year;
    int new_stock;

    printf("Novo titulo (enter para manter): ");
    if (fgets(new_title, sizeof new_title, stdin))
    {
        remove_newline(new_title);
        if (strlen(new_title) > 0)
        {
            strncpy(book_to_edit->title, new_title, sizeof(book_to_edit->title) - 1);
            book_to_edit->title[sizeof(book_to_edit->title) - 1] = '\0';
        }
    }

    printf("Novo autor (enter para manter): ");
    if (fgets(new_author, sizeof new_author, stdin))
    {
        remove_newline(new_author);
        if (strlen(new_author) > 0)
        {
            strncpy(book_to_edit->author, new_author, sizeof(book_to_edit->author) - 1);
            book_to_edit->author[sizeof(book_to_edit->author) - 1] = '\0';
        }
    }

    printf("Novo ano (atual %d): ", book_to_edit->year);
    if (scanf("%d", &new_year) == 1)
        book_to_edit->year = new_year;
    clear_input_buffer();

    printf("Novo stock (atual %d): ", book_to_edit->available);
    if (scanf("%d", &new_stock) == 1)
        book_to_edit->available = new_stock;
    clear_input_buffer();

    printf("[book] Livro com ID %u atualizado.\n", id);
}

void book_delete(DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    unsigned id;
    printf("\n--- ELIMINAR LIVRO ---\n");
    printf("Insira o ID do livro: ");
    if (scanf("%u", &id) != 1)
    {
        printf("ID invalido.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    Book *existing = db_find_book_by_id(db, id);
    if (!existing)
    {
        printf("[book] Livro com ID %u nao encontrado.\n", id);
        return;
    }

    if (db_remove_book(db, id) == 0)
        printf("[book] Livro removido.\n");
    else
        printf("[book] Erro ao remover livro.\n");
}

void book_search(DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    DList *books = db_get_books(db);
    if (!books || !books->head)
    {
        printf("[book] Nao existem livros para pesquisar.\n");
        return;
    }

    int opcao;
    printf("\n--- PESQUISAR LIVROS ---\n");
    printf("1. ID (correspondencia exata)\n");
    printf("2. Titulo (termo parcial)\n");
    printf("3. Autor (termo parcial)\n");
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

    if (opcao == 1)
    {
        printf("Insira o ID: ");
        if (scanf("%u", &id_search) != 1)
        {
            printf("ID invalido.\n");
            clear_input_buffer();
            return;
        }
        clear_input_buffer();
    }
    else if (opcao == 2 || opcao == 3)
    {
        const char *label = (opcao == 2) ? "titulo" : "autor";
        printf("Termo de pesquisa (%s): ", label);
        if (!fgets(term, sizeof term, stdin))
            return;
        remove_newline(term);
        if (term[0] == '\0')
        {
            printf("O termo nao pode ser vazio.\n");
            return;
        }
    }
    else
    {
        printf("Opcao invalida.\n");
        return;
    }

    int count = 0;
    printf("\n[book] Resultados:\n");

    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        int match = 0;

        if (opcao == 1)
            match = (b->id == id_search);
        else if (opcao == 2)
            match = contains_case_insensitive(b->title, term);
        else if (opcao == 3)
            match = contains_case_insensitive(b->author, term);

        if (match)
        {
            printf("  id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
                   b->id, b->title, b->author, b->year, b->available);
            ++count;

            if (opcao == 1)
                break;
        }
    }

    if (count == 0)
        printf("  Nenhum livro encontrado.\n");
    else
        printf("  %d livro(s) encontrado(s).\n", count);
}

void book_duplicate(DB *db)
{
    if (!db)
    {
        printf("[book] DB invalida.\n");
        return;
    }

    unsigned id_original;
    printf("\n--- DUPLICAR LIVRO ---\n");
    printf("Insira o ID original: ");
    if (scanf("%u", &id_original) != 1)
    {
        printf("ID invalido.\n");
        clear_input_buffer();
        return;
    }
    clear_input_buffer();

    Book *original = book_find_by_id(db, id_original);
    if (!original)
    {
        printf("[book] Livro com ID %u nao encontrado.\n", id_original);
        return;
    }

    unsigned new_id = get_next_book_id(db);

    Book duplicate;
    book_init(&duplicate,
              new_id,
              original->title,
              original->author,
              original->year,
              original->available);

    if (db_add_book(db, &duplicate) == 0)
        printf("[book] Livro duplicado com sucesso. Novo ID=%u.\n", new_id);
    else
        printf("[book] Erro ao duplicar livro.\n");
}
