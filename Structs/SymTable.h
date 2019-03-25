#pragma once
#include "Stack.h"
#include <assert.h>
#define SYM_SIZE 1000
#define GlobalSymbolTable GST
#define GlobalScopeStack GSS

struct SymbolTableRecord **GlobalSymbolTable;
Stack *GlobalScopeStack;
enum SymType
{
        LCL,
        GLBL,
        LIBFUNC,
        USRFUNC,
        FORMAL
};
struct SymbolTableRecord;
void insert(char *name, enum SymType type, unsigned int scope, unsigned int line);
struct SymbolTableRecord *lookup(char *name, enum SymType type, unsigned int line,unsigned int expected);
struct SymbolTableRecord *lookupGlobal(char *name, enum SymType type, unsigned int line, unsigned int expected);
int hash_f(struct SymbolTableRecord *record);
void display();
void sym_init();
void hide(unsigned int scope);
void increaseScope(int isFunct);
void decreaseScope();
unsigned int getScope();
void printRecord(struct SymbolTableRecord *r);
void printGSS();