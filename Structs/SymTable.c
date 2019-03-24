
#include "SymTable.h"
#include "Queue.h"
#include "Stack.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

struct SymbolTableRecord;

char *typeNames[] = {"LOCAL", "GLOBAL", "LIBFUNC", "USRFUNC", "FORMAL"};

typedef struct SymbolTableRecord
{
        char *name;
        struct SymbolTableRecord *next;
        enum SymType type;
        unsigned int scope;
        unsigned int line;
        unsigned char **args; //func
        unsigned int active;
        union{  
                char* stringValue;
                int intValue;
                float floatValue;
                unsigned int boolValue;
        }value;
} SymbolTableRecord;

typedef struct Scope
{
        Queue *queue;
        int isFunction;
} Scope;

int hash_f(SymbolTableRecord *record)
{
        unsigned int key = 0;
        unsigned int i;
        char* tmp = record->name;
        for (i = 0; tmp[i]!='\0'; i++)
        {
                // printf("%d\n",tmp);
                key += tmp[i];
        }

        return key % SYM_SIZE;
}

SymbolTableRecord *lookup(char *name, enum SymType type, unsigned int line, unsigned int expected)
{
        //expected == 1 , we need to use the var
        //expected == 0 , we want to declare the var
        SymbolTableRecord *record = (SymbolTableRecord *)malloc(sizeof(SymbolTableRecord));
        record->name = name;
        int hash_index = hash_f(record);
        free(record);

        SymbolTableRecord *iter = GST[hash_index];
        record = NULL;

        int i;
        Scope *scope;
        Queue_Node *node;
        // STACK WITHOUT GLOBALS
        for (i=0; i<GSS->size-1; i++) {
                scope = (Scope *)Stack_get(GSS, i);
                node = scope->queue->head;
                while (node != NULL) {
                        iter = (SymbolTableRecord *) node->content;
                        if (strcmp(iter->name, name)==0) {
                                record = iter;
                                break;
                        }
                        node = node->next;
                }
                if (record != NULL || scope->isFunction) break;
        }
        // GLOBALS
        if (record == NULL) {
                scope = (Scope *)Stack_get(GSS, GSS->size-1);
                node = scope->queue->head;
                while (node != NULL) {
                        iter = (SymbolTableRecord *) node->content;
                        if (strcmp(iter->name, name)==0) {
                                record = iter;
                                break;
                        }
                        node = node->next;
                }
        }
        printGSS();
        printRecord(record);
        
        char *buffer = (char*)malloc(30+strlen(name));
        if (record == NULL) {
                if (expected == 1){
                sprintf(buffer, "Undefined token \'%s\' line %u", name, line);
                alpha_yyerror(buffer);
                }
        } else if (expected == 0) { 
                // SymType rtype = record->type;
                if (record->type==LIBFUNC && type==USRFUNC) {
                        sprintf(buffer, "Function already defined as LIBFUNC \'%s\' line %u", name, line);
                        alpha_yyerror(buffer);
                } else if (record->type == USRFUNC && type != USRFUNC) {
                        sprintf(buffer, "Cannot define a function again as a var \'%s\' line %u", name, line);
                        alpha_yyerror(buffer);
                }else if (record->type != USRFUNC && type == USRFUNC) {
                        sprintf(buffer, "Cannot define a var again as a function \'%s\' line %u", name, line);
                        alpha_yyerror(buffer);
                }
        }
        return record;
}

SymbolTableRecord *lookupGlobal(char *name, enum SymType type, unsigned int line, unsigned int expected)
{

        SymbolTableRecord *iter, *record = NULL;
        int i;
        Scope *scope;
        Queue_Node *node;
        scope = (Scope *)Stack_get(GSS, GSS->size-1);
                node = scope->queue->head;
                while (node != NULL) {
                        iter = (SymbolTableRecord *) node->content;
                        if (strcmp(iter->name, name)==0) {
                                record = iter;
                                break;
                        }
                        node = node->next;
                }
        return record;
}
void insert(char *name, enum SymType type, unsigned int scope, unsigned int line)
{
        SymbolTableRecord *record = (SymbolTableRecord *)malloc(sizeof(SymbolTableRecord));
        record->name = name;
        record->type = type;
        record->scope = scope;
        record->line = line;
        record->active = 1;

        //get the hash
        int hash_index = hash_f(record);
        SymbolTableRecord *iter = GST[hash_index];
        
        // printf("%s -> %d\n",record->name,hash_index);
        //  exit(EXIT_FAILURE);
        record->next = GST[hash_index];
        GST[hash_index] = record;
        //assert(scope == GSS->size -1);
        Queue_enqueue(((Scope *)Stack_get(GSS, GSS->size-1-scope))->queue, record);
}


void printRecord(SymbolTableRecord *r) {
        if (r == NULL) {
                printf("NULL\n");
        } else printf("%30s %10s %5u %5u\n", r->name, typeNames[r->type], r->scope, r->line);
        
}

void printGSS() {
        printf("------------------------\n");
        int i, j;
        Scope *scope;
        Queue *q;
        SymbolTableRecord *r;
        for (i=0; i<GSS->size; i++) {
                scope = (Scope *) Stack_get(GSS, i);
                printf("\nScope %d\n", GSS->size-i-1);
                printf("IsFunctionScope %d\n", scope->isFunction);
                q = scope->queue;
                for (j=0; j<q->size; j++) {
                        r = (SymbolTableRecord *) Queue_get(q, j);
                        printf("\t");
                        printRecord(r);
                }
                printf("\n");
        }
        printf("------------------------\n");
}

void increaseScope(int isFunct)
{
        Scope *scope = (Scope*)malloc(sizeof(Scope));
        scope->queue = Queue_init();
        scope->isFunction = isFunct;
        Stack_append(GSS, scope);
}

void decreaseScope()
{
        SymbolTableRecord *record = NULL;
        Scope *scope =  Stack_pop(GSS);
        while ((record = Queue_dequeue(scope->queue)) != NULL) record->active = 0;
        Queue_destroy(scope->queue);
        free(scope);
}

unsigned int getScope(){
        return GSS->size-1;
}

void display()
{
        printf("====================================================\n");
        int i = 0;
        SymbolTableRecord *iter;
        printf("%20s %10s %3s %5s %3s\n", "name", "type", "scope", "line", "active");
        for (i = 0; i < SYM_SIZE; i++)
        {
                iter = GST[i];
                while (iter != NULL)
                {
                       if(iter->type!=LIBFUNC)printf("%20s %10s %3d %5d %3d [%3d] \n", iter->name, typeNames[iter->type], iter->scope, iter->line, iter->active ,i);
                        iter = iter->next;
                }
        }
        printf("====================================================\n");
}

void sym_init()
{

        int i;
        GST = (SymbolTableRecord **)malloc(sizeof(Queue *) * SYM_SIZE);
        GSS = Stack_init();
        increaseScope(0);

        for (i = 0; i < SYM_SIZE; i++)
        {
                GST[i] = NULL;
        }
        insert("print",LIBFUNC,0,0);
        insert("input",LIBFUNC,0,0);
        insert("objectmemberkeys",LIBFUNC,0,0);
        insert("objecttotalmembers",LIBFUNC,0,0);
        insert("objectcopy",LIBFUNC,0,0);
        insert("totalarguments",LIBFUNC,0,0);
        insert("argument",LIBFUNC,0,0);
        insert("typeof",LIBFUNC,0,0);
        insert("strtonum",LIBFUNC,0,0);
        insert("sqrt",LIBFUNC,0,0);
        insert("cos",LIBFUNC,0,0);
        insert("sin",LIBFUNC,0,0);


}