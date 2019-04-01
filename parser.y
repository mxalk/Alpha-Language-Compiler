%{ //introduction
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./Structs/SymTable.h"
#include "./Structs/Stack.h"
#define debug 1
#define errors_halt 0
#define printf(...) if(debug)printf(__VA_ARGS__);
int alpha_yyerror (const char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;
unsigned int arg_scope = 0;
unsigned int anon_funct_count = 0;
unsigned int isLamda = 0;
unsigned int in_loop = 0;
SymbolTableRecord* func_for_args = NULL;
SymbolTableRecord* dummy;
SymbolTableRecord* dummy2;

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
%union {  char* lvName;} 
%type <lvName> lvalue
%type <lvName> expr
%type <lvName> member


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
			|BREAK SEMI {if(!in_loop) alpha_yyerror("use of break outside loop"); printf("stmt ->  break SEMI \n");}
			|CONTINUE SEMI {if(!in_loop) alpha_yyerror("use of continue outside loop"); printf("stmt ->  continue SEMI\n");}
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
			|lvalue{
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal instruction++ on undefined variable \n");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal instruction++ on function  \n");
          				
				}
			} INCR {printf("term ->   lvalue ++ \n");}
			|INCR lvalue {printf("term ->  ++ lvalue\n");
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal ++instruction on undefined variable \n");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal ++instruction on function  \n");
          				
				}}
			|lvalue{
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal instruction-- on undefined variable \n");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal instruction-- on function  \n");
          				
				}
			} DECR {printf("term ->  lvalue -- \n");}
			|DECR lvalue {printf("term ->  -- lvalue \n");
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal --instruction on undefined variable ");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal --instruction on function  ");
          				
				}	
			}
			|primary {printf("term ->  primary\n");};

assignexpr:		lvalue ASSIGN expr {
				//dummy = dummy2;
				printf("assignexpr ->  lvalue = expr\n");
				dummy2 = NULL;
				if($1 == NULL || $3 == NULL || alpha_yylval.intValue){
						// alpha_yyerror("Error on assigment found\n");	
				}else{
					dummy = lookup($1,LCL,alpha_yylineno,1); //type(arg:#4) is not important when we are expecting this var
					if( dummy != NULL){
						if( dummy->type == USRFUNC || dummy->type == LIBFUNC ){
							printf("%s = %s at %d\n",$1,$3,alpha_yylineno);
							alpha_yyerror("Illegal assigment to function\n");
							exit(EXIT_FAILURE);
						}
					}else{
						dummy2 = lookup($3,LCL,alpha_yylineno,1);
						if(dummy2!=NULL){
							if(dummy2->type == USRFUNC){
								insert($1,dummy->type,getScope(),alpha_yylineno);
							}
						}
					}
				}
			};

primary:		lvalue	{printf("primary ->  lvalue\n");}
			|call {printf("primary ->  call\n");}
			|objectdef {printf("primary ->  objectdef\n");}
			|ANGL_O funcdef ANGL_C {printf("primary ->  ( funcdef )\n");isLamda = 1;}
			|const {printf("primary ->  const\n");};

