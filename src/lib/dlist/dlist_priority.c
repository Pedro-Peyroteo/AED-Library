#include <dlist.h>
#include "internal.h"

/*
    Priority-based insertion.
    This function keeps the list ordered by priority.

    Rules (default behavior):
        - Higher priority comes first.
        - If two nodes have the same priority, the new one goes AFTER
          the existing ones (stable insertion).

    This is basically the logic behind a priority queue,
    but implemented on top of a doubly linked list.

    Steps:
        1. Create node.
        2. Handle empty list.
        3. If new node has higher priority than head, it becomes new head.
        4. Walk the list until we find a node with lower priority.
        5. Insert right before that node.
*/
void dlist_insert_priority(DList *list, void *data, int priority)
{
    // Create the new node to insert.
    Node *node = dlist_create_node(data, priority);
    if (!node)
        return;

    // Empty list -> easy case.
    if (!list->head) {
        list->head = node;
        list->tail = node;
        list->size = 1;
        return;
    }

    // If new node has higher priority than the head -> becomes new head.
    if (priority > list->head->priority) {
        node->next = list->head;
        list->head->prev = node;
        list->head = node;
        list->size++;
        return;
    }

    // Walk through the list until we find the correct spot.
    Node *curr = list->head;

    /*
        We advance as long as:
            current priority >= new priority.
        This means higher or equal priority nodes stay in front,
        giving us a stable, descending ordered list.
    */
    while (curr && curr->priority >= priority) {
        curr = curr->next;
    }

    // If we reached the end, insert at the tail.
    if (!curr) {
        node->prev = list->tail;
        list->tail->next = node;
        list->tail = node;
        list->size++;
        return;
    }

    // Otherwise we insert BEFORE 'curr'.
    node->next = curr;
    node->prev = curr->prev;

    curr->prev->next = node;
    curr->prev = node;

    // If insertion point was head, adjust head pointer.
    if (curr == list->head)
        list->head = node;

    // This one is self-explanatory.
    list->size++;
}
