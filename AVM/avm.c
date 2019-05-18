// ---------------------------------------------------------------------------
// INSTRUCTION SET
// ---------------------------------------------------------------------------
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
    label_a,
    global_a,
    formal_a,
    local_a,
    number_a,
    string_a,
    bool_a,
    nil_a,
    userfunc_a,
    libfunc_a,
    retval_a
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

double *numConsts;
unsigned totalNumConsts;
char **stringConsts;
unsigned totalStringConsts;
char **namedLibFuncs;
unsigned totalNamedLibFuncs;
struct userfunc *userFuncs;
unsigned totalUserFuncs;

// MEMORY
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))

struct avm_table;
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


struct avm_memcell stack[AVM_STACKSIZE];


struct avm_memcell *avm_translate_operand (struct vmarg *arg, struct avm_memcell *reg) {
    switch (arg->type) {
        // VARIABLES
        case global_a: return &stack[AVM_STACKSIZE-1-arg->val];
        case local_a: return &stack[topsp-arg->val];
        case formal_a: return &stack[topsp+AVM_STACKENV_SIZE+1+arg->val];
        case retval_a: return &retval;
        // CONSTANTS
        case number_a:
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;
        case string_a:
            reg->type = string_m;
            reg->data.strVal = consts_getstring(arg->val);
            return reg;
        case bool_a:
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        case nil_a:
            reg->type = nil_m;
            return reg;
        //  FUNCTIONS
        case userfunc_a:
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        case libfunc_a:
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;

    }
}
// ---------------------------------------------------------------------------
// OPERAND TRANSLATE
// ---------------------------------------------------------------------------
#define AVM_STACKENV_SIZE 4
struct avm_memcell ax, bx, cx, retval;
unsigned top, topsp;
double consts_getnumber(unsigned index);
char *consts_getstring(unsigned index);
char *libfuncs_getused(unsigned index);


// ---------------------------------------------------------------------------
// DYNAMIC ARRAYS
// ---------------------------------------------------------------------------
#define AVM_TABLE_HASHSIZE 211

struct avm_table_bucket {
    struct avm_memcell key;
    struct avm_memcell value;
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

void avm_tableincrefcounter(struct avm_table *t) {
    ++t->refCounter;
}

void avm_tabledecrefcounter(struct avm_table *t) {
    assert(t->refCounter > 0);
    if (!--t->refCounter) avm_tabledestroy(t);
}

void avm_tablebucketsinit(struct avm_table_bucket **p) {
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++) p[i] = (struct avm_table_bucket *) 0;
}

struct avm_table *avm_tablenew(void) {
    struct avm_table *t = (struct avm_table *) malloc(sizeof(struct avm_table));
    AVM_WIPEOUT(*t);
    t->refCounter = t->total = 0;
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->ufncIndexed);
    avm_tablebucketsinit(t->lfncIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    return t;
}

void avm_memcellclear (struct avm_memcell *m);

void avm_tablebucketsdestroy(struct avm_table_bucket **p) {
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++, p++) {
        for (struct avm_table_bucket *b = *p; b;) {
            struct avm_table_bucket *del = b;
            b = b->next;
            avm_memcellclear(&del->key);
            avm_memcellclear(&del->value);
            free(del);
        }
        p[i] = (struct avm_table_bucket *) 0;
    }
}

void avm_tabledestroy(struct avm_table *t) {
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->ufncIndexed);
    avm_tablebucketsdestroy(t->lfncIndexed);
    avm_tablebucketsdestroy(t->boolIndexed);
    free(t);
}

// ---------------------------------------------------------------------------
// DISPATCHER
// ---------------------------------------------------------------------------
typedef void (*execute_func_t)(struct instruction *);
#define AVM_MAX_INSTRUCTIONS (unsigned) nop_v

extern void execute_assign (struct instruction*);
extern void execute_add (struct instruction*);
extern void execute_sub (struct instruction*);
extern void execute_mul (struct instruction*);
extern void execute_div (struct instruction*);
extern void execute_div (struct instruction*);
extern void execute_mod (struct instruction*);
extern void execute_mod (struct instruction*);
extern void execute_uminus (struct instruction*);
extern void execute_and (struct instruction*);
extern void execute_and (struct instruction*);
extern void execute_or (struct instruction*);
extern void execute_not (struct instruction*);
extern void execute_jeq (struct instruction*);
extern void execute_jne (struct instruction*);
extern void execute_jle (struct instruction*);
extern void execute_jge (struct instruction*);
extern void execute_jge (struct instruction*);
extern void execute_jlt (struct instruction*);
extern void execute_jgt (struct instruction*);
extern void execute_call (struct instruction*);
extern void execute_pusharg (struct instruction*);
extern void execute_funcenter (struct instruction*);
extern void execute_funcexit (struct instruction*);
extern void execute_newtable (struct instruction*);
extern void execute_tablegetelem (struct instruction*);
extern void execute_tablesetelem (struct instruction*);
extern void execute_nop (struct instruction*);

