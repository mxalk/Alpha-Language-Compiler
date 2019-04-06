#include "Quad.h"
#include "SymTable.h"

Quad *quads = (Quad *) 0;
unsigned total = 0;
unsigned int currQuad = 0;

#define EXPAND_SIZE 1024
#define CURR_SIZE (total*sizeof(Quad))
#define NEW_SIZE (EXPAND_SIZE*sizeof(Quad)+CURR_SIZE)


unsigned temp_no = 0;
SymbolTableRecord *new_temp() {
    int no = temp_no, digits = 0;
    do {
        no = no/10;
        digits++;
    } while(no!=0);
    char *name = malloc(sizeof(char)*(digits+4));
    strcpy(name, "_tmp");
    itoa(temp_no, name+4, 10);
    temp_no++;
    return insert(name, LCL, getScope(), 0);        
}

void reset_temp() {
    temp_no = 0;
}

Expr *new_expression(Expr_t type) {
    Expr *expression = (Expr *) malloc(sizeof(Expr));
    expression->type = type;
    return expression;
}

// dialexi 9, diafania 41
void expand() {
    assert(total==currQuad);
    Quad* p = (Quad*)malloc(NEW_SIZE);
    if (quads) {
        memcopy(p, quads, CURR_SIZE);
        free(quads);
    }
    quads = p;
    total += EXPAND_SIZE;
}

void emit(Iopcode iopcode, Expr *arg1, Expr *arg2, Expr *result, unsigned label, unsigned line) {
    
    if (currQuad == total) expand();

    Quad *p = quads+currQuad++;
    p->arg1 = arg1;
    p->arg2 = arg2;
    p->result = result;
    p->label = label;
    p->line = line;

    switch (iopcode) {
        case assign:
            break;
    }
}

Expr *emit_iftableitem(Expr *e) {
    if (e->type != tableitem_e) return e;
    Expr *result = new_expression(var_e);
    result->sym = new_temp();
    emit(tablegetelem, e, e->index, result, 0, 0);
    return result;
}

Expr *member_item(Expr *lvalue,char *name) {
    lvalue = emit_iftableitem(lvalue);
    Expr *item = new_expression(tableitem_e);
    item->sym = lvalue->sym;
    item->index = newexpr_conststring(name);
}