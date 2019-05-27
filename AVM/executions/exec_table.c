#include "../avm.h"

void printtable(struct avm_table *t) {
    printf("================================ %u\n", t->boolIndexed[106]);
}

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
            avm_warning("hash invalid index");
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
                if (!strcmp(bucket->key.data.strVal, index->data.strVal)) return &bucket->value;
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
            avm_warning("invalid table index type (%s)", typeStrings[index->type]);
            break;
    }
    struct avm_memcell *new_mem = (struct avm_memcell *)malloc(sizeof(struct avm_memcell));
    new_mem->type = nil_m;
    return new_mem;
}
void avm_tablesetelem (struct avm_table *table, struct avm_memcell *index, struct avm_memcell *content){
    struct avm_table_bucket *bucket, *new_cell;
    unsigned b = hsh(index);
    if(content->type == table_m) avm_tableincrefcounter(content->data.tableVal);
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
            new_cell = (struct avm_table_bucket *) malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->numIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->numIndexed[b] = new_cell;
            break;
        case string_m:
            bucket = table->strIndexed[b];
            while(bucket) {
                if (!strcmp(bucket->key.data.strVal, index->data.strVal)) {
                    bucket->value = *content;
                    return;
                }
                bucket = bucket->next;
            }
            new_cell = (struct avm_table_bucket *) malloc(sizeof(struct avm_table_bucket));
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
            new_cell = (struct avm_table_bucket *) malloc(sizeof(struct avm_table_bucket));
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
            new_cell = (struct avm_table_bucket *) malloc(sizeof(struct avm_table_bucket));
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
            new_cell = (struct avm_table_bucket *) malloc(sizeof(struct avm_table_bucket));
            new_cell->next = table->lfncIndexed[b];
            new_cell->key = *index;
            new_cell->value = *content;
            table->lfncIndexed[b] = new_cell;
            break;
        case nil_m:
        case table_m:
        case undef_m:
            avm_warning("avm tablesetelem: nil or a undef");
            break;
    }
    table->total++;

}

void execute_tablegetelem(struct instruction *instr) {
    struct avm_memcell *lv = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *t = avm_translate_operand(&instr->arg1, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg2, &ax);
    // printf("retval %u \n", retval);
    // printf("instr->result %d \n", instr->result.val);
    // printf("type %d \n", lv->type);

    
    // // autes einai oi default
    // assert(lv && (&stack[N] >= lv && lv < &stack[top] || lv == &retval));
    // assert(t && &stack[N] >= t && t < &stack[top]);
    assert(i);
    avm_memcellclear(lv);
    lv->type = nil_m;
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table! PC %d", typeStrings[t->type],pc);
        return;
    }
    struct avm_memcell *content = avm_tablegetelem(t->data.tableVal, i);
    if (content) {
        avm_assign(lv, content);
        return;
    }else{
        // char *ts = avm_tostring(t);
        // char *is = avm_tostring(i);
        avm_warning(" not found!");//, ts, is);
        // free(ts);
        // free(is);
    }
}

