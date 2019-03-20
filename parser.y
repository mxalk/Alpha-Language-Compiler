%{ //introduction
#include <stdio.h>
#include <stdlib.h>
int alpha_yyerror (const char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;

%}
/*%define api.prefix {alpha_yy}*/
%name-prefix="alpha_yy"
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
%token ASSIGN PLUS MINUS MUL DIV PERC EQUALS NEQUALS INCR DECR GREATER LESS GREATER_E LESS_E

%left  ANGL_O ANGL_C
%left  BRAC_O BRAC_C
%left  DOT DOTDOT
%right NOT INCR DECR UMINUS 
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

/* Den eimai sigouros an xreiazontai ta semicolons sto telos ka8e kanona */
/* opou exei [ ] *  einai regular expression leei alla dn to pianei */

program: start ;

start: stmt start { printf("start -> stmt start\n");}| ;

stmt: expr { printf("stmt -> expr\n");}
    | ifstmt { printf("stmt -> ifstmt\n");}
    | whilestmt { printf("stmt -> whilestmt\n");}
    | forstmt { printf("stmt -> forstmt\n");}
    | returnstmt { printf("stmt -> return stmt\n");}
    | BREAK { printf("stmt -> BREAK\n");}
    | CONTINUE { printf("stmt -> CONTINUE\n");}
    | block { printf("stmt -> block\n");}
    | funcdef { printf("stmt -> funcdef\n");}
    | { printf("stmt -> end\n");};

expr: assignexpr { printf("expr -> assignexpr\n");}
    | expr op expr { printf("expr -> expr op expr\n");}
    | term { printf("expr -> term\n");};
    
op: PLUS | MINUS | MUL | DIV | PERC | LESS | LESS_E | GREATER | GREATER_E | EQUALS | NEQUALS | AND | OR ;

term: ANGL_O expr ANGL_C  { printf("term -> ( expr )\n");}
    | UMINUS expr { printf("term -> -expr\n");}
    | NOT expr { printf("term -> NOT expr \n");}
    | INCR lvalue { printf("term -> ++expr \n");}
    | lvalue INCR { printf("term -> expr++\n");}
    | DECR lvalue { printf("term -> --expr \n");}
    | lvalue DECR { printf("term ->  expr--\n");}
    | primary { printf("term -> primary\n");};

assignexpr: lvalue ASSIGN expr { printf("assignexpr -> lvalue ASSIGN expr\n");};

primary: lvalue { printf("primary -> lvalue\n");}
       | call { printf("primary -> call\n");}
       | objectdef { printf("primary -> objectdef\n");}
       | ANGL_O funcdef ANGL_C { printf("primary -> ( funcdef )\n");}
       | const { printf("primary -> const\n");};
    
lvalue: ID { printf("lvalue -> ID %s\n",$1);}
      | LOCAL ID { printf("lvalue -> local id %s\n" , $2);}
      | DCOLON ID { printf("lvalue -> dcolon id %s\n" , $2);}
      | member { printf("lvalue -> member\n");}; 

member: lvalue DOT ID { printf("member -> lvalue dot id %s\n" , $3);}
      | lvalue BRAC_O expr BRAC_C { printf("member -> lvalue [ expr ] \n");}
      | call DOT ID { printf("member -> call DOT ID %s" , $3);}
      | call BRAC_O expr BRAC_C { printf("member -> [ expr ]\n");};

call: call ANGL_O elist ANGL_C { printf("call -> ( elist )\n");}
    | lvalue callsuffix { printf("call  -> callsuffix \n");}
    | ANGL_O funcdef ANGL_C ANGL_O elist ANGL_C { printf("call -> ( funcdef ) ( elist )\n");};

callsuffix: normcall { printf("callsuffix -> normcall\n");}
          | methodcall { printf("callsuffix -> methodcall\n");};

normcall: ANGL_O elist ANGL_C { printf("normcall -> ( elist )\n");};

methodcall: DOTDOT ID ANGL_O elist ANGL_C { printf("methodcall -> DOTDOT ID %s ( elist )\n" , $2);}; 

elist:  expr elist_comm { printf("elist -> elist elist_comm\n");}| ;
elist_comm: COMMA expr elist_comm { printf("elist_comm -> COMMA expr elist_comm \n");}| ; 

objectdef: BRAC_O objectdef_or BRAC_C { printf("objectdef -> [ objectdef_or ]\n");} ;
objectdef_or: elist { printf("objectdef_or -> elist\n");}| indexed{ printf("objectdef_or -> indexed\n");} | { printf("objectdef_or -> empty\n");};

indexed: indexedelem indexed_comm { printf("indexed -> indexedelem indexed_comm\n");}| { printf("indexed -> empty\n");};
indexed_comm: COMMA indexedelem indexed_comm { printf("indexed_comm -> COMMA indexedelem indexed_comm\n");}| { printf("indexed_comm -> empty\n");};

indexedelem: CURL_O expr COLON expr CURL_C { printf("indexedelem -> { expr:expr }\n");};

block: CURL_O start CURL_C { printf("block -> {start}\n");};

funcdef: FUNCTION if_id ANGL_O idlist ANGL_C block { printf("funcdef -> FUNCTION [ID] ( idlist ) block \n");};

if_id: ID { printf("if_id -> ID %s\n",$1);}| { printf("if_id -> empty\n");};

const: INTNUM | REALNUM | STRING | NIL | TRUE | FALSE ;

idlist: ID idlist_comm { printf("idlist -> ID idlist %s\n",$1);}| { printf("idlist -> empty\n");};

idlist_comm: COMMA ID idlist_comm { printf("idlist_comm -> COMMA ID idlist_comm %s" , $2);}| { printf("idlist_comm -> empty\n");};

ifstmt: IF ANGL_O expr ANGL_C stmt else_stmt { printf("ifstmt -> IF (expr) stmt else_stmt\n");};

else_stmt: ELSE stmt { printf("else_stmt -> ELSE stmt\n");} | { printf("else_stmt -> empty\n");};

whilestmt: WHILE ANGL_O expr ANGL_C stmt { printf("whilestmt -> WHILE ( expr ) stmt \n");};

forstmt: FOR ANGL_O elist SEMI expr SEMI elist ANGL_C stmt { printf("forstmt -> FOR ( elist; expr ; elist) stmt\n");};

returnstmt: RETURN expr { printf("returnstmt -> RETURN expr\n");}| RETURN { printf("returnstmt -> RETURN empty\n");};


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