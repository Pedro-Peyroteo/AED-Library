/* 
	Implementação do Módulo de Controle de Clientes
	================================================
	
	Este módulo implementa todas as operações de gestão de clientes:
	- Listagem com ordenação por ID
	- Adição de novos clientes
	- Remoção de clientes existentes
	- Persistência automática dos dados
*/

// Includes necessários
#include <stdio.h>  /* Para printf */
#include <string.h>  /* Para strcspn, strlen */
#include <stdlib.h>  /* Para malloc, free, qsort */
#include "model/user.h"  /* Definição da estrutura User */
#include "lib/dlist/dlist.h"  /* Definição da lista dinâmica */
#include "fs/users_file.h"  /* Funções de carregamento/gravação */

/* 
	Função comparadora para ordenação de utilizadores por ID
	Utilizada pelo qsort para ordenar array de Users
	Retorna: negativo se a < b, 0 se a == b, positivo se a > b
*/
int cmp_user_id(const void *a, const void *b) {
    const User *ua = *(const User**)a;  /* Desreferencia duplo pointer a */
    const User *ub = *(const User**)b;  /* Desreferencia duplo pointer b */
    return (ua->id > ub->id) - (ua->id < ub->id);  /* Comparação aritmética dos IDs */
}

/* Remove um utilizador da lista pelo seu ID */
void user_remove(DList *users)
{
    if (!users) {  /* Valida se lista é válida */
        printf("[user] Lista invalida.\n");  /* Mensagem de erro */
        return;  /* Sai da função */
    }
    
    unsigned id;  /* Variável para armazenar ID a remover */
    printf("\n--- Remover utilizador ---\n");  /* Título da operação */
    printf("ID do utilizador a remover: ");  /* Pede o ID */
    scanf("%u", &id);  /* Lê o ID */
    
    DListNode *node = users->head;  /* Começa na cabeça da lista */
    while (node) {  /* Itera pela lista */
        User *u = (User *)node->data;  /* Obtém o utilizador do nó */
        if (u->id == id) {  /* Se encontrou o ID */
            DListNode *next = node->next;  /* Guarda o próximo nó */
            if (node->prev) node->prev->next = next;  /* Remove ligação anterior */
            else users->head = next;  /* Se era a cabeça, actualiza head */
            if (next) next->prev = node->prev;  /* Remove ligação seguinte */
            else users->tail = node->prev;  /* Se era a cauda, actualiza tail */
            free(node->data);  /* Liberta o User */
            free(node);  /* Liberta o nó */
            users->size--;  /* Decrementa tamanho */
            file_save_users("data/users.txt", users);  /* Guarda em ficheiro */
            printf("Utilizador removido com sucesso!\n");  /* Confirmação */
            return;  /* Sai da função */
        }
        node = node->next;  /* Vai ao próximo nó */
    }
    printf("Utilizador com ID %u nao encontrado.\n", id);  /* Utilizador não encontrado */
}

/* Adiciona um novo utilizador à lista */
void user_add(DList *users)
{
    if (!users) {  /* Valida se lista é válida */
        printf("[user] Lista invalida.\n");  /* Mensagem de erro */
        return;  /* Sai da função */
    }

    User novo;  /* Estrutura temporária para novo utilizador */
    printf("\n--- Adicionar novo utilizador ---\n");  /* Título da operação */
    printf("ID: ");  /* Pede o ID */
    scanf("%u", &novo.id);  /* Lê o ID */
    getchar();  /* Consome o '\n' deixado pelo scanf */
    printf("Nome: ");  /* Pede o nome */
    fgets(novo.name, sizeof(novo.name), stdin);  /* Lê o nome com espaços */
    novo.name[strcspn(novo.name, "\n")] = '\0';  /* Remove o '\n' */
    printf("Email: ");  /* Pede o email */
    fgets(novo.email, sizeof(novo.email), stdin);  /* Lê o email com espaços */
    novo.email[strcspn(novo.email, "\n")] = '\0';  /* Remove o '\n' */
    
    User *u = malloc(sizeof(User));  /* Aloca memória para novo User */
    *u = novo;  /* Copia dados para memória alocada */
    dlist_push_back(users, u);  /* Adiciona ao fim da lista */
    file_save_users("data/users.txt", users);  /* Guarda em ficheiro */
    printf("Utilizador adicionado com sucesso!\n");  /* Confirmação */
}

/* Lista todos os utilizadores ordenados por ID */
void user_list_all(DList *users)
{
    if (!users) {  /* Valida se lista é válida */
        printf("[user] Lista invalida.\n");  /* Mensagem de erro */
        return;  /* Sai da função */
    }

    if (!users->head) {  /* Valida se lista está vazia */
        printf("[user] Nao existem utilizadores registados.\n");  /* Mensagem de lista vazia */
        return;  /* Sai da função */
    }

    /* 
		Copiar ponteiros para array temporário.
		Isto permite ordenar sem modificar a lista original.
	*/
    size_t n = users->size;  /* Obtém o número de utilizadores */
    User **arr = malloc(n * sizeof(User*));  /* Aloca array de ponteiros */
    DListNode *node = users->head;  /* Começa na cabeça */
    for (size_t i = 0; i < n; i++) {  /* Itera por todos os nós */
        arr[i] = (User*)node->data;  /* Copia ponteiro para User */
        node = node->next;  /* Vai ao próximo nó */
    }
    
    /* 
		Ordena o array usando qsort.
		Utiliza a função comparadora cmp_user_id para ordenar por ID.
	*/
    qsort(arr, n, sizeof(User*), cmp_user_id);  /* Ordena o array */
    
    /* Imprime os utilizadores ordenados */
    printf("[user] Lista de clientes:\n");  /* Título da lista */
    for (size_t i = 0; i < n; i++) {  /* Itera pelo array ordenado */
        printf("  id=%u, nome=%s, email=%s\n", arr[i]->id, arr[i]->name, arr[i]->email);  /* Imprime cada utilizador */
    }
    free(arr);  /* Liberta o array temporário */
}
