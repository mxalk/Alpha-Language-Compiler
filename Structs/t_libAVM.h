#pragma once
#include "Quad.h"

extern void generate_ADD(Quad *);
extern void generate_SUB(Quad *);
extern void generate_MUL(Quad *);
extern void generate_DIV(Quad *);
extern void generate_MOD(Quad *);
extern void generate_UMINUS(Quad *);
extern void generate_NEWTABLE(Quad *);
extern void generate_TABLEGETELEM(Quad *);
extern void generate_TABLESETELEM(Quad *);
extern void generate_ASSIGN(Quad *);
extern void generate_NOP(Quad *);
extern void generate_JUMP(Quad *);
extern void generate_IF_EQ(Quad *);
extern void generate_IF_NOTEQ(Quad *);
extern void generate_IF_GREATER(Quad *);
extern void generate_IF_GREATEREQ(Quad *);
extern void generate_IF_LESS(Quad *);
extern void generate_IF_LESSEQ(Quad *);
extern void generate_AND(Quad *);
extern void generate_NOT(Quad *);
extern void generate_OR(Quad *);
extern void generate_CALL(Quad *);
extern void generate_PARAM(Quad *);
extern void generate_GETRETVAL(Quad *);
extern void generate_FUNCSTART(Quad *);
extern void generate_RETURN(Quad *);
extern void generate_FUNCEND(Quad *);

extern char *vmopcode_name[];

typedef struct userfunc userfunc;
typedef enum vmopcode
{
	assign_v,
	add_v,
	sub_v,
	mul_v,
	div_v,
	mod_v,
	uminus_v,
	and_v,
	or_v,
	not_v,
	jeq_v,
	jne_v,
	jle_v,
	jge_v,
	jlt_v,
	jgt_v,
	jump_v,
	call_v,
	pusharg_v,
	funcenter_v,
	funcexit_v,
	newtable_v,
	tablegetelem_v,
	tablesetelem_v,
	nop_v
} vmopcode;

typedef enum vmarg_t
{
	label_a=0,
	global_a=1,
	formal_a=2,
	local_a=3,
	number_a=4,
	string_a=5,
	bool_a=6,
	nil_a=7,
	userfunc_a=8,
	libfunc_a=9,
	retval_a=10,
	empty_a=11
} vmarg_t;

double *numConsts;
unsigned totalNumConsts;
char **stringConsts;
unsigned totalStringConsts;
char **nameLibfuncs;
unsigned totalNamedLibfuncs;
userfunc *userFuncs;
unsigned int totalUserFuncs;
struct instruction *instructions;
unsigned int totalInstructions;
unsigned int currInstruction;
unsigned int currprocessedquads;

typedef struct incomplete_jump incomplete_jump;
struct incomplete_jump
{
	unsigned instrNo;
	unsigned iaddress;
};

typedef struct vmarg
{
	vmarg_t type;
	unsigned val;
} vmarg;

typedef struct instruction
{
	vmopcode opcode;
	vmarg result;
	vmarg arg1;
	vmarg arg2;
	unsigned srcLine;
} instruction;

struct userfunc
{
	unsigned address;
	unsigned localSize;
	char *id;
};
void emit_instr(instruction *t);
unsigned consts_newstring(char *s);
unsigned consts_newnumber(double n);
unsigned libfuncs_newused(char *s);
unsigned userfuncs_newfunc(SymbolTableRecord *sym);
void make_operand(Expr *e, vmarg *arg);
void make_numberoperand(vmarg *arg, double val);
void make_booloperand(vmarg *arg, unsigned val);
void make_retvaloperand(vmarg *arg);

void add_incomplete_jump(unsigned insrtNo, unsigned iaddress);
unsigned int nextinstructionlabel();
Queue *ij_head;
void backpatch(Queue *q, unsigned int next_ilabel);

void add_incomplete_jump(unsigned insrtNo, unsigned iaddress);
void expand_instructions();
void patch_incomplete_jumps(void);
void generateCode(void);
void display_instr();
