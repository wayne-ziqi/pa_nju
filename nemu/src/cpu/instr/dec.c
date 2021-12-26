#include "cpu/instr.h"
/*
Put the implementations of `dec' instructions here.
*/

static void instr_execute_1op(){
    operand_read(&opr_src);
    uint32_t originCF = cpu.eflags.CF;
    opr_src.val = alu_sub(0x1,opr_src.val,opr_src.data_size);
    cpu.eflags.CF = originCF;
    operand_write(&opr_src);
}

make_instr_func(dec_v){
    int len = 1;
    len += modrm_rm(eip+1,&opr_dest);
    opr_dest.data_size = data_size;
    operand_read(&opr_dest);
    uint32_t originCF = cpu.eflags.CF;
    opr_dest.val = alu_sub(0x1,opr_dest.val,data_size);
    operand_write(&opr_dest);
    cpu.eflags.CF = originCF;  //对CF的值没有影响
    print_asm_1("dec","",len,&opr_dest);
    return len;
}


make_instr_impl_1op(dec,r,v)
