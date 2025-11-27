#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> // Para tolower, usado em stristr

#include "app/book_controller.h"
#include "db/db.h"
#include "model/books.h"
#include "lib/dlist/dlist.h"

// 0. FUNÇÕES AUXILIARES

// Remove o caractere de nova linha (\n) do final de uma string.
void remove_newline(char *s)
{
    size_t len = strlen(s);
    if (len > 0 && s[len - 1] == '\n')
    {
        s[len - 1] = '\0';
    }
}

unsigned find_max_book_id(const DB *db)
{
    unsigned max_id = 0;

    if (!db || !db->books || !db->books->head)
    {
        return 1; // Se vazia, o próximo ID será 1
    }

    DList *books = db_get_books(db); // Deve usar db_get_books para garantir acesso
    if (!books)
    {
        return 1;
    }

    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        if (b->id > max_id)
        {
            max_id = b->id;
        }
    }
    return max_id + 1; // Retorna o PRÓXIMO ID disponível
}

// Procura uma substring (needle) dentro de outra (haystack) de forma case-insensitive.
char *stristr(const char *haystack, const char *needle)
{
    if (!*needle)
        return (char *)haystack;

    char *lower_haystack = strdup(haystack);
    char *lower_needle = strdup(needle);

    if (!lower_haystack || !lower_needle)
    {
        free(lower_haystack);
        free(lower_needle);
        return NULL;
    }

    for (char *p = lower_haystack; *p; p++)
        *p = tolower((unsigned char)*p);
    for (char *p = lower_needle; *p; p++)
        *p = tolower((unsigned char)*p);

    char *result_in_lower = strstr(lower_haystack, lower_needle);

    char *result = NULL;
    if (result_in_lower)
    {
        result = (char *)haystack + (result_in_lower - lower_haystack);
    }

    free(lower_haystack);
    free(lower_needle);

    return result;
}

// 1. PESQUISA E UTILIDADES

// Usa a função de pesquisa da camada DB (db_find_book_by_id).
Book *book_find_by_id(const DB *db, unsigned id)
{
    return db_find_book_by_id(db, id);
}

// Função para listar todos os livros (Case 6)
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
        printf("  id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
               b->id, b->title, b->author, b->year, b->available);
    }
}

// 2. CRUD - CREATE (Case 1)

