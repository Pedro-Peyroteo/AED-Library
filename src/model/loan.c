#include "model/loans.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void loan_init(Loan* l, unsigned id,
               unsigned user_id,
               unsigned book_id,
               unsigned date_borrow,
               unsigned date_return)
{
    l->id = id;
    l->user_id = user_id;
    l->book_id = book_id;
    l->date_borrow = date_borrow;
    l->date_return = date_return;
}

int loan_from_csv(Loan* l, const char* line) {
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* token = strtok(tmp, ";");
    if (!token) return 0;
    l->id = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    l->user_id = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    l->book_id = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    l->date_borrow = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    l->date_return = atoi(token);

    return 1;
}

void loan_to_csv(const Loan* l, char* out, size_t out_size) {
    snprintf(out, out_size, "%u;%u;%u;%u;%u",
        l->id,
        l->user_id,
        l->book_id,
        l->date_borrow,
        l->date_return
    );
}
