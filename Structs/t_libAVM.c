#include "t_libAVM.h"
#include "./../AVM/writer.h"
#define EXPAND_SIZE 1024
#define CURR_SIZE (totalInstructions * sizeof(struct instruction))
#define NEW_SIZE (EXPAND_SIZE * sizeof(struct instruction) + CURR_SIZE)
#define false 0
#define true 1
extern void alpha_yyerror();
extern unsigned alpha_yylineno;
#define _stop_ alpha_yyerror("Stop");
char *vmopcode_name[] = {
    "assign",
    "add",
    "sub",
    "mul",
    "div",
    "mod",
    "uminus",
    "and",
    "or",
    "not",
    "jeq",
    "jne",
    "jle",
    "jge",
    "jlt",
    "jgt",
    "jump",
    "call",
    "pusharg",
    "funcenter",
    "funcexit",
    "newtable",
    "tablegetelem",
    "tablesetelem",
    "nop"
};

struct instruction *instructions = (struct instruction *)0;

unsigned int totalInstructions = 0;
unsigned int currInstruction = 0;
unsigned int currprocessedquads;

// Queue *ij_head;

Stack *funcstack;

void userfunctions_add(unsigned address, unsigned localSize, char *id)
{
    userfunc *new_ufunc = (userfunc *)malloc(sizeof(userfunc));
    new_ufunc->address = (unsigned int)address;
    new_ufunc->id = strdup(id);
    new_ufunc->localSize = (unsigned int) localSize;
    if(localSize >1000000)new_ufunc->localSize=0;
    printf("%u assert\n", localSize);
    totalUserFuncs++;

    Queue_enqueue(userfunctions, new_ufunc);
}

void push_funcstack(SymbolTableRecord *sym)
{
    Stack_append(funcstack, sym);
}
unsigned int nextinstructionlabel()
{
    return currInstruction;
}

typedef void (*generator_func_t)(Quad *);
generator_func_t generators[] = {
    generate_ASSIGN,
    generate_ADD,
    generate_SUB,
    generate_MUL,
    generate_DIV,
    generate_MOD,
    generate_UMINUS,
    generate_AND,
    generate_OR,
    generate_NOT,
    generate_IF_EQ,
    generate_IF_NOTEQ,
    generate_IF_LESSEQ,
    generate_IF_GREATEREQ,
    generate_IF_LESS,
    generate_IF_GREATER,
    generate_JUMP,
    generate_CALL,
    generate_PARAM,
    generate_RETURN,
    generate_GETRETVAL,
    generate_FUNCSTART,
    generate_FUNCEND,
    generate_NEWTABLE,
    generate_TABLEGETELEM,
    generate_TABLESETELEM,
    generate_NOP,
    generate_JUMP
};

void emit_instr(instruction *t)
{
    if (currInstruction == totalInstructions)
        expand_instructions(); // tbchanged

    instruction *inst = instructions + currInstruction++;
    inst->opcode = t->opcode;
    inst->result = t->result;
    inst->arg1 = t->arg1;
    inst->arg2 = t->arg2;
    inst->srcLine = t->srcLine;
}

void reset_operand(vmarg *arg)
{
    arg->type = empty_a;
}

void expand_instructions()
{
    unsigned i = currInstruction;
    assert(totalInstructions == currInstruction);
    instruction *p = (instruction *)malloc(NEW_SIZE);
    if (instructions)
    {
        memcpy(p, instructions, CURR_SIZE);
        free(instructions);
    }
    instructions = p;
    totalInstructions += EXPAND_SIZE;
    for (int k = 0; k < totalInstructions; k++) //init
    {
        instructions[k].arg1.type = empty_a;
        instructions[k].arg2.type = empty_a;
        instructions[k].result.type = empty_a;
        // instructions[k].arg1.val = 66;
        // instructions[k].arg2.val = 66;
        // instructions[k].result.val = 66;
    }
}

void generate(vmopcode op, Quad *quad)
{
    instruction t;
    printf("LINE %u\n", quad->line);
    t.srcLine = quad->line;
    t.opcode = op;
    if (quad->arg1)
    {
        // printf("arg1\n");
        make_operand(quad->arg1, &t.arg1);
    }
    if (quad->arg2)
    {
        // printf("arg2\n");
        make_operand(quad->arg2, &t.arg2);
    }
    if (quad->result)
    {
        // printf("result\n");
        make_operand(quad->result, &t.result);
    }
    quad->taddress = nextinstructionlabel();
    emit_instr(&t);
}

