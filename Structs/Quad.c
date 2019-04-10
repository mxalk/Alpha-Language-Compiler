#include "Quad.h"
//#include "SymTable.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

Quad *quads = (Quad *) 0;
unsigned total = 0;
unsigned int currQuad = 0;

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(Quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(Quad)+CURR_SIZE)

unsigned temp_no = 0;

const char *iopcodeNames[] = {
	"ASSIGN",
	"ADD",
	"SUB",
	"MUL",
	"DIV",
	"MOD",
	"UMINUS",
	"AND",
	"OR",
	"NOT",
	"IF_EQ",
	"IF_NOTEQ",
	"IF_LESSEQ",
	"IF_GREATEREQ",
	"IF_LESS",
	"IF_GREATER",
	"CALL",
	"PARAM",
	"RETURN",
	"GETRETVAL",
	"FUNCSTART",
	"FUNCEND",
	"TABLECREATE",
	"TABLEGETELEM",
	"TABLESETELEM"
};

const char *expr_tNames[] = {
    "variable",
	"table item",

	"program function",
	"library function",

	"arithmetic expression",
	"boolean expression",
	"assign expression",
	"new table",

	"constatnt number",
	"constant boolean",
	"constant string",

	"nil"
};

Symbol *new_temp() {
    int no = temp_no, digits = 0;
    do {
        no = no/10;
        digits++;
    } while(no!=0);
    char *name = malloc(sizeof(char)*(digits+4));
    strcpy(name, "_tmp");
    sprintf(name+4, "%u", temp_no);
    // itoa(temp_no, name+4, 10);
    temp_no++;
    insert(name, LCL, getScope(), 0);        
    Symbol* new_sym = (Symbol*) malloc(sizeof(Symbol));
    new_sym->name = name;
    return new_sym;
}

void reset_temp() {
    temp_no = 0;
}

Expr *new_expr(Expr_t type) {
    Expr *expression = (Expr *) malloc(sizeof(Expr));
    expression->type = type;
    return expression;
}

Expr* lvalue_expr (Symbol* sym){
    assert(sym);
    Expr * e = (Expr*)malloc(sizeof(Expr));
    memset(e,0,sizeof(Expr));
    e->next = (Expr*)0;
    e->sym = sym;
    switch(sym->type){
        case var_s:         e->type = var_e; break;
        case programfunc_s: e->type = programfunc_e; break;
        case libraryfunc_s: e->type = libraryfunc_e; break;
        default: assert(0);
    }
    return e;
}

Expr* make_call(Expr* lvalue, Expr* elist){ //elist = arg list
    Expr* func = emit_iftableitem(lvalue);
    //for each in reverse elist do
    // emit(param,arg);
    // emit(call,func);
    Expr* result = new_expr(var_e);
    result->sym = new_temp();
    // emit(getretval,result);
    return result; 
}

Expr *newexpr_conststring(const char* name){
    Expr *expression = (Expr *) malloc(sizeof(Expr));
    expression->type = conststring_e;
    // expression->index = //;
    return expression;
}

