%{ //introduction
#include <stdio.h>
#include <stdlib.h>
#include "./Structs/SymTable.h"
int alpha_yyerror (const char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;
unsigned int arg_scope = 0;
%}
%define api.prefix {alpha_yy}
// %name-prefix="alpha_yy"
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



program:	stmt_star {printf("program ->  statements\n");};

stmt: 		expr SEMI {printf("stmt ->  expr SEMI\n");}
      |ifstmt {printf("stmt ->  ifstmst\n");}
      |whilestmt {printf("stmt ->  whilestmt\n");}
			|forstmt {printf("stmt ->  forstmt\n");}
			|returnstmt {printf("stmt ->  returnstmt\n");}
			|BREAK SEMI {printf("stmt ->  break SEMI \n");}
			|CONTINUE SEMI {printf("stmt ->  continue SEMI\n");}
			|block {printf("stmt ->  block\n");}
			|funcdef {printf("stmt ->  funcdef\n");}
			|SEMI {printf("stmt ->  SEMI\n");};

stmt_star:			stmt stmt_star {printf("stmt_star ->  stmt and stmt_star\n");}
			| {printf("stmt_star ->  nothing\n");};

expr:			assignexpr  {printf("expr ->  assignexpr\n");}
			|expr PLUS expr {printf("expr ->  expr + expr\n");}
			|expr MINUS expr {printf("expr ->  expr - expr\n");}
			|expr MUL expr {printf("expr ->  expr * expr\n");}
			|expr DIV expr {printf("expr ->  expr / expr\n");}
			|expr PERC expr {printf("expr ->  expr PERC expr\n");}
			|expr GREATER expr {printf("expr ->  expr > expr\n");}
			|expr GREATER_E expr {printf("expr ->  expr >= expr\n");}
			|expr LESS expr {printf("expr ->  expr < expr\n");}
			|expr LESS_E expr {printf("expr ->  expr <= expr\n");}
			|expr EQUALS expr {printf("expr ->  expr == expr\n");}
			|expr NEQUALS expr {printf("expr ->  expr != expr\n");}
			|expr AND expr {printf("expr ->  expr and expr\n");}
			|expr OR expr {printf("expr ->  expr or expr\n");}
			|term {printf("expr ->  term\n");};

term:	 ANGL_O expr ANGL_C {printf("term ->  ( expr )\n");}
			|MINUS expr {printf("term ->  - expr \n");}
			|NOT expr {printf("term ->  not expr \n");}
			|lvalue INCR {printf("term ->   lvalue ++ \n");}
			|INCR lvalue {printf("term ->  ++ lvalue\n");}
			|lvalue DECR {printf("term ->  lvalue -- \n");}
			|DECR lvalue {printf("term ->  -- lvalue \n");}
			|primary {printf("term ->  primary\n");};

assignexpr:		lvalue ASSIGN expr {printf("assignexpr ->  lvalue = expr\n");
			printf("funcdef ->  function ID ( idlist ) block\n");
					if(lookup($1,LCL,alpha_yylineno,0)==NULL)
						insert($1,LCL,getScope(),alpha_yylineno);};

primary:		lvalue	{printf("primary ->  lvalue\n");}
			|call {printf("primary ->  call\n");}
			|objectdef {printf("primary ->  objectdef\n");}
			|ANGL_O funcdef ANGL_C {printf("primary ->  ( funcdef )\n");}
			|const {printf("primary ->  const\n");};

lvalue:			ID {printf("lvalue -> ID \n") ; $$ = $1}
			|LOCAL ID {printf("lvalue ->  LOCAL ID\n");}
			|DCOLON ID {printf("lvalue ->  DSCOPE ID\n");}
			|member {printf("lvalue ->  member\n");};

member: 	lvalue DOT ID {printf("member ->  lvalue . ID\n");}
			|lvalue BRAC_O expr BRAC_C  {printf("member ->  lvalue [ expr ]\n");}
			|call DOT ID {printf("member ->  calL . ID\n");}
			|call BRAC_O expr BRAC_C {printf("member ->  call [ expr ]\n");};

call:			call ANGL_O elist ANGL_C {printf("call ->  call ( elist )\n");}
			|lvalue callsuffix {
				printf("call ->  lvalue callsuffix \n");}
			|ANGL_O funcdef ANGL_C ANGL_O elist ANGL_C {printf("call ->  ( funcdef ) ( elist )  \n");};

callsuffix:		normcall {printf("callsuffix ->  normcall\n");}
			|methodcall {printf("callsuffix ->  methodcall\n");};

normcall:		ANGL_O elist ANGL_C {printf("normcall ->  ( elist )\n");};

methodcall:		DOTDOT ID ANGL_O elist ANGL_C {printf("methodcall ->  .. ID ( elist )\n");};

elist:		expr exprs {printf("elist ->  expr exprs\n");}
			|	{printf("elist ->  nothing\n");};

exprs:			COMMA expr exprs {printf("exprs ->  , expr exprs\n");}
			| {printf("exprs ->  nothing\n");};

objectdef:		BRAC_O elist BRAC_C  {printf("objectdef ->  [ elist ]\n");}
			|BRAC_O indexed BRAC_C  {printf("objectdef ->  [ indexed ]\n");};

indexed:		indexedelem indexedelem_comm {printf("indexed ->  indexedelem indexedelem_comm\n");};

indexedelem:		CURL_O expr COLON expr CURL_C {printf("indexedelem ->  { expr : expr }\n");};

indexedelem_comm:		COMMA indexedelem indexedelem_comm {printf("indexedelem_comm ->  , indexedelem indexedelem_comm\n");}
			| {printf("indexedelem_comm ->  nothing\n");};


funcdef:		FUNCTION ANGL_O {increaseScope();} idlist ANGL_C CURL_O stmt_star CURL_C {decreaseScope();printf("funcdef ->  function ( idlist ) block \n");}
			|FUNCTION ID ANGL_O{increaseScope();} idlist  ANGL_C CURL_O stmt_star CURL_C 
			{
				decreaseScope();
				printf("funcdef ->  function ID ( idlist ) block\n");
					if(lookup($2,USRFUNC,alpha_yylineno,0)==NULL)
						insert($2,USRFUNC,getScope(),alpha_yylineno);
			};
			
block:			CURL_O{increaseScope();}  stmt_star CURL_C{decreaseScope();}  {printf("block ->  { stmt_star }\n");};

const:			INTNUM {printf("const ->  INTNUM\n");}
			|REALNUM {printf("const ->  REALNUM\n");}
			|STRING {printf("const ->  STRING\n");}
			|NIL {printf("const ->  NIL\n");}
			|TRUE {printf("const ->  TRUE\n");}
			|FALSE {printf("const ->  FALSE\n");};

idlist:	ID ids {printf("idlist ->  ID ids\n");
								if(lookup($1,FORMAL,alpha_yylineno,0)!=NULL)
									insert($1,LCL,getScope(),alpha_yylineno);}
			| {printf("idlist ->  nothing\n");};

ids: COMMA ID ids {printf("ids ->  , ID ids\n");
									 if(lookup($2,FORMAL,alpha_yylineno,0)!=NULL)
									 	insert($2,LCL,getScope(),alpha_yylineno);}
			| {printf("ids ->  nothing\n");};

ifstmt:			IF ANGL_O expr ANGL_C stmt {printf("ifstmt ->  if ( expr ) stmt \n");}
            |IF ANGL_O expr ANGL_C stmt ELSE stmt {printf("ifstmt ->  if ( expr ) stmt else stmt \n");};

whilestmt:		WHILE ANGL_O expr ANGL_C stmt {printf("whilestmt ->  while ( expr ) stmt \n");};

forstmt:		FOR ANGL_O elist SEMI expr SEMI elist ANGL_C stmt {printf("forstmt ->  for ( elist ; expr ; elist ) stmt \n");};

returnstmt:		RETURN SEMI {printf("returnstmt ->  return ; \n");} |RETURN expr SEMI {printf("returnstmt ->  return expr ; \n");};

%%

int alpha_yyerror (const char* yaccProvidedMessage){
  fprintf(stderr,"\033[0;31m Error %s %u \033[0m\n",yaccProvidedMessage,alpha_yylineno);
  exit(EXIT_FAILURE);
}


int main (int argc, char** argv) {
    sym_init();
    if (argc > 1) {
      if (!(alpha_yyin = fopen(argv[1], "r"))) {
        fprintf(stderr, "Cannot read file: %s\n",argv[1]);
        yyerror("");
        return 1;
      }
    }
    else alpha_yyin= stdin;
    yyparse();

    display();
    return 0;
}