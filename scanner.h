#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./Structs/Stack.h"
#include "./Structs/Queue.h"

int token_id = 1;
const char *typeNames[] = {"KEYWORD", "ID", "STRING", "LINECOMMENT", "INTCONST", "DOUBLECONST", "PUNCTUATION", "OPERATOR", "BLOCKCOMMENT"};

typedef enum alpha_token_type_t {
        keyword,
        id,
        string,
        linecomm,
        intconst,
        doubleconst,
        punct,
        oper,
        blockcomm
} alpha_token_type_t;

typedef struct alpha_token_t
{
        alpha_token_type_t type;
        char *value;
        unsigned int token_num;
        unsigned int lineno;

} alpha_token_t;

typedef struct comment_node {
        int line_open, line_close, isBlock;
} comment_node;

alpha_token_t *new_token(Queue *q, unsigned int yylineno, char *yytext, alpha_token_type_t type) {
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
        token->token_num = token_id++;
        Queue_enqueue(q, (void *)token);
}

void print_queue(Queue *q) {
        Queue_Node *n = q->head;
        alpha_token_t *t;
        printf("\n");
        printf("|%4s|%5s|%50s|%15s|\n", "LINE", "ID#", "VALUE", "IDENTIFIER");
        printf("\n");
        while (n != NULL)
        {
                t = n->content;
                printf("|%4d|%5d|%50s|%15s|\n", t->lineno, t->token_num, t->value, typeNames[t->type]);
                n = n->next;
        }
        printf("\n");
}