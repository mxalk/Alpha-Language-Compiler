#include "t_libAVM.h"

#define EXPAND_SIZE 1024
#define CURR_SIZE (total * sizeof(struct instruction))
#define NEW_SIZE (EXPAND_SIZE * sizeof(struct instruction) + CURR_SIZE)
#define false 0
#define true 1

struct instruction *instructions = (struct instruction *)0;

unsigned int totalInstructions = 0;
unsigned int currInstruction = 0;
unsigned int currprocessedquads;
Queue *userfunctions;
Stack *funcstack;

void userfunctions_add(unsigned address, unsigned localSize, char *id)
{
    userfunc *new_ufunc = (userfunc *)malloc(sizeof(userfunc));
    new_ufunc->address = address;
    new_ufunc->id = strdup(id);
    new_ufunc->localSize = localSize;
    Queue_Node *new_ufunc_node = (Queue_Node *)malloc(sizeof(Queue_Node));
    new_ufunc_node->content = (userfunc *)new_ufunc;
    Queue_enqueue(userfunctions, new_ufunc_node);
}

void push_funcstack(SymbolTableRecord *sym)
{
    Stack_Node *new_ufunc_node = (Stack_Node *)malloc(sizeof(Stack_Node));
    new_ufunc_node->content = (SymbolTableRecord *)sym;
    Stack_append(funcstack, new_ufunc_node);
}

unsigned int nextinstructionlabel()
{
    return currInstruction;
}

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
    generate_NOP};

void emit_instr(instruction *t)
{
    // if(currInstruction == totalInstructions)
    // 	expand_instructions(); // tbchanged

    instruction *inst = instructions + currInstruction++;
    inst->opcode = t->opcode;
    inst->result = t->result;
    inst->arg1 = t->arg1;
    inst->arg2 = t->arg2;
    inst->srcLine = t->srcLine;
}

void reset_operand(vmarg *arg)
{
    arg = NULL;
}

void generate(vmopcode op, Quad *quad)
{
    instruction t;
    t.opcode = op;
    make_operand(quad->arg1, &t.arg1);
    make_operand(quad->arg2, &t.arg2);
    make_operand(quad->result, &t.result);
    quad->taddress = nextinstructionlabel();
    emit_instr(&t);
}

void generateCode(void) // main target code function
{
    //init
    funcstack = Stack_init();
    userfunctions = Queue_init();
    //init
    unsigned int i;
    for (i = 0; i < currQuad; i++)
    {
        assert(quads + i);
        (*generators[quads[i].op])(quads + i);
    }
    patch_incomplete_jumps();
}
#if 1
void make_operand(Expr *e, vmarg *arg)
{
    switch (e->type)
    {
    case var_e:
    case tableitem_e:
    case arithexpr_e:
    case boolexpr_e:
    case newtable_e:
    {
        arg->val = e->sym->offset;
        switch (e->sym->space)
        {
        case programvar:
            arg->type = global_a;
            break;
        case functionlocal:
            arg->type = local_a;
            break;
        case formalarg:
            arg->type = formal_a;
            break;
        default:
            assert(0);
        }
    }
    case constbool_e:
    {
        arg->val = e->value.boolConst;
        arg->type = bool_a;
        break;
    }
    case conststring_e:
    {
        arg->val = consts_newstring(e->value.strConst);
        arg->type = string_a;
        break;
    }
    case constnum_e:
    {
        arg->val = consts_newnumber(e->value.numConst);
        arg->type = number_a;
        break;
    }
    case nil_e:
    {
        arg->type = nil_a;
        break;
    }
    case programfunc_e:
    {
        // printf("in tcode address is:%d\n",e->sym->taddress);
        arg->val = userfuncs_newfunc(e->sym);
        arg->type = userfunc_a;
        break;
    }
    case libraryfunc_e:
    {
        arg->type = libfunc_a;
        arg->val = libfuncs_newused(e->sym->name);
        break;
    }
    default:
        assert(0);
    }
}
#endif

void patch_incomplete_jumps()
{
    int i;
    incomplete_jump *iter = ij_head;
    for (i = 1; i < ij_total; i++)
    {
        // if (iter->iaddress = intermediate code size)
        //     instructions[iter->instrNo].result = target code size;
        // else
        //     instructions[iter->instrNo].result = quads[iter->iaddress].taddress;

        iter = iter->next;
    }
}

