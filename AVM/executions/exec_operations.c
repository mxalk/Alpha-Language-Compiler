#include "../avm.h"

// #define execute_add(x) execute_arithmetic(x)
// #define execute_sub(x) execute_arithmetic(x)
// #define execute_mul(x) execute_arithmetic(x)
// #define execute_div(x) execute_arithmetic(x)
// #define execute_mod(x) execute_arithmetic(x)

void execute_add (struct instruction* x){
    execute_arithmetic(x);
};
void execute_sub (struct instruction* x){
    execute_arithmetic(x);
};
void execute_mul (struct instruction* x){
    execute_arithmetic(x);
};
void execute_div (struct instruction* x){
    execute_arithmetic(x);
};  
void execute_mod (struct instruction* x){
    execute_arithmetic(x);
};

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

void execute_uminus(struct instruction *instr) {
    avm_error("Unsuported op: %d", instr->opcode);
}
void execute_and(struct instruction *instr) {
    avm_error("Unsuported op: %d", instr->opcode);
}
void execute_or(struct instruction *instr) {
    avm_error("Unsuported op: %d", instr->opcode);
}
void execute_not(struct instruction *instr) {
    avm_error("Unsuported op: %d", instr->opcode);
}