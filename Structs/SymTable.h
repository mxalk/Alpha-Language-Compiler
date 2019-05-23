#pragma once
#include <string.h>
#include "Stack.h"
#include "Queue.h"
char* file_name;
#define SYM_SIZE 1000
#define GlobalSymbolTable GST
#define GlobalScopeStack GSS


typedef enum SymType{
        LCL,
        GLBL,
        LIBFUNC,
        USRFUNC,
        FORMAL
} SymType;

typedef enum scopespace_t {
    programvar,
    functionlocal,
    formalarg
} Scopespace_t;

typedef enum symbol_t {
	var_s,
	programfunc_s,
	libraryfunc_s
} Symbol_t;

typedef struct SymbolTableRecord
{
    char *name;
    struct SymbolTableRecord *next;
    SymType type;
    unsigned int scope;
    unsigned int line;
    unsigned char **args; //func
    unsigned int active;
    Symbol_t stype;
	Scopespace_t space;
    Queue* returnList;
	unsigned offset;
	unsigned iaddress;
    unsigned taddress;
	unsigned totallocals;
}SymbolTableRecord;

typedef struct Scope {
        Queue *queue;
        int isFunction;
} Scope;
// struct SymbolTableRecord;
Stack *GlobalScopeStack;
struct SymbolTableRecord **GlobalSymbolTable;


SymbolTableRecord* insert(char *name, SymType type, unsigned int scope, unsigned int line);
struct SymbolTableRecord *lookup(char *name, SymType type, unsigned int line,unsigned int expected,unsigned int func_def,unsigned local);
struct SymbolTableRecord *lookupGlobal(char *name, SymType type, unsigned int line, unsigned int expected);
int hash_f(struct SymbolTableRecord *record);
void display();
void sym_init();
void hide(unsigned int scope);
void increaseScope(int isFunct);
void decreaseScope();
unsigned int getScope();
void printRecord(struct SymbolTableRecord *r);
void printGSS();
void displaySymbolsWithOffset();