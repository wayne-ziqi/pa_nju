#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `in' instructions here.
*/

//call uint32_t pio_read(uint16_t port, size_t len) to get data;
//port 为edx寄存器（dx）中的内容，读出内容放到ax

make_instr_func(in_b){  //0xec
    opr_src.data_size = 16;
    opr_src.type = OPR_REG;
    opr_src.addr = REG_DX;
    operand_read(&opr_src);
    uint16_t port = opr_src.val;
    opr_dest.val = pio_read(port, 1);
    opr_dest.data_size = 8;
    opr_dest.type = OPR_REG;
    opr_dest.addr = REG_AL;
    operand_write(&opr_dest);
    print_asm_1("in","",1,&opr_src);
    return 1;
}

make_instr_func(in_v){  //0xed
    opr_src.data_size = 16;
    opr_src.type = OPR_REG;
    opr_src.addr = REG_DX;
    operand_read(&opr_src);
    uint16_t port = opr_src.val;
    opr_dest.val = pio_read(port, data_size/8);
    opr_dest.data_size = data_size;
    opr_dest.type = OPR_REG;
    opr_dest.addr = REG_AL;
    operand_write(&opr_dest);
    print_asm_1("in","",1,&opr_src);
    return 1;
}
