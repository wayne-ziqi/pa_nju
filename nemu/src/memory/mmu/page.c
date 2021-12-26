#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	paddr_t pdbr = cpu.cr3.pdbr << 12;  //页表目录入口地址
	//printf("laddr = %#08x\n", laddr);
	//printf("cpu.cr3.pdbr = %#08x\n", cpu.cr3.pdbr);
	paddr_t offset1 = laddr >> 22;   //一级页表偏移量 10位
	PDE currPde;
	currPde.val = paddr_read(pdbr + offset1 * 4, 4);  //4 indicates the size of each pde entry
	//printf("currPde.val = %#08x\n", currPde.val);
	assert(currPde.present == 1);
	paddr_t pteBegin = currPde.page_frame << 12; //二级页表入口地址 
	paddr_t offset2 = (laddr >> 12) & 0x000003ff;  //二级页表偏移量10位
	PTE currPte;
	currPte.val = paddr_read(pteBegin + offset2 * 4, 4);
	//printf("currPte.val = %#08x\n", currPte.val);
	assert(currPte.present == 1);    
	paddr_t paddr = (currPte.page_frame << 12) | (laddr & 0x00000fff);
	//printf("paddr = %#08x\n", paddr);
	return paddr;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