lvalue:			ID {printf("lvalue -> ID \n") ; /*scope lookup and decide what type of var it is*/
				dummy=NULL;
				//char *buffer = (char*)malloc(30+strlen(alpha_yylval.stringValue));
				int sc = getScope();
				dummy=lookup(alpha_yylval.stringValue,(sc==0)?GLBL:LCL,alpha_yylineno,0);
				if(dummy==NULL){
					$$ = $1;
					if(sc){
						insert(alpha_yylval.stringValue,LCL,getScope(),alpha_yylineno);
					}else{ 
						insert(alpha_yylval.stringValue,GLBL,getScope(),alpha_yylineno);
					}
				}else{
					Scope* curr_scope = (Scope *)Stack_get(GSS, getScope());
					if(curr_scope->isFunction == 1 ){
						alpha_yyerror("cannot access variable");
					}
					//fprintf(stderr,"->>>>>>>>>>>>>>%s. %d\n",alpha_yylval.stringValue,getScope());
				}
				
			}
			|LOCAL ID {
				printf("lvalue ->  LOCAL ID\n");
				dummy=NULL;
				char *buffer = (char*)malloc(30+strlen(alpha_yylval.stringValue));
				int sc = getScope();
				dummy=lookup(alpha_yylval.stringValue,(sc==0)?GLBL:LCL,alpha_yylineno,0);
				if(dummy!=NULL){
					$$ = $2;
					if(dummy->scope == 0 && sc != 0 && dummy->type!=LIBFUNC) insert(alpha_yylval.stringValue,LCL,getScope(),alpha_yylineno);
					else if (dummy->type==LIBFUNC) {
						sprintf(buffer, "Function already defined as LIBFUNC \'%s\' line %u", alpha_yylval.stringValue, alpha_yylineno);
						alpha_yyerror(buffer);
					}
				}else{
					$$ = $2;
					if(sc){
						insert(alpha_yylval.stringValue,LCL,getScope(),alpha_yylineno);
					}else{ 
						insert(alpha_yylval.stringValue,GLBL,getScope(),alpha_yylineno);
					}
				}
			}|DCOLON ID {
					$$ = $2;
				printf("lvalue ->  DCOLON ID\n");
				if(lookupGlobal(alpha_yylval.stringValue,GLBL,alpha_yylineno,0)==NULL){
						char *buffer = (char*)malloc(30+strlen(alpha_yylval.stringValue));
						sprintf(buffer, "Global variable %s not defined \n",alpha_yylval.stringValue);
                				alpha_yyerror(buffer);
				}}
			|member {printf("lvalue ->  member\n");};

member: 	lvalue DOT ID {printf("member ->  lvalue . ID = %s\n", $3);}
			|lvalue BRAC_O expr BRAC_C  {printf("member ->  lvalue [ expr ]\n");}
			|call DOT ID {printf("member ->  call . ID = %s\n", $3);}
			|call BRAC_O expr BRAC_C {printf("member ->  call [ expr ]\n");};

call:			call ANGL_O elist ANGL_C {printf("call ->  call ( elist )\n");}
			|lvalue{
				
			} callsuffix {
				printf("call ->  lvalue callsuffix \n");}
			|ANGL_O funcdef ANGL_C ANGL_O elist ANGL_C {printf("call ->  ( funcdef ) ( elist )  \n");};

callsuffix:		normcall {printf("callsuffix ->  normcall\n");}
			|methodcall {printf("callsuffix ->  methodcall\n");};

normcall:		ANGL_O elist ANGL_C {printf("normcall ->  ( elist )\n");};

methodcall:		DOTDOT ID ANGL_O elist ANGL_C {printf("methodcall ->  .. ID=%s ( elist )\n",$2);};

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


funcdef:		FUNCTION {
				char funct_name[1024];
				char num[1024];
				strcpy(funct_name,"$f_anon");
				sprintf(num,"%d",anon_funct_count++);
				char* name = strdup(strcat(funct_name,num));
				func_for_args = insert(name,USRFUNC,getScope(),alpha_yylineno);
				
			} ANGL_O {increaseScope(1);} idlist ANGL_C CURL_O stmt_star CURL_C {decreaseScope();printf("funcdef ->  function ( idlist ) block \n");}
			|FUNCTION ID{
				dummy=NULL;
				char *buffer = (char*)malloc(30+strlen(alpha_yylval.stringValue));
				int sc = getScope();
				dummy=lookup(alpha_yylval.stringValue,(sc==0)?GLBL:LCL,alpha_yylineno,0);
				if(dummy!=NULL){
					if (dummy->type==LIBFUNC) {
						sprintf(buffer, "Function already defined as LIBFUNC \'%s\' line %u", alpha_yylval.stringValue, alpha_yylineno);
						alpha_yyerror(buffer);
					}else if(dummy->scope == 0 && getScope()>0){
						func_for_args = insert(alpha_yylval.stringValue,USRFUNC,getScope(),alpha_yylineno);

					}else{
						alpha_yyerror("on function declaration OR function is already defined ");
					}

				}else{
					func_for_args = insert(alpha_yylval.stringValue,USRFUNC,getScope(),alpha_yylineno);
				}
			} ANGL_O{increaseScope(1);} idlist  ANGL_C CURL_O stmt_star CURL_C 
			{
				decreaseScope();
				printf("funcdef ->  function ID ( idlist ) block\n");
					
			};
			
