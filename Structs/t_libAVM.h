#ifndef T_LIBAVM_H
#define T_LIBAVM_H
#pragma once
#include "Quad.h"


void generate_ADD(Quad*);
void generate_SUB(Quad*);
void generate_MUL(Quad*);
void generate_DIV(Quad*);
void generate_MOD(Quad*);
void generate_UMINUS(Quad*);
void generate_NEWTABLE(Quad*);
void generate_TABLEGETELEM(Quad*);
void generate_TABLESETELEM(Quad*);
void generate_ASSIGN(Quad*);
void generate_NOP(Quad*);
void generate_JUMP(Quad*);
void generate_IF_EQ(Quad*);
void generate_IF_NOTEQ(Quad*);
void generate_IF_GREATER(Quad*);
void generate_IF_GREATEREQ(Quad*);
void generate_IF_LESS(Quad*);
void generate_IF_LESSEQ(Quad*);
void generate_AND(Quad*);
void generate_NOT(Quad*);
void generate_OR(Quad*);
void generate_PARAM(Quad*);
void generate_CALL(Quad*);
void generate_GETRETVAL(Quad*);
void generate_FUNCSTART(Quad*);
void generate_RETURN(Quad*);
void generate_FUNCEND(Quad*);

typedef struct userfunc userfunc;
typedef void (*generator_func_t)(Quad *);
generator_func_t generators[] = {
	generate_ASSIGN,
	generate_ADD,
	generate_SUB,
	generate_MUL,
	generate_DIV,
	generate_MOD,
	generate_UMINUS,
	generate_AND,
	generate_OR,
	generate_NOT,
	generate_IF_EQ,
	generate_IF_NOTEQ,
	generate_IF_LESSEQ,
	generate_IF_GREATEREQ,
	generate_IF_LESS,
	generate_IF_GREATER,
	generate_CALL,
	generate_PARAM,
	generate_RETURN,
	generate_JUMP,
	generate_GETRETVAL,
	generate_FUNCSTART,
	generate_FUNCEND,
	generate_NEWTABLE,
	generate_TABLEGETELEM,
	generate_TABLESETELEM,
	generate_NOP
};
typedef enum vmopcode{
	assign_v,	add_v,		sub_v,
	mul_v,		div_v,		mod_v,
	uminus_v,	and_v,		or_v,
	not_v,		jeq_v,		jne_v,
	jle_v,		jge_v,		jlt_v,
	jgt_v,		call_v,		jump_v,
	pusharg_v,	funcenter_v,	funcexit_v,
	newtable_v,	tablegetelem_v,	tablesetelem_v,
	nop_v
}vmopcode;
typedef enum vmarg_t{
	label_a	,
	global_a ,
	formal_a ,
	local_a ,
	number_a ,
	string_a ,
	bool_a ,
	nil_a ,
	userfunc_a ,
	libfunc_a ,
	retval_a ,
	useless_a 
}vmarg_t;


// double*		numConsts = (double*) 0;
// unsigned 	totalNumConsts = 0;
// char**		stringConsts = (char**)0;
// unsigned	totalStringConsts = 0;
// char**		nameLibfuncs = (char**)0;
// unsigned	totalNamedLibfuncs = 0;
// userfunc*	userFuncs = (userfunc*)0;
// unsigned int	totalUserFuncs = 0;

typedef struct incomplete_jump incomplete_jump;
struct incomplete_jump{
    unsigned instrNo;
    unsigned iaddress;
    incomplete_jump* next;
};

typedef struct vmarg{
    vmarg_t type;
    unsigned val;
} vmarg;

typedef struct instruction{
    vmopcode opcode;
    vmarg result;
    vmarg arg1;
    vmarg arg2;
    unsigned srcLine;
}instruction;

struct userfunc {
	unsigned address;
	unsigned localSize;
	char* id;
};

unsigned consts_newstring(char* s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char* s);
void make_operand(Expr* e,vmarg* arg);

// incomplete_jump* ij_head = (incomplete_jump*) 0;
// unsigned ij_total = 0;

void add_incomplete_jump(unsigned insrtNo, unsigned iaddress);

void patch_incomplete_jumps(void);
void display();


#endif