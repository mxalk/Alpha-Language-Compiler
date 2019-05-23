#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "./../Structs/t_libAVM.h"

#define MAGICNUMBER magic
#define FILENAME "test.abc"
#define BYTE char
#define MODE "w"
#define fwrite(...) fwrite(__VA_ARGS__)

// extern void* Queue_get(Queue*,int);

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
unsigned userfunctions_gen();
unsigned libfunctions();
unsigned usrfunc(unsigned);
unsigned code();
unsigned operand_gen(vmarg*);