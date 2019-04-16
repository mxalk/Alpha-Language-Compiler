%{ //introduction
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./Structs/Stack.h"
#include "./Structs/Quad.h"
#define debug 1
#define errors_halt 1
#define exit(x) if(errors_halt)exit(x)
#define printf(...) if(debug)printf(__VA_ARGS__);
int alpha_yyerror (const char* yaccProvidedMessage);
int alpha_yylex(void);
extern int alpha_yylineno;
extern char* alpha_yytext;
extern FILE* alpha_yyin;
Queue* global_elist;
Queue* global_indexed_q;
Queue* curr_elist = NULL;
Queue* curr_indexed = NULL;
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
%define parse.error verbose
%start program

%union{
  	char *stringValue;
  	int intValue;
  	float floatValue;
		struct expr *expression;
		unsigned int iop;
		struct alpha_func_t* afunc;
		struct Queue* queue;
		struct SymbolTableRecord* symbol;
		struct Pair{
			struct expr* x;
			struct expr* y;
		}*pair;
		struct Loop{
			struct Queue *breaklist;
			struct Queue *contlist;
		}*loop;
}
%type <symbol> funcdef
%type <expression> lvalue
%type <expression> expr
%type <expression> member
%type <expression> primary
%type <expression> assignexpr
%type <expression> const
%type <expression> term
%type <expression> call
%type <expression> objectdef
%type <iop> lop
%type <iop> aop
%type <iop> bop
%type <afunc> normcall
%type <afunc> methodcall
%type <afunc> callsuffix
%type <queue> elist
%type <queue> exprs
%type <queue> indexed
%type <pair> indexedelem
%type <iop>ifprefix
%type <iop>elseprefix
%type <iop>whilestart
%type <iop>whilecond



%token <stringValue> ID 
%token <intValue> INTNUM 
%token <floatValue> REALNUM
%token <stringValue> STRING
%token LINECOMM BLOCKCOMM IF ELSE WHILE FOR FUNCTION RETURN BREAK CONTINUE AND NOT OR LOCAL TRUE FALSE NIL 
%token CURL_O CURL_C BRAC_O BRAC_C ANGL_O ANGL_C SEMI COMMA COLON DCOLON DOT DOTDOT 
%token ASSIGN PLUS MINUS MUL DIV PERC EQUALS NEQUALS INCR DECR GREATER LESS GREATER_E LESS_E

%right ASSIGN
%left  OR
%left  AND
%nonassoc EQUALS NEQUALS
%nonassoc GREATER GREATER_E LESS LESS_E
%left  PLUS MINUS
%left  MUL DIV PERC 
%right NOT INCR DECR UMINUS
%left  DOT DOTDOT
%left  BRAC_O BRAC_C
%left  ANGL_O ANGL_C

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
			|expr aop expr {
				printf("expr ->  expr op expr %d\n",alpha_yylineno);
				Expr* expr0 = new_expr(arithexpr_e);
				expr0->sym = new_temp();
				emit($2,$1,$3,expr0,0);
				$$ = expr0;
			}| expr lop expr{
				Expr* result;
				result = new_expr(boolexpr_e);
				result->sym = new_temp();
				emit($2, $1 , $3,NULL, nextQuad()+3);
				emit(assign, newexpr_constbool(0),NULL, result,0);
				emit(jump,NULL,NULL,NULL,nextQuad()+2);
				emit(assign, newexpr_constbool(1),NULL, result,0); // arg2 = NULL , label = NULL on assign OR aop
				$$ = result;
			}|
			expr bop expr{
				Expr* result = new_expr(boolexpr_e);
				result->sym = new_temp();
				emit($2, $1 , $3, result,0);
				$$ = result;
			}|term {printf("expr ->  term\n");
				$$ = $1;
			};
aop:	PLUS{$$ = add;}|MUL{$$ = mul;}|DIV{$$ = divi;}|PERC{$$ = mod;}|MINUS{$$ = sub;};

lop:	GREATER{ $$ = if_greater;}|GREATER_E{$$ = if_gratereq;} | LESS {$$ = if_less;}|LESS_E {$$ = if_lesseq;}| EQUALS {$$ = if_eq;} | NEQUALS {$$ = if_noteq;};

bop:	AND{$$ = and;} | OR{$$ = or;};

term:	 ANGL_O expr ANGL_C {printf("term ->  ( expr )\n");
				$$ = $2;
			}
			|MINUS expr {printf("term ->  - expr \n");
				if($2->type==constbool_e ||	$2->type==conststring_e || $2->type==nil_e || $2->type==newtable_e ||$2->type==programfunc_e ||$2->type==libraryfunc_e ||	$2->type==boolexpr_e)alpha_yyerror("Illegal Expression to unary -");
				Expr* new_e;
				new_e = new_expr(arithexpr_e);
				new_e->sym = new_temp();
				emit(uminus, $2, NULL,new_e,0);
				$$ = new_e;
			}
			|NOT expr {printf("term ->  not expr \n");
				Expr* term = new_expr(arithexpr_e);
				term->sym = new_temp();
				emit(not,$2,NULL,term,0);
				$$ = term;
			}
			|lvalue{
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1,0); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					printf("%s\n",alpha_yylval.stringValue);
					alpha_yyerror("Illegal instruction++ on undefined variable \n");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					printf("%s\n",alpha_yylval.stringValue);
					alpha_yyerror("Illegal instruction++ on function  \n");
          				
				}
			} INCR {printf("term ->   lvalue ++ \n");
				Expr* term = new_expr(var_e);
				Expr* value;
				term->sym = new_temp();
				if($1->type == tableitem_e){
					value = emit_iftableitem($1);
					emit(assign,value,NULL,term,0);
					emit(add,value,newexpr_constnum(1),value,0);
					emit(tablesetelem,$1,$1->index,value,0);
				}else{
					emit(assign,$1,NULL,term,0);
					emit(add,$1,newexpr_constnum(1),$1,0);
				}
				$$ = term;
			}
			|INCR lvalue {printf("term ->  ++ lvalue\n");
					dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1,0); //type(arg:#4) is not important when we are expecting this var
					if(dummy == NULL){
						printf("%s\n",alpha_yylval.stringValue);
						alpha_yyerror("Illegal ++instruction on undefined variable \n");
										
					}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
						printf("%s\n",alpha_yylval.stringValue);
						alpha_yyerror("Illegal ++instruction on function  \n");
										
					}
					Expr* term;
					Expr* value;
					if($2->type == tableitem_e){
						term = emit_iftableitem($2);
						emit(add,term,newexpr_constnum(1),term,0);
						emit(tablesetelem,$2,$2->index,term,0);
					}else{
						emit(add,$2,newexpr_constnum(1),$2,0);
						term = new_expr(arithexpr_e);
						term->sym = new_temp();
						emit(assign,$2,NULL,term,0);
					}
					$$ = term;
				}
			|lvalue{
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1,0); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal instruction-- on undefined variable \n");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal instruction-- on function  \n");
          				
				}
			} DECR {printf("term ->  lvalue -- \n");
				Expr* term = new_expr(var_e);
				Expr* value;
				term->sym = new_temp();
				if($1->type == tableitem_e){
					value = emit_iftableitem($1);
					emit(assign,value,NULL,term,0);
					emit(sub,value,newexpr_constnum(1),value,0);
					emit(tablesetelem,$1,$1->index,value,0);
				}else{
					emit(assign,$1,NULL,term,0);
					emit(sub,$1,newexpr_constnum(1),$1,0);
				}
				$$ = term;
			}
			|DECR lvalue {printf("term ->  -- lvalue \n");
				dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,1,0); //type(arg:#4) is not important when we are expecting this var
				if(dummy == NULL){
					alpha_yyerror("Illegal --instruction on undefined variable ");
          				
				}else if(dummy->type == LIBFUNC || dummy->type == USRFUNC){
					alpha_yyerror("Illegal --instruction on function  ");
          				
				}	
				Expr* term;
					Expr* value;
					if($2->type == tableitem_e){
						term = emit_iftableitem($2);
						emit(sub,term,newexpr_constnum(1),term,0);
						emit(tablesetelem,$2,$2->index,term,0);
					}else{
						emit(sub,$2,newexpr_constnum(1),$2,0);
						term = new_expr(arithexpr_e);
						term->sym = new_temp();
						emit(assign,$2,NULL,term,0);
					}
					$$ = term;
			}
			|primary {printf("term ->  primary\n");
				$$ = $1;
			};

