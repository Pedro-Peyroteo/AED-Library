#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "app/suggestion_controller.h"
#include "model/suggestion.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

static void read_line(char *buffer, size_t size)
{
    if (!fgets(buffer, (int)size, stdin))
    {
        buffer[0] = '\0';
        return;
    }

    size_t len = strlen(buffer);
    while (len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
    {
        buffer[--len] = '\0';
    }
}

static unsigned next_suggestion_id(const DB *db)
{
    unsigned max_id = 0;
    DList *suggestions = db_get_suggestions(db);
    if (!suggestions)
        return 1;

    DLIST_FOREACH(suggestions, node)
    {
        Suggestion *s = (Suggestion *)node->data;
        if (s->id > max_id)
            max_id = s->id;
    }

    return max_id + 1;
}

static int strings_equal_ci(const char *a, const char *b)
{
    size_t i = 0;
    while (a[i] && b[i])
    {
        char ca = (char)tolower((unsigned char)a[i]);
        char cb = (char)tolower((unsigned char)b[i]);
        if (ca != cb)
            return 0;
        ++i;
    }
    return a[i] == b[i];
}

void suggestion_register(DB *db)
{
    if (!db)
    {
        printf("[sugestoes] DB invalida.\n");
        return;
    }

    char title[128];
    char author[128];
    char isbn[32];

    printf("\n--- REGISTAR SUGESTAO ---\n");

    printf("Titulo desejado: ");
    read_line(title, sizeof title);
    if (title[0] == '\0')
    {
        printf("Titulo nao pode ser vazio.\n");
        return;
    }

    printf("Autor: ");
    read_line(author, sizeof author);
    if (author[0] == '\0')
    {
        printf("Autor nao pode ser vazio.\n");
        return;
    }

    printf("ISBN (opcional, pressione Enter para saltar): ");
    read_line(isbn, sizeof isbn);

    DList *books = db_get_books(db);
    if (books)
    {
        DLIST_FOREACH(books, node)
        {
            Book *b = (Book *)node->data;
            if (strings_equal_ci(b->title, title) && strings_equal_ci(b->author, author))
            {
                printf("[sugestoes] Este livro ja existe na biblioteca.\n");
                return;
            }
        }
    }

    DList *suggestions = db_get_suggestions(db);
    if (suggestions)
    {
        DLIST_FOREACH(suggestions, node)
        {
            Suggestion *s = (Suggestion *)node->data;
            if (strings_equal_ci(s->title, title) && strings_equal_ci(s->author, author) && strings_equal_ci(s->isbn, isbn))
            {
                printf("[sugestoes] Ja existe uma sugestao identica registada.\n");
                return;
            }
        }
    }

    Suggestion new_suggestion;
    suggestion_init(&new_suggestion, next_suggestion_id(db), title, author, isbn);

    if (db_add_suggestion(db, &new_suggestion) == 0)
        printf("[sugestoes] Sugestao registada com sucesso (id=%u).\n", new_suggestion.id);
    else
        printf("[sugestoes] Erro ao guardar sugestao.\n");
}

void suggestion_list_all(const DB *db)
{
    if (!db)
    {
        printf("[sugestoes] DB invalida.\n");
        return;
    }

    DList *suggestions = db_get_suggestions(db);
    if (!suggestions || !suggestions->head)
    {
        printf("[sugestoes] Nao existem sugestoes registadas.\n");
        return;
    }

    printf("[sugestoes] Lista de sugestoes:\n");
    DLIST_FOREACH(suggestions, node)
    {
        Suggestion *s = (Suggestion *)node->data;
        printf("  id=%u, titulo=%s, autor=%s, isbn=%s\n",
               s->id, s->title, s->author, s->isbn[0] ? s->isbn : "(n/d)");
    }
}
