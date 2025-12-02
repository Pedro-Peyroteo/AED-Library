#ifndef SUGGESTIONS_FILE_H
#define SUGGESTIONS_FILE_H

#include "lib/dlist/dlist.h"
#include "model/suggestion.h"

DList *file_load_suggestions(const char *path);
int file_save_suggestions(const char *path, const DList *suggestions);

#endif /* SUGGESTIONS_FILE_H */
