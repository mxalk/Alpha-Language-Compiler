#include "t_libAVM.h"

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
	generate_CALL,
	generate_PARAM,
	generate_RETURN,
	generate_JUMP,
	generate_GETRETVAL,
	generate_FUNCSTART,
	generate_FUNCEND,
	generate_NEWTABLE,
	generate_TABLEGETELEM,
	generate_TABLESETELEM,
	generate_NOP
};

void generateCode(void){
	unsigned int i;
	for( i = 0; i < currQuad; i++){

		assert(quads +i);
		(*generators[quads[i].op])(quads + i);
	}
	patch_incomplete_jumps();
}
#if 1
void make_operand(Expr* e,vmarg* arg){
    switch(e->type){
        case var_e:
        case tableitem_e:
        case arithexpr_e:
        case boolexpr_e:
        case newtable_e:{
            arg->val = e->sym->offset;
            switch (e->sym->space)
            {
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
        }
        case constbool_e:{
            arg->val = e->value.boolConst;
            arg->type = bool_a;
            break;
        }
        case conststring_e:{
            arg->val = consts_newstring(e->value.strConst);
            arg->type = string_a;
            break;
        }
        case constnum_e:{
            arg->val = consts_newnumber(e->value.numConst);
            arg->type = number_a;
            break;
        }
        case nil_e:{
            arg->type = nil_a;
            break;
        }
        case programfunc_e:{
            arg->val = (unsigned int)e->sym->iaddress; // taddress is recommended ? what is taddress ?
            arg->type = userfunc_a;
            break;
        }
        case libraryfunc_e:{
            arg->val = (unsigned int)e->sym->name; // taddress is recommended ? what is taddress ?
            arg->type = libfuncs_newused(e->sym->name);
            break;
        }
        default:
            assert(0);

    }
}
#endif

void patch_incomplete_jumps() {
    // int i;
    // incomplete_jump* iter = ij_head;
    // for (i = 1 ; i < ij_total ; i++){
    //     // if (iter->iaddress = intermediate code size)
    //     //     instructions[iter->instrNo].result = target code size;
    //     // else
    //     //     instructions[iter->instrNo].result = quads[iter->iaddress].taddress;

    //     iter=iter->next;
    // }
}

void generate_ADD(Quad* q){
    return;
}
void generate_SUB(Quad* q){
    return;
}
void generate_MUL(Quad* q){
    return;
}
void generate_DIV(Quad* q){
    return;
}
void generate_MOD(Quad* q){
    return;
}
void generate_UMINUS(Quad* q){
    return;
}
void generate_NEWTABLE(Quad* q){
    return;
}
void generate_TABLEGETELEM(Quad* q){
    return;
}
void generate_TABLESETELEM(Quad* q){
    return;
}
void generate_ASSIGN(Quad* q){
    return;
}
void generate_NOP(Quad* q){
    return;
}
void generate_JUMP(Quad* q){
    return;
}
void generate_IF_EQ(Quad* q){
    return;
}
void generate_IF_NOTEQ(Quad* q){
    return;
}
void generate_IF_GREATER(Quad* q){
    return;
}
void generate_IF_GREATEREQ(Quad* q){
    return;
}
void generate_IF_LESS(Quad* q){
    return;
}
void generate_IF_LESSEQ(Quad* q){
    return;
}
void generate_AND(Quad* q){
    return;
}
void generate_NOT(Quad* q){
    return;
}
void generate_OR(Quad* q){
    return;
}
void generate_PARAM(Quad* q){
    return;
}
void generate_CALL(Quad* q){
    return;
}
void generate_GETRETVAL(Quad* q){
    return;
}
void generate_FUNCSTART(Quad* q){
    return;
}
void generate_RETURN(Quad* q){
    return;
}
void generate_FUNCEND(Quad* q){
    return;
}

void display_geao(){
    printf("Geia");
}

unsigned consts_newstring(char* s){
    
}
unsigned consts_newnumber(double n){
    
}
unsigned libfuncs_newused(char* s){
    
}


void add_incomplete_jump(unsigned insrtNo, unsigned iaddress){
    return;
}
