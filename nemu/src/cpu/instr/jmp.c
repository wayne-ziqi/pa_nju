#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short){
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;
    
    operand_read(&rel);
    
    int offset = sign_ext(rel.val,8);
    
    print_asm_1("jmp", "",2, &rel);
    
    cpu.eip += offset;
    
    return 2;
}

make_instr_func(jmp_near_rm){
    OPERAND rel;
    int len = 1;
    rel.data_size = data_size;
    len += modrm_rm(eip + 1, &rel);
  
    
    operand_read(&rel);
    
    int offset = sign_ext(rel.val, data_size);
    print_asm_1("jmp","",len,&rel);
    
    cpu.eip = offset;
    
    return 0;
}

make_instr_func(jmp_far){  //高两位给CS， 低四位给eip.
    int len = 1;
    
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 32;
    rel.addr = eip + 1;
    operand_read(&rel);
    //printf("rel.val = %#08x\n", rel.val);
    len += 4;
    
    print_asm_1("ljmp","",len + 2,&rel);
    
    cpu.cs.val = laddr_read(cpu.eip + len, 2) & 0xffff;
    //printf("cpu.cs.val == %#08x\n", cpu.cs.val);
    len = len + 2;
    load_sreg(SREG_CS);
    
    cpu.eip = rel.val;  //最后再改
    return 0;
}
