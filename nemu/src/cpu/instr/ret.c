#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/

make_instr_func(ret_near_l){
    OPERAND ret_dest;
    ret_dest.data_size = 32;
    ret_dest.type = OPR_MEM;
    ret_dest.addr = cpu.esp;
    ret_dest.sreg = SREG_SS;
    operand_read(&ret_dest);
    print_asm_0("ret","",1);
    cpu.eip = ret_dest.val;
    cpu.esp = cpu.esp + 4;
    return 0;
}

make_instr_func(ret_near_iw){
    OPERAND ret_dest;
    OPERAND imm;
    int len = 1;
    ret_dest.data_size = 32;
    ret_dest.type = OPR_MEM;
    ret_dest.addr = cpu.esp;
    ret_dest.sreg = SREG_SS;
    operand_read(&ret_dest);
    
    imm.data_size = 16;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    imm.addr = eip + 1;
    operand_read(&imm);
    len+=data_size/8;
    print_asm_0("ret","",len);
    cpu.eip = ret_dest.val;
    cpu.esp = cpu.esp + 4 + imm.val;  //需要在弹栈之后使栈顶指针多移动immediete位使得实参也被弹出。
    return 0;
}
