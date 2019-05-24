#pragma once

#define MAGICNUMBER 194623425 //655*639*465 from 3655 3639 3465
FILE *stream;
FILE *bin_file;
char* bin_file_name;

int avmbinaryfile();
int magicnumber();
int arrays();
int arrays_strings();
int arrays_numbers();
int arrays_userfunctions();
int arrays_libfunctions();
int t_code();
int operand(struct vmarg *);
int readString(char **);
int readUnsigned(unsigned *);
int readDouble(double *);
int readByte(char *);