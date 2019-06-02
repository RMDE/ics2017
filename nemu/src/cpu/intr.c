#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  printf("%08x\n",cpu.idtr.base);
  uint32_t addr=cpu.idtr.base+NO*8;
  printf("%08x\n",addr);
  //GateDesc data;
  struct{
	union {
	    GateDesc data;
		struct{
			uint32_t data1;
			uint32_t data2;
		};
	};
  }data3;
  data3.data2=vaddr_read(addr,4);
  data3.data1=vaddr_read(addr+4,4);
  printf("%08x %08x\n",data3.data1,data3.data2);
  if((data3.data.present&0x1)!=1)assert(0);
  rtl_push(&cpu.flag);
  uint32_t cs=cpu.cs&0xffff;
  rtl_push(&cs);
  rtl_push(&ret_addr);
  addr=data3.data.offset_31_16&0xffff;
  addr<<=16;
  addr=addr|(data3.data.offset_15_0&0xffff);
  cpu.eip=addr;
  printf("%08x\n",addr);
  /*rtl_push((rtlreg_t *)&cpu.flag);
  rtl_push((rtlreg_t*)&cpu.cs);
  rtl_push((rtlreg_t*)&ret_addr);
  uint32_t idtr_base=cpu.idtr.base;
  uint32_t eip_low,eip_high,offset;
  eip_low=vaddr_read(idtr_base+NO*8,4)&0x0000ffff;
  eip_high=vaddr_read(idtr_base+NO*8+4,4)&0xffff0000;
  offset=eip_low|eip_high;
  printf("%08x\n",offset);
  decoding.jmp_eip=offset;
  decoding.is_jmp=true;*/
}

void dev_raise_intr() {
}