execute_func_t executeFuncs[] = {
    execute_assign,
    execute_add,
    execute_sub,
    execute_mul,
    execute_div,
    execute_mod,
    execute_uminus,
    execute_and,
    execute_or,
    execute_not,
    execute_jeq,
    execute_jne,
    execute_jle,
    execute_jge,
    execute_jlt,
    execute_jgt,
    execute_call,
    execute_pusharg,
    execute_funcenter,
    execute_funcexit,
    execute_newtable,
    execute_tablegetelem,
    execute_tablesetelem,
    execute_nop
};

unsigned char executionFinished = 0;
unsigned pc = 0;
unsigned currLine = 0;
unsigned codeSize = 0;
unsigned totalActuals = 0;
struct instruction *code = (struct instruction *) 0;
#define AVM_ENDING_PC codeSize

void execution_cycle (void) {
    if (executionFinished) return;
    if (pc == AVM_ENDING_PC) {
        executionFinished = 1;
        return;
    }
    assert(pc < AVM_ENDING_PC);
    struct instruction *instr = code + pc;
    assert(instr->opcode >=0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
    if (instr->srcLine) currLine = instr->srcLine;
    unsigned oldPC = pc;
    (*executeFuncs[instr->opcode])(instr);
    if (pc == oldPC) ++pc;
}

// ---------------------------------------------------------------------------
// INSTRUCTION IMPLEMENTATION
// ---------------------------------------------------------------------------
typedef void (*memclear_func_t)(struct avm_memcell *);

extern void memclear_string (struct avm_memcell *m) {
    assert(m->data.strVal);
    free(m->data.strVal);
}

extern void memclear_table (struct avm_memcell *m) {
    assert(m->data.tableVal);
    avm_tabledecrefcounter(m->data.tableVal);
}
memclear_func_t memclearFuncs[] = {
    0, // NUMBER
    memclear_string,
    0, // BOOLEAN
    memclear_table,
    0, // USERFUNC
    0, // LIBFUNC
    0, // NIL
    0  // UNDEF
};

void avm_memcellclear(struct avm_memcell *m) {
    if (m->type != undef_m) {
        memclear_func_t f = memclearFuncs[m->type];
        if (f) (*f)(m);
        m->type = undef_m;
    }
}

extern void avm_warning(char *format, ...);

void execute_assign(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);
    assert(lv && (&stack[N-1] >= lv && lv > &stack[top] || lv == &retval));
    assert(rv);
    avm_assign(lv, rv);
}

// extern void avm_assign(struct avm_memcell *lv, struct avm_memcell *rv);
void avm_assign (struct avm_memcell *lv, struct avm_memcell *rv) {
    if (lv == rv) return;
    if (lv->type == table_m && rv->type == table_m && lv->data.tableVal == rv->data.tableVal) return;
    if (rv->type == undef_m) avm_warning("Assigning from 'undef' content!");
    avm_memcellclear(lv);
    memcpy(lv, rv, sizeof(struct avm_memcell));
    if (lv->type == string_m)
        lv->data.strVal = strdup(rv->data.strVal);
    else if (lv->type == string_m)
        avm_tableincrefcounter(lv->data.tableVal);
}

extern void avm_error(char *format, ...);
extern void avm_calllibfunc(char *funcName);

// extern void avm_callsaveenvironment(void);
void avm_callsaveenvironment (void) {
    avm_push_envvalue(totalActuals);
    avm_push_envvalue(pc+1);
    avm_push_envvalue(top + totalActuals + 2);
    avm_push_envvalue(topsp);
}

void execute_call(struct instruction *instr) {
    struct avm_memcell *func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    avm_callsaveenvironment();
    switch (func->type) {
        case userfunc_m:
            pc = func->data.funcVal;
            assert(pc < AVM_ENDING_PC);
            assert(code[pc].opcode == funcenter_v);
            break;
        case string_m:
            avm_calllibfunc(func->data.strVal);
            break;
        case libfunc_m:
            avm_calllibfunc(func->data.funcVal);
            break;
        case table_m:
            avm_calllibfunc(func->data.tableVal);
            break;
        default:
            char *s = avm_tostring(func);
            avm_error("Call: cannot bind '%s' to function!", s);
            free(s);
            executionFinished = 1;
    }
}

