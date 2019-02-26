#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define LEX_Q lex_global_q
#define LEX_ST lex_global_stack

enum alpha_token_type_t;
struct alpha_token_t;
struct t_queue;
struct node;
struct t_queue *lex_global_q;
struct alpha_stack *lex_global_stack;
void t_enqueue(struct t_queue *q, struct alpha_token_t *t);
struct alpha_token_t *t_dequeue(struct t_queue *q);
void print_queue(struct t_queue *q);

const char *typeNames[] = {"KEYWORD", "ID", "STRING", "COMMENT", "INTCONST", "DOUBLECONST", "PUNCTUATION", "OPERATOR", "NESTEDCOMMENT"};

typedef struct alpha_stack
{
        unsigned int capacity;
        struct alpha_token_t **container;
        int top;
} alpha_stack;

void initStack()
{
        LEX_ST = (alpha_stack *)malloc(sizeof(alpha_stack));
        LEX_ST->capacity = 10;
        LEX_ST->container = (struct alpha_token_t **)malloc(sizeof(alpha_stack *) * 10);//memory chunks of 10
        LEX_ST->top = -1;
}
int s_isEmpty()
{
        return (LEX_ST->top == -1) ? 1 : 0;
}
int s_isFull()
{
        return (LEX_ST->capacity == 0)? 1 : 0;
}
void push_back(struct alpha_token_t *token)
{
        if (s_isFull()) 
        {
              LEX_ST->container = (struct alpha_token_t**)realloc(LEX_ST->container,sizeof(LEX_ST->container)+10);
              LEX_ST->capacity += 10;  
        }
        LEX_ST->capacity--;
        LEX_ST->container[++LEX_ST->top] = token; 
}

typedef enum alpha_token_type_t
{
        keyword,
        id,
        string,
        comment,
        intconst,
        doubleconst,
        punct,
        oper,
        nestcomm
} alpha_token_type_t;

typedef struct alpha_token_t
{
        alpha_token_type_t type;
        char *value;
        unsigned int token_num;
        unsigned int lineno;

} alpha_token_t;

typedef struct node
{
        alpha_token_t *token;
        struct node *next;
} node;

typedef struct t_queue
{
        unsigned int token_counter;
        node *head;
        node *tail;
} t_queue;

t_queue *queue_init()
{
        t_queue *q;
        if ((q = (t_queue *)malloc(sizeof(t_queue))) == NULL)
        {
                printf("Token queue malloc error\n");
                exit(EXIT_FAILURE);
        }
        q->head = NULL;
        q->tail = NULL;
        q->token_counter = 0;
        return q;
}

void new_token(t_queue *q, unsigned int yylineno, char *yytext, alpha_token_type_t type)
{
        alpha_token_t *token;
        if ((token = (alpha_token_t *)malloc(sizeof(alpha_token_t))) == NULL)
        {
                printf("Token malloc error\n");
                exit(EXIT_FAILURE);
        }
        token->type = type;
        if ((token->value = (char *)malloc(yyleng + 1)) == NULL)
        {
                printf("Token value malloc error\n");
                exit(EXIT_FAILURE);
        }
        strcpy(token->value, yytext);
        token->lineno = yylineno;
        token->token_num = q->token_counter++;
        t_enqueue(q, token);
}

void t_enqueue(t_queue *q, alpha_token_t *t)
{

        node *new_node;
        if ((new_node = (node *)malloc(sizeof(node))) == NULL)
        {
                printf("Token node malloc error\n");
                exit(EXIT_FAILURE);
        }

        new_node->token = t;
        new_node->next = NULL;

        if (q->tail == NULL)
        {
                q->head = new_node;
                q->tail = new_node;
        }
        else
        {
                q->tail->next = new_node;
                q->tail = new_node;
        }
}

alpha_token_t *t_dequeue(t_queue *q)
{
        node *head = q->head;
        if (head == NULL)
        {
                q->tail = NULL;
                return NULL;
        }
        alpha_token_t *t = head->token;
        q->head = head->next;
        free(head);
        return t;
}

void print_queue(t_queue *q)
{
        node *n = q->head;
        alpha_token_t *t;
        printf("\n");
        printf("|%4s|%5s|%50s|%15s|\n", "LINE", "ID#", "VALUE", "IDENTIFIER");
        printf("\n");
        while (n != NULL)
        {
                t = n->token;
                printf("|%4d|%5d|%50s|%15s|\n", t->lineno, t->token_num, t->value, typeNames[t->type]);
                n = n->next;
        }
        printf("\n");
}