#pragma once

typedef enum iopcode Iopcode;
typedef enum expr_t Expr_t;
typedef struct expr Expr;
typedef struct quad Quad;

enum iopcode {
	assign,
	add,
	sub,
	mul,
	div,
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
	call,
	param,
	ret,
	getretval,
	funcstart,
	funcend,
	tablecreate,
	tablegetelem,
	tablesetelem
};

enum expr_t {
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
};

struct expr {
	Expr_t type;
	SymbolTableRecord *sym;
	Expr *index;
	double numConst;
	char *strConst;
	unsigned char boolConst;
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