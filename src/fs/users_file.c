/*
    Módulo responsável pela leitura e escrita de clientes em ficheiros CSV.
    Também inclui funções relacionadas com a gestão de clientes.
*/

#include <stdio.h>  /* Para printf, fopen, fgets, fprintf, fclose */
#include <stdlib.h>  /* Para malloc, free */
#include <string.h>  /* Para strlen, strcspn, strncmp */

#include "model/user.h"  /* Definição da estrutura User */
#include "lib/dlist/dlist.h"  /* Definição da lista dinâmica */

#include "fs/users_file.h"  /* Declarações das funções deste módulo */

/* Constante: tamanho máximo de uma linha CSV */
#define USER_LINE_MAX 512  /* Limita o tamanho das linhas lidas do ficheiro */

/*
    Adiciona clientes fictícios a uma lista de clientes.
    Útil para testes e demonstrações.
    
*/
void adicionar_clientes_ficticios(DList *users) {
    User *u1 = malloc(sizeof(User));  /* Aloca memória para primeiro cliente */
    User *u2 = malloc(sizeof(User));  /* Aloca memória para segundo cliente */
    User *u3 = malloc(sizeof(User));  /* Aloca memória para terceiro cliente */
    
    if (u1 && u2 && u3) {  /* Verifica se a alocação foi bem sucedida */
        user_init(u1, 1, "Ana Silva", "ana.silva@email.com");  /* Inicializa primeiro cliente */
        user_init(u2, 2, "Bruno Costa", "bruno.costa@email.com");  /* Inicializa segundo cliente */
        user_init(u3, 3, "Carla Dias", "carla.dias@email.com");  /* Inicializa terceiro cliente */
        dlist_push_back(users, u1);  /* Adiciona primeiro cliente ao final da lista */
        dlist_push_back(users, u2);  /* Adiciona segundo cliente ao final da lista */
        dlist_push_back(users, u3);  /* Adiciona terceiro cliente ao final da lista */
    } else {  /* Se alocação falhar */
        free(u1); free(u2); free(u3);  /* Liberta a memória alocada */
    }
}

/* 
    Remove quebras de linha e retorno de carro do final de uma string.
    Isto é necessário porque fgets() inclui o '\n' na string lida.
*/
static void trim_newline(char *s)
{
    if (!s)  /* Valida o ponteiro */
        return;  /* Sai se NULL */

    size_t len = strlen(s);  /* Obtém o comprimento da string */
    while (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))  /* Enquanto terminar em \n ou \r */
    {
        s[--len] = '\0';  /* Remove o caractere de fim de linha */
    }
}

/*
    Carrega todos os utilizadores de um ficheiro CSV para uma lista dinâmica.

    Formato do ficheiro CSV:
        - Linha opcional de cabeçalho começando com "id;"
        - Uma linha por cada utilizador no formato CSV

    Retorna: 
    - DList com os utilizadores carregados
    - Uma DList vazia se o ficheiro não existir
    - NULL em caso de erro de alocação de memória
*/
DList *file_load_users(const char *path)
{
    FILE *f = fopen(path, "r");  /* Abre ficheiro para leitura */

    if (!f)  /* Se ficheiro não existe */
    {
        DList *empty = dlist_create(false, NULL);  /* Cria lista vazia */
        return empty;  /* Retorna lista vazia */
    }

    DList *list = dlist_create(false, NULL);  /* Cria lista para armazenar utilizadores */
    if (!list)  /* Se alocação de lista falha */
    {
        fclose(f);  /* Fecha ficheiro */
        return NULL;  /* Retorna NULL indicando erro */
    }

    char line[USER_LINE_MAX];  /* Buffer para ler linha do ficheiro */

    /* Tenta pular linha de cabeçalho opcional */
    if (fgets(line, sizeof line, f))  /* Se consegue ler primeira linha */
    {
        trim_newline(line);  /* Remove quebra de linha */
        if (strncmp(line, "id;", 3) != 0)  /* Se primeira linha NÃO é cabeçalho */
        {
            User *first = malloc(sizeof *first);  /* Aloca memória para primeiro utilizador */
            if (first && user_from_csv(first, line) == 0)  /* Se consegue converter CSV para User */
            {
                dlist_push_back(list, first);  /* Adiciona à lista */
            }
            else if (first)  /* Se conversão falha */
            {
                free(first);  /* Liberta memória */
            }
        }
    }

    /* Lê restantes linhas do ficheiro */
    while (fgets(line, sizeof line, f))  /* Enquanto conseguir ler linha */
    {
        trim_newline(line);  /* Remove quebra de linha */

        if (line[0] == '\0')  /* Se linha está vazia */
            continue;  /* Salta para próxima linha */

        User *u = malloc(sizeof *u);  /* Aloca memória para novo utilizador */
        if (!u)  /* Se alocação falha */
            break;  /* Sai do loop */

        if (!user_from_csv(u, line))  /* Se conversão CSV para User falha */
        {
            free(u);  /* Liberta memória */
            continue;  /* Salta para próxima linha */
        }

        dlist_push_back(list, u);  /* Adiciona utilizador à lista */
    }

    fclose(f);  /* Fecha ficheiro */
    return list;  /* Retorna lista carregada */
}

/*
    Guarda todos os utilizadores da lista num ficheiro CSV.
    
    Formato de saída:
        - Linha de cabeçalho: "id;name;email"
        - Uma linha por cada utilizador no formato CSV

    Retorna: 0 em sucesso, -1 em erro
*/
int file_save_users(const char *path, const DList *users)
{
    FILE *f = fopen(path, "w");  /* Abre ficheiro para escrita */
    if (!f)  /* Se não consegue abrir */
        return -1;  /* Retorna erro */

    char buffer[USER_LINE_MAX];  /* Buffer para converter User para CSV */

    /* Escreve cabeçalho */
    fprintf(f, "id;name;email\n");  /* Escreve linha de cabeçalho */

    DLIST_FOREACH(users, node)  /* Itera por cada nó da lista */
    {
        User *u = (User *)node->data;  /* Obtém o utilizador do nó */

        user_to_csv(u, buffer, sizeof buffer);  /* Converte User para formato CSV */
        fprintf(f, "%s\n", buffer);  /* Escreve linha CSV no ficheiro */
    }

    fclose(f);  /* Fecha ficheiro */
    return 0;  /* Retorna sucesso */
}
