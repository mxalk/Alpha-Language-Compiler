
#include "SymTable.h"
#include "Queue.h"
#include "Stack.h"
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
        unsigned int key = 0U;
        unsigned int i;
        for (i = 0U; record->name[i] != '\0'; i++)
        {
                // printf("%d\n",record->name[i]);
                key += record->name[i];
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

        //  int i;
        // Stack_Node *stack_node = GSS->top;
        // Queue_Node *queue_node;
        // // STACK WITHOUT GLOBALS
        // for (i=0; i<GSS->size-1; i++) {
        //         queue_node = ((Scope *)stack_node->content)->queue->head;
        //         while (queue_node != NULL) {
        //                 iter = (SymbolTableRecord *) queue_node->content;
        //                 if (iter->name == name) {
        //                         record = iter;
        //                         break;
        //                 }
        //                 queue_node = queue_node->next;
        //         }
        //         if (record != NULL) break;
        //         if (((Scope *)stack_node->content)->isFunction) break;
        //         stack_node = stack_node->next;
        // }
        // // GLOBALS
        // if (record == NULL) {
        //         // scope = Stack_get(GSS, GSS->size-1); // or -1
        //         while(stack_node->next != NULL) stack_node = stack_node->next;
        //         queue_node = ((Scope *)stack_node->content)->queue->head;
        //         while (queue_node != NULL) {
        //                 iter = (SymbolTableRecord *) queue_node->content;
        //                 if (iter->name == name) {
        //                         record = iter;
        //                         break;
        //                 }
        //                 queue_node = queue_node->next;
        //         }
        // }

        int i;
        Scope *scope;
        Queue_Node *node;
        // STACK WITHOUT GLOBALS
        for (i=0; i<GSS->size-1; i++) {
                scope = Stack_get(GSS, i);
                node = scope->queue->head;
                while (node != NULL) {
                        iter = (SymbolTableRecord *) node->content;
                        if (iter->name == name) {
                                record = iter;
                                break;
                        }
                        node = node->next;
                }
                if (record != NULL) break;
                if (scope->isFunction) break;
        }
        // GLOBALS
        if (record == NULL) {
                scope = Stack_get(GSS, GSS->size-1); // or -1
                node = scope->queue->head;
                while (node != NULL) {
                        iter = (SymbolTableRecord *) node->content;
                        if (iter->name == name) {
                                record = iter;
                                break;
                        }
                        node = node->next;
                }
        }

        // while (iter != NULL)
        // {
        //         if (iter->name == name && iter->active == 1 && iter->scope == scope) {
        //                 token = iter;
        //                 break;
        //         }
        //         iter = iter->next;
        // }
        char *buffer = (char*)malloc(30+strlen(name));
        if (record == NULL) {
                if (expected == 1){
                sprintf(buffer, "Undefined token \'%s\' line %u", name, line);
                alpha_yyerror(buffer);
                }
        } else if (expected == 0 && record->type==LIBFUNC && type==USRFUNC) {
                sprintf(buffer, "try to define a lib function  \'%s\' line %u", name, line);
                alpha_yyerror(buffer);
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
        while (iter != NULL)
        {
                //printf("ti fash\n");
                iter = iter->next;
        }
        printf("%s -> %d\n",record->name,hash_index);
        //  exit(EXIT_FAILURE);

        GST[hash_index] = record;
        assert(scope == GSS->size -1);
        Queue_enqueue((Queue *)GSS->top->content, record);
}

void increaseScope()
{
        Scope *scope = (Scope*)malloc(sizeof(Scope));
        scope->queue = Queue_init();
        scope->isFunction = 0;
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
        printf("=========================\n");
        int i = 0;
        SymbolTableRecord *iter;
        printf("%30s %10s %3s %5s %3s\n", "name", "type", "scope", "line", "active");
        for (i = 0; i < SYM_SIZE; i++)
        {
                iter = GST[i];
                while (iter != NULL)
                {
                        if (iter->type != LIBFUNC) printf("%30s %10s %3d %5d %3d [%3d] \n", iter->name, typeNames[iter->type], iter->scope, iter->line, iter->active ,i);
                        iter = iter->next;
                }
        }
        printf("=========================\n");
}

void sym_init()
{

        int i;
        GST = (SymbolTableRecord **)malloc(sizeof(SymbolTableRecord *) * SYM_SIZE);
        GSS = Stack_init();
        increaseScope();

        for (i = 0; i < SYM_SIZE; i++)
        {
                GST[i] = NULL;
        }
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

//hide -- NO LONGER NEEDED, IMPLEMENTED IN decreaseScope()
// void hide(unsigned int scope) {
//         int i = 0;
//         SymbolTableRecord *iter;
//         for (i = 0; i < SYM_SIZE; i++)
//         {
//                 iter = GST[i];
//                 while (iter != NULL) {
//                         if (iter->scope == scope) iter->active = 0;
//                         iter = iter->next;
//                 }
//         }
// }