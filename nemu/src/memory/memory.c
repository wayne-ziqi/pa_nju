#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];
uint64_t hw_mem_access_time = 0;

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len) 
{
    int map_No = is_mmio(paddr);
    if(map_No == -1){
        uint32_t ret = 0;
    #ifdef CACHE_ENABLED
		ret = cache_read(paddr, len);     // 通过cache进行读
    #else
		ret = hw_mem_read(paddr, len);
		
		#ifdef MEMTIME_TEST
		    hw_mem_access_time += 10;
		#endif
    #endif
	    return ret;
    }else{  // map_No 传入mmio_read();
	    return mmio_read(paddr, len, map_No);
	}
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data) 
{
    int map_No = is_mmio(paddr);
    if(map_No == -1){
    #ifdef CACHE_ENABLED
		cache_write(paddr, len, data);    // 通过cache进行写
    #else
		hw_mem_write(paddr, len, data);
		
		#ifdef MEMTIME_TEST
		    hw_mem_access_time += 10;
		#endif
    #endif
    }else{
        mmio_write(paddr, len, data, map_No);
    }
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{  
#ifndef IA32_PAGE
	return paddr_read(laddr, len);
#else
    if(cpu.cr0.pg == 1){
        //printf("len = %u\n", len);
        uint32_t d_laddr = (laddr & 0x00000fff) + len - 0x00001000;  //超出的部分
        if((int)d_laddr > 0){  //数据发生跨页需要将一次读写拆分成两次物理地址读写来进行
            //printf("r_cross!\n");
            //printf("len = %u\n,d_laddr = %u\n", len, d_laddr);
            paddr_t paddr = page_translate(laddr);
            uint32_t lower = paddr_read(paddr, len - d_laddr);
            paddr = page_translate(laddr + len - d_laddr);
            uint32_t upper = paddr_read(paddr, d_laddr) << ((len - d_laddr) * 8);
            //assert(0);
            return upper | lower;
        }else{
            paddr_t paddr = page_translate(laddr);
            return paddr_read(paddr, len);
        }
    }
    else
        return paddr_read(laddr, len);
#endif
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
#ifndef IA32_PAGE
	paddr_write(laddr, len, data); 
#else
    if(cpu.cr0.pg == 1){
        uint32_t d_laddr = (laddr & 0x00000fff) + len - 0x00001000;
        if((int)d_laddr > 0){   //数据发生跨页需要将一次读写拆分成两次物理地址读写来进
            //printf("w_cross!\n");
            //printf("len = %u\n", len);
            paddr_t paddr = page_translate(laddr);
            uint32_t lower = (data << d_laddr * 8) >> (d_laddr * 8);  //如0x12345678 高8位在另一页 低位：0x00345678, 高位：0x00000012
            paddr_write(paddr, len - d_laddr, lower);
            paddr = page_translate(laddr + len - d_laddr);
            uint32_t upper = data >> ((len - d_laddr) * 8);
            paddr_write(paddr, d_laddr, upper);
            
        }else{
            paddr_t paddr = page_translate(laddr);
            paddr_write(paddr, len, data);
        }
    }
    else paddr_write(laddr, len, data);
#endif
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG 
	    return laddr_read(vaddr, len);
#else
        uint32_t laddr = vaddr;
        if(cpu.cr0.pe == 1){   //protected mode enabled
            laddr = segment_translate(vaddr,sreg);
        }
        return laddr_read(laddr, len);
#endif
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifndef IA32_SEG
	    laddr_write(vaddr, len, data);
#else
        uint32_t laddr = vaddr;
        if(cpu.cr0.pe == 1){
            laddr = segment_translate(vaddr, sreg);
        }
        laddr_write(laddr, len, data);
#endif
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
    #ifdef CACHE_ENABLED
	    init_cache();                             // 初始化cache
    #endif
    #ifdef TLB_ENABLED
	    make_all_tlb();
	    init_all_tlb();
    #endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
