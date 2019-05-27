#include "SymTable.h"

//#define alpha_yyerror(x) fprintf(stderr,x);

extern int alpha_yyerror (const char* yaccProvidedMessage);

char *typeNames[] = {"LOCAL", "GLOBAL", "LIBFUNC", "USRFUNC", "FORMAL"};

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

        //expected == 1 , we need to use the var
        //expected == 0 , we want to declare the var
SymbolTableRecord *lookup(char *name, enum SymType type, unsigned int line, unsigned int expected, unsigned int func_def, unsigned local)
{
        SymbolTableRecord *record = (SymbolTableRecord *)malloc(sizeof(SymbolTableRecord));
        record->name = name;
        int hash_index = hash_f(record);
        free(record);

        SymbolTableRecord *iter = GST[hash_index];
        record = NULL;

        int i, wasFunction = 0, isFunction = 0;
        int all_scopes = GSS->size;
        Scope *scope;
        Queue_Node *node;
        if(func_def){
                all_scopes = 1; //func_def: only check the scope that the function is defined
        }

        for (i=0; i < all_scopes; i++) {
                if (wasFunction) isFunction = 1;
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
                if (record != NULL || local) break;
                if (scope->isFunction) wasFunction = 1;
        }
        if(func_def){
                i=GSS->size-1;
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
        }
        if(record != NULL){
                if(record->type == LIBFUNC && func_def){
                        char *buffer = (char*)malloc(50+strlen(name));
                        sprintf(buffer, "library function cannot be shadowed with func: \'%s\'", name);
                        alpha_yyerror(buffer);
                }
                if(record->type == LIBFUNC)
                        record->stype = libraryfunc_s;
                else if(record->type == USRFUNC)
                        record->stype = programfunc_s;
                else record->stype = var_s;
        }
        //  printGSS();
        // printRecord(record);
        if(!func_def){
                if (isFunction && record!= NULL && record->scope!=0) {
                        char *buffer = (char*)malloc(50+strlen(name));
                        sprintf(buffer, "variable with scope %d access not allowed \'%s\'",record->scope, name);
                        alpha_yyerror(buffer);
                }
        }
        if (expected){
                if (record != NULL) return record;
                if (record == NULL) {
                        char *buffer = (char*)malloc(50+strlen(name));
                        sprintf(buffer, "Undefined variable \'%s\'", name);
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
        if(record){
                if(record->type == LIBFUNC)
                        record->stype = libraryfunc_s;
                else if(record->type == USRFUNC)
                        record->stype = programfunc_s;
                else record->stype = var_s;
        }
        return record;
}
SymbolTableRecord* insert(char *name, enum SymType type, unsigned int scope, unsigned int line)
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
        return record;
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

void displaySymbolsWithOffset()
{
        printf("=========================================================\n");
        int i = 0;
        SymbolTableRecord *iter;
        printf("%20s %10s %3s %5s %3s %3s\n", "name", "type", "scope","line", "offset", "active");
        for (i = 0; i < SYM_SIZE; i++)
        {
                iter = GST[i];
                while (iter != NULL)
                {
                       if(iter->type!=LIBFUNC)printf("%20s %10s %3d %5d %5d %3d [%3d] \n", iter->name, typeNames[iter->type], iter->scope, iter->line,iter->offset, iter->active ,i);
                        iter = iter->next;
                }
        }
        printf("=========================================================\n");
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