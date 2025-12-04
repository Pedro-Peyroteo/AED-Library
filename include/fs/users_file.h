#ifndef USERS_FILE_H  /* Proteção contra múltiplas inclusões */
#define USERS_FILE_H  /* Protecção contra múltiplas inclusões */

#include <stdio.h>  /* Para entrada/saída */

#include "model/user.h"  /* Definição da estrutura User */
#include "lib/dlist/dlist.h"  /* Definição da lista dinâmica */

/* 
	Funções de carregamento e gravação de utilizadores em ficheiro CSV
*/
DList *file_load_users(const char *path);  /* Carrega utilizadores do ficheiro CSV */
int    file_save_users(const char *path, const DList *users);  /* Guarda utilizadores no ficheiro CSV */

#endif /* USERS_FILE_H */  /* Fim da proteção de múltiplas inclusões */
