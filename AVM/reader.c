#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "avm.h"
#include "reader.h"


// int main(){
//     bin_file_name = "temp.abc";
//     avmbinaryfile();
//     printf("STRINGS\n");
//     for (unsigned i = 0; i<totalStringConsts; i++) {
//         printf("%u: %s\n", i, consts_getstring(i));
//     }
//     printf("NUMCONSTS\n");
//     for (unsigned i = 0; i<totalNumConsts; i++) {
//         printf("%u: %f\n", i, consts_getnumber(i));
//     }
//     printf("USRFUNCS\n");
//     for (unsigned i = 0; i<totalUserFuncs; i++) {
//         printf("%u: %u %u %s\n", i,userFuncs[i].address,userFuncs[i].localSize,userFuncs[i].id);
        
//     }
//     printf("LIBFUNCS\n");
//     for (unsigned i = 0; i<totalNamedLibFuncs; i++) {
//         printf("%u: %s\n", i, libfuncs_getused(i));
//     }
//     printf("\n");
//     return 0;
// }

int avmbinaryfile() {
    bin_file = fopen(bin_file_name,"rb");
    if (!bin_file) {
        avm_error("BINARY FILE ERROR");
        return 0;
    }
    if(!magicnumber()) {
        avm_error("Error reading magicnumber");
        return 0;
    }
    if(!arrays()) {
        avm_error("Error reading arrays");
        return 0;
    }
    if(!t_code()) {
        avm_error("Error reading code");
        return 0;
    }
    printf("=========================================================\n");
    return 1;
}

int magicnumber() {
    unsigned n;
    if(!readUnsigned(&n)) return 0;
    printf("=========================================================\n");
    if (n != MAGICNUMBER) {
        avm_error("MAGIC NUMBER MISMATCH");
        return 0;
    }
    return 1;
}

int arrays() {
    return arrays_strings() && arrays_numbers() && arrays_userfunctions() && arrays_libfunctions();
}

int arrays_strings() {
    if(!readUnsigned(&totalStringConsts)) {
        avm_error("Error reading number of total strings");
        return 0;
    }
    stringConsts =(char **) malloc(sizeof(char *) * totalStringConsts);
    for (int i = 0; i<totalStringConsts; i++) if (!readString(&stringConsts[i])) {
        avm_error("Error reading string(%d)", i);
        return 0;
    }
    printf("totalStringConsts: %u '%s'\n", totalStringConsts, stringConsts[0]);
    return 1;
}

int arrays_numbers() {
    if(!readUnsigned(&totalNumConsts)) {
        avm_error("Error reading number of total numbers");
        return 0;
    }
    numConsts = malloc(sizeof(double) * totalNumConsts);
    for (int i = 0; i<totalNumConsts; i++) if(!readDouble(&numConsts[i])) {
        avm_error("Error reading number(%d)", i);
        return 0;
    }
    return 1;
}

int arrays_userfunctions() {
    if(!readUnsigned(&totalUserFuncs)) {
        avm_error("Error reading number of total userfuncs");
        return 0;
    }
    struct userfunc *iter;
    userFuncs = malloc(sizeof(struct userfunc) * totalUserFuncs);
    for (int i = 0; i<totalUserFuncs; i++) {
        iter = &userFuncs[i];
        if(!readUnsigned(&iter->address)) return 0;
        if(!readUnsigned(&iter->localSize)) return 0;
        if(!readString(&iter->id)) return 0;
        iter->address++;
    }
    return 1;
}

int arrays_libfunctions() {
     if(!readUnsigned(&totalNamedLibFuncs)) {
        avm_error("Error reading number of total libfuncs");
        return 0;
    }
    namedLibFuncs = (char**)malloc(sizeof(char *) * totalNamedLibFuncs);
    for (int i = 0; i<totalNamedLibFuncs; i++) if (!readString(&namedLibFuncs[i])) {
        avm_error("Error reading libfunc(%d)", i);
        return 0;
    }
    return 1;
}

int t_code() {
    if (!readUnsigned(&GlobalProgrammVarOffset)) {
        avm_error("Error reading number of globals");
        return 0;
    }
    if (!readUnsigned(&codeSize)) {
        avm_error("Error reading number of total instructions");
        return 0;
    }
    struct instruction *instr;
    code = (struct instruction*)malloc(sizeof(struct instruction) * codeSize);
printf("currInstr / totalInstr : opcode \n");
    for (int i = 0; i<codeSize; i++) {
        instr = &code[i];
        if (!readByte((char *)&instr->opcode)) {
            avm_error("Error reading instruction(%d) opcode", i);
            return 0;
        }
printf("%d/%d:%d = ",i,codeSize-1,instr->opcode);
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
                    avm_error("Error reading instruction(%d) arg2", i);
                    return 0;
                }
            case assign_v:
                if(!operand(&instr->arg1)) {
                    avm_error("Error reading instruction(%d) arg1", i);
                    return 0;
                }
            case jump_v:
            case funcenter_v:
            case funcexit_v:
                if(!operand(&instr->result)) {
                    avm_error("Error reading instruction(%d) arg1", i);
                    return 0;
                }
                break;
            case call_v:
            case pusharg_v:
            case newtable_v:
                if(!operand(&instr->arg1)) {
                    avm_error("Error reading instruction(%d) arg1", i);
                    return 0;
                }
            case nop_v:
                break;
            case uminus_v:
            case and_v:
            case or_v:
            case not_v:
                avm_error("Error reading instruction(%d), illegal opcode", i);
            default:
                avm_error("Error reading instruction(%d), invalid opcode", i);
                assert(0);
        }
printf(" res: %u, a1: %u, a2: %u\n",instr->result.type,instr->arg1.type,instr->arg2.type);
    }
    return 1;
}

int operand(struct vmarg *vmarg) {
    if (!readByte((char *)&vmarg->type)) {
        avm_error("Error reading operand type");
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
        case libfunc_a:
            if (!readUnsigned(&vmarg->val)){
                avm_error("Error reading operand value");
                return 0;
            }
        case retval_a:
            break;
        default:
            avm_error("Error invalid vmarg type(%u)", vmarg->type);
            return 0;
    }
    return 1;
}

int readString(char **str) {
    unsigned s;
    if (!readUnsigned(&s)) {
        return 0;
    }
    *str = (char *)malloc(s+1);
    if (!fread(*str, sizeof(char), s, bin_file)){
        return 0;
    }
    (*str)[s] = '\0';
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