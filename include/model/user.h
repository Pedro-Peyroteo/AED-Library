#ifndef USER_H
#define USER_H

#include <stddef.h>

typedef struct {
    unsigned id;
    char name[128];
    char email[128];
} User;

void user_init(User* u, unsigned id,
               const char* name,
               const char* email);

int  user_from_csv(User* u, const char* line);
void user_to_csv(const User* u, char* out, size_t out_size);

#endif
