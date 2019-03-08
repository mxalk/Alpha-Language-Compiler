%{ //introduction
#include <stdio.h>

int yyerror (char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;

%}

%error-verbose
%name-prefix="alpha_yy"
%start program

%token OTHER LINECOMM BLOCKCOMM IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL 
%token CURL_O CURL_C BRAC_O BRAC_C ANGL_O ANGL_C SEMI COMMA COLON DCOLON DOT DOTDOT 
%token ASSIGN PLUS MINUS MUL SLASH PERC EQUALS NEQUALS INCR DECR GREATER LESS GREATER_E LESS_E

%left ADD  SUB 
%left MUL  DIV 
%right EXP 
%nonassoc EQ

%%

program:        ;
%%

int main (int argc, char** argv) {
    if (argc > 1) {
      if (!(alpha_yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot read file: %s\n",argv[1]);
        return 1;
      }
    }
    else alpha_yyin= stdin;
    yyparse();
    return 0;
}