void generate_relational(vmopcode op, Quad *quad)
{
    instruction t;
    t.opcode = op;
    make_operand(quad->arg1, &t.arg1);
    make_operand(quad->arg2, &t.arg2);
    t.result.type = label_a;
    if (quad->label < currprocessedquads)
        t.result.val = quads[quad->label].taddress;
    else
        add_incomplete_jump(nextinstructionlabel(), quad->label);
    quad->taddress = nextinstructionlabel();
    emit_instr(&t);
}

void generate_NOT(Quad *quad)
{

    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, false);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 3;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 2;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);
}

void generate_OR(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 4;
    emit_instr(&t);

    make_operand(quad->arg2, &t.arg1);
    t.result.val = nextinstructionlabel() + 3;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 2;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    // similar impl for AND
} //funcstack impl for bellow

void generate_PARAM(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = pusharg_v;
    make_operand(quad->arg1, &t.arg1);
    emit_instr(&t);
}
void generate_CALL(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = call_v;
    make_operand(quad->arg1, &t.arg1);
    emit_instr(&t);
}
void generate_GETRETVAL(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = assign_v;
    make_operand(quad->result, &t.result);
    make_retvaloperand(&t.arg1);
    emit_instr(&t);
}

void generate_FUNCSTART(Quad *q)
{
    SymbolTableRecord *f = q->result->sym;
    f->taddress = nextinstructionlabel();
    q->taddress = nextinstructionlabel();
    userfunctions_add(f->taddress, f->totallocals, f->name);
    instruction t;
    t.opcode =funcenter_v;
    make_operand(q->result,&t.result);
    emit_instr(&t); 
    return;
}
void generate_RETURN(Quad *q)
{
    return;
}
void generate_FUNCEND(Quad *q)
{
    return;
}

void generate_ADD(Quad *q)
{
    generate(add_v, q);
    return;
}
void generate_SUB(Quad *q)
{
    generate(sub_v, q);
    return;
}
void generate_MUL(Quad *q)
{
    generate(mul_v, q);
    return;
}
void generate_DIV(Quad *q)
{
    generate(div_v, q);
    return;
}
void generate_MOD(Quad *q)
{
    generate(mod_v, q);
    return;
}

void generate_UMINUS(Quad *q)
{
    return;
}

void generate_NEWTABLE(Quad *q)
{
    generate(newtable_v, q);
    return;
}
void generate_TABLEGETELEM(Quad *q)
{
    generate(sub, q);
    return;
}
void generate_TABLESETELEM(Quad *q)
{
    generate(sub, q);
    return;
}
void generate_ASSIGN(Quad *q)
{
    generate(sub, q);
    return;
}
void generate_NOP(Quad *q)
{
    instruction t;
    t.opcode = nop_v;
    emit_instr(&t);
    return;
}

void generate_JUMP(Quad *q)
{
    generate_relational(jump_v, q);
    return;
}
void generate_IF_EQ(Quad *q)
{
    generate_relational(jeq_v, q);
    return;
}
void generate_IF_NOTEQ(Quad *q)
{
    generate_relational(jne_v, q);
    return;
}
void generate_IF_GREATER(Quad *q)
{
    generate_relational(jgt_v, q);
    return;
}
void generate_IF_GREATEREQ(Quad *q)
{
    generate_relational(jge_v, q);
    return;
}
void generate_IF_LESS(Quad *q)
{
    generate_relational(jlt_v, q);
    return;
}
void generate_IF_LESSEQ(Quad *q)
{
    generate_relational(jle_v, q);
    return;
}
void generate_AND(Quad *q)
{
    return;
}

void display_geao()
{
    printf("Geia");
}

unsigned consts_newstring(char *s)
{
}
unsigned consts_newnumber(double n)
{
}
unsigned libfuncs_newused(char *s)
{
}

unsigned userfuncs_newfunc(SymbolTableRecord *sym)
{
}

void make_numberoperand(vmarg *arg, double val)
{
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}
void make_booloperand(vmarg *arg, unsigned val)
{
    arg->val = val;
    arg->type = bool_a;
}
void make_retvaloperand(vmarg *arg)
{
    arg->type = retval_a;
}

void add_incomplete_jump(unsigned insrtNo, unsigned iaddress)
{
    return;
}
