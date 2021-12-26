#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/

static void instr_execute_1op(){
    OPERAND rel;
    rel.addr = cpu.esp;
    rel.type = OPR_MEM;
    rel.data_size = opr_src.data_size;
    rel.sreg = SREG_SS;
    operand_read(&rel);
    cpu.esp = cpu.esp + opr_src.data_size/8;
    opr_src.val = rel.val;  //数据传送不能忘
    operand_write(&opr_src);
}

make_instr_impl_1op(pop,r,v);

static void pop_regs(){
    OPERAND rel;
    rel.addr = cpu.esp;
    rel.type = OPR_MEM;
    rel.data_size = 32;
    rel.sreg = SREG_SS;
    operand_read(&rel);
    cpu.esp = cpu.esp + 4;
    opr_src.val = rel.val;  //数据传送不能忘
}

make_instr_func(popa){
    //reverse: eax ecx edx ebx esp ebp esi edi
    pop_regs();
    cpu.edi = opr_src.val;
    pop_regs();
    cpu.esi = opr_src.val;
    pop_regs();
    cpu.ebp = opr_src.val;
    pop_regs();
    //throw away esp;
    pop_regs();
    cpu.ebx = opr_src.val;
    pop_regs();
    cpu.edx = opr_src.val;
    pop_regs();
    cpu.ecx = opr_src.val;
    pop_regs();
    cpu.eax = opr_src.val;
    print_asm_0("popa","",1);
    return 1;
}