void generateCode(void) // main target code function
{
    //init
    ij_head = Queue_init();
    funcstack = Stack_init();
    userfunctions = Queue_init();
    libfuncs = Queue_init();
    //init
    unsigned int i;
    for (i = 0; i < currQuad; i++)
    {
        assert(quads + i);
        // printf("===> Quad #%d op: %10s %20s\n", i, iopcodeNames[(quads + i)->op], "to target code ...");
        (*generators[quads[i].op])(quads + i);
        currprocessedquads++;
        
    }
    currprocessedquads-=1;
    patch_incomplete_jumps();
    printf("================= Target Code Generated =================\n");
}

#if 1
void make_operand(Expr *e, vmarg *arg) {
if(e==NULL){
    reset_operand(arg);
}
    // printf("%d\n",e->type);
    switch (e->type) {
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case assignexpr_e:
        case boolexpr_e:
        case newtable_e:
            arg->val = e->sym->offset;
            printf("~make_operand for %d %d\n",e->type,e->sym->space);
            switch (e->sym->space) {
                case programvar:
                    arg->type = global_a;
                    break;
                case functionlocal:
                    arg->type = local_a;
                    break;
                case formalarg:
                    arg->type = formal_a;
                    break;
                default:
                    assert(0);
            }
            break;
        case constbool_e:
            arg->val = e->value.boolConst;
            arg->type = bool_a;
            break;
        case conststring_e:
            arg->val = consts_newstring(e->value.strConst);
            arg->type = string_a;
            break;
        case constnum_e:
            arg->val = consts_newnumber(e->value.numConst);
            arg->type = number_a;
            break;
        case nil_e:
            arg->type = nil_a;
            break;
        case programfunc_e:
            // printf("in tcode address is:%d\n",e->sym->taddress);
            // userfuncs_newfunc(e->sym);
            arg->val = e->sym->taddress;
            int sz =Queue_getSize(userfunctions);
            for(int w = 0 ; w <sz; w ++){
                if(((userfunc*)Queue_get(userfunctions,w))->address == arg->val){
                    arg->val = w;
                }
            }
            arg->type = userfunc_a;
            break;
        case libraryfunc_e:
            arg->type = libfunc_a;
            arg->val = libfuncs_newused(e->sym->name);
            break;
        default:
            assert(0);
    }
}
#endif

void patch_incomplete_jumps()
{
    incomplete_jump *iter;
    while (iter = Queue_dequeue(ij_head)) {
        if (iter->iaddress == currInstruction){
            printf("IJ_1 %d %d\n",iter->iaddress,currInstruction);
            instructions[iter->instrNo].result.val = currInstruction;
        }else{
            printf("IJ_2 %d %d %d\n",iter->iaddress,currInstruction,iter->instrNo);
            if(!quads[iter->iaddress-1].taddress)
            instructions[iter->instrNo].result.val = currInstruction;
            else
            {
                instructions[iter->instrNo].result.val  = quads[iter->iaddress-1].taddress;
            }
            
        }
    }
}

void generate_relational(vmopcode op, Quad *quad)
{
    instruction t;
    t.srcLine = quad->line;
    reset_operand(&t.result);
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.opcode = op;
    if (quad->arg1)
    make_operand(quad->arg1, &t.arg1);
    if (quad->arg2)
    make_operand(quad->arg2, &t.arg2);
    t.result.type = label_a;
    // printf("rel gen %s %d %d\n", vmopcode_name[op],quad->label,currprocessedquads);
    if (quad->label < currprocessedquads){
        printf("rel gen %s %d %d\n", vmopcode_name[op],quad->label,currprocessedquads);
        t.result.val = quads[quad->label].taddress-1;
    }
    else{
        printf("rel gen ij %s %d %d\n", vmopcode_name[op],quad->label,currprocessedquads);
        add_incomplete_jump(nextinstructionlabel(), quad->label);
    }
    quad->taddress = nextinstructionlabel();
    emit_instr(&t);
}

void generate_NOT(Quad *quad)
{

    quad->taddress = nextinstructionlabel();
    instruction t;
    t.srcLine = quad->line;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, false);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 3;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 2;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);
}

