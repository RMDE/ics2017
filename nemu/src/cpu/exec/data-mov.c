#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  //TODO();
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_push(&t0);
  print_asm_template1(push);
}

make_EHelper(pop) {
  //TODO();
  rtl_pop(&t0);
  operand_write(id_dest,&t0);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  //TODO();
  t0=cpu.esp;
  rtl_push(&cpu.eax);
  rtl_push(&cpu.ecx);
  rtl_push(&cpu.edx);
  rtl_push(&cpu.ebx);
  rtl_push(&t0);
  rtl_push(&cpu.ebp);
  rtl_push(&cpu.esi);
  rtl_push(&cpu.edi);

  print_asm("pusha");
}

make_EHelper(popa) {
  //TODO();
  rtl_pop(&cpu.edi);
  rtl_pop(&cpu.esi);
  rtl_pop(&cpu.ebp);
  rtl_pop(&t0);
  rtl_pop(&cpu.ebx);
  rtl_pop(&cpu.edx);
  rtl_pop(&cpu.ecx);
  rtl_pop(&cpu.eax);
  cpu.esp=t0;

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
	rtl_sext(&t1,&cpu.eax,2);
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
	rtl_sext(&t1,&cpu.eax,1);
	rtl_slt(&t0,&t1,&tzero);
	if(t0)
		cpu.gpr[0]._8[1]=0xff;
	else
		cpu.gpr[0]._8[1]=0x0;
  }
  else {
    //TODO();
	rtl_sext(&t1,&cpu.eax,2);
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
