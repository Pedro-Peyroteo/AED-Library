#ifndef DLIST_INTERNAL_H
#define DLIST_INTERNAL_H

#include <dlist.h>


/*
    Internal alias — shorter to write, easier to read.
    Makes code inside src/ a bit cleaner.
*/
typedef DListNode Node;

/*
    Allocates a new node and sets its fields.
    Keeping it as a separate helper reduces code duplication,
    in push_front, push_back, and priority insertions.
*/
Node *dlist_create_node(void *data, int priority);

#endif