assignexpr:		lvalue{
					fprintf(stderr,"%s %d %d %d\n",alpha_yylval.stringValue,alpha_yylineno,getScope(),((Scope *)Stack_get(GSS, GSS->size - getScope() - 1))->isFunction);
					dummy = lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,0,0); //type(arg:#4) is not important when we are expecting this var
					if( dummy != NULL){
						//if(dummy->scope == 0){
							if( dummy->type == USRFUNC || dummy->type == LIBFUNC ){
								//printf("%s = %s at %d\n",$1,$3,alpha_yylineno);
								alpha_yyerror("Illegal assigment to function\n");
								exit(EXIT_FAILURE);
							}
						//}
					}
					
			} ASSIGN expr {
					Expr* assign_ret;
					if ($1->type == tableitem_e) {
							emit(tablesetelem,$1,$1->index,$4,0);// that is: lvalue[index] = expr
							assign_ret = emit_iftableitem ($1);	// Will always emit. 
							assign_ret->type = assignexpr_e;
					} else {
							emit(assign,$4,NULL,$1,0);// that is: lvalue = expr
							assign_ret = new_expr(assignexpr_e);
							assign_ret->sym = new_temp();
							emit(assign, $1, NULL, assign_ret,0);
					}
					$$ = assign_ret;
			};

