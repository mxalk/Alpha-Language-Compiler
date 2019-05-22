#include <stdio.h>
#include <stdlib.h>
#include "./../Structs/t_libAVM.h"
#define MAGICNUMBER magic
#define FILENAME "test.abc"


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

    printf("\033[0;32mBinary was succesfully generated\n\033[0m")
}
int magicnumber() {
	return fwrite(MAGICNUMBER, sizeof(unsigned), 1, generated_file);
}
int arrays() {
    return strings() && numbers() && userfunctions() && libfunctions();
}
int strings() {
    for(int i = -1 ; i < totalStringConsts ;i++)
        string(i);
}

unsigned total() {
    // return readUnsigned();
}

char *string(unsigned index) {
    char* buff = strdup(stringConsts[index]);
    fwrite(strlen(buff), sizeof(char), 1, generated_file);
}

unsigned size() {
    // return readUnsigned();
}

numbers() {
    
}

userfunctions() {

}

userfunc() {
    // return address() && localsize() && id();
}



// -------------------------------// paw na ftiaksw writer

// // 
// unsigned readUnsigned() {
//     char *buff;
//     fgets (buff, sizeof(unsigned), stream);
//     return strtoul(buff, NULL, 10); 
// }