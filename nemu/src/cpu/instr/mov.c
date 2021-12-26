#include "cpu/instr.h"

static void instr_execute_2op() 
{
	operand_read(&opr_src);
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

make_instr_impl_2op(mov, r, rm, b)
make_instr_impl_2op(mov, r, rm, v)
make_instr_impl_2op(mov, rm, r, b)
make_instr_impl_2op(mov, rm, r, v)
make_instr_impl_2op(mov, i, rm, b)
make_instr_impl_2op(mov, i, rm, v)
make_instr_impl_2op(mov, i, r, b)
make_instr_impl_2op(mov, i, r, v)
make_instr_impl_2op(mov, a, o, b)
make_instr_impl_2op(mov, a, o, v)
make_instr_impl_2op(mov, o, a, b)
make_instr_impl_2op(mov, o, a, v)

make_instr_func(mov_zrm82r_v) {
	int len = 1;
	OPERAND r, rm;
	r.data_size = data_size;
	rm.data_size = 8;
	len += modrm_r_rm(eip + 1, &r, &rm);
	
	operand_read(&rm);
	r.val = rm.val;
	operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
	return len;
}

make_instr_func(mov_zrm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);

        operand_read(&rm);
        r.val = rm.val;
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm82r_v) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = data_size;
        rm.data_size = 8;
        len += modrm_r_rm(eip + 1, &r, &rm);
        
	operand_read(&rm);
        r.val = sign_ext(rm.val, 8);
        operand_write(&r);
	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

make_instr_func(mov_srm162r_l) {
        int len = 1;
        OPERAND r, rm;
        r.data_size = 32;
        rm.data_size = 16;
        len += modrm_r_rm(eip + 1, &r, &rm);
        operand_read(&rm);
        r.val = sign_ext(rm.val, 16);
        operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
        return len;
}

// protection mode

make_instr_func(mov_rm2sr_w){  //mov rm to sreg r;
    int len = 1;
    OPERAND sr, rm;
    sr.data_size = rm.data_size = 16;
    len += modrm_r_rm(eip + 1, &sr, &rm);
    uint8_t sregNum = sr.addr;
    operand_read(&rm);
    cpu.segReg[sregNum].val = rm.val;
    //printf("sregNum = %#08x, sreg.val = %#08x\n", sregNum, rm.val);
    load_sreg(sregNum);
    print_asm_2("mov","w",len + 1, &rm, &sr);
    return len;
}


make_instr_func(mov_cr2r_l){
    int len  = 1;
    OPERAND r, cr;
    r.data_size = cr.data_size = 32;
    len += modrm_r_rm(eip + 1, &cr, &r);
    //printf("len == %#08x\n", len);
    //printf("cr.addr == %#08x, r.addr == %#08x\n", cr.addr, r.addr);
    
    if(cr.addr == 0)
        r.val = cpu.cr0.val;
    else if(cr.addr == 3)
        r.val = cpu.cr3.val;
    else{
        printf("there's an unimplemented control register in mov, implement me at mov.c\n");
        assert(0);
    }
    operand_write(&r);
    print_asm_2("mov","l",len, &cr,&r);
    return len;
    
}

make_instr_func(mov_r2cr_l){
    int len = 1;
    OPERAND r,cr;
    r.data_size = cr.data_size = 32;
    len += modrm_r_rm(eip+1, &cr, &r);
    operand_read(&r);
    //printf("cr.addr == %#08x, r.addr == %#08x\n, r.val == %#08x\n", cr.addr, r.addr, r.val);
    if(cr.addr == 0)
        cpu.cr0.val = r.val;
    else if(cr.addr == 3)
        cpu.cr3.val = r.val;
    else{
        printf("there's an unimplemented control register in mov, implement me at mov.c\n");
        assert(0);
    }
    print_asm_2("mov","l",len,&r,&cr);
    return len;
}