block:			CURL_O{increaseScope(0);}  stmt_star CURL_C{decreaseScope();}  {printf("block ->  { stmt_star }\n");};

const:			INTNUM {printf("const ->  INTNUM\n");}
			|REALNUM {printf("const ->  REALNUM\n");}
			|STRING {printf("const ->  STRING\n");}
			|NIL {printf("const ->  NIL\n");}
			|TRUE {printf("const ->  TRUE\n");}
			|FALSE {printf("const ->  FALSE\n");};

idlist:	ID{
	dummy = NULL;
	int sc = getScope();
	dummy=lookup(alpha_yylval.stringValue,FORMAL,alpha_yylineno,0);

	if(dummy==NULL){
		printf("-->%s\n",alpha_yylval.stringValue);
		insert(alpha_yylval.stringValue,FORMAL,getScope(),alpha_yylineno);
	}
	else{
		if(dummy->type==LIBFUNC){
			alpha_yyerror("Can't shadow a LIBFUNC");
		}
		else if(dummy->scope == 0 && getScope()>0){
			insert(alpha_yylval.stringValue,FORMAL,getScope(),alpha_yylineno);
		}else{
			printf("===> %s\n",alpha_yylval.stringValue);
			alpha_yyerror("on argument declaration OR arg is already defined");
		}
	}
} ids {
	printf("idlist ->  ID ids\n");
	}
	| {printf("idlist ->  nothing\n");};

ids: COMMA ID{
		dummy = NULL;
	int sc = getScope();
	dummy=lookup(alpha_yylval.stringValue,FORMAL,alpha_yylineno,0);

	if(dummy==NULL){
		printf("-->%s\n",alpha_yylval.stringValue);
		insert(alpha_yylval.stringValue,FORMAL,getScope(),alpha_yylineno);
	}
	else{
		if(dummy->type==LIBFUNC){
			alpha_yyerror("Can't shadow a LIBFUNC");
		}
		else if(dummy->scope == 0){
			insert(alpha_yylval.stringValue,FORMAL,getScope(),alpha_yylineno);
		}else{
			printf("===> %s\n",alpha_yylval.stringValue);
			alpha_yyerror("on argument declaration");
		}
	}
	} ids {
		printf("ids ->  , ID ids\n");
	}
		| {printf("ids ->  nothing\n");};

ifstmt:			IF ANGL_O expr ANGL_C stmt {printf("ifstmt ->  if ( expr ) stmt \n");}
            |IF ANGL_O expr ANGL_C stmt ELSE stmt {printf("ifstmt ->  if ( expr ) stmt else stmt \n");};

whilestmt:		WHILE ANGL_O expr ANGL_C {in_loop++;}
						  stmt {printf("whilestmt ->  while ( expr ) stmt \n"); in_loop--;};

forstmt:		FOR ANGL_O elist SEMI expr SEMI elist ANGL_C {in_loop++;}
						stmt {printf("forstmt ->  for ( elist ; expr ; elist ) stmt \n"); in_loop--;};

returnstmt:		RETURN SEMI {printf("returnstmt ->  return ; \n");} |RETURN expr SEMI {printf("returnstmt ->  return expr ; \n");};

%%

int alpha_yyerror (const char* yaccProvidedMessage){
  fprintf(stderr,"\033[0;31mError %s, line %u \033[0m\n",yaccProvidedMessage,alpha_yylineno);
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

		//printGSS();
    display();
    return 0;
}