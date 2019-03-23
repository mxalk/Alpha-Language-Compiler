
#include "SymTable.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct SymbolTableRecord;

char *typeNames[] = {"LOCAL", "GLOBAL", "LIBFUNC", "USRFUNC", "UNDEF"};

typedef struct SymbolTableRecord
{
        char *name;
        struct SymbolTableRecord *next;
        enum SymType type;
        unsigned int scope;
        unsigned int line;
        unsigned char **args; //func
} SymbolTableRecord;

int hash_f(SymbolTableRecord *record)
{
        unsigned int key = 0U;
        unsigned int i;
        for (i = 0U; record->name[i] != '\0'; i++)
        {
                // printf("%d\n",record->name[i]);
                key += record->name[i];
        }

        return key % SYM_SIZE;
}

SymbolTableRecord *lookup(char *name,unsigned int scope, enum SymType type)
{
        SymbolTableRecord *record = (SymbolTableRecord *)malloc(sizeof(SymbolTableRecord));
        record->name = name;

        int hash_index = hash_f(record);
        SymbolTableRecord *iter = GST[hash_index];
        while (iter != NULL)
        {
                if (iter->name == record->name)
                        return iter;
                iter = iter->next;
        }
        return NULL;
}

void insert(char *name, enum SymType type, unsigned int scope, unsigned int line)
{
        SymbolTableRecord *record = (SymbolTableRecord *)malloc(sizeof(SymbolTableRecord));
        record->name = name;
        record->type = type;
        record->scope = scope;
        record->line = line;

        //get the hash
        int hash_index = hash_f(record);
        SymbolTableRecord *iter = GST[hash_index];
        // while (iter != NULL)
        // {
        //         printf("ti fash\n");
        //         iter = iter->next;
        // }
        // printf("%s -> %d\n",record->name,hash_index);
        //  exit(EXIT_FAILURE);

        GST[hash_index] = record;
}

void display()
{
        printf("=========================\n");
        int i = 0;
        for (i = 0; i < SYM_SIZE; i++)
        {
                SymbolTableRecord *iter = GST[i];
                while (iter != NULL)
                {
                        printf("%s %s %d %d [%d] \n", iter->name, typeNames[iter->type], iter->scope, iter->line, i);
                        iter = iter->next;
                }
        }
        printf("=========================\n");
}

void sym_init()
{
        int i;
        GST = (SymbolTableRecord **)malloc(sizeof(SymbolTableRecord *) * SYM_SIZE);
        for (i = 0; i < SYM_SIZE; i++)
        {
                GST[i] = NULL;
        }
}