// dialexi 9, diafania 41
void expand() {
    assert(total==currQuad);
    Quad* p = (Quad*)malloc(NEW_SIZE);
    if (quads) {
        memcpy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

void emit_error(Iopcode iopcode, Expr *arg) {
    fprintf(stderr, "\nCannot perform \'%s\' with argument of type \'%s\'\n", iopcodeNames[iopcode], expr_tNames[arg->type]);
}

void emit(Iopcode iopcode, Expr *arg1, Expr *arg2, Expr *result, unsigned label, unsigned line) {
    
    if (currQuad == total) expand();
    Quad *p = quads+currQuad++;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;
}

void printQuads() {
    int qi;
    Quad q;
    Iopcode iopcode;
    Expr *arg1, *arg2, *result;
    Expr *expressions[2];
    for (qi=0; qi<currQuad; qi++) {
        q = quads[qi];

        iopcode = q.op;
        arg1 = q.arg1;
        arg2 = q.arg2;
        result = q.result;
        expressions[0] = arg1;
        expressions[1] = arg2;
        int i;

        printf("%15s", iopcodeNames[iopcode]);
        switch (iopcode) {

            case assign:
                assert(result->type == assignexpr_e);
                printf(" %15s", result->sym->name);
                for (i=0; i<2; i++) {
                    printf(" ");
                    switch (expressions[i]->type) {
                        case var_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case constbool_e:
                            printf("%15s", expressions[i]->boolConst?"TRUE":"FALSE");
                            break;
                        case constnum_e:
                            printf("%15d", expressions[i]->numConst);
                            break;
                        case conststring_e:
                            printf("%15s", expressions[i]->strConst);
                            break;
                        default: emit_error(iopcode, expressions[i]);
                    }
                }
                break;

            case add:
            case sub:
            case mul:
            case divi:
            case mod:
                assert(result->type == arithexpr_e);
                printf(" %15s", result->sym->name);
                for (i=0; i<2; i++) {
                    printf(" ");
                    switch (expressions[i]->type) {
                        case var_e:
                        case arithexpr_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case constnum_e:
                            printf("%15d", expressions[i]->numConst);
                            break;
                        default: emit_error(iopcode, expressions[i]);
                    }
                }
                break;
                
            case uminus:
                assert(result->type == arithexpr_e);
                printf(" %15s", result->sym->name);
                printf(" ");
                switch (arg1->type) {
                    case var_e:
                    case arithexpr_e:
                        printf("15s", arg1->sym->name);
                        break;
                    // case constnum_e:
                    //     printf("%15d ", arg1->numConst);
                    //     break;
                    default: emit_error(iopcode, arg1);
                }
                break;

            case and:
            case or:
                assert(result->type == boolexpr_e);
                printf(" %15s", result->sym->name);
                for (i=0; i<2; i++) {
                    printf(" ");
                    switch (expressions[i]->type) {
                        case var_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case tableitem_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case programfunc_e:
                            printf("%15s", "TRUE");
                            break;
                        case libraryfunc_e:
                            printf("%15s", "TRUE");
                            break;
                        case constbool_e:
                            printf("%15s", expressions[i]->boolConst?"TRUE":"FALSE");
                            break;
                        case constnum_e:
                            printf("%15d", expressions[i]->numConst);
                            break;
                        case conststring_e:
                            printf("%15s", expressions[i]->strConst);
                            break;
                        default: emit_error(iopcode, expressions[i]);
                    }
                }
                break;
                
            case not:
                assert(result->type == boolexpr_e);
                printf(" %15s", result->sym->name);
                printf(" ");
                switch (arg1->type) {
                    case var_e:
                        printf("%15s", arg1->sym->name);
                        break;
                    case tableitem_e:
                        printf("%15s", arg1->sym->name);
                        break;
                    case programfunc_e:
                        printf("%15s", "TRUE");
                        break;
                    case libraryfunc_e:
                        printf("%15s", "TRUE");
                        break;
                    case constbool_e:
                        printf("%15s", arg1->boolConst?"TRUE":"FALSE");
                        break;
                    case constnum_e:
                        printf("%15d", arg1->numConst);
                        break;
                    case conststring_e:
                        printf("%15s", arg1->strConst);
                        break;
                    default: emit_error(iopcode, arg1);
                }
                break;

            case if_eq:
            case if_noteq:
                assert(result->type == boolexpr_e);
                printf(" %15u", q.label);

                break;

            case if_lesseq:
            case if_gratereq:
            case if_less:
            case if_greater:
                assert(result->type == boolexpr_e);
                printf(" %15s", result->sym->name);
                for (i=0; i<2; i++) {
                    printf(" ");
                    switch (expressions[i]->type) {
                        case var_e:
                        case arithexpr_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case constnum_e:
                            printf("%15d", expressions[i]->numConst);
                            break;
                        default: emit_error(iopcode, expressions[i]);
                    }
                }
                break;

            case call:
                break;

            case param:
                break;

            case ret:
                printf(" %15s", result->sym->name);
                break;

            case getretval:
                break;

            case funcstart:
                assert(result->type == programfunc_e);
                printf(" %15s", result->sym->name);
                break;

            case funcend:
                assert(result->type == programfunc_e);
                printf(" %15s", result->sym->name);
                break;

            case tablecreate:
                break;

            case tablegetelem:
                assert(result->type == var_e);
                printf(" %15s", result->sym->name);
                for (i=0; i<2; i++) {
                    printf(" ");
                    switch (expressions[i]->type) {
                        case var_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case tableitem_e:
                            printf("%15s", expressions[i]->sym->name);
                            break;
                        case programfunc_e:
                            printf("%15s", "TRUE");
                            break;
                        case libraryfunc_e:
                            printf("%15s", "TRUE");
                            break;
                        case constbool_e:
                            printf("%15s", expressions[i]->boolConst?"TRUE":"FALSE");
                            break;
                        case constnum_e:
                            printf("%15d", expressions[i]->numConst);
                            break;
                        case conststring_e:
                            printf("%15s", expressions[i]->strConst);
                            break;
                        default: emit_error(iopcode, expressions[i]);
                    }
                }

            case tablesetelem:
                break;
            
            default: assert(0);
        }
        printf("\n");
    }
}

Expr *emit_iftableitem(Expr *e) {
    if (e->type != tableitem_e) return e;
    Expr *result = new_expr(var_e);
    result->sym = new_temp();
    emit(tablegetelem, e, e->index, result, 0, 0);
    return result;
}

Expr *member_item(Expr *lvalue, char *name) {
    lvalue = emit_iftableitem(lvalue);
    Expr *item = new_expr(tableitem_e);
    item->sym = lvalue->sym;
    item->index = newexpr_conststring(name);
}

//dialeksi 11, diafania 5
Expr * valid_arithop(Iopcode iop, Expr *e1, Expr *e2)    { 

    Expr * valid_expr;

    if(e1->type == programfunc_e || e1->type ==  libraryfunc_e || e1->type ==  boolexpr_e
    || e1->type == newtable_e    || e1->type ==  constbool_e   || e1->type ==  conststring_e 
    || e1->type ==  nil_e
    || e2->type == programfunc_e || e2->type ==  libraryfunc_e || e2->type ==  boolexpr_e
    || e2->type == newtable_e    || e2->type ==  constbool_e   || e2->type ==  conststring_e 
    || e2->type ==  nil_e) 
    {
        return NULL;
    }

    if ((e1->type == constnum_e) && (e2->type == constnum_e)) {
        valid_expr = new_expr(constnum_e);
    }

    /*analoga tin pra3i 8a prepei na tin ektelesi kai na epistepsei
    to antistoixo expr */
    
}

unsigned programVarOffset = 0;
unsigned functionLocalOffset = 0;
unsigned formalArgOffset = 0;
unsigned scopeSpaceCounter = 1;

Symbol * new_symbol(const char* name){
    Symbol* new_sym = (Symbol*)malloc(sizeof(Symbol));
    new_sym->name = strdup(name);
    return new_sym;
}

unsigned currscopeoffset (){
    switch(currscopespace()){
        case programvar     : return programVarOffset;
        case functionlocal  : return functionLocalOffset;
        case formalarg      : return formalArgOffset;
        default: assert(0);
    }
}

void inccurrscopeoffset(){
    switch(currscopespace()){
        case programvar     : ++programVarOffset; break;
        case functionlocal  : ++functionLocalOffset; break;
        case formalarg      : ++formalArgOffset; break;
        default: assert(0);
    }
}

void enterscopespace(){
    ++scopeSpaceCounter;
}

void exitscopespace(){
    assert(scopeSpaceCounter>1);
    --scopeSpaceCounter;
}

enum scopespace_t currscopespace(void) {
	if (scopeSpaceCounter == 1) return programvar;
	if (!(scopeSpaceCounter % 2)) return formalarg;
	return functionlocal;
}