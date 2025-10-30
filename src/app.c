#include <stdio.h>
#include "app.h"
#include "books.h"

void app_init(void) {
    // load configs, allocate resources, etc.
    printf("App iniciada.\n");
}

void app_run(void) {
    int option;

    while (1) {
        printf("Menu Principal\n");
        printf("1 - Clientes\n");
        printf("2 - Livros\n");
        printf("3 - Emprestimos\n");
        printf("4 - Sugestoes\n");
        printf("0 - Sair\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &option);

        switch (option) {
            case 1: menuClientes(); break;
            case 2: menuLivros(); break;
            case 3: menuEmprestimos(); break;
            case 4: menuSugestoes(); break;
            case 0:
                printf("A encerrar a aplicação...\n");
                return;
            default:
                printf("Opção inválida!\n");
        }
    }
}

void app_shutdown(void) {
    // free memory, close files, etc.
    printf("App encerrada com sucesso.\n");
}

