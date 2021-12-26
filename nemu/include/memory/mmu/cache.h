#ifndef __CACHE_H__
#define __CACHE_H__

#include "nemu.h"

#ifdef CACHE_ENABLED


extern uint8_t hw_mem[];

#define ASSOC_NUM 8                         //8-way associated
#define CACHEBLOCK_SIZE 64
#define CACHELINE_SIZE 1024

#define markbits(addr) ((addr & 0xffffe000) >> 13)  // higher 19 bits
#define groupbits(addr) ((addr & 0x1fc0) >> 6)     // middle 7 bits
#define offsetbits(addr) (addr & 0x3f)      // lower 6 bits
#define groupBegin(group) (ASSOC_NUM * group);
#define locate(paddr)   \
    uint32_t cur_mark;      \
	uint8_t cur_group;      \
	uint8_t cur_offset;     \
	uint32_t grpBegin;      \
	cur_mark = markbits(paddr);                             \
    cur_group = groupbits(paddr);                           \
    cur_offset = offsetbits(paddr);                         \
    grpBegin = groupBegin(cur_group);                       \
    uint32_t pos;                                                \
    uint32_t last_not_valid = grpBegin + ASSOC_NUM;                               \
	for(pos = grpBegin; pos < grpBegin + ASSOC_NUM; pos++){                   \
	    if(cache[pos].validbit == 1){          \
	        if(cur_mark == cache[pos].mark)break;              \
	    }                                                   \
	    else last_not_valid = pos;     \
	}                                                       




typedef struct{
    bool validbit;
    uint32_t mark;      //mark bits in a cache line : 19 bits
    uint8_t data_block[CACHEBLOCK_SIZE];
}CacheLine;


// init the cache
void init_cache();

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data);

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len);

#endif

#endif
