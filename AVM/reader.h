#pragma once


int avmbinaryfile();
int magicnumber();
int arrays();
int arrays_strings();
int arrays_numbers();
int arrays_userfunctions();
int arrays_libfunctions();
int t_code();
int operand(struct vmarg *vmarg);
int readString(char *str);
int readUnsigned(unsigned *u);
int readDouble(double *d);
int readByte(char *c);