primary:		lvalue	{printf("primary ->  lvalue\n");
						Expr* primary = emit_iftableitem($1);
						$$ = primary; 
					}
			|call {printf("primary ->  call\n");$$=$1;}
			|objectdef {printf("primary ->  objectdef\n"); $$ = $1;}
			|ANGL_O funcdef ANGL_C {printf("primary ->  ( funcdef )\n");isLamda = 1;
				Expr* prim = new_expr(programfunc_e);
				prim->sym = $2;
				$$ = prim;
			}
			|const {printf("primary ->  const\n");
				$$ = $1;
			};

lvalue:			ID {printf("lvalue -> ID \n") ; /*scope lookup and decide what type of var it is*/
				// $$ = $1;
				Scope* curr_scope = (Scope *)Stack_get(GSS, 0);
				int expected =0;// curr_scope->isFunction?1:0;
				fprintf(stderr,"%d %s\n",getScope(),alpha_yylval.stringValue);
				dummy =	lookup(alpha_yylval.stringValue,getScope()?LCL:GLBL,alpha_yylineno,expected,0);
				if (dummy==NULL) {
					dummy = insert(alpha_yylval.stringValue,getScope()?LCL:GLBL,getScope(),alpha_yylineno);
					dummy->space = currscopespace();
					dummy->offset = currscopeoffset();
					inccurrscopeoffset();
				}
				$$ = lvalue_expr(dummy);
			}
			|LOCAL ID {
				// $$ = $2; // ?
				dummy =	lookup(alpha_yylval.stringValue,getScope()?LCL:GLBL,alpha_yylineno,0,0);
				// ki an uparxei hdh local? de prepei na einai opws panw if dummy == NULL ?
				dummy = insert(alpha_yylval.stringValue,getScope()?LCL:GLBL,getScope(),alpha_yylineno);
				dummy->space = currscopespace();
				dummy->offset = currscopeoffset();
				inccurrscopeoffset();
				$$ = lvalue_expr(dummy);
			}
			|DCOLON ID {
					// $$ = $2;
				printf("lvalue ->  DCOLON ID\n");
				dummy = lookupGlobal(alpha_yylval.stringValue,GLBL,alpha_yylineno,1);
				if(dummy==NULL){
						char *buffer = (char*)malloc(30+strlen(alpha_yylval.stringValue));
						sprintf(buffer, "Global variable %s not defined \n",alpha_yylval.stringValue);
                				alpha_yyerror(buffer);
				}
				Expr* dcolon = lvalue_expr(dummy);
				$$ = dcolon;
				}
			|member {printf("lvalue ->  member\n");};

