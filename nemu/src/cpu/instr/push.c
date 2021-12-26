#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/

static void instr_execute_1op(){
    operand_read(&opr_src);
    //printf("esp = %x");
    cpu.esp  = cpu.esp - 4;
    opr_dest.val = opr_src.val;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp;
    opr_dest.sreg = SREG_SS;
    opr_dest.type = OPR_MEM;   //写栈空间
    operand_write(&opr_dest);
}

make_instr_impl_1op(push,r,v)
make_instr_impl_1op(push,rm,v)
make_instr_impl_1op(push,i,b)
make_instr_impl_1op(push,i,v)


make_instr_func(push_cs){
    cpu.esp = cpu.esp - 4;
    opr_dest.val = cpu.cs.val;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp;
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    operand_write(&opr_dest);
    return 1;
}

static void push_regs(){
    cpu.esp  = cpu.esp - 4;
    opr_dest.val = opr_src.val;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp;
    opr_dest.sreg = SREG_SS;
    opr_dest.type = OPR_MEM;   //写栈空间
    operand_write(&opr_dest);
}
make_instr_func(pusha){
    // eax ecx edx ebx esp ebp esi edi
    uint32_t tmpEsp = cpu.esp;
    opr_src.val = cpu.eax;
    push_regs();
    opr_src.val = cpu.ecx;
    push_regs();
    opr_src.val = cpu.edx;
    push_regs();
    opr_src.val = cpu.ebx;
    push_regs();
    opr_src.val = tmpEsp;
    push_regs();
    opr_src.val = cpu.ebp;
    push_regs();
    opr_src.val = cpu.esi;
    push_regs();
    opr_src.val = cpu.edi;
    push_regs();
    print_asm_0("pusha","",1);
    //assert(0);
    return 1;
}


