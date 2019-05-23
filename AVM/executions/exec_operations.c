#include "../avm.h"
#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic

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

void execute_arithmetic (struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    //assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
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

// NOT SUPPORTED

void execute_uminus(instruction *instr)
{
    avm_error("invalid type %d", instr->opcode);
}

void execute_and(instruction *instr)
{
    avm_error("invalid type %d", instr->opcode);
}
void execute_or(instruction *instr)
{
    avm_error("invalid type %d", instr->opcode);
}
void execute_not(instruction *instr)
{
    avm_error("invalid type %d", instr->opcode);
}