void generate_OR(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.srcLine = quad->line;
    t.opcode = jeq_v;
    make_operand(quad->arg1, &t.arg1);
    make_booloperand(&t.arg2, true);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 4;
    emit_instr(&t);

    make_operand(quad->arg2, &t.arg1);
    t.result.val = nextinstructionlabel() + 3;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, false);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    t.opcode = jump_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    t.result.val = nextinstructionlabel() + 2;
    emit_instr(&t);

    t.opcode = assign_v;
    make_booloperand(&t.arg1, true);
    reset_operand(&t.arg2);
    make_operand(quad->result, &t.result);
    emit_instr(&t);

    // similar impl for AND
} //funcstack impl for bellow
void generate_AND(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
	instruction t;
    t.srcLine = quad->line;
	t.opcode = jeq_v;
	make_operand(quad->arg1, &t.arg1);
	make_booloperand(&t.arg2, false);
	t.result.type = label_a;
	t.result.val = nextinstructionlabel() + 4;
	emit_instr(&t);

    
	make_operand(quad->arg2, &t.arg1);
	t.result.val = nextinstructionlabel() + 3;
	emit_instr(&t);

    reset_operand(&t.result);
	reset_operand(&t.arg1);

	t.opcode = assign_v;
	make_booloperand (&t.arg1, true);
	make_operand(quad->result, &t.result);
	emit_instr(&t);

	t.opcode = jump_v;
	reset_operand(&t.result);

	t.result.type = label_a;
	t.result.val = nextinstructionlabel() + 2;
	emit_instr(&t);

	t.opcode = assign_v;
	make_booloperand (&t.arg1, false);
	reset_operand(&t.arg2);
	make_operand(quad->result, &t.result);
	emit_instr(&t);
}

void generate_PARAM(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
	instruction t;
    t.srcLine = quad->line;
    t.opcode = pusharg_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    reset_operand(&t.result);
    printf("%d\n",quad->result->type);
    if(quad->result->type==libfunc_a)printf("=====%s\n",quad->result->value.strConst);
    // _stop_;
    make_operand(quad->result, &t.arg1);
    emit_instr(&t);
}

void generate_GETRETVAL(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.srcLine = quad->line;
    t.opcode = assign_v;
    make_operand(quad->result, &t.result);
    make_retvaloperand(&t.arg1);
    emit_instr(&t);
}

void generate_FUNCSTART(Quad *q)
{
    // printf("q->result->sym seg check %s\n",q);
    

    SymbolTableRecord *f = q->arg1->sym;
    assert(f);
    // printf("%s %d %d assert\n", f->name,f->totallocals,f->taddress);
    f->taddress = nextinstructionlabel();
    q->taddress = nextinstructionlabel();
    f->returnList = Queue_init();
    userfunctions_add(f->taddress, f->totallocals, f->name);
    // _stop_
    // push_funcstack(f);

    Stack_append(funcstack, f);
    instruction t_jump;
    t_jump.srcLine = q->line;
    unsigned int *d = (unsigned*)malloc(sizeof(unsigned));;
    reset_operand(&t_jump.result);
    reset_operand(&t_jump.arg1);
    reset_operand(&t_jump.arg2);
    t_jump.opcode = jump_v;
    t_jump.result.type = label_a;
    t_jump.result.val = nextinstructionlabel();
    // printf("jump to funcend %d\n",nextinstructionlabel());
    *d = nextinstructionlabel();
    Queue_enqueue(f->returnList, d);
    emit_instr(&t_jump);

    instruction t;
    t.opcode = funcenter_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    reset_operand(&t.result);
    make_operand(q->arg1, &t.result);
    emit_instr(&t);
    // printf("emit_instr %u\n",t.result.val);
    return;
}
void generate_RETURN(Quad *q)
{
    q->taddress = nextinstructionlabel();
    instruction t;
    t.srcLine = q->line;
    if(q->result){
        make_retvaloperand(&t.result);
    // if(q->result){
        t.opcode = assign_v;
    // printf("first emit done\n");
        make_operand(q->result, &t.arg1);
        emit_instr(&t);
        // }
    }
    // printf("second emit done\n");
    SymbolTableRecord *f = (SymbolTableRecord *)Stack_top(funcstack);
    // printf("%s \n", f->name);

    unsigned int *i =(unsigned*)malloc(sizeof(unsigned));
    *i = nextinstructionlabel();
    Queue_enqueue(f->returnList, i);
    // printf("last emit done\n");
    t.opcode = jump_v;
    reset_operand(&t.result);
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    t.result.type = label_a;
    emit_instr(&t);
    // _stop_;
    return;
}
void generate_FUNCEND(Quad *q)
{
    SymbolTableRecord *f = (SymbolTableRecord *)Stack_pop(funcstack);
    assert(f->returnList);
    backpatch(f->returnList, currInstruction);
    // printf("after bp first emit done\n");

    q->taddress = nextinstructionlabel();
    instruction t;
    t.srcLine = q->line;
    t.opcode = funcexit_v;
    // printf("seclast emit done\n");
    make_operand(q->arg1, &t.result);
    emit_instr(&t);
    // printf("last emit done\n");

    return;
}

