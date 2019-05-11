#include "cpu/exec.h"

make_EHelper(add) {
 // TODO();
 t3=id_dest->val;
 rtl_subi(&t2,&id_dest->val,0x1);
 rtl_shr(&t3,&t3,&t2);
 rtl_add(&t0,&id_dest->val,&id_src->val);
 operand_write(id_dest,&t0);
 rtl_update_ZFSF(&t0,id_dest->width);
 //set CF 无符号溢出标志 
 t1=id_src->val;
 rtl_sltu(&t2,&t0,&t1);
 rtl_set_CF(&t2);//b=a+b < a
 //set SF 带符号溢出标志
 rtl_subi(&t2,&id_src->val,0x1);
 rtl_shr(&t1,&t1,&t2);//取符号位
 rtl_subi(&t2,&id_dest->val,0x1);
 rtl_shr(&t0,&t0,&t2);
 rtl_xor(&t3,&t3,&t1);
 rtl_not(&t3);
 rtl_xor(&t0,&t0,&t1);
 rtl_and(&t0,&t3,&t0);
 rtl_set_SF(&t0); 

 print_asm_template2(add);
}

make_EHelper(sub) {
  //TODO();
 /* rtl_sltu(&t1,&id_dest->val,&id_src->val);//判断是否进位
  rtl_sub(&t0,&id_dest->val,&id_src->val);//将差值存储在t0中
  operand_write(id_dest,&t0);//将结果存入目标操作数
  rtl_update_ZFSF(&t0,id_dest->width);
  //set CF
  rtl_xori(&t1,&t1,0x1);//CF=C(n)^sub(=1)
  rtl_set_CF(&t1);
  //set OF
  rtl_xori(&t3,&id_src->val,0x0);
  rtl_addi(&t3,&t3,0x1);//对减数取补码 -> t3
  rtl_add(&t0,&t2,&t3); //补码之和 -> t0
  rtl_sltu(&t0,&t0,&t3); //判断最高位是否进位 -> t0 ,即C(n)
  rtl_andi(&t2,&id_src->val,0x7fffffff);//被减数去掉最高位 -> t2
  rtl_andi(&t3,&t3,0x7fffffff); //减数去掉最高位 -> t3
  rtl_add(&t1,&t2,&t3);// 和 -> t1
  rtl_sari(&t1,&t1,31); //最高位 -> t1 ,即C(n-1)
  rtl_xor(&t0,&t1,&t0); //OF=C(n-1)^C(n) -> t0
  rtl_set_OF(&t0);*/
  rtl_sub(&t0,&id_dest->val,&id_src->val);//t0=x-y
  operand_write(id_dest,&t0);
  rtl_update_ZFSF(&t0,id_dest->width);
  t1=id_src->val;
  rtl_not(&t1);
  rtl_addi(&t1,&t1,0x1);//取反码，将减法变为加法 t1=~y+1
  rtl_add(&t0,&id_dest->val,&id_src->val);//t0=x+y
  rtl_sltu(&t2,&t0,&id_src->val);//t2=C(n)
  rtl_xori(&t3,&t2,0x1);//t3=C(n)^1
  rtl_set_CF(&t3);//set CF
  rtl_shli(&t0,&id_dest->val,0x1);
  rtl_shri(&t0,&t0,1);//t0=(x<<1)>>1
  rtl_shli(&t1,&t1,0x1);
  rtl_shri(&t1,&t1,1);//t1=(t1<<1)>>1
  rtl_add(&t0,&t0,&t1);//t0=t0+t1
  rtl_shri(&t0,&t0,31);//t0 符号位
  rtl_andi(&t0,&t0,0x1);//t0=C(n-1)
  rtl_xor(&t3,&t2,&t0);//t3=C(n)^C(n-1)
  rtl_set_OF(&t3);//set OF


  print_asm_template2(sub);
}

make_EHelper(cmp) {
  //TODO();
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_update_ZFSF(&t2, id_dest->width);
 
  rtl_sltu(&t0, &t2, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);
  rtl_xor(&t0, &t2, &id_src->val);
  rtl_xor(&t1, &t2, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);
  
  print_asm_template2(cmp);
}

make_EHelper(inc) {
 // TODO();
 rtl_addi(&t0,&id_dest->val,0x1);
 rtl_shri(&t1,&t0,0x1f);
 rtl_shri(&t2,&id_dest->val,0x1f);
 rtl_not(&t1);
 rtl_and(&t1,&t1,&t2);
 operand_write(id_dest,&t0);
 rtl_set_OF(&t1);//只有当最高位为0加1后变为1时溢出
 rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template1(inc);
}

make_EHelper(dec) {
  //TODO();
 rtl_subi(&t0,&id_dest->val,0x1);
 rtl_shri(&t1,&t0,0x1f);
 rtl_shri(&t2,&id_dest->val,0x1f);
 rtl_not(&t1);
 rtl_and(&t1,&t1,&t2);
 operand_write(id_dest,&t0);
 rtl_set_OF(&t1);//只有当最高位为1减1后变为0时溢出
 rtl_update_ZFSF(&t0,id_dest->width);
  print_asm_template1(dec);
}

make_EHelper(neg) {
  TODO();

  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);//低32位借位
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_src2->val, &id_src2->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_src2->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_li(&t1, 0);
      rtl_lr_w(&t0, R_AX);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_li(&t1, 0);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_div(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_sext(&t0, &t0, 2);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_idiv(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(idiv);
}
