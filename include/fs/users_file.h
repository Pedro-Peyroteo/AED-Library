#ifndef USERS_FILE_H
#define USERS_FILE_H

#include <stdio.h>

#include "model/user.h"
#include "lib/dlist/dlist.h"

DList *file_load_users(const char *path);
int    file_save_users(const char *path, const DList *users);

#endif // USERS_FILE_H
