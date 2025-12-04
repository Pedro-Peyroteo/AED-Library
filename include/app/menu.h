/*
	Header do Módulo de Menus
	=========================
	
	Define as funções de menu disponíveis na aplicação.
	Cada função menu exibe um menu interativo para o utilizador.
*/

#pragma once  /* Garante que este ficheiro é incluído apenas uma vez */

#include "lib/dlist/dlist.h"  /* Inclui a definição da lista dinâmica */

/* 
	Funções de menu para a interface da aplicação
	Cada uma representa um submenu diferente
*/

/* Menu de gestão de clientes - recebe lista de users */
void menuClientes(DList *users);

/* Menu de gestão de livros */
void menuLivros(void);

/* Menu de gestão de empréstimos */
void menuEmprestimos(void);

/* Menu de sugestões de novos livros */
void menuSugestoes(void);
