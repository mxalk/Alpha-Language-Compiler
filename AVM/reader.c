#include <stdio.h>
#include <stdlib.h>
#include "avm.h"
#define MAGICNUMBER 194623425 //655*639*465 from 3655 3639 3465
FILE *stream;

void avmbinaryfile() {
    // return magicnumber() && arrays() && code() ;
    magicnumber();
    arrays();
    t_code();
}

magicnumber() {
    unsigned n = readUnsigned();
    return match(n) && n == MAGICNUMBER;
}

arrays() {
    return strings() && numbers() && userfunctions() && libfunctions();
}

//  -------------------------------------------
//  --- GLOBALS -------------------------------
//  -------------------------------------------

strings() {
    unsigned n = total();
    consts_string = malloc(sizeof(char *) * n);
    for (int i = 0; i<n; i++) consts_string[i] = string();
}

numbers() {
    unsigned n = total();
    consts_number = malloc(sizeof(double) * n);
    for (int i = 0; i<n; i++) consts_number[i] = readDouble();
}

userfunctions() {
    unsigned n = total();
    for (int i = 0; i<n; i++) userfunc();
}

libfunctions() {
    unsigned n = total();
    libfuncs_used = malloc(sizeof(char *) * n);
    for (int i = 0; i<n; i++) libfuncs_used[i] = string();
}

//  -------------------------------------------

unsigned userfunc() {
    return address() && localsize() && id();
}

unsigned address() {
    return readUnsigned();
}

unsigned localsize() {
    return readUnsigned();
}


id() {
    return string();
}

//  -------------------------------------------
//  --- CODE ----------------------------------
//  -------------------------------------------

t_code() {
    unsigned n = total();
    struct instruction instr;
    if (!match(n))
        return 0;
    code = malloc(sizeof(struct instruction) * n);
    for (int i = 0; i<n; i++) {
        AVM_WIPEOUT(code[i]);
        instruction(&code[i]);
    }
}

//  -------------------------------------------

instruction(struct instruction *instr) {
    instr->opcode = opcode();
    switch (instr->opcode) {
        case add_v:
        case sub_v:
        case mul_v:
        case div_v:
        case mod_v:
        case jeq_v:
        case jne_v:
        case jle_v:
        case jge_v:
        case jlt_v:
        case jgt_v:
        case tablegetelem_v:
        case tablesetelem_v:
            operand(&instr->arg2);
        case assign_v:
            operand(&instr->arg1);
        case jump_v:
        case call_v:
        case pusharg_v:
        case funcenter_v:
        case funcexit_v:
        case newtable_v:
            operand(&instr->result);
        case nop_v:
            break;
        case uminus_v:
        case and_v:
        case or_v:
        case not_v:
            error("ILLEGAL OPCODE!");
        default:
            error("INVALID OPCODE!");
            assert(0);
    }
    return 1;
}

char opcode() {
    return readByte();
}

operand(struct vmarg *vmarg) {
    vmarg->type = type();
    switch (vmarg->type) {
        case label_a:
        case global_a:
        case formal_a:
        case local_a:
        case number_a:
        case string_a:
        case bool_a:
        case nil_a:
        case userfunc_a:
            vmarg->val = value();
        case retval_a:
            break;
        default:
            error("INVALID VMARG TYPE!");
            assert(0);
    }
    return 1;
}

char type() {
    return readByte();
}

unsigned value() {
    return readUnsigned();
}

//  -------------------------------------------
//  --- GENERIC -------------------------------
//  -------------------------------------------

unsigned total() {
    return readUnsigned();
}

unsigned size() {
    return readUnsigned();
}

//  -------------------------------------------

char *string() {
    unsigned s;
    s = size()+1;
    char *buff = malloc(s);
    if (!fread(buff, sizeof(char), s, stream)) error();
    fgets (buff, s, stream);
    return buff;
}

unsigned readUnsigned() {
    unsigned buff;
    if (!fread(&buff, sizeof(unsigned), 1, stream)) error();
    return buff; 
}

char readDouble() {
    double buff;
    if (!fread(&buff, sizeof(double), 1, stream)) error();
    return buff;
}

char readByte() {
    char buff;
    if (!fread(&buff, sizeof(char), 1, stream)) error();
    return buff;
}

void error(char *error) {
    printf("%s\n", error);
}