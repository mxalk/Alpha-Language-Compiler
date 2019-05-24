#include "../avm.h"

void execute_newtable(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    //assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}

struct avm_memcell *avm_tablegetelem (struct avm_table *table, struct avm_memcell *index){
    avm_error("avm_tablegetelem NOT YET IMPLEMENTED\n");

}
void avm_tablesetelem (struct avm_table *table, struct avm_memcell *index, struct avm_memcell *content){
    avm_error("avm_tablesetelem NOT YET IMPLEMENTED\n");
}

void execute_tablegetelem(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *t = avm_translate_operand(&instr->arg1, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg2, &ax);
    //assert(lv && (&stack[N-1] >= lv && lv < &stack[top] || lv == &retval));
    //assert(t && &stack[N-1] >= t && t < &stack[top]);
    // assert(i);
    avm_memcellclear(lv);
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
    //assert(t && &stack[N-1] >= t && &stack[top]);
    assert(i && c);
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table", t->type);
        return;
    }
    avm_tablesetelem(t->data.tableVal, i, c);
}