member: 		lvalue DOT ID {printf("member ->  lvalue . ID = %s\n", alpha_yylval.stringValue);
				// increaseScope(0); // giati increase scope? // exw arxisei na to afairw arakse
				// $$ = $3;
				// dummy =	lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,0,0);
				// if(dummy==NULL){
				// 		insert(alpha_yylval.stringValue,getScope()?LCL:GLBL,getScope(),alpha_yylineno);
				// }
				// decreaseScope();
				
				Expr* tableitem = member_item($1,$3);
				$$ = tableitem;
			}
			|lvalue BRAC_O expr BRAC_C  {
				printf("member ->  lvalue [ expr ]\n");
				Expr* obj = $1,*item=NULL;
				obj = emit_iftableitem(obj);
				item = new_expr (tableitem_e);
				item->sym = obj->sym;
				item->index = $3; // The index is the expression.
				$$ = item;
				
				}
			|call DOT ID {printf("member ->  call . ID = %s\n", alpha_yylval.stringValue);
				// increaseScope(0);
				// // $$ = $3;
				// dummy =	lookup(alpha_yylval.stringValue,LCL,alpha_yylineno,0,0);
				// if(dummy==NULL){
				// 		insert(alpha_yylval.stringValue,getScope()?LCL:GLBL,getScope(),alpha_yylineno);
				// }
				// decreaseScope();
			}
			|call BRAC_O expr BRAC_C {printf("member ->  call [ expr ]\n");};

call:			call ANGL_O elist ANGL_C {printf("call ->  call ( elist )\n");
					Expr* call = make_call($1,$3);
					$$ = call;
					curr_elist = NULL; 
			}
			|lvalue callsuffix {
				af_t* callsuffix = $2;
				// callsuffix->elist = Queue_init();
				if(callsuffix->method){
					Expr* lval = $1;
					Expr* self= lval;
					lval = emit_iftableitem(member_item(self,callsuffix->name));
					Queue_enqueue(callsuffix->elist,(Expr*)self);
				}
				$$ = make_call($1,callsuffix->elist);
				printf("call ->  lvalue callsuffix \n");
				curr_elist = NULL;

			}
			|ANGL_O funcdef ANGL_C ANGL_O elist ANGL_C {printf("call ->  ( funcdef ) ( elist )  \n");
				Expr* func = new_expr(programfunc_e);
				func->sym = $2;
				Expr* call =  make_call(func,$5);
				$$ = call;
				curr_elist = NULL;
			};

callsuffix:		normcall {printf("callsuffix ->  normcall\n");$$ = $1;}
			|methodcall {printf("callsuffix ->  methodcall\n");$$ = $1;};

normcall:		ANGL_O elist ANGL_C {printf("normcall ->  ( elist )\n");
			af_t* norm = (af_t*)malloc(sizeof(af_t));
			norm->elist = $elist;
			norm->method = 0;
			norm->name = NULL;
			$$ = norm;
};

methodcall:		DOTDOT ID ANGL_O elist ANGL_C {printf("methodcall ->  .. ID ( elist )\n");
		af_t* meth = (af_t*)malloc(sizeof(af_t));
		meth->elist = $elist;
		meth->method = 1;
		meth->name = strdup($ID);
		$$ = meth;
};

elist:		expr exprs {printf("elist ->  expr exprs\n");
					if(curr_elist==NULL)curr_elist = Queue_init();
					// printf("(0) %d\n",curr_elist->size);

					Queue_enqueue(curr_elist,$1);

					// printf("(1) %d\n",curr_elist->size);
					$$ = Queue_get(global_elist,global_elist->size-1);
					curr_elist	=  NULL;
			}
			|	{printf("elist ->  nothing\n");/*printf("(2)\n");*/};

