#include "writer.h"
#include <string.h>

unsigned *magic;
FILE *generated_file;
char* gen_file_name;

void init_writter(){
    printf("=========================================================\n");
    printf("==================== to binary ==========================\n");
    magic = (unsigned*)malloc(sizeof(unsigned));
    *magic= 194623425; //655*639*465 from 3655 3639 3465
    
    int init_size = strlen(file_name),i = 0,k=0;
	char delim[] = ".";
    char* tok[init_size];
    char* file_name_dup = strdup(file_name);
	char *ptr = strtok(file_name_dup, delim);
    // tok[i++] = strdup(ptr);
        printf("===========================1==============================\n");

	while(ptr != NULL)
	{
        tok[i++] = strdup(ptr);
		ptr = strtok(NULL, delim);
	}
        printf("==========================2===============================\n");

    gen_file_name = (char*)malloc(sizeof(file_name)+5);
        gen_file_name[0]= '\0';
    while(k<i-1){
        strcat(gen_file_name,strdup(tok[k++]));
    }
    sprintf(gen_file_name,"%s%s",gen_file_name,".abc");
    generated_file = fopen(gen_file_name,MODE);
}

void create_avmbinaryfile() {
    init_writter();
    if(!magicnumber())
        fprintf(stderr,"\033[0;31mError writing magicnumber\033[0m\n");
    
    if(!arrays())
        fprintf(stderr,"\033[0;31mError writing arrays\033[0m\n");

    if(!code())
        fprintf(stderr,"\033[0;31mError writing code\033[0m\n");

    printf("<==\033[0;32m %s \033[0m\n",file_name);
    printf("==>\033[0;32m Compilation completed succesfully \033[0m\n");
    printf("==>\033[0;32m %s \033[0m\n",strdup(gen_file_name));
    printf("=========================================================\n");

}


int magicnumber() {
	return fwrite(MAGICNUMBER, sizeof(unsigned), 1, generated_file);
}
int arrays() {
    return strings() && numbers() && userfunctions_gen() && libfunctions();
}
int strings() {
    if(!total_str()){
        fprintf(stderr,"\033[0;31mError writing total\033[0m\n");
        return 0;
    }
    char* buff;
    for(int i = 0 ; i < totalStringConsts ;i++){
        
        buff = string_get(i);
        if(!size(buff)){
            return 0;
        }
        if(!fwrite(buff, sizeof(char), strlen(buff), generated_file)){
            return 0;
        }

    }
    return 1;
}

unsigned total_str() {
    unsigned* total_ptr = (unsigned*)malloc(sizeof(unsigned));
    *total_ptr = totalStringConsts;
    if(!fwrite(total_ptr, sizeof(unsigned), 1, generated_file)){
            return 0;
    }
    return 1;
}

char *string_get(unsigned index) {
    char* buff = strdup(stringConsts[index]);
    assert(buff);
    return buff;
}

unsigned size(char* buff) {
    unsigned* size_ptr = (unsigned*)malloc(sizeof(unsigned));
    *size_ptr = strlen(buff);
    if(!fwrite(size_ptr, sizeof(char), 1, generated_file)){
            return 0;
    }
    return 1;
}

unsigned numbers() {
    //total
    unsigned* total_ptr = (unsigned*)malloc(sizeof(unsigned));
    *total_ptr = totalNumConsts;
    if(!fwrite(total_ptr, sizeof(unsigned), 1, generated_file)){
            return 0;
    }
    for(int i = 0 ; i < totalNumConsts ; i++){
        if(!fwrite(&numConsts[i], sizeof(unsigned), 1, generated_file)){
            return 0;
        } 
    }
    return 1;
}

unsigned userfunctions_gen() {
    //total
    unsigned* total_ptr = (unsigned*)malloc(sizeof(unsigned));
    *total_ptr = totalUserFuncs;
    if(!fwrite(total_ptr, sizeof(unsigned), 1, generated_file)){
            return 0;
    } 
    for(int i = 0 ; i < totalUserFuncs ; i++){
        if(!usrfunc(i)){
            return 0;
        }
    }
    return 1;
    //for each
}

unsigned usrfunc(unsigned index){
    userfunc* iter = (userfunc*)Queue_get(userfunctions,index);
    if(!fwrite(&iter->address, sizeof(unsigned), 1, generated_file)){
        return 0;
    } 
    if(!fwrite(&iter->localSize, sizeof(unsigned), 1, generated_file)){
        return 0;
    }
    char* id_ptr = strdup(iter->id);
    assert(id_ptr);
    unsigned* size_ptr = (unsigned*)malloc(sizeof(unsigned));
    *size_ptr = strlen(id_ptr);
    if(!fwrite(size_ptr, sizeof(unsigned), 1, generated_file)){
        return 0;
    }    
    if(!fwrite(&id_ptr, sizeof(char), strlen(id_ptr), generated_file)){
        return 0;
    }    
    return 1; 
}

unsigned libfunctions(){
    char* buff;
    unsigned* total_ptr = (unsigned*)malloc(sizeof(unsigned));
    *total_ptr = totalNamedLibfuncs;
    if(!fwrite(total_ptr, sizeof(unsigned), 1, generated_file)){
            return 0;
    }
    for(int i = 0 ; i < totalNamedLibfuncs ;i++){
        
        char* buff = strdup((char*)Queue_get(libfuncs,i));
        assert(buff);
        if(!size(buff)){
            return 0;
        }
        if(!fwrite(buff, sizeof(char), strlen(buff), generated_file)){
            return 0;
        }

    }
    return 1;
}

unsigned code(){
    char* buff;
    unsigned* total_ptr = (unsigned*)malloc(sizeof(unsigned));
    *total_ptr = totalInstructions;
    if(!fwrite(total_ptr, sizeof(unsigned), 1, generated_file)){
            return 0;
    }
    for(int i = 0 ; i < totalInstructions ;i++){
        instruction t = instructions[i];
        if(!fwrite(&t.opcode, sizeof(BYTE), 1, generated_file)){
            return 0;
        }
            switch (t.opcode) {
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
                operand_gen(&t.arg2);
            case assign_v:
                operand_gen(&t.arg1);
            case jump_v:
            case call_v:
            case pusharg_v:
            case funcenter_v:
            case funcexit_v:
            case newtable_v:
                operand_gen(&t.result);
            case nop_v:
                break;
            case uminus_v:
            case and_v:
            case or_v:
            case not_v:
                fprintf(stderr,"\033[0;31mError illigal opcode\033[0m\n");
            default:
                fprintf(stderr,"\033[0;31mError invalid opcode\033[0m\n");
                assert(0);
        }
    }
    return 1;
}
unsigned operand_gen(vmarg* v){
    if(!fwrite(&v->type, sizeof(BYTE), 1, generated_file)){
        return 0;
    }
    if(!fwrite(&v->val, sizeof(unsigned), 1, generated_file)){
        return 0;
    }
    return 1;
}