#include "cpu/exec.h"
#include "memory/mmu.h"

void raise_intr(uint8_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  //TODO();
  uint32_t addr=cpu.idtr.base+NO*8;
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
  data3.data1=vaddr_read(addr,4);
  data3.data2=vaddr_read(addr+4,4);
  if((data3.data.present&0x1)!=1)assert(0);
  rtl_push(&cpu.flag);
  uint32_t cs=cpu.cs&0xffff;
  rtl_push(&cs);
  rtl_push(&ret_addr);
  addr=data3.data.offset_31_16&0xffff;
  addr<<=16;
  addr=addr|(data3.data.offset_15_0&0x0000ffff);
  decoding.jmp_eip=addr;
  decoding.is_jmp=true;
}

void dev_raise_intr() {
}
