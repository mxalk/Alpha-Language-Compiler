%{ //introduction
#include <stdio.h>
#include <stdlib.h>
int alpha_yyerror (const char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;

%}
%define api.prefix {alpha_yy}
%error-verbose
%start program

%union{
  char* stringValue;
  int intValue;
  float floatValue;
}

%token <stringValue> ID
%token <intValue> INTNUM
%token <floatValue> REALNUM
%token <stringValue> STRING
%token LINECOMM BLOCKCOMM IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL 
%token CURL_O CURL_C BRAC_O BRAC_C ANGL_O ANGL_C SEMI COMMA COLON DCOLON DOT DOTDOT 
%token ASSIGN PLUS MINUS MUL SLASH PERC EQUALS NEQUALS INCR DECR GREATER LESS GREATER_E LESS_E

%left  ANGL_O ANGL_C
%left  BRAC_O BRAC_C
%left  DOT DOTDOT
%right NOT INCR DECR UMINUS // check again for minus
%left  MUL DIV PERC 
%left  PLUS MINUS 
%nonassoc GREATER LE GREATER_E LESS LESS_E
%nonassoc EQUALS NEQUALS
%left  AND
%left  OR
%right ASSIGN

%right EXP


%destructor { free($$);  }ID
%%

program: stmt* ;

stmt: expr ;
    | ifstmt
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | break ;
    | continue ;
    | block
    | funcdef
    | ;

expr: assignexpr
    | expr op expr
    | term ;
    
op: PLUS | - | * | / | % | > | >= | < | <= | == | != | and | or
term  ( expr )
| - expr
| not expr

%%
int alpha_yyerror (const char* yaccProvidedMessage){
  fprintf(stderr,"\033[0;31m Error %s \033[0m\n",yaccProvidedMessage);
  exit(EXIT_FAILURE);
}


int main (int argc, char** argv) {
    if (argc > 1) {
      if (!(alpha_yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot read file: %s\n",argv[1]);
        yyerror("");
        return 1;
      }
    }
    else alpha_yyin= stdin;
    yyparse();
    return 0;
}