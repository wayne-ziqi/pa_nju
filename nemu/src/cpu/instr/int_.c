#include "cpu/intr.h"
#include "cpu/instr.h"

/*
Put the implementations of `int' instructions here.

Special note for `int': please use the instruction name `int_' instead of `int'.
*/
make_instr_func(int_){
    OPERAND imm;
    //int len = 1;
    imm.data_size = 8;
    imm.type = OPR_IMM; 
	imm.sreg = SREG_CS; 
	imm.addr = eip + 1; 
	//len += imm.data_size / 8;
	operand_read(&imm);
	uint8_t intr_no = imm.val;
	print_asm_0("int","",2);
	raise_sw_intr(intr_no);
	return 0;
}