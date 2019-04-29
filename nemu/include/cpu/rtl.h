#ifndef __RTL_H__
#define __RTL_H__

#include "nemu.h"

extern rtlreg_t t0, t1, t2, t3;
extern const rtlreg_t tzero;

/* RTL basic instructions */
#define c_add(a, b) ((a) + (b))
#define c_sub(a, b) ((a) - (b))
#define c_and(a, b) ((a) & (b))
#define c_or(a, b)  ((a) | (b))
#define c_xor(a, b) ((a) ^ (b))
#define c_shl(a, b) ((a) << (b))
#define c_shr(a, b) ((a) >> (b))
#define c_sar(a, b) ((int32_t)(a) >> (b))
#define c_slt(a, b) ((int32_t)(a) < (int32_t)(b))
#define c_sltu(a, b) ((a) < (b))
#define make_rtl_arith_logic(name) \
  static inline void concat(rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  static inline void concat3(rtl_, name, i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    *dest = concat(c_, name) (*src1, imm); \
  }
make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(slt)
make_rtl_arith_logic(sltu)
static inline void rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}   //读入立即数

static inline void rtl_mul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("mul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}  //带符号乘法

static inline void rtl_imul(rtlreg_t* dest_hi, rtlreg_t* dest_lo, const rtlreg_t* src1, const rtlreg_t* src2) {
  asm volatile("imul %3" : "=d"(*dest_hi), "=a"(*dest_lo) : "a"(*src1), "r"(*src2));
}   //无符号乘法

static inline void rtl_div(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("div %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}   //带符号除法

static inline void rtl_idiv(rtlreg_t* q, rtlreg_t* r, const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  asm volatile("idiv %4" : "=a"(*q), "=d"(*r) : "d"(*src1_hi), "a"(*src1_lo), "r"(*src2));
}   //无符号除法

static inline void rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);  
}   //内存读取

static inline void rtl_sm(rtlreg_t* addr, int len, const rtlreg_t* src1) {
  vaddr_write(*addr, len, *src1);
}   //内存写入

static inline void rtl_lr_b(rtlreg_t* dest, int r) {
  *dest = reg_b(r);
}   //读取字节寄存器到目的操作数地址

static inline void rtl_lr_w(rtlreg_t* dest, int r) {
  *dest = reg_w(r);
}   //读取单字寄存器到目的操作数地址

static inline void rtl_lr_l(rtlreg_t* dest, int r) {
  *dest = reg_l(r);
}   //读取双字寄存器到目的操作数地址

static inline void rtl_sr_b(int r, const rtlreg_t* src1) {
  reg_b(r) = *src1;
}  //读取源操作数到字节寄存器

static inline void rtl_sr_w(int r, const rtlreg_t* src1) {
  reg_w(r) = *src1;
}   //读取源操作数到单字寄存器

static inline void rtl_sr_l(int r, const rtlreg_t* src1) {
  reg_l(r) = *src1;
}   //读取源操作数到双字寄存器

/* RTL psuedo instructions */

static inline void rtl_lr(rtlreg_t* dest, int r, int width) {
  switch (width) {
    case 4: rtl_lr_l(dest, r); return;
    case 1: rtl_lr_b(dest, r); return;
    case 2: rtl_lr_w(dest, r); return;
    default: assert(0);
  }
}  //读取寄存器到目标操作数地址

static inline void rtl_sr(int r, int width, const rtlreg_t* src1) {
  switch (width) {
    case 4: rtl_sr_l(r, src1); return;
    case 1: rtl_sr_b(r, src1); return;
    case 2: rtl_sr_w(r, src1); return;
    default: assert(0);
  }
}   //读取源操作数到寄存器

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f=*src; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest=cpu.eflags.f; \
  }

make_rtl_setget_eflags(CF)
make_rtl_setget_eflags(OF)
make_rtl_setget_eflags(ZF)
make_rtl_setget_eflags(SF)

#define make_rtl_setget_eflags(f) \
  static inline void concat(rtl_set_, f) (const rtlreg_t* src) { \
    cpu.eflags.f=*src; \
  } \
  static inline void concat(rtl_get_, f) (rtlreg_t* dest) { \
    *dest=cpu.eflags.f; \
  }

