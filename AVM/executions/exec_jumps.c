#include "../avm.h"

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

void execute_jump (struct instruction *instr) {
    assert(instr->result.type == label_a);
    if (!executionFinished) pc = instr->result.val;
}

void execute_jeq (struct instruction *instr) {
    
    assert(instr->result.type == label_a);
    printf("ar1_instr====%d\n", instr->arg1.type);
    printf("ar2_instr====%d\n", instr->arg2.type);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);
    printf("ar1====%d\n", rv1->type);
    printf("ar2====%d\n", rv2->type);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jeq'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        result = rv1->type == rv2->type;
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        result = avm_tobool(rv1) == avm_tobool(rv2);
    } else if (rv1->type != rv2->type){
      avm_error("%s == %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
        // EQUALITY CHECK WITH DISPATCHING
        avm_error("jeq: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");


    }
    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jne (struct instruction *instr) {

    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jne'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        result = rv1->type != rv2->type;
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        result = avm_tobool(rv1) != avm_tobool(rv2);
    } else if (rv1->type != rv2->type){
      avm_error("%s != %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
         // EQUALITY CHECK WITH DISPATCHING
         avm_error("jne: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");
    }
    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jle (struct instruction *instr) {

    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jle'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        avm_error("'nil' involved in 'jle'");
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        avm_error("'bool' involved in 'jle'");
    } else if (rv1->type != rv2->type){
      avm_error("%s <= %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
         // EQUALITY CHECK WITH DISPATCHING
         avm_error("jle: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");
    }
    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jlt (struct instruction *instr) {

    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jlt'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        avm_error("'nil' involved in 'jlt'");
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        avm_error("'bool' involved in 'jlt'");
    } else if (rv1->type != rv2->type){
      avm_error("%s < %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
         // EQUALITY CHECK WITH DISPATCHING
         avm_error("jlt: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");
    }
    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jge (struct instruction *instr) {
    
    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jge'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        avm_error("'nil' involved in 'jge'");
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        avm_error("'bool' involved in 'jge'");
    } else if (rv1->type != rv2->type){
      avm_error("%s => %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
         // EQUALITY CHECK WITH DISPATCHING
         avm_error("jge: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");
    }
    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jgt (struct instruction *instr) {

    assert(instr->result.type == label_a);
    struct avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);

    unsigned char result = 0;
    if (rv1->type == undef_m || rv2->type == undef_m) {
        avm_error("'undef' involved in 'jgt'");
    } else if (rv1->type == nil_m || rv2->type == nil_m) {
        avm_error("'nil' involved in 'jgt'");
    } else if (rv1->type == bool_m || rv2->type == bool_m) {
        avm_error("'bool' involved in 'jgt'");
    } else if (rv1->type != rv2->type){
      avm_error("%s > %s has illegal types", typeStrings[rv1->type], typeStrings[rv2->type]);
    } else {
         // EQUALITY CHECK WITH DISPATCHING
         avm_error("jgt: EQUAlITY CHECK DISPATCH NOT YET IMPLEMENTED");
    }
    if (!executionFinished && result) pc = instr->result.val;
}