#include "writer.h"
#include <string.h>

#define MAGICNUMBER magic_number //655*639*465 from 3655 3639 3465
unsigned magic_number = 194623425;
FILE *bin_file;
char* bin_file_name;

void init_writter() {
    printf("=========================================================\n");
    printf("==================== to binary ==========================\n");
    
    int init_size = strlen(file_name),i = 0,k=0;
	char delim[] = ".";
    char* tok[init_size+1];
    char* file_name_dup = strdup(file_name);
	char *ptr = strtok(file_name_dup, delim);
    // tok[i++] = strdup(ptr);
	while(ptr != NULL)
	{
        tok[i++] = strdup(ptr);
		ptr = strtok(NULL, delim);
	}
    bin_file_name = (char*)malloc(init_size+5);
        bin_file_name[0]= '\0';
    while(k<i-1){
        strcat(bin_file_name,strdup(tok[k++]));
    }
    sprintf(bin_file_name,"%s%s",bin_file_name,".abc");
    bin_file = fopen(bin_file_name,"wb");
}

void avmbinaryfile() {
    init_writter();
    if(!magicnumber()) {
        fprintf(stderr,"\033[0;31mError writing magicnumber\033[0m\n");
        return;
    }
    if(!arrays()) {
        fprintf(stderr,"\033[0;31mError writing arrays\033[0m\n");
        return;
    }
    if(!t_code()) {
        fprintf(stderr,"\033[0;31mError writing code\033[0m\n");
        return;
    }
    printf("<==\033[0;32m %s \033[0m\n",file_name);
    printf("==>\033[0;32m Compilation completed succesfully \033[0m\n");
    printf("==>\033[0;32m %s \033[0m\n",strdup(bin_file_name));
    printf("=========================================================\n");
}

int magicnumber() {
	return writeUnsigned(MAGICNUMBER);
}

int arrays() {
    return arrays_strings() && arrays_numbers() && arrays_userfunctions() && arrays_libfunctions();
}

int arrays_strings() {
    if(!writeUnsigned(totalStringConsts)) {
        fprintf(stderr,"\033[0;31mError writing number of total strings\033[0m\n");
        return 0;
    }
    for(int i = 0 ; i < totalStringConsts ;i++) if (!writeString(stringConsts[i])) {
        fprintf(stderr,"\033[0;31mError writing string(%d)\033[0m\n", i);
        return 0;
    }
    return 1;
}

int arrays_numbers() {
    if(!writeUnsigned(totalNumConsts)) {
        fprintf(stderr,"\033[0;31mError writing number of total numbers\033[0m\n");
        return 0;
    }
    for(int i = 0 ; i < totalNumConsts ; i++) if(!writeDouble(numConsts[i])) {
        fprintf(stderr,"\033[0;31mError writing number(%d)\033[0m\n", i);
        return 0;
    }
    return 1;
}

int arrays_userfunctions() {
    if(!writeUnsigned(totalUserFuncs)) {
        fprintf(stderr,"\033[0;31mError writing number of total userfuncs\033[0m\n");
        return 0;
    }
    userfunc* iter;
    for(int i = 0 ; i < totalUserFuncs ; i++) {
        iter = (userfunc*)Queue_get(userfunctions,i);
        if(!writeUnsigned(iter->address)) return 0;
        if(!writeUnsigned(iter->localSize)) return 0;
        if(!writeString(iter->id)) return 0;
    }
    return 1;
}

int arrays_libfunctions() {
    if(!writeUnsigned(totalNamedLibfuncs)) {
        fprintf(stderr,"\033[0;31mError writing number of total libfuncs\033[0m\n");
        return 0;
    }
    for(int i = 0 ; i < totalNamedLibfuncs ;i++){
        if(!writeString((char*)Queue_get(libfuncs,i))) {
            fprintf(stderr,"\033[0;31mError writing libfunc(%d)\033[0m\n", i);
            return 0;
        }
    }
    return 1;
}

int t_code() {
    char* buff;
    if(!writeUnsigned(totalInstructions)) {
        fprintf(stderr,"\033[0;31mError writing number of total instructions\033[0m\n");
            return 0;
    }
    struct instruction *instr;
    for(int i = 0 ; i < totalInstructions ;i++){
        instr = &instructions[i];
        if(!writeByte(instr->opcode)) {
            fprintf(stderr,"\033[0;31mError writing instruction(%d) opcode\033[0m\n", i);
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
                    fprintf(stderr,"\033[0;31mError writing instruction(%d) arg2\033[0m\n", i);
                    return 0;
                }
            case assign_v:
                if(!operand(&instr->arg1)) {
                    fprintf(stderr,"\033[0;31mError writing instruction(%d) arg1\033[0m\n", i);
                    return 0;
                }
            case jump_v:
            case call_v:
            case pusharg_v:
            case funcenter_v:
            case funcexit_v:
            case newtable_v:
                if(!operand(&instr->result)) {
                    fprintf(stderr,"\033[0;31mError writing instruction(%d) result\033[0m\n", i);
                    return 0;
                }
            case nop_v:
                break;
            case uminus_v:
            case and_v:
            case or_v:
            case not_v:
                fprintf(stderr,"\033[0;31mError writing instruction(%d), illegal opcode\033[0m\n", i);
            default:
                fprintf(stderr,"\033[0;31mError writing instruction(%d), invalid opcode\033[0m\n", i);
                assert(0);
        }
    }
    return 1;
}

int operand(vmarg *v) {
    if(!writeByte(v->type)) {
        fprintf(stderr,"\033[0;31mError writing operand type\033[0m\n");
        return 0;
    }
    if(!writeUnsigned(v->val)) {
        fprintf(stderr,"\033[0;31mError writing operand value\033[0m\n");
        return 0;
    }
    return 1;
}

int writeString(char *str) {
    unsigned s = strlen(str);
    writeUnsigned(s);
    if (!fwrite(str, sizeof(char), s, bin_file)) return 0;
    return 1;
}

int writeUnsigned(unsigned u) {
    if (!fwrite(&u, sizeof(unsigned), 1, bin_file)) return 0;
    return 1; 
}

int writeDouble(double d) {
    if (!fwrite(&d, sizeof(double), 1, bin_file)) return 0;
    return 1;
}

int writeByte(char b) {
    if (!fwrite(&b, sizeof(char), 1, bin_file)) return 0;
    return 1;
}