void book_insert(DB *db)
{
    char title[100], author[100];
    int year, stock;

    if (!db)
    {
        printf("[book] DB inválida.\n");
        return;
    }

    printf("\n--- INSERIR NOVO LIVRO ---\n");

    // 1. Obter um ID único usando a função de cálculo MAX ID + 1
    unsigned new_id = find_max_book_id(db); // <--- CORRIGIDO

    // 2. Pedir dados ao utilizador
    printf("Título: ");
    if (!fgets(title, sizeof(title), stdin))
        return;
    remove_newline(title);

    printf("Autor(es): ");
    if (!fgets(author, sizeof(author), stdin))
        return;
    remove_newline(author);

    printf("Ano de publicação: ");
    if (scanf("%d", &year) != 1)
    {
        printf("Ano inválido.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    printf("Número de exemplares em stock: ");
    if (scanf("%d", &stock) != 1)
    {
        printf("Stock inválido.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    // 3. Preparar e adicionar à DB
    Book temp_book;
    book_init(&temp_book, new_id, title, author, year, stock);

    // Usamos db_add_book que aloca a memória e insere
    if (db_add_book(db, &temp_book) == 0)
    {
        // REMOVIDO: find_max_book_id(db); // Era redundante
        // REMOVIDO: db_increment_book_id(db); // Função que não existe
        printf("[book] Livro '%s' (ID: %u) inserido com sucesso.\n", temp_book.title, new_id);
    }
    else
    {
        printf("[ERRO] Falha ao adicionar livro na base de dados (memória?).\n");
    }
}

// 3. CRUD - UPDATE (Case 2)
// ... (código inalterado) ...
void book_edit(DB *db)
{
    unsigned id;
    char new_title[100], new_author[100];
    int new_year, new_stock;

    if (!db)
    {
        printf("[book] DB inválida.\n");
        return;
    }

    printf("\n--- EDITAR LIVRO ---\n");
    printf("Insira o ID do livro a editar: ");
    if (scanf("%u", &id) != 1)
    {
        printf("ID inválido.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    Book *book_to_edit = book_find_by_id(db, id);

    if (!book_to_edit)
    {
        printf("[book] Livro com ID %u não encontrado.\n", id);
        return;
    }

    printf("Livro atual: id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
           book_to_edit->id, book_to_edit->title, book_to_edit->author, book_to_edit->year, book_to_edit->available);

    printf("\nNovos dados (Deixe em branco para manter):\n");

    printf("Novo Título (atual: %s): ", book_to_edit->title);
    if (!fgets(new_title, sizeof(new_title), stdin))
        return;
    remove_newline(new_title);
    if (strlen(new_title) > 0)
    {
        // Usa strncpy para segurança
        strncpy(book_to_edit->title, new_title, sizeof(book_to_edit->title) - 1);
        book_to_edit->title[sizeof(book_to_edit->title) - 1] = '\0';
    }

    printf("Novo Autor(es) (atual: %s): ", book_to_edit->author);
    if (!fgets(new_author, sizeof(new_author), stdin))
        return;
    remove_newline(new_author);
    if (strlen(new_author) > 0)
    {
        // Usa strncpy para segurança
        strncpy(book_to_edit->author, new_author, sizeof(book_to_edit->author) - 1);
        book_to_edit->author[sizeof(book_to_edit->author) - 1] = '\0';
    }

    printf("Novo Ano de publicação (atual: %d): ", book_to_edit->year);
    if (scanf("%d", &new_year) == 1)
    {
        book_to_edit->year = new_year;
    }
    while (getchar() != '\n')
        ;

    printf("Novo Stock (exemplares disponíveis) (atual: %d): ", book_to_edit->available);
    if (scanf("%d", &new_stock) == 1)
    {
        book_to_edit->available = new_stock;
    }
    while (getchar() != '\n')
        ;

    printf("[book] Livro com ID %u editado com sucesso.\n", id);
}

// 4. CRUD - DELETE (Case 3)
// ... (código inalterado) ...
void book_delete(DB *db)
{
    unsigned id;

    if (!db)
    {
        printf("[book] DB inválida.\n");
        return;
    }

    printf("\n--- ELIMINAR LIVRO ---\n");
    printf("Insira o ID do livro a eliminar: ");
    if (scanf("%u", &id) != 1)
    {
        printf("ID inválido.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    // Encontra o livro (para obter o título antes de ser removido/libertado)
    Book *book_info = db_find_book_by_id(db, id);

    if (!book_info)
    {
        printf("[book] Livro com ID %u não encontrado.\n", id);
        return;
    }

    // A Camada DB (db_remove_book) trata da remoção do nó e da libertação do Book*
    if (db_remove_book(db, id) == 0)
    {
        printf("[book] Livro com ID: %u eliminado com sucesso.\n", id);
    }
    else
    {
        printf("[ERRO] Falha ao remover o livro com ID %u na base de dados.\n", id);
    }
}

// 5. PESQUISA (Case 4)
// ... (código inalterado) ...
void book_search(DB *db)
{
    int opcao;
    char term[100] = {0};
    unsigned id_search = 0;
    int count = 0;

    if (!db)
    {
        printf("[book] DB inválida.\n");
        return;
    }

    DList *books = db_get_books(db);
    if (!books || !books->head)
    {
        printf("[book] Não existem livros para pesquisar.\n");
        return;
    }

    // 1. Apresentar o menu de pesquisa
    printf("\n--- PESQUISAR LIVROS ---\n");
    printf("Procurar por:\n");
    printf("1. ID (correspondência exata)\n");
    printf("2. Título (termo parcial e flexível)\n");
    printf("3. Autor (termo parcial e flexível)\n");
    printf("Escolha uma opção: ");

    if (scanf("%d", &opcao) != 1)
    {
        printf("Opção inválida.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    // 2. Capturar o termo de pesquisa
    if (opcao == 1)
    {
        printf("Insira o ID do livro: ");
        if (scanf("%u", &id_search) != 1)
        {
            printf("ID inválido.\n");
            while (getchar() != '\n')
                ;
            return;
        }
        while (getchar() != '\n')
            ; // Limpar buffer
    }
    else if (opcao == 2 || opcao == 3)
    {
        const char *campo = (opcao == 2) ? "Título" : "Autor";
        printf("Insira o termo de pesquisa (%s): ", campo);
        if (!fgets(term, sizeof(term), stdin))
            return;
        remove_newline(term);
        if (strlen(term) == 0)
        {
            printf("O termo de pesquisa não pode ser vazio.\n");
            return;
        }
    }
    else
    {
        printf("Opção inválida. Tente novamente.\n");
        return;
    }

    // 3. Processar a pesquisa na lista
    printf("\n[book] Resultados:\n");

    DLIST_FOREACH(books, node)
    {
        Book *b = (Book *)node->data;
        int match = 0;

        switch (opcao)
        {
        case 1: // ID: Correspondência exata
            if (b->id == id_search)
            {
                match = 1;
            }
            break;

        case 2: // Título: Termo parcial e case-insensitive
            if (stristr(b->title, term))
            {
                match = 1;
            }
            break;

        case 3: // Autor: Termo parcial e case-insensitive
            if (stristr(b->author, term))
            {
                match = 1;
            }
            break;
        }

        if (match)
        {
            printf("  id=%u, titulo=%s, autor=%s, ano=%d, disponivel=%d\n",
                   b->id, b->title, b->author, b->year, b->available);
            count++;

            // Otimização: Se for pesquisa por ID, o primeiro match é o único resultado possível.
            if (opcao == 1)
                break;
        }
    }

    // 4. Resultado final
    if (count == 0)
    {
        printf("  Nenhum livro encontrado.\n");
    }
    else
    {
        printf("  %d livro(s) encontrado(s).\n", count);
    }
}

// 6. DUPLICAÇÃO (Case 5)

void book_duplicate(DB *db)
{
    unsigned id_original;

    if (!db)
    {
        printf("[book] DB inválida.\n");
        return;
    }

    printf("\n--- DUPLICAR LIVRO ---\n");
    printf("Insira o ID do livro a duplicar: ");
    if (scanf("%u", &id_original) != 1)
    {
        printf("ID inválido.\n");
        while (getchar() != '\n')
            ;
        return;
    }
    while (getchar() != '\n')
        ; // Limpar buffer

    Book *original_book = book_find_by_id(db, id_original);

    if (!original_book)
    {
        printf("[book] Livro com ID %u não encontrado.\n", id_original);
        return;
    }

    // 1. Obter novo ID usando a função de cálculo MAX ID + 1
    unsigned new_id = find_max_book_id(db); // <--- CORRIGIDO

    // 2. Criar uma cópia na stack para usar db_add_book
    Book new_book_data;

    // 3. Copiar os dados para a nova estrutura (usando book_init é mais limpo)
    book_init(&new_book_data,
              new_id,
              original_book->title,
              original_book->author,
              original_book->year,
              original_book->available);

    // 4. Inserir na lista da DB (db_add_book aloca memória no heap)
    if (db_add_book(db, &new_book_data) == 0)
    {
        // REMOVIDO: db_increment_book_id(db); // Função que não existe
        printf("[book] Livro '%s' duplicado com sucesso. Novo ID: %u.\n", original_book->title, new_id);
    }
    else
    {
        printf("[ERRO] Falha na alocação de memória para duplicação.\n");
    }
}

// ************************************************
// FUNÇÕES DE GESTÃO DE SUGESTÕES DE LIVROS
// ************************************************

// Dentro de src/app/book_controller.c

/*void suggestion_register(DB *db)
{
    char title[SUGG_TITLE_MAX], author[SUGG_AUTHOR_MAX];

    printf("\n--- REGISTAR SUGESTÃO DE LIVRO ---\n");

    // 1. Obter um ID único (Assumindo a função db_get_next_suggestion_id)
    unsigned new_id = db_get_next_suggestion_id(db);

    // 2. Ler Título
    printf("Título do livro a sugerir: ");
    if (!fgets(title, sizeof(title), stdin))
        return;
    title[strcspn(title, "\n")] = 0; // Remove '\n'

    // 3. Ler Autor
    printf("Autor do livro a sugerir: ");
    if (!fgets(author, sizeof(author), stdin))
        return;
    author[strcspn(author, "\n")] = 0; // Remove '\n'

    // 4. Alocar e inicializar a estrutura Suggestion (Assumindo que está definida e suggestion_init existe)
    Suggestion *new_sug = (Suggestion *)malloc(sizeof(Suggestion));
    if (!new_sug)
    {
        printf("[sugestoes] ERRO: Falha na alocação de memória.\n");
        return;
    }

    suggestion_init(new_sug, new_id, title, author);

    // 5. Inserir na lista de sugestões da DB (Assumindo a função db_get_suggestions)
    dlist_push_back(db_get_suggestions(db), new_sug);
    db_increment_suggestion_id(db); // Atualiza o próximo ID

    printf("[sugestoes] Sugestão '%s' (ID: %u) registada com sucesso.\n", title, new_id);
}

void suggestion_register(DB *db)
{
    char title[SUGG_TITLE_MAX], author[SUGG_AUTHOR_MAX];

    // NOTA: É fundamental que limpes o buffer de input (stdin) antes de usar fgets,
    // especialmente se a função anterior foi scanf (como em menu.c).

    printf("\n--- REGISTAR SUGESTÃO DE LIVRO ---\n");

    // 1. Obter um ID único para a sugestão (Assumindo a função db_get_next_suggestion_id)
    unsigned new_id = db_get_next_suggestion_id(db);

    // 2. Ler Título (fgets é seguro para strings com espaços)
    printf("Título do livro a sugerir: ");
    if (!fgets(title, sizeof(title), stdin))
        return;
    // Remove o newline ('\n') adicionado pelo fgets
    title[strcspn(title, "\n")] = 0;

    // 3. Ler Autor
    printf("Autor do livro a sugerir: ");
    if (!fgets(author, sizeof(author), stdin))
        return;
    // Remove o newline ('\n') adicionado pelo fgets
    author[strcspn(author, "\n")] = 0;

    // 4. Alocar e inicializar a estrutura Suggestion
    Suggestion *new_sug = (Suggestion *)malloc(sizeof(Suggestion));
    if (!new_sug)
    {
        printf("[sugestoes] ERRO: Falha na alocação de memória.\n");
        return;
    }

    suggestion_init(new_sug, new_id, title, author);

    // 5. Inserir na lista de sugestões da DB (Assumindo a função db_get_suggestions)
    dlist_push_back(db_get_suggestions(db), new_sug);
    db_increment_suggestion_id(db); // Atualiza o próximo ID

    printf("[sugestoes] Sugestão '%s' (ID: %u) registada com sucesso.\n", title, new_id);
}
*/