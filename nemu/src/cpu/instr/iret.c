#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/

static void single_pop(){
    OPERAND rel;
    rel.addr = cpu.esp;
    rel.type = OPR_MEM;
    rel.data_size = 32;
    rel.sreg = SREG_SS;
    operand_read(&rel);
    cpu.esp = cpu.esp + 4;
    opr_src.val = rel.val;  //数据传送不能忘
}

make_instr_func(iret){
    print_asm_0("iret","",1);
    single_pop();
    cpu.eip = opr_src.val;
    single_pop();
    cpu.cs.val = opr_src.val;
    load_sreg(SREG_CS);
    single_pop();
    cpu.eflags.val = opr_src.val;
    return 0;
}