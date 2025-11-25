#include <stdio.h>
#include <stdlib.h>

#include "db/db.h"
#include "model/books.h"
#include "model/user.h"
#include "model/loans.h"
#include "lib/dlist/dlist.h"

/*
    Exemplo independente que demonstra como usar a camada DB.
    Mostra:
      - CRUD de utilizadores (User)
      - CRUD de livros (Book)
      - CRUD de emprestimos (Loan)
      - Relações entre emprestimos, utilizadores e livros

    NÃO usa a camada app/menus.
    Apenas escreve no ecrã e usa uma DB em memória,
    que por sua vez é persistida em ficheiros de texto
    através da camada fs (data/ *.txt).

    A ideia é que um aluno de 2.º ano consiga ler este
    ficheiro de cima para baixo e perceber a sintaxe e
    a lógica de cada operação sem complexidade extra.
*/

static void print_users(const DB *db)
{
    /* Obter a lista interna de utilizadores da DB. */
    printf("\nUSERS:\n");
    DList *users = db_get_users(db);
    DLIST_FOREACH(users, node)
    {
        /* node->data é um void*, fazemos cast para User* */
        User *u = (User *)node->data;
        printf("  User{id=%u, name='%s', email='%s'}\n",
               u->id, u->name, u->email);
    }
}

static void print_books(const DB *db)
{
    /* Mesmo padrão dos utilizadores, mas para livros. */
    printf("\nBOOKS:\n");
    DList *books = db_get_books(db);
    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        printf("  Book{id=%u, title='%s', author='%s', year=%d, available=%d}\n",
               b->id, b->title, b->author, b->year, b->available);
    }
}

static void print_loans_with_relations(const DB *db)
{
    printf("\nLOANS (with relations):\n");
    DList *loans = db_get_loans(db);
    /*
        Para cada emprestimo procuramos também o
        utilizador e o livro associados (via id).
        Isto mostra como a DB liga os três modelos.
    */
    DLIST_FOREACH(loans, node)
    {
        Loan *l = (Loan *)node->data;

        /* Resolver as relações por id usando os helpers da DB. */
        User *u = db_find_user_by_id(db, l->user_id);
        Book *b = db_find_book_by_id(db, l->book_id);

        printf("  Loan{id=%u, user_id=%u, book_id=%u, borrow=%u, return=%u}\n",
               l->id, l->user_id, l->book_id, l->date_borrow, l->date_return);

        printf("    -> User:  %s\n", u ? u->name : "(not found)");
        printf("    -> Book:  %s\n", b ? b->title : "(not found)");
    }
}

int main(void)
{
    const char *books_path = "data/books.txt";
    const char *users_path = "data/users.txt";
    const char *loans_path = "data/loans.txt";

    DB db; /* in-memory database instance */
    /*
        Carregar dados dos ficheiros de texto para memória.
        Se os ficheiros não existirem, obtemos simplesmente
        listas vazias (a camada fs trata disso).
    */
    if (db_init(&db, books_path, users_path, loans_path) != 0)
    {
        printf("db_init failed (starting with empty DB).\n");
        /* we still continue; lists may be empty */
    }

    printf("Initial state loaded from files.\n");
    print_users(&db);
    print_books(&db);
    print_loans_with_relations(&db);

    /* --- USER CRUD EXAMPLE --- */
    printf("\n=== User CRUD example ===\n");

    User new_user;
    /*
        user_init preenche os campos da struct.
        Depois db_add_user COPIA esta struct para um
        User* alocado no heap e possuído pela DB.
    */
    user_init(&new_user, 1000, "Alice", "alice@example.com");
    db_add_user(&db, &new_user);

    printf("After adding user 1000:\n");
    print_users(&db);

    db_remove_user(&db, 1000);
    printf("After removing user 1000:\n");
    print_users(&db);

    /* --- BOOK CRUD EXAMPLE --- */
    printf("\n=== Book CRUD example ===\n");

    Book new_book;
    /* Mesma ideia que em User: inicializar na stack, copiar para a DB. */
    book_init(&new_book, 2000, "Example Book", "Example Author", 2025, 1);
    db_add_book(&db, &new_book);

    printf("After adding book 2000:\n");
    print_books(&db);

    db_remove_book(&db, 2000);
    printf("After removing book 2000:\n");
    print_books(&db);

    /* --- LOAN + RELATIONS EXAMPLE --- */
    printf("\n=== Loan + Relations example ===\n");

    /* For a real example we assume there is at least one user and one book. */
    /*
        Para criar um emprestimo precisamos de um utilizador e
        um livro existentes. Neste exemplo escolhemos apenas o
        primeiro de cada lista.
    */
    DList *users = db_get_users(&db);
    DList *books = db_get_books(&db);

    if (users && users->head && books && books->head)
    {
        User *u = (User *)users->head->data;
        Book *b = (Book *)books->head->data;

        Loan new_loan;
        /*
            Aqui 20250101 é uma data de empréstimo de exemplo
            no formato "YYYYMMDD" e 0 significa "ainda não devolvido".
        */
        loan_init(&new_loan, 3000, u->id, b->id, 20250101, 0); /* open loan */
        db_add_loan(&db, &new_loan);

        printf("After adding loan 3000 (user %u, book %u):\n", u->id, b->id);
        print_loans_with_relations(&db);

        db_remove_loan(&db, 3000);
        printf("After removing loan 3000:\n");
        print_loans_with_relations(&db);
    }
    else
    {
        printf("Not enough users/books to demonstrate loans. Add some in data/*.txt.\n");
    }

    /* Save any changes and clean up. */
    if (db_save(&db, books_path, users_path, loans_path) != 0)
        printf("Warning: db_save failed.\n");

    db_destroy(&db);

    printf("Example finished.\n");
    return 0;
}
