#include "cpu/cpu.h"
#include "cpu/alu.h"
#include<stdlib.h> //调试

/*=========alu_add implementation========*/

void set_CF_add(uint32_t result,uint32_t src, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF>>(32 - data_size)),data_size);
    src = sign_ext(src & (0xFFFFFFFF >>(32 - data_size)),data_size);
    cpu.eflags.CF = result<src;
}

void set_PF(uint32_t result){
    //result = sign_ext(result & (0xFFFFFFFF>>(32-data_size)),data_size);
    uint32_t tmp = result;
    int cnt = 0;
    int ind  = 0;
    for(;ind < 8; ind++){
        if((tmp & 0x00000001)!=0x00000000)cnt++;
        tmp>>=1;
    }
   
    cpu.eflags.PF = ((cnt & 0x00000001) == 0x00000000) ;
    //printf("%u\n",cpu.eflags.PF);
}

void set_ZF(uint32_t result, uint32_t data_size){
    result = result & (0xFFFFFFFF >> (32-data_size));
    cpu.eflags.ZF = (result == 0);
}

void set_SF(uint32_t result, uint32_t data_size){
    result = sign_ext(result & (0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.SF = sign(result);
}

void set_OF_add(uint32_t result, uint32_t src, uint32_t dest, uint32_t data_size){
    switch(data_size){
        case 8:{
            result = sign_ext(result & 0xFF, 8);
            src = sign_ext(src & 0xFF, 8);
            dest = sign_ext(dest& 0xFF, 8);
            break;
        }
        case 16:{
            result = sign_ext(result & 0xFFFF, 16);
            src = sign_ext(src & 0xFFFF, 16);
            dest = sign_ext(dest & 0xFFFF, 16);
            break;
        }
        default: break;
    }
    if(sign(src) == sign(dest)){
        if(sign(result)!=sign(src))cpu.eflags.OF = 1;
        else cpu.eflags.OF = 0;
    }
    else cpu.eflags.OF = 0;
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest + src; // 获取计算结果
    set_CF_add(res, src, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_add(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*==========================================*/


/*=========alu_adc implementation========*/

void set_CF_adc(uint32_t result, uint32_t src, uint32_t CF, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >>(32 - data_size)),data_size);
    if(CF == 0) cpu.eflags.CF = result < src;
    else if(CF == 1) cpu.eflags.CF = result <= src;
}


uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = 0;
	uint32_t CF = cpu.eflags.CF;
	res = dest + src + CF;
	set_CF_adc(res, src,CF, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_add(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
	
#endif
}

/*================================================*/

/*=========alu_sub implementation========*/

void set_CF_sub(uint32_t result, uint32_t dest, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF>>(32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = result > dest;
}

void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest, size_t data_size){ //两个操作数符号相反，结果符号与减数相同OF=1，其他情况均为OF=0。即减法只有异号才会溢出。
    switch(data_size){       
        case 8:{
            result = sign_ext(result & 0xFF, 8);
            src = sign_ext(src & 0xFF, 8);
            dest = sign_ext(dest& 0xFF, 8);
            break;
        }
        case 16:{
            result = sign_ext(result & 0xFFFF, 16);
            src = sign_ext(src & 0xFFFF, 16);
            dest = sign_ext(dest & 0xFFFF, 16);
            break;
        }
        default: break;
    }
    if(sign(dest) != sign(src)){
        if(sign(result) == sign(src))
            cpu.eflags.OF = 1;
        else 
            cpu.eflags.OF = 0;
    }
    else
        cpu.eflags.OF = 0;
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest - src; // 获取计算结果
    set_CF_sub(res, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/

/*=========alu_sbb implementation========*/


void set_CF_sbb(uint32_t result, uint32_t dest, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF>>(32 - data_size)), data_size);
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    if(cpu.eflags.CF == 1) cpu.eflags.CF = result >= dest;
    else if(cpu.eflags.CF == 0) cpu.eflags.CF = result > dest;
}


uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	uint32_t res = 0;
	uint32_t CF = cpu.eflags.CF;
    res = dest - (src + CF); // 获取计算结果
    set_CF_sbb(res, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_sub(res, src, dest, data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}


/*=================================================*/
/*=========alu_mul implementation========*/

void set_CF_OF_mul(uint64_t result, size_t data_size){//,uint32_t* M_OF,uint32_t* M_CF
    switch(data_size){
        case 8:{
            uint8_t upper = result>>data_size;
            if(upper == 0){
                //*M_OF = *M_CF = 0;
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else{
                //*M_OF = *M_CF = 1;
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
            break;
        }
        case 16:{
            uint16_t upper = result>>data_size;
            if(upper == 0){
                //*M_OF = *M_CF = 0;
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else{
                //*M_OF = *M_CF = 1;
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
            break;
        }
        case 32:{
           uint32_t upper = result>>data_size;
            if(upper == 0){
                //*M_OF = *M_CF = 0;
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else{
                //*M_OF = *M_CF = 1;
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
            break;
        }
        
        default: break;
    }
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
/*
    uint64_t tst_result = __ref_alu_mul(src, dest, data_size);
    uint32_t M_CF = 0,M_OF = 0;
    uint64_t res = 0;
	//dest = dest & (0xFFFFFFFF >> (32 -data_size));
	//src = src & (0xFFFFFFFF >> (32 -data_size));
    res = (uint64_t)dest * (uint64_t)src; // 获取计算结果
    set_CF_OF_mul(res, data_size, &M_OF,&M_CF); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    if(tst_result!=res || M_OF!=cpu.eflags.OF||M_CF!= cpu.eflags.CF){
        printf("data_size = %#x,src = %#x,dest = %#x, tst_result = %llx, my_result =%llx, CF = %#x, my_CF = %#x,OF = %#x,MY_OF = %#x \n",data_size,src,dest,tst_result,res,cpu.eflags.CF,M_CF,cpu.eflags.OF,M_OF);
    }
*/
	return __ref_alu_mul(src, dest, data_size);
#else
	uint64_t res = 0;
	//dest = dest & (0xFFFFFFFF >> (32 -data_size));
	//src = src & (0xFFFFFFFF >> (32 -data_size));
    res = (uint64_t)dest * (uint64_t)src; // 获取计算结果
    set_CF_OF_mul(res,  data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    return res ;//& (0xFFFFFFFFFFFFFFFF >> (64 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/
/*=========alu_mul implementation========*/

void set_CF_OF_imul(int64_t result,size_t data_size){
    switch(data_size){
        case 8:{
            int8_t lower = result;
            int8_t upper = result>>data_size;
            lower = lower>>data_size;
            if(lower == upper){
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else {
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
        }
        case 16:{
            int16_t lower = result;
            int16_t upper = result>>data_size;
            lower = lower>>data_size;
            if(lower == upper){
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else {
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
        }
        case 32:{
            int32_t lower = result;
            int32_t upper = result>>data_size;
            lower = lower>>data_size;
            if(lower == upper){
                cpu.eflags.CF = cpu.eflags.OF = 0;
            }
            else {
                cpu.eflags.CF = cpu.eflags.OF = 1;
            }
        }
    }
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	int64_t res = 0;
	//dest = dest & (0xFFFFFFFF >> (32 -data_size));
	//src = src & (0xFFFFFFFF >> (32 -data_size));
    res = (int64_t)dest * (int64_t)src; // 获取计算结果
    set_CF_OF_imul(res,  data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    return res;  
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	uint32_t res = 0;
	res = dest/src;
	return res & (0xFFFFFFFF >> (32 -data_size));
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	int32_t res = 0;
	res = dest/src;
	return res & (0xFFFFFFFF >> (32 -data_size));
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	uint32_t res = 0;
	res = dest % src;
    return res ;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	int32_t res = 0;
	res = dest%src;
	return res;
#endif
}

/*=========alu_and implementation========*/
uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest & src;// 获取计算结果
    cpu.eflags.CF = 0; // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
	
#endif
}

/*=================================================*/

/*=========alu_xor implementation========*/
uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest ^ src;// 获取计算结果
    cpu.eflags.CF = 0; // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/

/*=========alu_or implementation========*/
uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest | src;// 获取计算结果
    cpu.eflags.CF = 0; // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/
/*=========alu_shl implementation========*/

void set_CF_shl(uint32_t src, uint32_t dest, size_t data_size){
    uint32_t tmp = dest<<(src -1);
    tmp = sign_ext(tmp&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF = sign(tmp);
}

void set_OF_shl(uint32_t src, uint32_t dest, size_t data_size){
    cpu.eflags.OF = sign(dest)^(dest&0x40000000);  //最高位和次位亦或
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest << src;// 获取计算结果
    set_CF_shl(src, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_shl(src,dest,data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/
/*=========alu_shr implementation========*/

void set_CF_shr(uint32_t src, uint32_t dest, size_t data_size){
    uint32_t tmp = dest >> ( src  - 1 );
    tmp = sign_ext(tmp & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = tmp & 0x00000001;
    //*my_cf = tmp & 0x00000001;
}

void set_OF_shr(uint32_t src, uint32_t dest, size_t data_size){
    cpu.eflags.OF = sign(dest);
    //*my_of = sign(dest);
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
/*
    uint32_t tst_result = __ref_alu_shr(src, dest, data_size);
    //printf("result = 0X%#x, CF = 0X%#x, OF = 0X%#x",tst_result,cpu.eflags.CF,cpu.eflags.OF);
    
    uint32_t M_CF = 0,M_OF = 0;
    uint32_t res = 0;
    res = dest >> src;// 获取计算结果
    set_CF_shr(src, dest, data_size,&M_CF); // 设置标志位
    //set_PF(res);
    // set_AF(); // 我们不模拟AF
    //set_ZF(res, data_size);
    //set_SF(res, data_size);
    set_OF_shr(src,dest,data_size,&M_OF);
    res =  res & (0xFFFFFFFF >> (32 - data_size)); // 高位清零并返回
    if(tst_result!=res || M_OF!=cpu.eflags.OF||M_CF!= cpu.eflags.CF){
        printf("data_size = %#x,src = %#x,dest = %#x, tst_result = %#x, my_result =%#x, CF = %#x, my_CF = %#x,OF = %#x,MY_OF = %#x \n",data_size,src,dest,tst_result,res,cpu.eflags.CF,M_CF,cpu.eflags.OF,M_OF);
    }
	return tst_result;
	*/
	return __ref_alu_shr(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size)); 
    res = dest >> src;// 获取计算结果
    set_CF_shr(src, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_shr(src,dest,data_size);
    res =  res & (0xFFFFFFFF >> (32 - data_size)); // 高位清零并返回
    return res;
#endif
}

/*=================================================*/
/*=========alu_sar implementation========*/

void set_CF_sar(uint32_t src, uint32_t dest, size_t data_size){
    switch(data_size){
        case 8:{  
            uint8_t data = dest, tmp;
            tmp = (int8_t)data>>(src-1);
            cpu.eflags.CF = tmp&0x01;
            //*M_CF = tmp&0x01;
            break;
        }
        case 16:{
            uint16_t data = dest, tmp;
            tmp = (int16_t)data>>(src-1);
            cpu.eflags.CF = tmp&0x0001;
            //*M_CF = tmp&0x0001;
            break;
        }
        case 32:{
            uint32_t data = dest, tmp;
            tmp = (int32_t)data>>(src-1);
            cpu.eflags.CF = tmp&0x00000001;
            //*M_CF = tmp&0x0001;
            break;
        }
        default:break;
    }
}


uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
/*
    uint32_t tst_result = __ref_alu_sar(src, dest, data_size);
    uint32_t res = 0 ,M_OF = 0,M_CF = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size)); 
    //res = dest >> src;// 获取计算结果
    switch(data_size){
        case 8: {
            uint8_t data = dest;
            res = (int8_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        case 16:{
            uint16_t data = dest;
            res = (int16_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        case 32:{
            uint32_t data = dest;
            res = (int32_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        default:break;
    }
    
    //set_CF_sar(src, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    //cpu.eflags.OF = 0;
    M_OF = 0;
    res =  res & (0xFFFFFFFF >> (32 - data_size)); // 高位清零并返回
    if(tst_result!=res || M_OF!=cpu.eflags.OF||M_CF!= cpu.eflags.CF){
        printf("data_size = %#x,src = %#x,dest = %#x, tst_result = %#x, my_result =%#x, CF = %#x, my_CF = %#x,OF = %#x,MY_OF = %#x \n",data_size,src,dest,tst_result,res,cpu.eflags.CF,M_CF,cpu.eflags.OF,M_OF);
    }
	return tst_result;
	*/
	return __ref_alu_sar(src, dest, data_size);
#else
	uint32_t res = 0;
	dest = dest & (0xFFFFFFFF >> (32 - data_size)); 
    //res = dest >> src;// 获取计算结果
    switch(data_size){
        case 8: {
            uint8_t data = dest;
            res = (int8_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        case 16:{
            uint16_t data = dest;
            res = (int16_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        case 32:{
            uint32_t data = dest;
            res = (int32_t)data>>src;
            set_CF_sar(src,dest,data_size);
            break;
        }
        default:break;
    }
    
    //set_CF_sar(src, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    cpu.eflags.OF = 0;
    res =  res & (0xFFFFFFFF >> (32 - data_size)); // 高位清零并返回
    return res;
#endif
}

/*=================================================*/
/*=========alu_sal implementation========*/



uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	uint32_t res = 0;
    res = dest << src;// 获取计算结果
    set_CF_shl(src, dest, data_size); // 设置标志位
    set_PF(res);
    // set_AF(); // 我们不模拟AF
    set_ZF(res, data_size);
    set_SF(res, data_size);
    set_OF_shl(src,dest,data_size);
    return res & (0xFFFFFFFF >> (32 -data_size)); // 高位清零并返回
#endif
}

/*=================================================*/