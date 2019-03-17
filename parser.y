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
%token ASSIGN ADD SUB MUL DIV PERC EQUALS NEQUALS INCR DECR GREATER LESS GREATER_E LESS_E

%left  ANGL_O ANGL_C
%left  BRAC_O BRAC_C
%left  DOT DOTDOT
%right NOT INCR DECR UMINUS 
%left  MUL DIV PERC 
%left  ADD SUB
%nonassoc GREATER LE GREATER_E LESS LESS_E
%nonassoc EQUALS NEQUALS
%left  AND
%left  OR
%right ASSIGN

%right EXP


%destructor { free($$);  }ID
%%

/* Den eimai sigouros an xreiazontai ta semicolons sto telos ka8e kanona */
/* opou exei [ ] *  einai regular expression leei alla dn to pianei */

program: stmt* ;

stmt: expr ;
    | ifstmt
    | ifstmt
    | whilestmt
    | forstmt
    | returnstmt
    | BREAK ;
    | CONTINUE ;
    | block
    | funcdef
    | ;

expr: assignexpr
    | expr op expr
    | term ;
    
op: ADD | SUB | MUL | DIV | PERC | LESS | LESS_E | GREATER | GREATER_E | EQUALS | NEQUALS | AND | OR ;

term: ANGL_O expr ANGL_C  
    | UMINUS expr
    | NOT expr
    | INCR lvalue 
    | lvalue INCR
    | DECR lvalue
    | lvalue DECR
    | primary ;

assginexpr: lvalue ASSIGN expr;

primary: lvalue
       | call
       | objectdef
       | ANGL_O funcdef ANGL_C
       | const ;
    
lvalue: ID
      | LOCAL ID
      | DCOLON ID
      | member ;

member: lvalue DOT ID
      | lvalue BRAC_O expr BRAC_C
      | call DOT ID
      | call BRAC_O expr BRAC_C ;

call: call ANGL_O elist ANGL_C
    | lvalue callsuffix
    | ANGL_O funcdef ANGL_C ANGL_O elist ANGL_C ;

callsuffix: normcall
          | methodcall ;

normcall: ANGL_O elist ANGL_C ;

methodcall: DOTDOT ID ANGL_O elist ANGL_C ;

elist: [ expr [COMMA expr] * ] ; 

objectdef: BRAC_O [elist | indexed] BRAC_C ;

indexed: [indexedelem [COMMA indexedelem] * ] ;

indexedelem: CURL_O expr COLON expr CURL_C ;

block: CURL_O [ stmt* ] CURL_C ;

funcdef: FUNCTION [ID] ANGL_O idlist ANGL_C block ;

const: INTNUM | REALNUM | STRING | NIL | TRUE | FALSE ;

idlist: [ID [COMMA ID] * ] ;

ifstmt: IF ANGL_O expr ANGL_C stmt [ ELSE stmt ] ;

whilestmt: WHILE ANGL_O expr ANGL_C stmt ;

forstmt: FOR ANGL_O elist SEMI expr SEMI elist ANGL_C stmt ;

returnstmt: RETURN [expr] ;


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