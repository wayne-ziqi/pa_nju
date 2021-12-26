#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/


make_instr_func(call_near_v){
    //先执行push(eip)操作
    OPERAND push_dest, imm;
    imm.data_size = data_size;
    imm.addr = eip + 1;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    operand_read(&imm);
    cpu.esp = cpu.esp - 4; 
    push_dest.val = cpu.eip + 1 + data_size / 8; //形如 e8 fe 00 00 00 
    push_dest.addr = cpu.esp;
    push_dest.data_size = 32;
    push_dest.type = OPR_MEM;
    push_dest.sreg = SREG_SS;
    print_asm_1("call","",data_size/8 + 1,&imm);
    operand_write(&push_dest);
    cpu.eip = cpu.eip + imm.val + data_size/8 + 1;

    return 0;
}

make_instr_func(call_gate_v){
    OPERAND  rel, push_dest;
    int len = 1;
    rel.data_size = data_size;
    len += modrm_rm(eip + 1,&rel);
    operand_read(&rel);
    //push eip;
    cpu.esp = cpu.esp - 4;
    push_dest.data_size = 32;
    push_dest.type = OPR_MEM;
    push_dest.addr = cpu.esp;
    push_dest.val = cpu.eip + len; 
    push_dest.sreg = SREG_SS;
    operand_write(&push_dest);
    
    print_asm_1("call","",len,&rel);
    cpu.eip = rel.val;
    return 0;
}