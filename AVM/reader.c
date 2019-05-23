#include <stdio.h>
#include <stdlib.h>
#include "avm.h"
#define MAGICNUMBER 194623425 //655*639*465 from 3655 3639 3465
FILE *stream;
FILE *bin_file;
char* bin_file_name;

int avmbinaryfile() {
    bin_file = fopen(bin_file_name,"r");
    if(!magicnumber()) {
        fprintf(stderr,"\033[0;31mError reading magicnumber\033[0m\n");
        return 0;
    }
    if(!arrays()) {
        fprintf(stderr,"\033[0;31mError reading arrays\033[0m\n");
        return 0;
    }
    if(!t_code()) {
        fprintf(stderr,"\033[0;31mError reading code\033[0m\n");
        return 0;
    }
    printf("=========================================================\n");
    return 1;
}

int magicnumber() {
    unsigned n;
    readUnsigned(&n);
    if (n != MAGICNUMBER) {
        fprintf(stderr,"\033[0;31mMAGIC NUMBER MISMATCH\033[0m\n");
        return 0;
    }
    return 1;
}

int arrays() {
    return arrays_strings() && arrays_numbers() && arrays_userfunctions() && arrays_libfunctions();
}

int arrays_strings() {
    if(!readUnsigned(&totalStringConsts)) {
        fprintf(stderr,"\033[0;31mError reading number of total strings\033[0m\n");
        return 0;
    }
    stringConsts = malloc(sizeof(char *) * totalStringConsts);
    for (int i = 0; i<totalStringConsts; i++) if (!readString(&stringConsts[i])) {
        fprintf(stderr,"\033[0;31mError reading string(%d)\033[0m\n", i);
        return 0;
    }
    return 1;
}

int arrays_numbers() {
    if(!readUnsigned(&totalNumConsts)) {
        fprintf(stderr,"\033[0;31mError reading number of total numbers\033[0m\n");
        return 0;
    }
    numConsts = malloc(sizeof(double) * totalNumConsts);
    for (int i = 0; i<totalNumConsts; i++) if(!readDouble(&numConsts[i])) {
        fprintf(stderr,"\033[0;31mError reading number(%d)\033[0m\n", i);
        return 0;
    }
    return 1;
}

int arrays_userfunctions() {
    if(!readUnsigned(&totalUserFuncs)) {
        fprintf(stderr,"\033[0;31mError reading number of total userfuncs\033[0m\n");
        return 0;
    }
    struct userfunc *iter;
    userFuncs = malloc(sizeof(struct userfunc) * totalUserFuncs);
    for (int i = 0; i<totalUserFuncs; i++) {
        iter = &userFuncs[i];
        if(!readUnsigned(&iter->address)) return 0;
        if(!readUnsigned(&iter->localSize)) return 0;
        if(!readString(&iter->id)) return 0;
    }
    return 1;
}

int arrays_libfunctions() {
     if(!readUnsigned(&totalNamedLibFuncs)) {
        fprintf(stderr,"\033[0;31mError reading number of total libfuncs\033[0m\n");
        return 0;
    }
    namedLibFuncs = malloc(sizeof(char *) * totalNamedLibFuncs);
    for (int i = 0; i<totalNamedLibFuncs; i++) if (!readString(&namedLibFuncs[i])) {
        fprintf(stderr,"\033[0;31mError reading libfunc(%d)\033[0m\n", i);
        return 0;
    }
    return 1;
}

int t_code() {
    if (!readUnsigned(&code_size)) {
        return 0;
    }
    struct instruction *instr;
    code = malloc(sizeof(struct instruction) * code_size);
    for (int i = 0; i<code_size; i++) {
        instr = &code[i];
        if (!readByte(&instr->opcode)) {
            fprintf(stderr,"\033[0;31mError reading instruction(%d) opcode\033[0m\n", i);
            return 0;
        }
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
                if(!operand(&instr->arg2)) {
                    fprintf(stderr,"\033[0;31mError reading instruction(%d) arg2\033[0m\n", i);
                    return 0;
                }
            case assign_v:
                if(!operand(&instr->arg1)) {
                    fprintf(stderr,"\033[0;31mError reading instruction(%d) arg1\033[0m\n", i);
                    return 0;
                }
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
    }
    return 1;
}

int operand(struct vmarg *vmarg) {
    if (!readByte(&vmarg->type)) {
        return 0;
    }
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
            if (!readUnsigned(&vmarg->val)){
                return 0;
            }
        case retval_a:
            break;
        default:
            error("INVALID VMARG TYPE!");
            assert(0);
    }
    return 1;
}

int readString(char *str) {
    unsigned s;
    if (!readUnsigned(&s)) {
        return 0;
    }
    char *str = malloc(s+1);
    if (!fread(str, sizeof(char), s, bin_file)){
        return 0;
    }
    str[s] = '\0';
    return 1;
}

int readUnsigned(unsigned *u) {
    if (!fread(u, sizeof(unsigned), 1, bin_file)) return 0;
    return 1; 
}

int readDouble(double *d) {
    if (!fread(d, sizeof(double), 1, bin_file)) return 0;
    return 1;
}

int readByte(char *c) {
    if (!fread(c, sizeof(char), 1, bin_file)) return 0;
    return 1;
}