exprs:			COMMA expr exprs {printf("exprs ->  , expr exprs\n");
					if(curr_elist==NULL)curr_elist = Queue_init();
					// printf("(3) %d\n",curr_elist->size);
					Queue_enqueue(curr_elist,$2);
					$$ = curr_elist;
			}
			| {printf("exprs ->  nothing\n");
				if(curr_elist==NULL)curr_elist = Queue_init();
				Queue_enqueue(global_elist,curr_elist);
				// printf("(4) \n");
			};

objectdef:		BRAC_O elist BRAC_C  {printf("objectdef ->  [ elist ]\n");
		Expr* t = new_expr(newtable_e);
		int i;
		Queue* elist = $2;
		t->sym = new_temp();
		emit(tablecreate,t,NULL,NULL,0);
		int iter = 0;
		if(elist!=NULL){
        printf("elist->size %d\n",elist->size);
        for(i = 0 ; i <elist->size; i++){
            emit(tablesetelem,t,newexpr_constnum(iter++),Queue_get(elist,i),0);
        }
    }
		$$ = t;
}
			|BRAC_O indexed BRAC_C  {printf("objectdef ->  [ indexed ]\n");
				Expr* t = new_expr(newtable_e);
				int i ;
				t->sym = new_temp();
				emit(tablecreate, t,NULL,NULL,0);;
				// for each <x,y> in $indexeddo
				Queue* indexed = $2;
				for(i = 0 ; i <indexed->size-1; i++){
					emit(tablesetelem, ((struct Pair*)Queue_get(indexed,i))->x, ((struct Pair*)Queue_get(indexed,i))->y,t,0);
				}
				$$ = t;
			};

indexed:		indexedelem indexedelem_comm {printf("indexed ->  indexedelem indexedelem_comm\n");
				Queue_enqueue(curr_indexed,$1);

				$$ = Queue_get(global_indexed_q,global_indexed_q->size-1);
				curr_indexed = NULL;
};

indexedelem:		CURL_O expr{
				Scope* curr_scope = (Scope *)Stack_get(GSS, 0);
				// printf("%d %s\n",expected,alpha_yylval.stringValue);
				dummy =	lookup(alpha_yylval.stringValue,getScope()?LCL:GLBL,alpha_yylineno,0,0);
				if(dummy==NULL){
					insert(alpha_yylval.stringValue,LCL,getScope(),alpha_yylineno);
				}
			} COLON expr CURL_C {printf("indexedelem ->  { expr : expr }\n");
				if(curr_indexed == NULL){
					curr_indexed = Queue_init();
					Queue_enqueue(global_indexed_q,curr_indexed);

				}
				struct Pair* new_pair = (struct Pair*)malloc(sizeof(struct Pair));
				new_pair->x = $2;
				new_pair->y = $5;
				printf("i(0)\n");	
				$$ = new_pair;
				printf("i(0.5)\n");
				Queue_enqueue(curr_indexed,new_pair);
			};

