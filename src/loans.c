#include "cutils.h"
#include "loans.h"
#include <stdio.h>
#include <string.h>

static ArrayList loans;

//Inicar a lista
void loans_init() {
    arraylist_init (&loans, sizeof(Loan));
}

//Libertar memória
void loans_free() {
    arraylist_free (&loans);
}

//Carregar empréstimos
void loadLoans (const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        printf("Ficheiro %s não encontrado. Será criado ao guardar. \n", filename);
        return;
    }

    Loan temp;
    while (fscanf(f, "%s %s %s %s %d", temp.customer_id, temp.book_id, temp.loan_date, temp.return_date, (int*)&temp.active) == 5) {
        arraylist_append(&loans, &temp);
    }

    fclose(f);
}

//Guardar empréstimos
void saveLoans (const char *filename){
    FILE *f = fopen(filename, "w");
    if (!f) {
        printf("Erro ao abrir %s para escrita \n", filename);
        return;
    }

    for (size_t i = 0; i < loans.count; i++) {
        Loan *l = (Loan*)arraylist_get(&loans, i);
        fprintf(f, "%s %s %s %s %d\n", l->customer_id, l->book_id, l->loan_date, l->return_date, l->active);
    }

    fclose(f);
}

//Registar novo empréstimo
void registerLoan() {
    Loan l;

    printf("ID do cliente: ");
    scanf("%s", l.customer_id);
    printf("ID do livro: ");
    scanf("%s", l.book_id);
    printf("Data do empréstimo (AAAA-MM-DD): ");
    scanf("%s", l.loan_date);

    strcpy(l.return_date, "-");
    l.active = true;

    if (arraylist_append(&loans, &l))
        printf("Empréstimo registado com sucesso!\n");
        else
        printf("Erro. Memória insuficiente.\n");
}

//Registar devolução
void registerReturn() {
    char book_id[MAX_ID];
    printf("ID do livro a devolver: ");
    scanf("%s", book_id);

    for (size_t i = 0; i < loans.count; i++) {
        Loan *l =(Loan*)arraylist_get(&loans, i);
        if (strcmp(l->book_id, book_id) == 0 && l->active) {
            printf("Data da devolução (AAAA-MM-DD): ");
            scanf("%s", l->return_date);
            l->active = false;
            printf("Devolução registada!\n");
            return;
        }
    }
    printf("Empréstimo não encontrado ou já devolvido.\n");
}

//Listar empréstimos ativos
void listActiveLoans() {
    printf("\nEmpréstimos ativos:\n");
    for (size_t i = 0; i < loans.count; i++) {
        Loan *l = (Loan*)arraylist_get(&loans, i);
        if (l->active) {
            printf("Cliente: %s | Livro: %s | Data: %s\n", l->customer_id, l->book_id, l->loan_date);
        }
    }
}