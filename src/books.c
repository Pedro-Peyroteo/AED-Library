#include <stdio.h>
#include "cutils.h"
// #include "app.h"

typedef struct
{
    int id;
    char titulo[100];
    char isbn[30];
    char autor[100];
    int numExemplares;
} Livro;

// Inicializa o ArrayList para armazenar livros
ArrayList livro;

// Menu principal Livro
void menuLivros()
{
    // Preparar/Inicializar o ArrayList
    arraylist_init(&livro, sizeof(Livro));

    printf("\n");
    printf("1- Inserir Cliente\n");
    printf("2- Editar Livro\n");
    printf("3- Eliminar Livro\n");
    printf("4- Pesquisar Livro\n");
    printf("5- Listar Livros\n");
    printf("6- Guardar/Ler em Ficheiro\n");
    printf("7- Menu Sugestoes de Livros\n");
    printf("8- Voltar ao Menu Principal\n");
    printf("0- Sair\n");
}

void menuSugestoes()
{

    printf("1- Cliente Sugerir Livro\n");
    printf("2- Listar Sugestoes de Livros\n");
    printf("3- Guardar/Ler Sugestoes de Livros em Ficheiro\n");
}

// Função para inserir um novo livro
void inserirLivro(Livro livros[], int *numLivros)
{

    Livro novoLivro;
    printf("Id Unico do Livro: ");
    scanf("%d", &novoLivro.id);
    printf("Digite o titulo do livro: ");
    scanf(" %[^\n]", novoLivro.titulo);
    printf("Digite o ISBN do livro: ");
    scanf(" %[^\n]", novoLivro.isbn);
    printf("Digite o Autor do livro: ");
    scanf(" %[^\n]", novoLivro.autor);
    printf("Numero de Exemplares em Stock: ");
    scanf("%d", &novoLivro.numExemplares);

    arraylist_append(&livro, &novoLivro);

    livros[*numLivros] = novoLivro;
    (*numLivros)++;
    printf("Livro inserido com sucesso!\n");
}

// Função para editar um livro existente
void editarLivro(Livro livros[], int numLivros)
{
    int id, i, encontrado = 0;
    printf("Digite o Id do livro a editar: ");
    scanf("%d", &id);

    for (i = 0; i < numLivros; i++)
    {
        if (livros[i].id == id)
        {
            encontrado = 1;
            printf("Editando Livro: %s\n", livros[i].titulo);
            printf("Novo Titulo: ");
            scanf(" %[^\n]", livros[i].titulo);
            printf("Novo ISBN: ");
            scanf(" %[^\n]", livros[i].isbn);
            printf("Novo Autor: ");
            scanf(" %[^\n]", livros[i].autor);
            printf("Novo Numero de Exemplares: ");
            scanf("%d", &livros[i].numExemplares);
            printf("Livro editado com sucesso!\n");
            break;
        }
    }
    if (!encontrado)
    {
        printf("Livro com Id %d nao encontrado.\n", id);
    }
}

// Função para eliminar um livro
void eliminarLivro(Livro livros[], int *numLivros)
{
    int id, i, j, encontrado = 0;
    printf("Lista Livros com ID:\n");
    listarLivros(livros, *numLivros);

    printf("Digite o Id do livro a eliminar: ");
    scanf("%d", &id);

    for (i = 0; i < *numLivros; i++)
    {
        if (livros[i].id == id)
        {
            encontrado = 1;
            for (j = i; j < *numLivros - 1; j++)
            {
                livros[j] = livros[j + 1];
            }
            (*numLivros)--;
            printf("Livro eliminado com sucesso!\n");
            break;
        }
    }
    if (!encontrado)
    {
        printf("Livro com Id %d nao encontrado.\n", id);
    }
}