void generate_ADD(Quad *q)
{
    generate(add_v, q);
    return;
}
void generate_SUB(Quad *q)
{
    generate(sub_v, q);
    return;
}
void generate_MUL(Quad *q)
{
    generate(mul_v, q);
    return;
}
void generate_DIV(Quad *q)
{
    generate(div_v, q);
    return;
}
void generate_MOD(Quad *q)
{
    generate(mod_v, q);
    return;
}

void generate_UMINUS(Quad *q)
{
    instruction t;
    t.srcLine = q->line;
    t.opcode = mul_v;
    if (q->arg1)
    {
        // printf("arg1\n");
        make_operand(q->arg1, &t.arg1);
    }
    Expr* arg2 = (Expr*)malloc(sizeof(Expr));
    arg2 =  newexpr_constnum(-1);
    q->arg2 = arg2;
    make_operand(q->arg2,&t.arg2);
    if (q->result)
    {
        // printf("result\n");
        make_operand(q->result, &t.result);
    }
    q->taddress = nextinstructionlabel();
    emit_instr(&t);
    return;
}

void generate_NEWTABLE(Quad *q)
{
    generate(newtable_v, q);
    return;
}
void generate_TABLEGETELEM(Quad *q)
{
    generate(tablegetelem_v, q);
    return;
}
void generate_TABLESETELEM(Quad *q)
{
    generate(tablesetelem_v, q);
    return;
}
void generate_ASSIGN(Quad *q)
{
    generate(assign_v, q);
    return;
}
void generate_NOP(Quad *q)
{
    instruction t;
    t.opcode = nop_v;
    emit_instr(&t);
    return;
}

void generate_JUMP(Quad *q)
{
    generate_relational(jump_v, q);
    return;
}
void generate_IF_EQ(Quad *q)
{
    generate_relational(jeq_v, q);
    return;
}
void generate_IF_NOTEQ(Quad *q)
{
    generate_relational(jne_v, q);
    return;
}
void generate_IF_GREATER(Quad *q)
{
    generate_relational(jgt_v, q);
    return;
}
void generate_IF_GREATEREQ(Quad *q)
{
    generate_relational(jge_v, q);
    return;
}
void generate_IF_LESS(Quad *q)
{
    generate_relational(jlt_v, q);
    return;
}
void generate_IF_LESSEQ(Quad *q)
{
    generate_relational(jle_v, q);
    return;
}

unsigned consts_newstring(char *s)
{
    int i;
    for (i=0; i<totalStringConsts; i++)
        if (!strcmp(stringConsts[i], s)) return i;
    if(!totalStringConsts){
		stringConsts = (char **) malloc(sizeof(char*));
	}else{
		stringConsts = (char **) realloc(stringConsts,  sizeof(char*) * (totalStringConsts + 1)  );
	}
    // _stop_
	stringConsts[totalStringConsts++] = strdup(s);
    printf("const string added \"%s\"\n",stringConsts[totalStringConsts-1] );
	return totalStringConsts - 1;
    
}
unsigned consts_newnumber(double n)
{
    int i;
    for (i=0; i<totalNumConsts; i++)
        if (numConsts[i] == n) return i;
    if(!totalNumConsts){
		numConsts = (double *) malloc(sizeof(double));
	}else{
		numConsts = (double *) realloc(numConsts,  sizeof(double) * (totalNumConsts + 1)  );
	}
	numConsts[totalNumConsts++] = n;
    printf("const number added %f\n",numConsts[totalNumConsts-1] );
	return totalNumConsts - 1;
}
unsigned libfuncs_newused(char *s)
{
    char *name;
    int i, size = Queue_getSize(libfuncs);
    for (i = 0; i<size; i++) 
        if (!strcmp(name = (char *)Queue_get(libfuncs, i), s)) break;
    if (i == size) {
        name = strdup(s);
        Queue_enqueue(libfuncs,name);
        totalNamedLibfuncs++;
    }
    printf("name %s at %d\n",name, i);
    return i;
}

