#include "model/books.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// Inicializa a estrutura Book.
void book_init(Book *b, unsigned id,
               const char *title,
               const char *author,
               int year,
               int available)
{
    b->id = id;
    strncpy(b->title, title, sizeof(b->title));
    strncpy(b->author, author, sizeof(b->author));
    b->year = year;
    b->available = available;
}

/* Lê uma linha CSV e preenche a estrutura Book.
Formato: ID;Título;Autor;Ano;Available
se conseguir ler todos os campos, 0 se falhar.*/
int book_from_csv(Book *b, const char *line)
{
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp) - 1] = '\0';

    char *token = strtok(tmp, ";");
    if (!token)
        return 0;
    b->id = atoi(token); // 1. Lê o ID

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(b->title, token, sizeof(b->title)); // 2. Lê o Título

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(b->author, token, sizeof(b->author)); // 3. Lê o Autor

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    b->year = atoi(token); // 4. Lê o Ano

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    b->available = atoi(token); // 5. Lê o Available

    return 1;
}
/* Converte a estrutura Book numa string CSV.
   Formato: ID;Título;Autor;Ano;Available
 */

void book_to_csv(const Book *b, char *out, size_t out_size)
{
    snprintf(out, out_size, "%u;%s;%s;%d;%d",
             b->id,
             b->title,
             b->author,
             b->year,
             b->available);
}

// =========================================================
// FUNÇÕES PARA O MODELO SUGGESTION (A SEREM ADICIONADAS)
// =========================================================

// --- Constantes para Sugestão (Se não estiverem em books.h) ---
#define SUGG_TITLE_MAX 100
#define SUGG_AUTHOR_MAX 100

// --- Implementação da Estrutura Suggestion ---

typedef struct
{
    unsigned id;
    char title[SUGG_TITLE_MAX];
    char author[SUGG_AUTHOR_MAX];
} Suggestion;

// Inicializa a estrutura Suggestion.
void suggestion_init(Suggestion *s, unsigned id,
                     const char *title, const char *author)
{
    s->id = id;
    strncpy(s->title, title, sizeof(s->title));
    s->title[sizeof(s->title) - 1] = '\0';
    strncpy(s->author, author, sizeof(s->author));
    s->author[sizeof(s->author) - 1] = '\0';
}

/**
 Lê uma linha CSV e preenche a estrutura Suggestion.
 Formato: ID;Título;Autor
 1 se conseguir ler todos os campos, 0 se falhar.
 */
int suggestion_from_csv(Suggestion *s, const char *line)
{
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp) - 1] = '\0';

    char *token = strtok(tmp, ";");
    if (!token)
        return 0;
    s->id = atoi(token); // 1. Lê o ID

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->title, token, sizeof(s->title)); // 2. Lê o Título
    s->title[sizeof(s->title) - 1] = '\0';

    token = strtok(NULL, ";");
    if (!token)
        return 0;
    strncpy(s->author, token, sizeof(s->author)); // 3. Lê o Autor
    s->author[sizeof(s->author) - 1] = '\0';

    return 1;
}

/* Converte a estrutura Suggestion numa string CSV.
  Formato: ID;Título;Autor
 */
void suggestion_to_csv(const Suggestion *s, char *out, size_t out_size)
{
    snprintf(out, out_size, "%u;%s;%s",
             s->id,
             s->title,
             s->author);
}
