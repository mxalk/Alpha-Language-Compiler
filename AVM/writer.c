#include <stdio.h>
#include <stdlib.h>
#include "./../Structs/t_libAVM.h"
#define MAGICNUMBER magic
#define FILENAME "test.abc"
#define BYTE char

unsigned usrfunc(unsigned);
void init_writter();
void create_avmbinaryfile();
int magicnumber();
int arrays();
int strings();
char *string_get(unsigned);
unsigned total_str();
unsigned size(char*);
unsigned numbers();
unsigned userfunctions();
unsigned usrfunc(unsigned);
unsigned code();
unsigned operand_gen(vmarg*);

unsigned *magic;
FILE *generated_file;

int main(){
    
    return 0;
}

void init_writter(){
    magic = (unsigned*)malloc(sizeof(unsigned));
    *magic= 194623425; //655*639*465 from 3655 3639 3465
    generated_file = fopen(FILENAME,"wb");
}

void create_avmbinaryfile() {
    // return magicnumber() && arrays() && code() ;
    

    if(!magicnumber())
        fprintf(stderr,"\033[0;31mError writing magicnumber\033[0m\n");
    
    if(!arrays())
        fprintf(stderr,"\033[0;31mError writing arrays\033[0m\n");

    if(!code())
        fprintf(stderr,"\033[0;31mError writing code\033[0m\n");

    printf("\033[0;32mBinary was succesfully generated\n\033[0m");
}


int magicnumber() {
	return fwrite(MAGICNUMBER, sizeof(unsigned), 1, generated_file);
}
int arrays() {
    return strings() && numbers() && userfunctions() && libfunctions();
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
    if(!fwrite(strlen(buff), sizeof(char), 1, generated_file)){
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

unsigned userfunctions() {
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
        
        char* buff = strdup((char*)Queue_get(libfuncs,index));
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
        if(!operand_gen(&t.arg1)&&operand_gen(&t.arg2)&& operand_gen(&t.result)){
            return 0;
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