#pragma once
#include <stdio.h>
#include <stdlib.h>
#include "./../Structs/t_libAVM.h"

#define FILENAME "test.abc"
#define fwrite(...) fwrite(__VA_ARGS__)

// extern void* Queue_get(Queue*,int);

void init_writter();
void avmbinaryfile();
int magicnumber();
int arrays();
int arrays_strings();
int arrays_numbers();
int arrays_userfunctions();
int arrays_libfunctions();
int t_code();
int operand(vmarg* v);
int writeString(char *buff);
int writeUnsigned(unsigned buff);
int writeDouble(double buff);
int writeByte(char buff);