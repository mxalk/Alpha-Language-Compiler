#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define AVM_STACKSIZE 4096
#define N AVM_STACKSIZE-1
#define AVM_STACKENV_SIZE 4
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
#define AVM_NUMACTUALS_OFFSET   +4
#define AVM_SAVEDPC_OFFSET      +3
#define AVM_SAVEDTOP_OFFSET     +2
#define AVM_SAVEDTOPSP_OFFSET   +1

extern unsigned warnings;
unsigned char executionFinished ;
unsigned pc;
unsigned currLine;
unsigned codeSize;
#define AVM_ENDING_PC codeSize
struct instruction *code ;
unsigned totalActuals;
unsigned GlobalProgrammVarOffset;

char *typeStrings[8];
void execute_arithmetic (struct instruction *);

void execute_assign (struct instruction*);
void execute_add (struct instruction*);
void execute_sub (struct instruction*);
void execute_mul (struct instruction*);
void execute_div (struct instruction*);
void execute_div (struct instruction*);
void execute_mod (struct instruction*);
void execute_mod (struct instruction*);
void execute_uminus (struct instruction*);
void execute_and (struct instruction*);
void execute_and (struct instruction*);
void execute_or (struct instruction*);
void execute_not (struct instruction*);
void execute_jeq (struct instruction*);
void execute_jne (struct instruction*);
void execute_jle (struct instruction*);
void execute_jge (struct instruction*);
void execute_jge (struct instruction*);
void execute_jlt (struct instruction*);
void execute_jgt (struct instruction*);
void execute_jump (struct instruction*);
void execute_call (struct instruction*);
void execute_pusharg (struct instruction*);
void execute_funcenter (struct instruction*);
void execute_funcexit (struct instruction*);
void execute_newtable (struct instruction*);
void execute_tablegetelem (struct instruction*);
void execute_tablesetelem (struct instruction*);
void execute_nop (struct instruction*);


enum vmopcode {
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
};

typedef enum vmarg_t {
    label_a     = 0,
    global_a    = 1,
    formal_a    = 2,
    local_a     = 3,
    number_a    = 4,
    string_a    = 5,
    bool_a      = 6,
    nil_a       = 7,
    userfunc_a  = 8,
    libfunc_a   = 9,
    retval_a    = 10,
    empty_a    = 11
}vmarg_t;

typedef enum avm_memcell_t {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
}avm_memcell_t;


struct vmarg {
    enum vmarg_t type;
    unsigned val;
};

struct instruction {
    enum vmopcode opcode;
    struct vmarg result;
    struct vmarg arg1;
    struct vmarg arg2;
    unsigned srcLine;
};

struct userfunc {
    unsigned address;
    unsigned localSize;
    char *id;
};

typedef struct avm_memcell  {
    enum avm_memcell_t type;
    union {
        double numVal;
        char *strVal;
        unsigned char boolVal;
        struct avm_table *tableVal;
        unsigned funcVal;
        char *libfuncVal;
    } data;
} avm_memcell;

struct avm_table_bucket {
    avm_memcell key;
    avm_memcell value;
    struct avm_table_bucket *next;
};

// LECTURE 13 SLIDE 25 BONUS TO IMPLEMENT
struct avm_table {
    unsigned refCounter;
    struct avm_table_bucket *strIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket *numIndexed[AVM_TABLE_HASHSIZE];
    struct avm_table_bucket *ufncIndexed[AVM_TABLE_HASHSIZE]; // BONUS
    struct avm_table_bucket *lfncIndexed[AVM_TABLE_HASHSIZE]; // BONUS
    struct avm_table_bucket *boolIndexed[AVM_TABLE_HASHSIZE]; // BONUS
    unsigned total;
};


avm_memcell ax, bx, cx, retval, stack[AVM_STACKSIZE];
unsigned top, topsp;
// ------------------- GLOBALS
unsigned totalStringConsts;
char **stringConsts;
char *consts_getstring(unsigned);

unsigned totalNumConsts;
double *numConsts;
double consts_getnumber(unsigned);

unsigned totalUserFuncs;
struct userfunc *userFuncs;
struct userfunc userFuncs_get(unsigned);

unsigned totalNamedLibFuncs;
char **namedLibFuncs;
char *libfuncs_getused(unsigned);
// ===========================================================================
avm_memcell *avm_translate_operand (struct vmarg *, struct avm_memcell *) ;
void avm_tableincrefcounter(struct avm_table *) ;
void avm_tabledecrefcounter(struct avm_table *) ;
void avm_tablebucketsinit(struct avm_table_bucket **) ;
struct avm_table *avm_tablenew(void);
void avm_tablebucketsdestroy(struct avm_table_bucket **) ;
void avm_tabledestroy(struct avm_table *) ;
void execution_cycle (void) ;
// ---------------------------------------------------------------------------
// INSTRUCTION IMPLEMENTATION
// ---------------------------------------------------------------------------
void avm_assign (struct avm_memcell *, struct avm_memcell *);
typedef void (*memclear_func_t)(struct avm_memcell *);
void avm_memcellclear(struct avm_memcell *) ;;
// extern void avm_callsaveenvironment(void);
void avm_callsaveenvironment (void);
void avm_dec_top(void) ;
void avm_push_envvalue(unsigned) ;
void avm_callsaveenvironment(void);;
unsigned avm_get_envvalue(unsigned) ;
typedef void (*library_func_t)(void);
library_func_t avm_getlibraryfunc(char *);
struct userfunc *avm_getfuncinfo(unsigned address);
void avm_calllibfunc(char *);
unsigned avm_totalactuals(void) ;
struct avm_memcell *avm_getactual(unsigned) ;
void avm_registerlibfunc(char *, library_func_t);
// ------------------- STRINGS
typedef char *(*tostring_func_t)(struct avm_memcell *);
// extern char *avm_tostring(struct avm_memcell *);
char *avm_tostring(struct avm_memcell *);
// ------------------- BOOLEAN
typedef unsigned char (*tobool_func_t)(struct avm_memcell *);
unsigned char avm_tobool(struct avm_memcell *) ;
// ------------------- AVM
void avm_initialize (void) ;
void avm_initstack();
void avm_error(char *format, ...);
void avm_warning(char *format, ...);
// ------------------- LIBS
library_func_t *library_func_t_addresses;

void avm_register_libfuncs();
void libfunc_print(void);
void libfunc_input(void);
void libfunc_objectmemberkeys(void);
void libfunc_objecttotalmembers(void);
void libfunc_objectcopy(void);
void libfunc_totalarguments(void);
void libfunc_argument(void);
void libfunc_typeof(void);
void libfunc_strtonum(void);
void libfunc_sqrt(void);
void libfunc_cos(void);
void libfunc_sin(void);
