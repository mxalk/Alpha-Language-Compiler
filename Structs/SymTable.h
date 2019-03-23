#pragma once
#define SYM_SIZE 1000
#define GlobalSymbolTable GST

struct SymbolTableRecord **GlobalSymbolTable;
enum SymType
{
        LCL,
        GLBL,
        LIBFUNC,
        USRFUNC,
        UNDEF
};
struct SymbolTableRecord;
void insert(char *name, enum SymType type, unsigned int scope, unsigned int line);
struct SymbolTableRecord *lookup(char *name,unsigned int scope, enum SymType type);
int hash_f(struct SymbolTableRecord *record);
void display();
void sym_init();