void avm_dec_top(void) {
    if (!top) {
        // STACK OVERFLOW
        avm_error("Stack Overflow!");
        executionFinished = 1;
        return;
    }
    top--;
}

void avm_push_envvalue(unsigned val) {
    stack[top].type = number_m;
    stack[top].data.numVal = val;
    avm_dec_top();
}

void avm_callsaveenvironment(void) {
    avm_push_envvalue(totalActuals);
    avm_push_envvalue(pc + 1);
    avm_push_envvalue(top + totalActuals + 2);
    avm_push_envvalue(topsp);
}

extern struct usrfunc *avm_getfuncinfo(unsigned address);

void execute_funcenter(struct instruction *instr) {
    struct avm_memcell *func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    assert(pc == func->data.funcVal);

    totalActuals = 0;
    struct userfunc *funcInfo = avm_getfuncinfo(pc);
    topsp = top;
    top = top - funcInfo->localSize;
}

unsigned avm_get_envvalue(unsigned i) {
    assert(stack[i].type = number_m);
    unsigned val = (unsigned) stack[i].data.numVal;
    assert(stack[i].data.numVal == ((double) val));
    return val;
}

void execute_funcexit(struct instruction *unused) {
    unsigned oldTop = top;
    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    while(++oldTop <= top) avm_memcellclear(&stack[oldTop]);
}

typedef void (*library_func_t)(void);
library_func_t avm_getlibraryfunc(char *id);

void avm_calllibfunc(char *id) {
    library_func_t f = avm_getlibraryfunc(id);
    if (!f) {
        avm_error("Unsupported lib func '%s' called!", id);
        executionFinished = 1;
    } else {
        topsp = top;
        totalActuals = 0;
        (*f)();
        if (!executionFinished) execute_funcexit((struct instruction *) 0);
    }
}

unsigned avm_totalactuals(void) {
    return avm_get
    (topsp + AVM_NUMACTUALS_OFFSET);
}

struct avm_memcell *avm_getactual(unsigned i) {
    assert(i < avm_totalactuals());
    return &stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}

void avm_registerlibfunc(char *id, library_func_t addr);

void execute_pusharg(struct instruction *instr) {
    struct avm_memcell *arg = avm_translate_operand(&instr->arg1, &ax);
    avm_assign(&stack[top], arg);
    totalActuals++;
    avm_dec_top();
}

// ------------------- STRINGS
typedef char * (*tostring_func_t)(struct avm_memcell *);

extern char *number_tostring(struct avm_memcell *);
extern char *string_tostring(struct avm_memcell *);
extern char *bool_tostring(struct avm_memcell *);
extern char *table_tostring(struct avm_memcell *);
extern char *userfunc_tostring(struct avm_memcell *);
extern char *libfunc_tostring(struct avm_memcell *);
extern char *nil_tostring(struct avm_memcell *);
extern char *undef_tostring(struct avm_memcell *);

tostring_func_t tostringFuncs[] = {
    number_tostring,
    string_tostring,
    bool_tostring,
    table_tostring,
    userfunc_tostring,
    libfunc_tostring,
    nil_tostring,
    undef_tostring
};

// extern char *avm_tostring(struct avm_memcell *);
char *avm_tostring(struct avm_memcell *m) {
    assert(m->type >= 0 && m->type <= undef_m);
    return (*tostringFuncs[m->type])(m);
}

// ------------------- ARITHMETIC
typedef double (*arithmetic_func_t)(double x, double y);

double add_impl (double x, double y) { return x+y; }
double sub_impl (double x, double y) { return x-y; }
double mul_impl (double x, double y) { return x*y; }
double div_impl (double x, double y) { return x/y; }
double mod_impl (double x, double y) { return ((unsigned) x) % ((unsigned) y); }

arithmetic_func_t arithmeticFuncs[] = {
    add_impl,
    sub_impl,
    mul_impl,
    div_impl,
    mod_impl
};

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic
void execute_arithmetic (struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
    assert(rv1 && rv2);

    if (rv1->type != number_m || rv2->type != number_m) {
        avm_error("Not a number in arithmetic!");
        executionFinished = 1;
        return;
    }
    arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];
    avm_memcellclear(lv);
    lv->type = number_m;
    lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
}

