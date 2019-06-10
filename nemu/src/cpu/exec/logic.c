#include "cpu/exec.h"

make_EHelper(test) {
  //TODO();
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_and(&t0,&id_dest->val,&id_src->val);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_set_CF(&tzero);
  rtl_set_OF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0); //将计算结果存入目标操作数
  rtl_set_CF(&tzero);//CF=0
  rtl_set_OF(&tzero);//OF=0
  rtl_update_ZFSF(&t0,id_dest->width);//更新SF，ZF

  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_set_OF(&tzero);
  rtl_set_CF(&tzero);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  //printf("%08x\n",id_dest->val);
  rtl_sext(&t0,&id_dest->val,id_dest->width);
  rtl_sar(&t0,&t0,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(shl);
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
 // TODO();
 t0=id_dest->val;
 rtl_not(&t0);
 operand_write(id_dest,&t0);

  print_asm_template1(not);
}

make_EHelper(rol) {
  t0=id_src->val;
  t3=id_dest->val;
  unsigned temcf=0;
  t1=(id_dest->width<<3)-1;
  while(t0!=0){
	temcf=(t3>>t1)&0x1;
	rtl_shli(&t3,&t3,1);
	rtl_add(&t3,&t3,&temcf);
	rtl_subi(&t0,&t0,1);
  }
  operand_write(id_dest,&t3);
  rtl_set_CF(&temcf);
  if(id_src->val==1) {
	t2=(t3>>t1)&0x1;
	t2=(t2!=temcf);
	rtl_set_OF(&t2);
  }
}
