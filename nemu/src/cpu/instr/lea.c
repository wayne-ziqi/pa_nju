#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/

make_instr_func(lea_v){
    int len = 1;
    opr_src.data_size = opr_dest.data_size = data_size;
    len += modrm_r_rm(eip + 1, &opr_dest, &opr_src);
    opr_dest.val = opr_src.addr;
    operand_write(&opr_dest);
    print_asm_2("lea", "", len, &opr_src, &opr_dest);
    return len;
}