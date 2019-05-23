

#define AVM_STACKSIZE 4096
#define AVM_STACKENV_SIZE 4
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_TABLE_HASHSIZE 211
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v
struct instruction *code = (struct instruction *) 0;
unsigned code_size;

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

enum vmarg_t {
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
    retval_a    = 10
};

enum avm_memcell_t {
    number_m,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
};

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

struct avm_memcell {
    enum avm_memcell_t type;
    union {
        double numVal;
        char *strVal;
        unsigned char boolVal;
        struct avm_table *tableVal;
        unsigned funcVal;
        char *libfuncVal;
    } data;
} typedef avm_memc;

struct avm_table_bucket {
    avm_memc key;
    avm_memc value;
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

typedef void (*execute_func_t)(struct instruction *);

// OPERAND TRANSLATE
avm_memc ax, bx, cx, retval, stack[AVM_STACKSIZE];
unsigned top, topsp;
// Reverse translation for constants:
// getting constant value from index

void avm_memcellclear (struct avm_memcell *m);

unsigned totalStringConsts;
char **stringConsts;
char *consts_getstring(unsigned index) {
    return stringConsts[index];
}

unsigned totalNumConsts;
double *numConsts;
double consts_getnumber(unsigned index) {
    return numConsts[index];
}

unsigned totalUserFuncs;
struct userfunc *userFuncs;

unsigned totalNamedLibFuncs;
char **namedLibFuncs;
char *libfuncs_getused(unsigned index) {
    return namedLibFuncs[index];
}