void generate_CALL(Quad *quad)
{
    quad->taddress = nextinstructionlabel();
    instruction t;
    t.opcode = call_v;
    reset_operand(&t.arg1);
    reset_operand(&t.arg2);
    reset_operand(&t.result);
    checkLIB(quad);
    make_operand(quad->result, &t.arg1);
    emit_instr(&t);
}
void checkLIB(Quad* q){
    assert(q->result->sym);
    if(q->result->sym->type==LIBFUNC){
        // SymbolTableRecord* dummy = lookup(q->result->sym->name,LIBFUNC,0,1,0,0);

        // if(dummy)
            q->result->type= libraryfunc_e;
    }
}

void make_numberoperand(vmarg *arg, double val)
{
    arg->val = consts_newnumber(val);
    arg->type = number_a;
}
void make_booloperand(vmarg *arg, unsigned val)
{
    arg->val = val;
    arg->type = bool_a;
}
void make_retvaloperand(vmarg *arg)
{
    arg->type = retval_a;
}

void backpatch(Queue *q, unsigned int funcend_position)
{
    // printf("bp\n");
    assert(q);
    unsigned int i= 0 ;
    unsigned sz = Queue_getSize(q)-1;
    unsigned flag = 1;
    unsigned int *k;
    while(k = (unsigned int*) Queue_dequeue(q))
    {
        printf("BP %d %d %d\n",*k,i++,sz);
        if(flag){
            instructions[*k].result.val = funcend_position+1; // initial jump
            flag = 0;
            sz--;
            continue;
        }
        instructions[*k].result.val = funcend_position;
        sz--;
    }
    return;
}


void add_incomplete_jump(unsigned insrtNo, unsigned jump_to)
{
    incomplete_jump *new_ij = (incomplete_jump *)malloc(sizeof(incomplete_jump));
    new_ij->iaddress = jump_to;
    new_ij->instrNo = insrtNo;
    Queue_enqueue(ij_head, new_ij);
    return;
}

void printTables(){
    int i = 0;
    for( i; i < totalNumConsts;i++){
        printf("%d | %f\n",i,numConsts[i]);
    }
    printf("---------------------------------------------------------\n");
    for( i = 0 ; i < totalStringConsts ; i++){
        printf("%d | %s\n",i,stringConsts[i]);
    }
    printf("---------------------------------------------------------\n");
    for( i=0; i < totalUserFuncs; i++){
        userfunc* f = (userfunc*)Queue_get(userfunctions,i);
        printf("%d | Func Address %d, Local Size %u, ID %s\n",i,f->address,f->localSize,f->id);
    }
    // _stop_
        printf("---------------------------------------------------------\n");
    for( i=0; i < totalNamedLibfuncs;i++){
        char* f = strdup((char*)Queue_get(libfuncs,i));
        printf("%d | Lib Func ID %s\n",i,f);
    }
}

void display_instr()
{
    printf("==== Target Code (lastInstruction / Total: %d/%d ) ====\n", currInstruction, totalInstructions);
    printf("=========================================================\n");
    printTables();
    printf("=========================================================\n");
    instruction instr;
    unsigned sz = currInstruction;
    for (int i = 0; i < sz; i++)
    {
        printf("%3d: ",i);
        instr = instructions[i];
        if(instr.result.type > 11)
            reset_operand(&instr.result);
        if(instr.arg1.type > 11)
            reset_operand(&instr.arg1);
        if(instr.arg2.type > 11)
            reset_operand(&instr.arg2);
        
        
        printf("[op %2d] ",instr.opcode);
        printf("\033[0;36m%20s \033[0m", vmopcode_name[instr.opcode]);

        switch(instr.opcode){
            case add_v:
            case sub_v:
            case mul_v:
            case div_v:
            case mod_v:
            case and_v:
            case or_v:
            case jeq_v:
            case jne_v:
            case jle_v:
            case jge_v:
            case jlt_v:
            case jgt_v:
            case tablegetelem_v:
            case tablesetelem_v:
                use_instr_result(instr.result);
                use_instr_arg1(instr.arg1);
                use_instr_arg2(instr.arg2);
                break;
            case assign_v:
            case not_v:
                use_instr_result(instr.result);
                use_instr_arg1(instr.arg1);
                break;
            case jump_v:
            case funcenter_v:
            case funcexit_v:
                use_instr_result(instr.result);
                break;
            case uminus_v:
            case nop_v:
                break;//TBI
            case call_v:
            case pusharg_v:
            case newtable_v:
                use_instr_arg1(instr.arg1);
                break;
            default:
                assert(0);
            
                
        }

        printf("\n");
    }
    

    avmbinaryfile();
}

