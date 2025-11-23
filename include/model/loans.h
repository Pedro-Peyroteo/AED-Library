#ifndef LOANS_H
#define LOANS_H

#include <stddef.h>

typedef struct {
    unsigned id;
    unsigned user_id;
    unsigned book_id;
    unsigned date_borrow;   // YYYYMMDD (integer)
    unsigned date_return;   // 0 if not returned
} Loan;

void loan_init(Loan* l, unsigned id,
               unsigned user_id,
               unsigned book_id,
               unsigned date_borrow,
               unsigned date_return);

int  loan_from_csv(Loan* l, const char* line);
void loan_to_csv(const Loan* l, char* out, size_t out_size);

#endif
