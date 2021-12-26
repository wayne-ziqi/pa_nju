#include "common.h"
#include "memory.h"
#include <string.h>

#define VMEM_ADDR 0xa0000
#define SCR_SIZE (320 * 200)
#define NR_PT ((SCR_SIZE + PAGE_SIZE - 1) / PAGE_SIZE) // number of page tables to cover the vmem 15个页表

//PDE vpdir[]
PTE vptable[NR_PTE] align_to_page; // video page tables

PDE *get_updir();

void create_video_mapping()
{
    
	/* TODO: create an identical mapping from virtual memory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) to physical memeory area
	 * [0xa0000, 0xa0000 + SCR_SIZE) for user program. You may define
	 * some page tables to create this mapping.
	 */
	//首先拿到用户进程的页目录表
	
	uint32_t ptable_idx;
	PDE *userpdir = (PDE*)va_to_pa(get_updir());
	PTE *ptable =  (PTE*)va_to_pa(vptable);
	
	userpdir[0].val = make_pde(ptable);

    for (ptable_idx = VMEM_ADDR/PAGE_SIZE; ptable_idx < VMEM_ADDR/PAGE_SIZE + NR_PT; ptable_idx++)
		{
		    vptable[ptable_idx].val = make_pte(ptable_idx << 12);
		    
		}
	//panic("please implement me");
}

void video_mapping_write_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		buf[i] = i;
	}
}

void video_mapping_read_test()
{
	int i;
	uint32_t *buf = (void *)VMEM_ADDR;
	for (i = 0; i < SCR_SIZE / 4; i++)
	{
		assert(buf[i] == i);
	}
}

void video_mapping_clear()
{
	memset((void *)VMEM_ADDR, 0, SCR_SIZE);
}
