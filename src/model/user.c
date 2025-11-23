#include "model/user.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void user_init(User* u, unsigned id,
               const char* name,
               const char* email) {
    u->id = id;
    strncpy(u->name, name, sizeof(u->name));
    strncpy(u->email, email, sizeof(u->email));
}

int user_from_csv(User* u, const char* line) {
    char tmp[512];
    strncpy(tmp, line, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* token = strtok(tmp, ";");
    if (!token) return 0;
    u->id = atoi(token);

    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(u->name, token, sizeof(u->name));

    token = strtok(NULL, ";");
    if (!token) return 0;
    strncpy(u->email, token, sizeof(u->email));

    return 1;
}

void user_to_csv(const User* u, char* out, size_t out_size) {
    snprintf(out, out_size, "%u;%s;%s",
        u->id,
        u->name,
        u->email
    );
}
