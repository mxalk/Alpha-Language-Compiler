#include "Queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


Queue *Queue_init() {
    Queue *queue;

    queue = (Queue *)malloc(sizeof(Queue));
    queue->head = NULL;
    queue->tail = NULL;
    queue->size = 0;

    return queue;
}

// Does not free node->content !!
void Queue_destroy(Queue *queue) {
    assert(queue != NULL);

    Queue_Node *node = queue->head;
    Queue_Node *next;
    while(node != NULL) {
        next = node->next;
        free(node);
        node = next;
    }
    free(queue);
}

int Queue_isEmpty(Queue *queue) {
    assert(queue != NULL);

    if (queue->size == 0) return 1;
    return 0;
}

void Queue_enqueue(Queue *queue, void *element) {
    assert(queue != NULL);

    Queue_Node *node;

    node = (Queue_Node *) malloc(sizeof(Queue_Node));
    node->content = element;
    node->next = NULL;
    if (queue->size == 0) {
        queue->head = node;
    } else queue->tail->next = node;
    queue->tail = node;
    (queue->size)++;
}

void *Queue_dequeue(Queue *queue) {
    assert(queue != NULL);

    Queue_Node *node;
    void *content;

    if (Queue_isEmpty(queue)) return NULL;
    node = queue->head;
    if (queue->size == 1) {
        queue->head = NULL;
        queue->tail = NULL;
    } else queue->head = queue->head->next;
    (queue->size)--;
    content = node->content;
    if (content == NULL) fprintf(stderr, "content is NULL\n");
    free(node);
    return content;
}

void *Queue_get(Queue *queue, int index) {
    assert(queue != NULL);

    Queue_Node *node;
    int i;

    if (queue->size <= index) return NULL;
    node = queue->head;
    for (i=0; i<index; i++) {
        node = node->next;
    }
    return node->content;
}