static inline void rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  // dest <- src1
  //TODO();
 *dest=*src1;
}

static inline void rtl_not(rtlreg_t* dest) {
  // dest <- ~dest
  //TODO();
  rtlreg_t t0=*dest;//将dest中的数据读入临时寄存器t0
  t0=c_xor(t0,0xffffffff);
  *dest=t0;
}

static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  //TODO();
  rtlreg_t t0=*src1;
  rtlreg_t t1=c_shl(width,3);
  t1=c_sub(0x2,t1); 
  t0=c_shl(t0,t1);//使src1[width*-1]变成最高位
  t0=c_sar(t0,t1); //符号右移
  *dest=t0;
}

static inline void rtl_push(const rtlreg_t* src1) {
  // esp <- esp - 4
  // M[esp] <- src1
  //TODO();
  cpu.gpr[4]._32=c_sub(cpu.gpr[4]._32,4);
  rtl_sm(&cpu.gpr[4]._32,4,src1);  //将src1所指数据放入esp所指内存
  //printf("push/call:0x%08x  0x%08x\n",vaddr_read(cpu.gpr[4]._32,4),*src1);
}

static inline void rtl_pop(rtlreg_t* dest) {
  // dest <- M[esp]
  // esp <- esp + 4
  //TODO();
  rtl_lm(dest,&cpu.gpr[4]._32,4); //将值从esp所指内存放入dest中
  printf("pop/return:0x%08x  0x%08x\n",vaddr_read(cpu.gpr[4]._32,4),*dest);
  cpu.gpr[4]._32=c_add(cpu.gpr[4]._32,4);
}

static inline void rtl_eq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 == 0 ? 1 : 0)
  //TODO();
  rtlreg_t t0=*src1; //将值放入临时寄存器
  t0=!t0;
  *dest=t0;
}

static inline void rtl_eqi(rtlreg_t* dest, const rtlreg_t* src1, int imm) {
  // dest <- (src1 == imm ? 1 : 0)
  //TODO();
  rtlreg_t t0=*src1;
  t0=!(c_xor(t0,imm)); //各位相等皆取0
  *dest=t0;
}

static inline void rtl_neq0(rtlreg_t* dest, const rtlreg_t* src1) {
  // dest <- (src1 != 0 ? 1 : 0)
  //TODO();
  rtlreg_t t0=*src1;
  t0=!!t0;  //0 -> 0; 非0 -> 1
  *dest=t0;
}

static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  //TODO();
  rtlreg_t t0=*src1;
  rtlreg_t t1=c_shl(width,3); //先左移width字节
  t1=c_sub(t1,0x1);
  t0=c_sar(t0,t1); //符号右移
  *dest=c_and(t0,0x1);
}

static inline void rtl_update_ZF(const rtlreg_t* result, int width) {
  // eflags.ZF <- is_zero(result[width * 8 - 1 .. 0])
  //TODO();
  rtlreg_t t0=*result;
  rtlreg_t t1=c_shl(width,3);
  t1=c_sub(32,t1);
  t1=c_shr(0xffffffff,t1); //使其最低width个字节每个位上全为1
  t0=c_add(t0,t1); //只取最低width个字节
  cpu.eflags.ZF=!t0; //0 -> 1; 1 -> 0
}

static inline void rtl_update_SF(const rtlreg_t* result, int width) {
  // eflags.SF <- is_sign(result[width * 8 - 1 .. 0])
  //TODO();
  rtlreg_t t0=*result;
  rtlreg_t t1=c_shl(width,3);
  t1=c_sub(t1,1); //计算移动位数
  t0=c_shr(t0,t1); //移动8*width-1位，此时符号位在最低位
  cpu.eflags.SF=c_and(t0,0x1); //将符号位保留
}

static inline void rtl_update_ZFSF(const rtlreg_t* result, int width) {
  rtl_update_ZF(result, width);
  rtl_update_SF(result, width);
}

#endif
