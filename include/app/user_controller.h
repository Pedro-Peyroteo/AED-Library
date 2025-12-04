/*
    Módulo de Controle de Clientes:
    É responsável por gerir todas as operações relacionadas com os clientes do sistema.
    Fornece funções para listar, adicionar e remover utilizadores da base de dados.
    As operações incluem validação básica e persistência automática dos dados em ficheiro.
    
    Funcionalidades:
    - Listar todos os utilizadores, ordenados por ID crescente
    - Adicionar novos utilizadores com dados de identificação
    - Remover utilizadores do sistema
*/

#pragma once  /* Garante que este ficheiro é incluído apenas uma vez durante a compilação */
#include "lib/dlist/dlist.h"  /* Inclui a definição da lista ligada dinâmica */

/* Operacoes relacionadas com utilizadores. */

/* Lista todos os utilizadores registados, ordenados por ID crescente. */
void user_list_all(DList *users);

/* Adiciona um novo utilizador ao sistema. */
void user_add(DList *users);

/* Remove um utilizador pelo ID. */
void user_remove(DList *users);