// Função para pesquisar um livro por título ou autor
void pesquisarLivro(Livro livros[], int numLivros)
{
    char termo[100];
    int i, encontrado = 0;
    printf("Digite o titulo ou autor do livro a pesquisar: ");
    scanf(" %[^\n]", termo);

    printf("Resultados da pesquisa:\n");
    for (i = 0; i < numLivros; i++)
    {
        if (strstr(livros[i].titulo, termo) != NULL || strstr(livros[i].autor, termo) != NULL)
        {
            printf("ID: %d, Titulo: %s, Autor: %s, ISBN: %s, Exemplares: %d\n",
                   livros[i].id, livros[i].titulo, livros[i].autor, livros[i].isbn, livros[i].numExemplares);
            encontrado = 1;
        }
    }
    if (!encontrado)
    {
        printf("Nenhum livro encontrado com o termo '%s'.\n", termo);
    }
}

// Função para listar todos os livros
void listarLivros(Livro livros[], int numLivros)
{
    int i;
    if (numLivros == 0)
    {
        printf("Nenhum livro cadastrado.\n");
        return;
    }
    printf("Lista de Livros:\n");
    for (i = 0; i < numLivros; i++)
    {
        printf("ID: %d, Titulo: %s, Autor: %s, ISBN: %s, Exemplares: %d\n",
               livros[i].id, livros[i].titulo, livros[i].autor, livros[i].isbn, livros[i].numExemplares);
    }
}

// Função para guardar livros em um ficheiro
void guardarLivrosEmFicheiro(Livro livros[], int numLivros, const char *books)
{
    FILE *file = fopen(books, "wb");
    if (file == NULL)
    {
        printf("Erro ao abrir o ficheiro para escrita.\n");
        return;
    }
    fwrite(&numLivros, sizeof(int), 1, file);
    fwrite(livros, sizeof(Livro), numLivros, file);
    fclose(file);
    printf("Livros guardados com sucesso em '%s'.\n", books);
}

// Função para ler livros de um ficheiro
void lerLivrosDeFicheiro(Livro livros[], int *numLivros, const char *books)
{
    FILE *file = fopen(books, "rb");
    if (file == NULL)
    {
        printf("Erro ao abrir o ficheiro para leitura.\n");
        return;
    }
    fread(numLivros, sizeof(int), 1, file);
    fread(livros, sizeof(Livro), *numLivros, file);
    fclose(file);
    printf("Livros lidos com sucesso de '%s'.\n", books);
}

// Função para sugerir um livro
void sugerirLivro(const char *suggestions)
{

    FILE *file = fopen(suggestions, "a");
    if (file == NULL)
    {
        printf("Erro ao abrir o ficheiro '%s' para escrita.\n", suggestions);
        return;
    }

    char titulo[100];
    char autor[100];

    // Limpa o buffer do teclado (importante se antes houve um scanf)
    getchar();

    printf("Digite o título do livro que deseja sugerir: ");
    fgets(titulo, sizeof(titulo), stdin);

    printf("Digite o autor do livro que deseja sugerir: ");
    fgets(autor, sizeof(autor), stdin);

    // remove o '\n' final das strings lidas com fgets
    titulo[strcspn(titulo, "\n")] = '\0';
    autor[strcspn(autor, "\n")] = '\0';

    // Mostra no ecrã e grava no ficheiro
    printf("Sugestão: %s - %s\n", titulo, autor);
    fprintf(file, "Título: %s | Autor: %s\n", titulo, autor);

    fclose(file);
    printf("Sugestão guardada com sucesso em '%s'.\n", suggestions);
}

// Função para listar sugestões de livros
void listarSugestoesDeLivros(const char *suggestions)
{
    FILE *file = fopen(suggestions, "r");
    if (file == NULL)
    {
        printf("Nenhuma sugestão encontrada (ficheiro '%s' não existe).\n", suggestions);
        return;
    }

    char linha[200];
    int count = 0;
    printf("\n Lista de sugestões de livros:\n");
    printf("----------------------------------------\n");

    while (fgets(linha, sizeof(linha), file))
    {
        printf("%d. %s", ++count, linha);
    }

    if (count == 0)
        printf("(Ficheiro vazio — nenhuma sugestão ainda.)\n");

    fclose(file);
    printf("----------------------------------------\n");
}

void voltarMenuPrincipal()
{
    // Chamar a função do menu principal (definida em app.c)
    // menuPrincipal();
}
// Função para liberar recursos do ArrayList
void liberarRecursos()
{
    arraylist_free(&livro);
}
