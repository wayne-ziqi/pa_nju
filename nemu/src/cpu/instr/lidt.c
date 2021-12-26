#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/
make_instr_func(lidt_v){
    opr_src.type = OPR_MEM;
    opr_src.data_size = 16;
    int len = 1;
    len += modrm_rm(eip + 1, &opr_src);
    uint32_t addr = opr_src.addr;
    uint8_t sregT = opr_src.sreg; 
    cpu.idtr.limit = vaddr_read(addr, sregT, 2);
    addr = addr + 2;
    cpu.idtr.base = vaddr_read(addr, sregT, 4);
    print_asm_1("lidt","", len, &opr_src);
    //printf("limit = %#08x, base = %#08x,sregT = %#08x\n", cpu.idtr.limit, cpu.idtr.base, sregT);
    //printf("len = %d\n", len);
    return len; 
}