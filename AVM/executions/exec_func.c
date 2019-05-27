#include "../avm.h"

void execute_call(struct instruction *instr) {
    struct avm_memcell *func = avm_translate_operand(&instr->arg1, &ax);
    assert(func);
    avm_callsaveenvironment();
    char *s;
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
            avm_calllibfunc(func->data.libfuncVal);
            break;
        case table_m:
            avm_error("DES %u %u\n", pc, instr->srcLine);
        default:
            s = avm_tostring(func);
            avm_error("Line %u: Call: cannot bind '%s' to function! PC %d",instr->srcLine, s,pc);
            free(s);
            executionFinished = 1;
            break;
    }
    // printf("%d %u")
}

void execute_funcenter(struct instruction *instr) {
    struct avm_memcell *func = avm_translate_operand(&instr->result, &ax);
    assert(func);
    // assert(pc == func->data.funcVal);

    totalActuals = 0;
    struct userfunc *funcInfo = avm_getfuncinfo(func->data.funcVal);
    topsp = top;
    top = top - funcInfo->localSize;
}

void execute_funcexit(struct instruction *unused) {
    unsigned oldTop = top;
    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    while(++oldTop <= top) avm_memcellclear(&stack[oldTop]);
}

void execute_pusharg(struct instruction *instr) {
    struct avm_memcell *arg = avm_translate_operand(&instr->arg1, &ax);
    avm_assign(&stack[top], arg);
    totalActuals++;
    avm_dec_top();
}