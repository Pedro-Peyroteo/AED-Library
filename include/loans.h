#ifndef LOANS_H
#define LOANS_H

#include "cutils.h"

#define MAX_ID 20

typedef struct {
    char customer_id[MAX_ID];
    char book_id [MAX_ID];
    char loan_date[11];       //AAAA-MM-DD
    char return_date[11];     //"-" se não devolido
    bool active;              //true = ativo, false = devolvido
} Loan;

//Funções
void loans_init();
void loans_free();
void loadLoans(const char *filename);
void saveLoans(const char *fileaname);
void registerLoan();
void registerReturn();
void listActiveLoans();

#endif