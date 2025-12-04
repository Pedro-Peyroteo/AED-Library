/*
	Módulo de Aplicação
	Este ficheiro é responsável por gerenciar a camada de aplicação.
	Coordena a inicialização do sistema, a execução do menu principal e o encerramento com persistência de dados.
	
	Estrutura:
	- app_init(): Carrega dados dos ficheiros
	- app_run(): Loop principal que exibe o menu e processa opções
	- app_shutdown(): Guarda dados e limpa recursos
*/

#include <stdio.h>  /* Biblioteca para entrada/saída (printf, scanf) */
#include <stdlib.h>  /* Biblioteca para malloc, free */
#include "app/app.h"  /* Header com protótipos das funções app_* */
#include "app/menu.h"  /* Header com protótipos dos menus */
#include "lib/dlist/dlist.h"  /* Header com definição da lista dinâmica */
#include "fs/users_file.h"  /* Header com funções de carregamento de users */

/* 
	Listas globais da aplicação (privadas).
	Armazenam os dados de users (clientes).
*/
static DList *users_list = NULL;

void app_init(void)  /* Inicializa a aplicação carregando dados */
{
	printf("App iniciada.\n");  /* Mensagem de início */

	/* 
		Carrega dados dos ficheiros para a memória.
		Se falhar, continua com listas vazias.
	*/
	users_list = file_load_users("data/users.txt");  /* Carrega users do ficheiro */
	if (users_list == NULL) {
		printf("Erro ao carregar utilizadores.\n");  /* Erro ao carregar */
		users_list = dlist_create(false, NULL);  /* Cria lista vazia sem prioridade */
	}

	/* 
		Se a lista de clientes estiver vazia, adiciona clientes de teste.
		Isto é útil para testes iniciais do programa.
	*/
	if (users_list && users_list->size == 0) {
		extern void adicionar_clientes_ficticios(DList *users);  /* Declaração da função externa */
		extern int file_save_users(const char *path, const DList *users);  /* Declaração da função externa */
		adicionar_clientes_ficticios(users_list);  /* Cria clientes de teste */
		file_save_users("data/users.txt", users_list);  /* Guarda clientes de teste */
	printf("Clientes registados adicionados e guardados para testes.\n");  /* Confirmação */
	}
}

void app_run(void)  /* Loop principal da aplicação com menu */
{
	int option;  /* Variável para armazenar a opção do utilizador */

	while (1)  /* Loop infinito até o utilizador escolher sair */
	{
		printf("Menu Principal\n");  /* Título do menu */
		printf("1 - Clientes\n");  /* Opção 1 */
		printf("2 - Livros\n");  /* Opção 2 */
		printf("3 - Emprestimos\n");  /* Opção 3 */
		printf("4 - Sugestoes\n");  /* Opção 4 */
		printf("0 - Sair\n");  /* Opção para sair */
		printf("Escolha uma opcao: ");  /* Pede opção ao utilizador */
		if (scanf("%d", &option) != 1)  /* Lê a opção */
		{
			/* Se a leitura falhar, limpa o buffer de entrada */
			int ch;
			while ((ch = getchar()) != '\n' && ch != EOF) {}
			printf("Entrada inválida.\n");  /* Mensagem de erro */
			continue;  /* Volta ao início do loop */
		}

		switch (option)  /* Executa ação conforme a opção */
		{
			case 1: menuClientes(users_list);    break;  /* Abre menu de clientes, passa a lista de users */
			case 2: menuLivros();      break;  /* Abre menu de livros */
			case 3: menuEmprestimos(); break;  /* Abre menu de empréstimos */
			case 4: menuSugestoes();   break;  /* Abre menu de sugestões */
			case 0:  /* Se opção é 0 (sair) */
				printf("A encerrar a aplicação...\n");  /* Mensagem de encerramento */
				return;  /* Sai da função */
			default:  /* Se opção é inválida */
				printf("Opção inválida!\n");  /* Mensagem de erro */
		}
	}
}

void app_shutdown(void)  /* Encerra a aplicação e guarda dados */
{
	/* Guarda os dados da memória para os ficheiros */
	if (users_list != NULL) {
		file_save_users("data/users.txt", users_list);  /* Guarda users */
	}

	/* Liberta a memória alocada */
	if (users_list != NULL) {
		dlist_destroy(users_list, free);  /* Liberta a lista de users (cada User é malloc'd) */
		users_list = NULL;  /* Define como NULL */
	}

	printf("App encerrada com sucesso.\n");  /* Mensagem de encerramento */
}