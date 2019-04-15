#pragma once
#include "SymTable.h"

// typedef enum scopespace_t {
//     programvar,
//     functionlocal,
//     formalarg
// } Scopespace_t;

// typedef enum symbol_t {
// 	var_s,
// 	programfunc_s,
// 	libraryfunc_s
// } Symbol_t;

typedef enum iopcode_t {
	assign,
	add,
	sub,
	mul,
	divi,
	mod,
	uminus,
	and,
	or,
	not,
	if_eq,
	if_noteq,
	if_lesseq,
	if_gratereq,
	if_less,
	if_greater,
	jump,
	call,
	param,
	ret,
	getretval,
	funcstart,
	funcend,
	tablecreate,
	tablegetelem,
	tablesetelem,
	jmp
} Iopcode;

typedef enum expr_t {
	var_e,
	tableitem_e,

	programfunc_e,
	libraryfunc_e,

	arithexpr_e,
	boolexpr_e,
	assignexpr_e,
	newtable_e,

	constnum_e,
	constbool_e,
	conststring_e,

	nil_e
} Expr_t;

// extern struct expr; 
// extern struct quad;
typedef struct expr Expr;
typedef struct quad Quad;
typedef struct alpha_func_t af_t;


struct alpha_func_t{
	Queue* elist;
	char* name;
	unsigned method;
};

struct expr {
	Expr_t type;
	SymbolTableRecord *sym;
	Expr *index;
	union Value {
		double numConst;
		char *strConst;
		unsigned char boolConst;
	} value;
	Expr *next;
};
struct quad {
	Iopcode op;
	Expr *result;
	Expr *arg1;
	Expr *arg2;
	unsigned label;
	unsigned line;
};

extern const char *iopcodeNames[];
extern const char *expr_tNames[];



// -----


extern unsigned programVarOffset;
extern unsigned functionLocalOffset;
extern unsigned formalArgOffset;
extern unsigned scopeSpaceCounter;

SymbolTableRecord *new_temp();

void reset_temp();

Expr *new_expr(Expr_t type);
Expr* lvalue_expr (SymbolTableRecord* sym);

Expr *newexpr_conststring(const char* name);
Expr *newexpr_constbool(const unsigned n);
// dialexi 9, diafania 41
void expand();
Expr* make_call(Expr* lvalue, Queue* elist); //elist = arg list

void emit(Iopcode iopcode, Expr *arg1, Expr *arg2, Expr *result, unsigned label);

Expr *emit_iftableitem(Expr *e);

Expr *member_item(Expr *lvalue,char *name);
unsigned nextQuad();
void patchlabel(unsigned int, unsigned int);

Scopespace_t currscopespace(void);
unsigned currscopeoffset ();
void inccurrscopeoffset();
// SymbolTableRecord * new_symbol(const char* name);

Expr * valid_arithop(Iopcode iop, Expr *e1, Expr *e2);