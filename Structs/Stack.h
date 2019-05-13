#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
typedef struct Stack_Node Stack_Node;
typedef struct Stack Stack;

struct Stack_Node{
        void *content;
        Stack_Node *next;
};

struct Stack{
        Stack_Node *top;
        unsigned int size;
};


Stack *Stack_init();

void Stack_destroy(Stack *stack);

int Stack_isEmpty(Stack *stack);

void Stack_append(Stack *stack, void *element);

void *Stack_pop(Stack *stack);

void *Stack_get(Stack *stack, int index);