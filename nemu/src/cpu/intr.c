#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"
OPERAND opr_src;
OPERAND opr_dest;
//operand_write(OPERAND& opr);

static void single_push(){
    //operand_read(&opr_src);
    //printf("esp = %x");
    cpu.esp  = cpu.esp - 4;
    opr_dest.val = opr_src.val;
    opr_dest.data_size = 32;
    opr_dest.addr = cpu.esp;
    opr_dest.sreg = SREG_SS;
    opr_dest.type = OPR_MEM;   //写栈空间
    operand_write(&opr_dest);
}

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
    //printf("call raise_intr\n");
    // push eflags, cs, eip
	opr_src.data_size = 32;
	opr_src.val = cpu.eflags.val;
	single_push();
	opr_src.val = cpu.cs.val;
	single_push();
	opr_src.val = cpu.eip;
	single_push();
	
	// find gate descriptor
	uint32_t gateStart = cpu.idtr.base + intr_no * 8;
	GateDesc currGate;
	currGate.val[0] = vaddr_read(gateStart, SREG_DS, 4);
	currGate.val[1] = vaddr_read(gateStart + 4, SREG_DS, 4);
	//clear IF if interupt
	if(currGate.type == 0xe)//interupt:1110B
	    cpu.eflags.IF = 0;
	else if(currGate.type == 0xf) //trap: 1111B;
	    cpu.eflags.IF = 1;
	else{
	    printf("error gate type(at intr.c)\n");
	    assert(0);
	}
	
	//set eip and cs;
	vaddr_t vaddr = (currGate.offset_31_16 << 16) | currGate.offset_15_0;
	cpu.eip = vaddr;
	cpu.cs.val = currGate.selector;
	load_sreg(SREG_CS);
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2; //eip waiting to be pushed pointing to the instruction after "int $0x80"
	raise_intr(intr_no);
	//assert(0);
}
