#include "cpu/instr.h"
#include "device/port_io.h"
/*
Put the implementations of `out' instructions here.
*/
//use void pio_write(uint16_t port, size_t len, uint32_t data)
make_instr_func(out_b){
    opr_src.data_size = 8;
    opr_src.type = OPR_REG;
    opr_src.addr = REG_AL;
    operand_read(&opr_src);
    uint32_t data = opr_src.val;
    opr_dest.data_size = 16;
    opr_dest.type = OPR_REG;
    opr_dest.addr = REG_DX;
    operand_read(&opr_dest);
    uint16_t port = opr_dest.val;
    pio_write(port, 1, data);
    print_asm_1("out","",1,&opr_dest);
    return 1;
}

make_instr_func(out_v){
    opr_src.data_size = data_size;
    opr_src.type = OPR_REG;
    opr_src.addr = REG_AL;
    operand_read(&opr_src);
    uint32_t data = opr_src.val;
    opr_dest.data_size = 16;
    opr_dest.type = OPR_REG;
    opr_dest.addr = REG_DX;
    operand_read(&opr_dest);
    uint16_t port = opr_dest.val;
    pio_write(port, data_size/8, data);
    print_asm_1("out","",1,&opr_dest);
    return 1;
}
