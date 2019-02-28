#include "Stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>


Stack *Stack_init() {
    Stack *stack;

    stack = (Stack *)malloc(sizeof(Stack));
    stack->top = NULL;
    stack->size = 0;

    return stack;
}

void Stack_destroy(Stack *stack) {
    assert(stack != 0);

    free(stack);
}

int Stack_isEmpty(Stack *stack) {
    assert(stack != NULL);

    if (stack->size == 0) return 1;
    return 0;
}

void Stack_append(Stack *stack, void *element) {
    assert(stack != 0);

    Stack_Node *node;

    node = (Stack_Node *) malloc(sizeof(Stack_Node));
    node->content = element;
    node->next = stack->top;
    stack->top = node;
    (stack->size)++;
}

void *Stack_pop(Stack *stack) {
    assert(stack != NULL);

    Stack_Node *node;
    void *content;

    if (Stack_isEmpty(stack)) return NULL;

    node = stack->top;
    stack->top = stack->top->next;
    (stack->size)--;
    content = node->content;
    free(node);
    return content;
}

void *Stack_get(Stack *stack, int index) {
    assert(stack != NULL);

    Stack_Node *node;
    int i;

    if (stack->size <= index) return NULL;
    node = stack->top;
    for (i=0; i<index; i++) {
        node = node->next;
    }
    return node->content;
}