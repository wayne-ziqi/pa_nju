#include "memory/mmu/cache.h"
#include<stdlib.h>
#ifdef CACHE_ENABLED

CacheLine cache[CACHELINE_SIZE];
extern uint8_t hw_mem[];
extern uint64_t hw_mem_access_time;


//uint32_t hw_mem_read(paddr_t paddr, size_t len);
void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);

// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for(int i = 0; i < CACHELINE_SIZE; i++){
	    //cache[i]. mark = 0;
	    cache[i]. validbit = 0;
	}
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
	locate(paddr)   //got a matched or not matched position
	if(last_not_valid == 0){}   //to avoid the werror of last_not_valid is not used
	if(pos != grpBegin + ASSOC_NUM){  //matched!
	
	    #ifdef MEMTIME_TEST
            hw_mem_access_time += 1;
        #endif
        
	    if(cur_offset + len < CACHEBLOCK_SIZE){
	        memcpy(cache[pos].data_block + cur_offset, &data, len);  //auto stored by LSB
	    }
	    else{
	        size_t lenlower = CACHEBLOCK_SIZE - cur_offset;
	        size_t lenupper = cur_offset + len - CACHEBLOCK_SIZE;
	        memcpy(cache[pos].data_block + cur_offset, &data, lenlower);  //write lower bits
	        uint32_t addrHigher = (paddr & 0xffffffc0) + 0x40;// the next block number
	        uint32_t dataHigher = data >> lenlower * 8;
	        cache_write(addrHigher, lenupper, dataHigher);  // write higher bits
	    }
	    //hw_mem_write(paddr, len, data); //write through
	    memcpy(hw_mem + paddr, &data, len);
	    cache[pos].validbit = 1;
	}
	else{
	    
	    #ifdef MEMTIME_TEST
            hw_mem_access_time += 10;
        #endif
        
	    //hw_mem_write(paddr, len, data); //not write allocate
	    memcpy(hw_mem + paddr, &data, len);
	}
	
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t ret = 0;
	locate(paddr)
	//printf("paddr = %#08x\n", paddr);
	if(pos != grpBegin + ASSOC_NUM){    //matched!
	
	    #ifdef MEMTIME_TEST
            hw_mem_access_time += 1;
        #endif
        
	    if(cur_offset + len < CACHEBLOCK_SIZE){
	        memcpy(&ret, cache[pos].data_block + cur_offset, len);
	    }
	    else { //select from two different blocks
	        size_t lenlower = CACHEBLOCK_SIZE - cur_offset;
	        size_t lenupper = cur_offset + len - CACHEBLOCK_SIZE;
	        memcpy(&ret, cache[pos].data_block + cur_offset, lenlower);
	        uint32_t addrHigher = (paddr & 0xffffffc0) + 0x40;
	        uint32_t retHigher = cache_read(addrHigher, lenupper);  //read the next memory block;
	        ret = (retHigher << lenlower * 8) | ret;
	    }
	}
	else{     //not matched: missed, full
	
	    #ifdef MEMTIME_TEST
            hw_mem_access_time += 10;
        #endif
        
	    if(last_not_valid == grpBegin + ASSOC_NUM){ // the group is full;
	        srand((unsigned)time(NULL));
	        uint32_t new_grp_offset =  rand() % ASSOC_NUM;
	        uint32_t newLine = grpBegin + new_grp_offset;  //randomly selected cache line
	        cache[newLine].validbit = 1;
	        cache[newLine].mark = cur_mark;
	        uint32_t hw_addr = paddr & 0xffffffc0;  //get memory block address
	        memcpy(cache[newLine].data_block, hw_mem + hw_addr, CACHEBLOCK_SIZE);  //copy memory to cache
	        ret = cache_read(paddr, len);
	    }
	    else{  // empty cache line remains
	        uint32_t newLine = last_not_valid;
	        cache[newLine].validbit = 1;
	        cache[newLine].mark = cur_mark;
	        uint32_t hw_addr = paddr & 0xffffffc0;
	        memcpy(cache[newLine].data_block, hw_mem + hw_addr, CACHEBLOCK_SIZE);
	        ret = cache_read(paddr, len);
	    }
	}
	return ret;
}

#endif