void use_instr_result(vmarg result){
    userfunc* f1= NULL;
    if(result.type == userfunc_a){
            f1 = (userfunc*)Queue_get(userfunctions,result.val);
    }
    switch (result.type) {
            case empty_a:
                break;
            case label_a:
                printf("00_%u ", result.val);
                break;
            case global_a:
                printf("01_%u ", result.val);
                break;
            case formal_a:
                printf("02_%u ", result.val);
                break;
            case local_a:
                printf("03_%u ", result.val);
                break;
            case number_a:
                printf("04_%u_[%f] ", result.val,numConsts[result.val]);
                break;
            case string_a:
                 printf("05_%u_[\"%s\"] ", result.val,strdup(stringConsts[result.val]));
                break;
            case bool_a:
                printf("06_%u ", result.val);
                break;
            case nil_a:
                printf("07_nill");
                break;
            case userfunc_a:
                printf("08_%u_[%s] ", result.val,strdup((f1->id)));
                break;
            case libfunc_a:
                printf("09_%u_[%s] ", result.val,strdup((char*)Queue_get(libfuncs,result.val)));
                break;
            case retval_a:
                printf("10_(retval) ", result.val);
                break;
            default:
                assert(0);
        }
}

void use_instr_arg1(vmarg arg1){
    userfunc* f2= NULL;
    if(arg1.type == userfunc_a){
            f2 = (userfunc*)Queue_get(userfunctions,arg1.val);
    }
    switch (arg1.type) {
            case empty_a:
                break;
            case label_a:
                printf("00_%u ", arg1.val);
                break;
            case global_a:
                printf("01_%u ", arg1.val);
                break;
            case formal_a:
                printf("02_%u ", arg1.val);
                break;
            case local_a:
                printf("03_%u ", arg1.val);
                break;
            case number_a:
                printf("04_%u_[%f] ", arg1.val,numConsts[arg1.val]);
                break;
            case string_a:
                 printf("05_%u_[\"%s\"] ", arg1.val,strdup(stringConsts[arg1.val]));
                break;
            case bool_a:
                printf("06_%u ", arg1.val);
                break;
            case nil_a:
                printf("07_nill");
                break;
            case userfunc_a:
                printf("08_%u_[%s] ", arg1.val,strdup((f2->id)));
                break;
            case libfunc_a:
                printf("09_%u_[%s] ", arg1.val,strdup((char*)Queue_get(libfuncs,arg1.val)));
                break;
            case retval_a:
                printf("10_(retval) ", arg1.val);
                break;
            default:
                assert(0);
        }
}

void use_instr_arg2(vmarg arg2){
    userfunc* f3= NULL;
    if(arg2.type == userfunc_a){
            f3 = (userfunc*)Queue_get(userfunctions,arg2.val);
    }
    switch (arg2.type) {
            case empty_a:
                break;
            case label_a:
                printf("00_%u ", arg2.val);
                break;
            case global_a:
                printf("01_%u ", arg2.val);
                break;
            case formal_a:
                printf("02_%u ", arg2.val);
                break;
            case local_a:
                printf("03_%u ", arg2.val);
                break;
            case number_a:
                printf("04_%u_[%f] ", arg2.val,numConsts[arg2.val]);
                break;
            case string_a:
                 printf("05_%u_[\"%s\"] ", arg2.val,strdup(stringConsts[arg2.val]));
                break;
            case bool_a:
                printf("06_%u ", arg2.val);
                break;
            case nil_a:
                printf("07_nill");
                break;
            case userfunc_a:
                printf("08_%u_[%s] ", arg2.val,strdup((f3->id)));
                break;
            case libfunc_a:
                printf("09_%u_[%s] ", arg2.val,strdup((char*)Queue_get(libfuncs,arg2.val)));
                break;
            case retval_a:
                printf("10_(retval) ", arg2.val);
                break;
            default:
                assert(0);
        }
}
