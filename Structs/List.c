#include "List.h"
#include <stdlib.h>
#include <assert.h>


List *List_init() {
    List *list;

    list = (List *)malloc(sizeof(List));
    list->front = NULL;
    list->size = 0;

    return list;
}

void List_destroy(List *list) {
    assert(list != 0);

    free(list);
}

int List_isEmpty(List *list) {
    assert(list != NULL);

    if (list->size == 0) return 1;
    return 0;
}

void List_append(List *list, void *element) {
    assert(list != 0);

    List_Node *node;

    node = (List_Node *) malloc(sizeof(List_Node));
    node->content = element;
    node->next = list->front;
    list->front = node;
    (list->size)++;
}

void *List_pop(List *list) {
    assert(list != NULL);

    List_Node *node;
    void *content;

    if (List_isEmpty(list)) return NULL;

    node = list->front;
    list->front = list->front->next;
    (list->size)--;
    content = node->content;
    free(node);
    return content;
}

void *List_get(List *list, int index) {
    assert(list != NULL);

    List_Node *node;
    int i;

    if (list->size <= index) return NULL;
    node = list->front;
    for (i=0; i<index; i++) {
        node = node->next;
    }
    return node->content;
}