// ------------------- BOOLEAN

typedef unsigned char (*tobool_func_t)(struct avm_memcell *);

unsigned char number_tobool (struct avm_memcell *m) {return m->data.numVal != 0;}
unsigned char string_tobool (struct avm_memcell *m) {return m->data.strVal[0] != 0;}
unsigned char bool_tobool (struct avm_memcell *m) {return m->data.boolVal;}
unsigned char table_tobool (struct avm_memcell *m) {return 1;}
unsigned char userfunc_tobool (struct avm_memcell *m) {return 1;}
unsigned char libfunc_tobool (struct avm_memcell *m) {return 1;}
unsigned char nil_tobool (struct avm_memcell *m) {return 0;}
unsigned char undef_tobool (struct avm_memcell *m) {assert(0);return 0;}

tobool_func_t toboolFuncs[] = {
    number_tobool,
    string_tobool,
    bool_tobool,
    table_tobool,
    userfunc_tobool,
    libfunc_tobool,
    nil_tobool,
    undef_tobool
};

unsigned char avm_tobool(struct avm_memcell *m) {
    assert(m->type >= 0 && m->type < undef_m);
    return (*toboolFuncs[m->type])(m);
}

// ------------------- COMPARISON

char *typeStrings[] = {
    "number",
    "string",
    "bool",
    "table",
    "userfunc",
    "libfunc",
    "nil",
    "undef"
};

void execute_jeq (struct instruction *instr) {
    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) avm_error("'undef' involved in equality");
    else if (rv1->type == nil_m || rv2->type == nil_m) result = rv1->type == rv2->type;
    else if (rv1->type == bool_m || rv2->type == bool_m) result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if (rv1->type != rv2->type) avm_error("%s == %s is illegal", typeStrings[rv1->type], typeStrings[rv2->type]);
    else {
        // EQUALITY CHECK WITH DISPATCHING
    }
    if (!executionFinished && result) pc = instr->result.val;
}


// ------------------- TABLE

void execute_newtable(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}

struct avm_memcell *avm_tablegetelem (struct avm_table *table, struct avm_memcell *index);
void avm_tablesetelem (struct avm_table *table, struct avm_memcell *index, struct avm_memcell *content);

void execute_tablegetelem(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *t = avm_translate_operand(&instr->arg1, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg2, &ax);
    assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
    assert(t && &stack[N-1] >= t && t < &stack[top]);
    assert(i);
    avm_memcell(lv);
    lv->type = nil_m;
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table!", typeStrings[t->type]);
        return;
    }
    struct avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
    if (content) {
        avm_assign(lv, content);
        return;
    }
    char *ts = avm_tostring(t);
    char *is = avm_tostring(i);
    avm_warning("\'%s[%s]\' not found!", ts, is);
    free(ts);
    free(is);
}

void execute_tablesetelem(struct instruction *instr) {
    struct avm_memcell *t = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *c = avm_translate_operand(&instr->arg2, &bx);
    assert(t && &stack[N-1] >= t && &stack[top]);
    assert(i && c);
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table", t->type);
        return;
    }
    avm_tablesetelem(t->data.tableVal, i, c);
}

// ---------------------------------------------------------------------------
// AVM
// ---------------------------------------------------------------------------


void avm_initialize (void) {
    avm_initstack();
    avm_registerlibfunc("print", libfunc_print);
    avm_registerlibfunc("typeof", libfunc_typeof);
}

// ------------------- LIBS

void libfunc_print(void) {
    unsigned n = avm_totalactuals();
    for (unsigned i = 0; i<n; i++) {
        char *s = avm_tostring(avm_getactual(i));
        puts(s);
        free(s);
    }
}

void libfunc_typeof(void) {
    unsigned n = avm_totalactuals();
    if (n!=1) {
        avm_error("One argument (not %d) exprected in \'typeof\'!", n);
        return;
    }
    avm_memcellclear(&retval);
    retval.type = string_m;
    retval.data.strVal = strdup(typeStrings[avm_getactual(0)->type]);
}

void libfunc_totalarguments(void) {
    unsigned p_topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    avm_memcellclear(&retval);
    if (!p_topsp) {
        avm_error("'totalarguments' call outside a function!");
        retval.type = nil_m;
        return;
    }
    retval.type = number_m;
    retval.data.numVal = avm_get_envvalue(p_topsp + AVM_NUMACTUALS_OFFSET);
}
