/*
    Módulo de Menus da Aplicação:
    Este ficheiro é responsável pela apresentação dos menus da aplicação ao cliente.
    Fornece interface interativa para acesso às diferentes funcionalidades:
    - Menu de Clientes: Listar, adicionar e remover clientes
    - Menu de Livros: Operações sobre o catálogo de livros
    - Menu de Empréstimos: Gestão de empréstimos de livros
    - Menu de Sugestões: Sugestões de novos livros
    
    Cada menu extrai os dados necessários e chama as funções apropriadas dos controladores para realizar as operações solicitadas.
*/

#include <stdio.h>  /* Biblioteca para entrada/saída (printf, scanf) */
#include "app/menu.h"  /* Header com protótipos das funções de menu */
#include "app/user_controller.h"  /* Header com funções de controle de clientes */
#include "app/book_controller.h"  /* Header com funções de controle de livros */
#include "app/loan_controller.h"  /* Header com funções de controle de empréstimos */

void menuClientes(DList *users)  /* Menu de gestão de clientes, recebe lista de clientes */
{
	int opcao;  /* Variável para armazenar a opção escolhida pelo cliente */
	do {  /* Inicia loop do-while para manter o menu aberto até sair */
		printf("\n=== MENU CLIENTES ===\n");  /* Imprime título do menu */
		printf("1 - Listar Clientes\n");  /* Opção 1: listar todos os clientes */
		printf("2 - Adicionar Cliente\n");  /* Opção 2: adicionar novo cliente*/
		printf("3 - Remover Cliente\n");  /* Opção 3: remover cliente */
		printf("0 - Voltar\n");  /* Opção 0: regressar ao menu anterior */
		printf("Escolha uma opcao: ");  /* Pede ao cliente que escolha */
		scanf("%d", &opcao);  /* Lê a opção digitada pelo cliente */
		switch(opcao) {  /* Executa ação conforme a opção escolhida */
			case 1:  /* Se opção é 1 */
				user_list_all(users);  /* Chama função para listar clientes */
				break;  /* Sai do switch */
			case 2:  /* Se opção é 2 */
				user_add(users);  /* Chama função para adicionar cliente */
				break;  /* Sai do switch */
			case 3:  /* Se opção é 3 */
				user_remove(users);  /* Chama função para remover cliente */
				break;  /* Sai do switch */
			case 0:  /* Se opção é 0 */
				break;  /* Sai do switch (vai sair do loop) */
			default:  /* Se opção é inválida */
				printf("Opcao invalida!\n");  /* Imprime mensagem de erro */
		}
	} while(opcao != 0);  /* Continua enquanto a opção não for 0 (sair) */
}

void menuLivros(void)  /* Menu de gestão de livros */
{
	printf("\n=== MENU LIVROS ===\n");  /* Imprime título do menu */
	printf("Funcionalidade de livros em desenvolvimento pelos colegas.\n");  /* Mensagem temporária */
}

void menuEmprestimos(void)  /* Menu de gestão de empréstimos */
{
	printf("\n=== MENU EMPRESTIMOS ===\n");  /* Imprime título do menu */
	printf("Funcionalidade de empréstimos em desenvolvimento pelos colegas.\n");  /* Mensagem temporária */
}

void menuSugestoes(void)  /* Menu de sugestões */
{
	printf("\n=== MENU SUGESTOES ===\n");  /* Imprime título do menu */
	printf("[sugestoes] Funcionalidade de sugestoes ainda nao implementada.\n");  /* Mensagem indicando que está em desenvolvimento */
}
