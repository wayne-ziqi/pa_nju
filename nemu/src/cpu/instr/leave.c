#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/

make_instr_func(leave_v){
    OPERAND d_esp,d_ebp;
    d_esp.data_size = d_ebp.data_size = data_size;
    d_esp.sreg = d_ebp.sreg = SREG_SS; 
    d_ebp.type = OPR_REG;
    d_ebp.addr = REG_BP;
    operand_read(&d_ebp);  //现在d_ebp中的值为reg ebp中的值
    
    d_esp.type = OPR_REG;   //先将ebp的值写入到esp中，现在esp指向原ebp指向的值
    d_esp.addr = REG_SP;
    d_esp.val = d_ebp.val;
    operand_write(&d_esp);
    
    d_esp.type = OPR_MEM;  //将esp指向的值弹出放入到ebp中
    d_esp.addr = cpu.esp;
    operand_read(&d_esp);
    //此时d_ebp的类型还是reg
    
    d_ebp.val = d_esp.val;
    operand_write(&d_ebp);
    
    print_asm_0("leave","",1);
    
    cpu.esp = cpu.esp + 4;
    
    return 1;
}