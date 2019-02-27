#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./Structs/Stack.h"
#include "./Structs/Queue.h"

int token_id = 1;
const char *categoryNames[] = {"KEYWORD", "ID", "STRING", "COMMENT", "INTCONST", "FLOATCONST", "PUNCTUATION", "OPERATOR"};

typedef enum alpha_token_category_t {
        keyword,
        id,
        string,
        comment,
        intconst,
        floatconst,
        punct,
        oper
} alpha_token_category_t;

const char *typeNames[] = {
"OTHER", "LINECOMM", "BLOCKCOMM", "IF","ELSE","WHILE","FOR","FUNCTION","RETURN","BREAK","CONTINUE","AND","NOT","OR","LOCAL","TRUE","FALSE","NIL",
"CURL_O","CURL_C","BRAC_O","BRAC_C","ANGL_O","ANGL_C","SEMI","COMMA","COLON","DCOLON","DOT","DOTDOT",
"ASSIGN", "PLUS","MINUS","MUL","SLASH","PERC","EQUALS","NEQUALS","INCR","DECR","GREATER","LESS","GREATER_E","LESS_E"
};

typedef enum alpha_token_type_t {
OTHER,LINECOMM,BLOCKCOMM,IF,ELSE,WHILE,FOR,FUNCTION,RETURN,BREAK,CONTINUE,AND,NOT,OR,LOCAL,TRUE,FALSE,NIL,
CURL_O,CURL_C,BRAC_O,BRAC_C,ANGL_O,ANGL_C,SEMI,COMMA,COLON,DCOLON,DOT,DOTDOT,
ASSIGN, PLUS,MINUS,MUL,SLASH,PERC,EQUALS,NEQUALS,INCR,DECR,GREATER,LESS,GREATER_E,LESS_E
}alpha_token_type_t;

typedef struct alpha_token_t
{
        char *value;
        unsigned int token_num;
        unsigned int lineno;
        alpha_token_category_t category;
        alpha_token_type_t type;
        void *data;

} alpha_token_t;

typedef struct comment_node {
        int line_open, line_close, isBlock;
} comment_node;

alpha_token_t *new_token(Queue *q, unsigned int yylineno, char *yytext, alpha_token_category_t category, alpha_token_type_t type) {
        alpha_token_t *token;
        if ((token = (alpha_token_t *)malloc(sizeof(alpha_token_t))) == NULL)
        {
                printf("Token malloc error\n");
                exit(EXIT_FAILURE);
        }
        if ((token->value = (char *)malloc(yyleng + 1)) == NULL)
        {
                printf("Token value malloc error\n");
                exit(EXIT_FAILURE);
        }
        strcpy(token->value, yytext);
        token->lineno = yylineno;
        token->token_num = token_id++;
        token->category = category;
        printf("%d ", type);
        if (type != OTHER) {
                token->type = type;
        } else if (category == intconst) {
                token->data = malloc(sizeof(int));
                *((int *)token->data) = atoi(token->value);
        } else if (category == floatconst) {
                token->data = malloc(sizeof(float));
                *((float *)token->data) = atof(token->value);
        } else if (category == id || category == string) {
                token->data = malloc(sizeof(strlen(token->value)+1));
                strcpy(token->data, token->value);
        }
        Queue_enqueue(q, (void *)token);
}

void print_queue(Queue *q) {
        Queue_Node *n = q->head;
        alpha_token_t *t;
        printf("\n");
        printf("|%4s|%5s|%30s|%15s|\n", "LINE", "ID#", "VALUE", "CATEGORY", "TYPE");
        printf("\n");
        while (n != NULL)
        {
                t = n->content;
                printf("|%4d|%5d|%30s|%15s|", t->lineno, t->token_num, t->value, categoryNames[t->category]);
                if (t->type != 0) {
                        printf("%30s|\n", typeNames[t->type]);
                } else if (t->category == intconst) {
                        printf("%30d|\n", (int) *((int *)t->data));
                } else if (t->category == floatconst) {
                        printf("%30f|\n", (float) *((float *)t->data));
                } else if (t->category == id || t->category == string) {
                        printf("%30s|\n", (char *) t->data);
                } else {
                        printf("\n");
                }
                n = n->next;
        }
        printf("\n");
}