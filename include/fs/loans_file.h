#ifndef LOANS_FILE_H
#define LOANS_FILE_H

#include <stdio.h>

#include "model/loans.h"
#include "lib/dlist/dlist.h"

DList *file_load_loans(const char *path);
int    file_save_loans(const char *path, const DList *loans);

#endif // LOANS_FILE_H