void avm_tableremoveindex(struct avm_table *table, struct avm_memcell *index) {
    struct avm_table_bucket *bucket, *bucket_next;
    unsigned b = hsh(index);
    switch (index->type) {
        case number_m:
            bucket = table->numIndexed[b];
            if (bucket && bucket->key.data.numVal == index->data.numVal) {
                table->numIndexed[b] = bucket->next;
                table->total--;
                break;
            }
            if (!bucket) break;
            while (bucket_next = bucket->next) {
                if (bucket_next && bucket_next->key.data.numVal == index->data.numVal) {
                    bucket->next = bucket_next->next;
                    table->total--;
                    break;
                }
                bucket = bucket->next;
            }
            break;
        case string_m:
            bucket = table->strIndexed[b];
            if (bucket && !strcmp(bucket->key.data.strVal, index->data.strVal)) {
                table->strIndexed[b] = bucket->next;
                table->total--;
                break;
            }
            if (!bucket) break;
            while (bucket_next = bucket->next) {
                if (bucket_next && !strcmp(bucket_next->key.data.strVal, index->data.strVal)) {
                    bucket->next = bucket_next->next;
                    table->total--;
                    break;
                }
                bucket = bucket->next;
            }
            break;
        case bool_m:
            bucket = table->boolIndexed[b];
            if (bucket && bucket->key.data.boolVal == index->data.boolVal) {
                table->boolIndexed[b] = bucket->next;
                table->total--;
                break;
            }
            if (!bucket) break;
            while (bucket_next = bucket->next) {
                if (bucket_next && bucket_next->key.data.boolVal == index->data.boolVal) {
                    bucket->next = bucket_next->next;
                    table->total--;
                    break;
                }
                bucket = bucket->next;
            }
            break;
        case userfunc_m:
            bucket = table->ufncIndexed[b];
            if (bucket && bucket->key.data.funcVal == index->data.funcVal) {
                table->ufncIndexed[b] = bucket->next;
                table->total--;
                break;
            }
            if (!bucket) break;
            while (bucket_next = bucket->next) {
                if (bucket_next && bucket_next->key.data.funcVal == index->data.funcVal) {
                    bucket->next = bucket_next->next;
                    table->total--;
                    break;
                }
                bucket = bucket->next;
            }
            break;
        case libfunc_m:
            bucket = table->lfncIndexed[b];
            if (bucket && bucket->key.data.libfuncVal == index->data.libfuncVal) {
                table->lfncIndexed[b] = bucket->next;
                table->total--;
                break;
            }
            if (!bucket) break;
            while (bucket_next = bucket->next) {
                if (bucket_next && bucket_next->key.data.libfuncVal == index->data.libfuncVal) {
                    bucket->next = bucket_next->next;
                    table->total--;
                    break;
                }
                bucket = bucket->next;
            }
            break;
        case nil_m:
        case table_m:
        case undef_m:
            avm_warning("avm table_removeindex: nil or a undef");
            break;
    }
}

void execute_tablesetelem(struct instruction *instr) {
    struct avm_memcell *t = avm_translate_operand(&instr->result, (struct avm_memcell *) 0);
    struct avm_memcell *i = avm_translate_operand(&instr->arg1, &ax);
    struct avm_memcell *c = avm_translate_operand(&instr->arg2, &bx);
    assert(t && &stack[N] >= t && &stack[top]);
    assert(i && c);
    if (t->type != table_m) {
        avm_error("Illegal use of type '%s' as table. PC %d", typeStrings[t->type],pc);
        return;
    }
    if (c->type == nil_m) avm_tableremoveindex(t->data.tableVal, i);
    else avm_tablesetelem(t->data.tableVal, i, c);
}

void avm_tableincrefcounter(struct avm_table *t) {
    ++t->refCounter;
    //printf("\e[95m refcount(after incr): %d \e[0m\n", t->refCounter);
}

void avm_tabledecrefcounter(struct avm_table *t) {
    assert(t->refCounter > 0);
    // printf("DEC REF COUNTER: %u\n", t->refCounter);
    if (!--t->refCounter) avm_tabledestroy(t);
}

void avm_tablebucketsinit(struct avm_table_bucket **p) {
    // change it from i++ to ++i 
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; ++i) p[i] = (struct avm_table_bucket *) 0;
}

struct avm_table *avm_tablenew(void) {
    struct avm_table *t = (struct avm_table *) malloc(sizeof(struct avm_table)*2);
    AVM_WIPEOUT(*t);
    t->refCounter = t->total = 0;
    avm_tablebucketsinit(t->numIndexed);
    avm_tablebucketsinit(t->strIndexed);
    avm_tablebucketsinit(t->ufncIndexed);
    avm_tablebucketsinit(t->lfncIndexed);
    avm_tablebucketsinit(t->boolIndexed);
    
    return t;
}

// void avm_tablebucketsdestroy(struct avm_table_bucket **p) {
//     printf("-address %u\n", p[106]);
//     struct avm_table_bucket *bucket, *bucket_next;
//     for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++, p++) {
//         if (p[i]) printf("address %u %u\n", i, p[i]);
//         bucket = p[i];
//         while (bucket) {
//             printf("-------1 %u\n", bucket);
//             bucket_next = bucket->next;
//             printf("-------2\n");
//             avm_memcellclear(&bucket->key);
//             avm_memcellclear(&bucket->value);
//             printf("-------3\n");
//             free(bucket);
//             bucket = bucket_next;
//         }
//         p[i] = (struct avm_table_bucket *) 0;
//     }
// }

void avm_tablebucketsdestroy(struct avm_table_bucket **p) {
    struct avm_table_bucket *b, *del;
    for (unsigned i=0; i<AVM_TABLE_HASHSIZE; i++, p++) {
        for (b = *p; b;) {
            del = b;
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
