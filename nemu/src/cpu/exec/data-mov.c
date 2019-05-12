#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&t0);
  operand_write(id_dest,&t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
 // TODO();
 rtl_mv(&cpu.esp,&cpu.ebp);
 rtl_pop(&cpu.ebp);

  print_asm("leave");
}

make_EHelper(cltd) {
  if (decoding.is_operand_size_16) {
    //TODO();
	rtl_sext(&t1,&cpu.eax,16);
	rtl_slt(&t0,&t1,&tzero);
	if(t0)
		cpu.gpr[3]._16=0xffff;
	else
		cpu.gpr[3]._16=0x0;
  }
  else {
    //TODO();
	rtl_slt(&t0,&cpu.eax,&tzero);
	if(t0)
		cpu.edx=0xffffffff;
	else
		cpu.edx=0x0;
  }

  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");
}

make_EHelper(cwtl) {
  if (decoding.is_operand_size_16) {
    //TODO();
	rtl_sext(&t1,&cpu.eax,8);
	rtl_slt(&t0,&t1,&tzero);
	if(t0)
		cpu.gpr[0]._8[0]=0xff;
	else
		cpu.gpr[0]._8[0]=0x0;
  }
  else {
    //TODO();
	rtl_sext(&t1,&cpu.eax,16);
	cpu.eax=t1;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
