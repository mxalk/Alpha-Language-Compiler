#include "../avm.h"

void execute_newtable(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->arg1, (struct avm_memcell *) 0);
    //assert(lv && (&stack[N] >= lv && lv < &stack[top] || lv == &retval));
    avm_memcellclear(lv);
    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}

unsigned hsh(struct avm_memcell * index ){
    unsigned int key = 0;
    unsigned int i;
    char* tmp;
    switch (index->type) {
        case number_m:
            key = index->data.numVal;
            break;
        case string_m:
            tmp = index->data.strVal;
            for (i = 0; tmp[i]!='\0'; i++){
                key += tmp[i];
            }
            break;
        case bool_m:
            key = index->data.boolVal;
            break;
        case userfunc_m:
            key = index->data.funcVal;
            break;
        case libfunc_m:
            tmp = index->data.libfuncVal;
            for (i = 0; tmp[i]!='\0'; i++){
                key += tmp[i];
            }
            break;
        case table_m:
        case nil_m:
        case undef_m:
            avm_error("invalid index");
            break;
    }
    return key % AVM_TABLE_HASHSIZE;
} 

struct avm_memcell *avm_tablegetelem (struct avm_table *table, struct avm_memcell *index){
    struct avm_table_bucket *bucket ;//= (struct avm_table_bucket*)malloc(sizeof(struct avm_table_bucket));
    unsigned b = hsh(index);
    switch (index->type) {
        case number_m:
            bucket = table->numIndexed[b];
            while(bucket) {
                if (bucket->key.data.numVal == index->data.numVal) return &bucket->value;
                bucket = bucket->next;
            }
            break;
        case string_m:
            bucket = table->strIndexed[b];
            while(bucket) {
                if (bucket->key.data.strVal == index->data.strVal) return &bucket->value;
                bucket = bucket->next;
            }
            break;
        case bool_m:
            bucket = table->boolIndexed[b];
            while(bucket) {
                if (bucket->key.data.boolVal == index->data.boolVal) return &bucket->value;
                bucket = bucket->next;
            }
            break;
        case userfunc_m:
            bucket = table->ufncIndexed[b];
            while(bucket) {
                if (bucket->key.data.funcVal == index->data.funcVal) return &bucket->value;
                bucket = bucket->next;
            }
            break;
        case libfunc_m:
            bucket = table->lfncIndexed[b];
            while(bucket) {
                if (bucket->key.data.libfuncVal == index->data.libfuncVal) return &bucket->value;
                bucket = bucket->next;
            }
            break;
        case table_m:
        case nil_m:
        case undef_m:
            avm_error("invalid index");
            break;
    }
    return NULL;
}
void avm_tablesetelem (struct avm_table *table, struct avm_memcell *index, struct avm_memcell *content){
    struct avm_table_bucket *bucket, *new_cell;
    unsigned b = hsh(index);
    printf("table set elem of type:%d\n",index->type);
    switch (index->type) {
        case number_m:
            bucket = table->numIndexed[b];
            while(bucket) {
                if (bucket->key.data.numVal == index->data.numVal) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->numIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->numIndexed[b] = new_cell;
            break;
        case string_m:
            bucket = table->strIndexed[b];
            while(bucket) {
                if (bucket->key.data.strVal == index->data.strVal) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->strIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->strIndexed[b] = new_cell;
            break;
        case bool_m:
            bucket = table->boolIndexed[b];
            while(bucket) {
                if (bucket->key.data.boolVal == index->data.boolVal) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->boolIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->boolIndexed[b] = new_cell;
            break;
        case userfunc_m:
            bucket = table->ufncIndexed[b];
            while(bucket) {
                if (bucket->key.data.funcVal == index->data.funcVal) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->ufncIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->ufncIndexed[b] = new_cell;
            break;
        case libfunc_m:
            bucket = table->lfncIndexed[b];
            while(bucket) {
                if (bucket->key.data.libfuncVal == index->data.libfuncVal) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->lfncIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->lfncIndexed[b] = new_cell;
            break;
        case table_m:
        case nil_m:
        case undef_m:
            avm_error("invalid index");
            break;
    }

}

void execute_tablegetelem(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *t = avm_translate_operand(&instr->arg1, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg2, &ax);
    printf("retval %u \n", retval);
    printf("instr->result %d \n", instr->result.val);
    printf("type %d \n", lv->type);

    // assert(lv);
    //assert((&stack[N] >= lv && lv < &stack[top]) );
    // assert(lv && (&stack[N] >= lv && lv < &stack[top] || lv == &retval));
    // assert(t && &stack[N] >= t && t < &stack[top]);
    assert(i);
    avm_memcellclear(lv);
    lv->type = nil_m;
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table!", typeStrings[t->type]);
        return;
    }
    struct avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
    if (content) {
        avm_assign(lv, content);
        return;
    }else{
        char *ts = avm_tostring(t);
        char *is = avm_tostring(i);
        avm_warning("\'%s[%s]\' not found!", ts, is);
        free(ts);
        free(is);
    }
}

void execute_tablesetelem(struct instruction *instr) {
    struct avm_memcell *t = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *c = avm_translate_operand(&instr->arg2, &bx);
    assert(t && &stack[N] >= t && &stack[top]);
    assert(i && c);
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table", t->type);
        return;
    }
    avm_tablesetelem(t->data.tableVal, i, c);
}


void avm_tableincrefcounter(struct avm_table *t) {
    ++t->refCounter;
}

void avm_tabledecrefcounter(struct avm_table *t) {
    assert(t->refCounter > 0);
    if (!--t->refCounter) avm_tabledestroy(t);
}

void avm_tablebucketsinit(struct avm_table_bucket **p) {
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++) p[i] = (struct avm_table_bucket *) 0;
}

struct avm_table *avm_tablenew(void) {
    struct avm_table *t = (struct avm_table *) malloc(sizeof(struct avm_table));
    AVM_WIPEOUT(*t);
    t->refCounter = t->total = 0;
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->ufncIndexed);
    avm_tablebucketsinit(t->lfncIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    return t;
}

void avm_tablebucketsdestroy(struct avm_table_bucket **p) {
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++, p++) {
        for (struct avm_table_bucket *b = *p; b;) {
            struct avm_table_bucket *del = b;
            b = b->next;
            avm_memcellclear(&del->key);
            avm_memcellclear(&del->value);
            free(del);
        }
        p[i] = (struct avm_table_bucket *) 0;
    }
}

void avm_tabledestroy(struct avm_table *t) {
    avm_tablebucketsdestroy(t->strIndexed);
    avm_tablebucketsdestroy(t->numIndexed);
    avm_tablebucketsdestroy(t->ufncIndexed);
    avm_tablebucketsdestroy(t->lfncIndexed);
    avm_tablebucketsdestroy(t->boolIndexed);
    free(t);
}
