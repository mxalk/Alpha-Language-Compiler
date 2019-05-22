#include <stdio.h>
#include <stdlib.h>
#define MAGICNUMBER 194623425 //655*639*465 from 3655 3639 3465
FILE *stream;

void avmbinaryfile() {
    // return magicnumber() && arrays() && code() ;
    magicnumber();
    arrays();
    code();
}

magicnumber() {
    unsigned n = readUnsigned();
    return match(n) && n == MAGICNUMBER;
}

arrays() {
    return strings() && numbers() && userfunctions() && libfunctions();
}

//  -------------------------------------------
//  --- GLOBALS -------------------------------
//  -------------------------------------------

strings() {
    unsigned n = total();
    if (!match(n))
        return 0;
    for (int i = 0; i<n; i++) string();
}

numbers() {
    unsigned n = total();
    if (!match(n))
        return 0;
    for (int i = 0; i<n; i++) readDouble();
}

userfunctions() {
    unsigned n = total();
    if (!match(n))
        return 0;
    for (int i = 0; i<n; i++) userfunc();
}

libfunctions() {
    return strings();
}

//  -------------------------------------------

unsigned userfunc() {
    return address() && localsize() && id();
}

unsigned address() {
    return readUnsigned();
}

unsigned localsize() {
    return readUnsigned();
}


id() {
    return string();
}

//  -------------------------------------------
//  --- CODE ----------------------------------
//  -------------------------------------------

code() {
    unsigned n = total();
    if (!match(n))
        return 0;
    for (int i = 0; i<n; i++) instruction();
}

//  -------------------------------------------

instruction() {
    return opcode() && operand() && operand() && operand();
}

char opcode() {
    return readByte();
}

operand() {
    return type() && value();
}


char type() {
    return readByte();
}

unsigned value() {
    return readUnsigned();
}

//  -------------------------------------------
//  --- GENERIC -------------------------------
//  -------------------------------------------

unsigned total() {
    return readUnsigned();
}

unsigned size() {
    return readUnsigned();
}

//  -------------------------------------------

char *string() {
    unsigned s;
    s = size()+1;
    char *buff = malloc(s);
    fgets (buff, s, stream);
    return buff;
}

unsigned readUnsigned() {
    char *buff;
    fgets (buff, sizeof(unsigned), stream);
    return strtoul(buff, NULL, 10); 
}

char readDouble() {
    double buff;
    fgets (&buff, sizeof(double), stream);
    return buff;
}

char readByte() {
    char buff;
    fgets (&buff, sizeof(char), stream);
    return buff; 
}