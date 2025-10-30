#ifndef BOOKS_H
#define BOOKS_H

// Assume-se que "cutils.h" define o tipo ArrayList
#include "cutils.h"
// #include "app.h" // Se for necessário para voltarMenuPrincipal

// Definição da estrutura para um Livro (Book structure definition)
typedef struct Livro
{
    int id;
    char titulo[100];
    char isbn[30];
    char autor[100];
    int numExemplares;
} Livro;

// Declaração do ArrayList global que armazena objetos Livro
extern ArrayList livro;

// Protótipos das funções de menu
void menuLivros();
void menuSugestoes();

// Protótipos das funções de gestão de livros
void inserirLivro(Livro livros[], int *numLivros);
void editarLivro(Livro livros[], int numLivros);
void eliminarLivro(Livro livros[], int *numLivros);
void pesquisarLivro(Livro livros[], int numLivros);
void listarLivros(Livro livros[], int numLivros);

// Protótipos das funções de ficheiros
void guardarLivrosEmFicheiro(Livro livros[], int numLivros, const char *books);
void lerLivrosDeFicheiro(Livro livros[], int *numLivros, const char *books);

// Protótipos das funções de sugestões
void sugerirLivro(const char *suggestions);
void listarSugestoesDeLivros(const char *suggestions);

// Outras funções
void voltarMenuPrincipal();
void liberarRecursos();

#endif // BOOKS_H