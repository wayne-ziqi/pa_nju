#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base = cpu.segReg[sreg].base;
	uint32_t limit = cpu.segReg[sreg].limit;
	//printf("sreg = %#08x, offset = %#08x\n", sreg, offset);
	if(offset > limit * 0x1000){  //granularity == 1 4kB
	    printf("segmentation fault\n");
	    assert(0);
	}

	return base + offset;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)  
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	 assert(sreg < 6);
	 SegDesc currDesc;
	 if(cpu.segReg[sreg].index > cpu.gdtr.limit){ //超出段表
	     printf("segmentation fault\n");
	     assert(0);
	 }
	 assert(cpu.segReg[sreg].ti == 0);  //只模拟ti==0
	 //提取
	 uint32_t startAddr = cpu.gdtr.base + cpu.segReg[sreg].index * 8; // 8 is the size of a descriptor, locate the sreg descriptor
	 currDesc.val[0] = laddr_read(startAddr, 4);
	 currDesc.val[1] = laddr_read(startAddr + 4, 4);
	 //next step: 提取 limit和base.
	 uint32_t segLimit = (currDesc.limit_19_16 << 16) | (currDesc.limit_15_0);
	 uint32_t segBase = (((currDesc.base_31_24 << 8) | currDesc.base_23_16) << 16) | currDesc.base_15_0;
	 //printf("segLimit = %#08x, segBase = %#08x\n", segLimit, segBase);
	 //必要检查
	 if(cpu.segReg[sreg].rpl > currDesc.privilege_level){
	     printf("segmentation fault\n");
	     assert(0);
	 }
	 assert(currDesc.present == 1);
	 assert(currDesc.granularity == 1);
	 assert(segLimit == 0x000fffff);
	 assert(segBase == 0);
	 //next step: 越界检查 在指令中进行
	 
	 //next step: 存入缓存
	 cpu.segReg[sreg].base = segBase;
	 cpu.segReg[sreg].limit = segLimit;
	 cpu.segReg[sreg].privilege_level = currDesc.privilege_level;
	 cpu.segReg[sreg].soft_use = currDesc.soft_use;
	 cpu.segReg[sreg].type = currDesc.type;
	 
}
