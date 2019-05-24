#include "../avm.h"
#define N AVM_STACKSIZE

void execute_assign(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);
    assert(lv && (&stack[N-1] >= lv && lv > &stack[top] || lv == &retval));
    assert(rv);
    avm_assign(lv, rv);
}

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