indexedelem_comm:		COMMA indexedelem indexedelem_comm {printf("indexedelem_comm ->  , indexedelem indexedelem_comm\n");
				printf("i(2)\n");	

			}
			| {printf("indexedelem_comm ->  nothing\n");
							printf("i(1)\n");	

			};


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
				dummy=lookup(alpha_yylval.stringValue,(sc==0)?GLBL:LCL,alpha_yylineno,0,1);
				if(dummy!=NULL){
					if (dummy->type==LIBFUNC) {
						sprintf(buffer, "Function already defined as LIBFUNC \'%s\' line %u", alpha_yylval.stringValue, alpha_yylineno);
						alpha_yyerror(buffer);
					}
					else if(dummy->type==USRFUNC){
						sprintf(buffer, "Function already defined as USERFUNC \'%s\' line %u", alpha_yylval.stringValue, alpha_yylineno);
						alpha_yyerror(buffer);
					}else if(dummy->scope == 0 && getScope()>0){
						func_for_args = insert(alpha_yylval.stringValue,USRFUNC,getScope(),alpha_yylineno);
					}
					else{
						sprintf(buffer, "Function definition on existing var \'%s\' line %u", alpha_yylval.stringValue, alpha_yylineno);
						alpha_yyerror(buffer);
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

const:			INTNUM {
							printf("const ->  INTNUM\n");
							Expr* tmp = new_expr(constnum_e);
							tmp->value.numConst = alpha_yylval.intValue;
							$$ = tmp;
							}
			|REALNUM {printf("const ->  REALNUM\n");
							Expr* tmp = new_expr(constnum_e);
							tmp->value.numConst = alpha_yylval.floatValue;
							$$ = tmp;
							}
			|STRING {printf("const ->  STRING\n");
							Expr* tmp = new_expr(conststring_e);
							tmp->value.strConst = alpha_yylval.stringValue;
							$$ = tmp;
							}
			|NIL {printf("const ->  NIL\n");
							Expr* tmp = new_expr(nil_e);
							$$ = tmp;
							}
			|TRUE {printf("const ->  TRUE\n");
							Expr* tmp = new_expr(constbool_e);
							tmp->value.boolConst = 1;
							$$ = tmp;
						}
			|FALSE {printf("const ->  FALSE\n");
							Expr* tmp = new_expr(constbool_e);
							tmp->value.boolConst = 0;
							$$ = tmp;
						};

idlist:	ID{
	dummy = NULL;
	int sc = getScope();
	dummy=lookup(alpha_yylval.stringValue,FORMAL,alpha_yylineno,0,0);

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
	dummy=lookup(alpha_yylval.stringValue,FORMAL,alpha_yylineno,0,0);

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

ifprefix: IF ANGL_O expr ANGL_C {
		emit(if_eq, $expr, newexpr_constbool(1), NULL, nextQuad()+2);
		$ifprefix = nextQuad();
		emit(jump, NULL, NULL, NULL, 0);
	};

elseprefix: ELSE {
	$elseprefix = nextQuad();
	emit(jump, NULL, NULL, NULL, 0);
};

ifstmt:	ifprefix stmt {
		patchlabel($ifprefix, nextQuad());
	}| ifprefix stmt elseprefix stmt {
		patchlabel($ifprefix, $elseprefix+1);
		patchlabel($elseprefix, nextQuad());
	};

whilestart: WHILE {
	$whilestart = nextQuad();
};

whilecond: ANGL_O expr ANGL_C {
	emit(if_eq, $expr, newexpr_constbool (1), NULL, nextQuad()+2);
	$whilecond = nextQuad();
	emit(jump, NULL, NULL, NULL, 0); 
	in_loop++;
};

whilestmt: whilestart whilecond stmt {
	printf("whilestmt ->  while ( expr ) stmt \n");
	// emit(jump, $whilestart);
	// patchlabel($whilecond, nextQuad());
	// patchlabel($stmt.breaklist, nextQuad());	// FIX
	// patchlabel($stmt.contlist, $whilestart);	//FIX
	in_loop--;
};

forstmt: FOR ANGL_O elist SEMI expr SEMI elist ANGL_C {in_loop++;}
						stmt {printf("forstmt ->  for ( elist ; expr ; elist ) stmt \n"); in_loop--;};

returnstmt: RETURN SEMI {printf("returnstmt ->  return ; \n");} |RETURN expr SEMI {printf("returnstmt ->  return expr ; \n");};

%%

int alpha_yyerror (const char* yaccProvidedMessage){
  fprintf(stderr,"\033[0;31mError %s, line %u \033[0m\n",yaccProvidedMessage,alpha_yylineno);
  exit(EXIT_FAILURE);
}


int main (int argc, char** argv) {
		global_elist = Queue_init();
		global_indexed_q = Queue_init();
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
    printQuads();
    return 0;
}
