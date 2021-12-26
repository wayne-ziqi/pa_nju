#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/

make_instr_func(lgdt_v){
    opr_src.type = OPR_MEM;
    opr_src.data_size = 16;
    int len = 1;
    len += modrm_rm(eip + 1, &opr_src);
    uint32_t addr = opr_src.addr;
    cpu.gdtr.limit = laddr_read(addr, 2);
    addr = addr + 2;
    cpu.gdtr.base = laddr_read(addr, 4);
    print_asm_1("lgdt","", len, &opr_src);
    //printf("limit = %#08x, base = %#08x\n", cpu.gdtr.limit, cpu.gdtr.base);
    //printf("len = %d\n", len);
    return len; 
}