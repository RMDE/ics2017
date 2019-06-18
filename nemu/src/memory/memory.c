#include "nemu.h"

#define PMEM_SIZE (128 * 1024 * 1024)

#define pmem_rw(addr, type) *(type *)({\
    Assert(addr < PMEM_SIZE, "physical address(0x%08x) is out of bound", addr); \
    guest_to_host(addr); \
    })

uint8_t pmem[PMEM_SIZE];

int is_mmio(paddr_t addr);
void mmio_write(paddr_t addr,int len,uint32_t data,int map_NO);
uint32_t mmio_read(paddr_t addr,int len,int map_NO);
/* Memory accessing interfaces */

uint32_t paddr_read(paddr_t addr, int len) {
	int mmio_id=is_mmio(addr);
	if(mmio_id!=-1)
		return mmio_read(addr,len,mmio_id);
	return pmem_rw(addr, uint32_t) & (~0u >> ((4 - len) << 3));
}

void paddr_write(paddr_t addr, int len, uint32_t data) {
	int mmio_id=is_mmio(addr);
	if(mmio_id!=-1)
		mmio_write(addr,len,data,mmio_id);
	else
		memcpy(guest_to_host(addr), &data, len);
}

/*paddr_t page_translate(vaddr_t vaddr){
	paddr_t firaddr=cpu.cr3.page_directory_base&0x000fffff;
	Log("vaddr:0x%08x",vaddr);
	Log("first page's base:0x%08x",firaddr);
	paddr_t move=(vaddr&0xffc00000)>>22;
	PDE pde;
	pde.val=paddr_read(firaddr|(move<<2),4);
	Log("pde:0x%08x",pde.val);
	if(!pde.present)
		assert(0);
	if(pde.accessed==0){
		pde.accessed=1;
		paddr_write(firaddr|(move<<2),4,pde.val);
	}
	paddr_t secaddr=pde.page_frame&0x000fffff;
	move=(vaddr&0x003ff000)>>12;
	Log("move:0x%08x  second page's base:0x%08x",move,secaddr);
	PTE pte;
	pte.val=paddr_read(secaddr|(move<<2),4);
	Log("pte:0x%08x",pte.val);
	if(!pte.present||pte.dirty)
		assert(0);
	if(pte.accessed==0||pte.dirty==0){
		pte.accessed=pte.dirty=1;
		paddr_write(secaddr|(move<<2),4,pte.val);
	}
	paddr_t addr=pte.page_frame&0x000fffff;
	paddr_t paddr=(addr<<12)|(vaddr&0x00000fff);
	Log("paddr:0x%08x",paddr);
	return paddr;
}*/
paddr_t page_translate(vaddr_t vaddr,bool is_write) {
  uint32_t DIR = vaddr >> 22;
  uint32_t PAGE = vaddr >> 12 & 0x000003FF;
  uint32_t OFFSET = vaddr & 0x00000FFF;
  paddr_t PhysicalAddr = vaddr;

  if (cpu.cr0.val & 0x80000000) {
    uint32_t PageTable = paddr_read(cpu.cr3.val + 4 * DIR, 4) & 0xFFFFF000;
    if(!(paddr_read(cpu.cr3.val + 4 * DIR, 4) & 0x00000001)) {
      Log("FATAL: Virtual Address is 0x%08X", vaddr);
      Log("FATAL: eip = 0x%08X at PD", cpu.eip);
    }
    assert(paddr_read(cpu.cr3.val + 4 * DIR, 4) & 0x00000001); // Present
    paddr_write(cpu.cr3.val + 4 * DIR, 4, (paddr_read(cpu.cr3.val + 4 * DIR, 4) | 0x00000020)); // Set accessed
    uint32_t PageTableEntry = paddr_read(PageTable + 4 * PAGE, 4);
    if(!(PageTableEntry & 0x00000001)) {
      Log("FATAL: Virtual Address is 0x%08X", vaddr);
      Log("FATAL: eip = 0x%08X at PT", cpu.eip);
    }
    assert(PageTableEntry & 0x00000001); // Present
    paddr_write(PageTable + 4 * PAGE, 4, (paddr_read(PageTable + 4 * PAGE, 4) | 0x00000020)); // Set accessed
    if (is_write) 
    paddr_write(PageTable + 4 * PAGE, 4, (paddr_read(PageTable + 4 * PAGE, 4) | 0x00000040)); // Set dirty
    PhysicalAddr = (paddr_read(PageTable + 4 * PAGE, 4) & 0xFFFFF000) + OFFSET;
	Log("PhysicalAddr = 0x%08X", PhysicalAddr);
  }
  return PhysicalAddr;
}

uint32_t vaddr_read(vaddr_t addr, int len) {
  //return paddr_read(addr, len);
  if(cpu.cr0.paging){
	  if((addr&0xfff)+len-1>0xfff){ //the capacity of every page is 4kb
		  /*this is a special case*/
		  assert(0);
	  }
	  else{
		  paddr_t paddr=page_translate(addr,0);
		  return paddr_read(paddr,len);
	  }
  }
  else
	  return paddr_read(addr,len);
}

void vaddr_write(vaddr_t addr, int len, uint32_t data) {
  //paddr_write(addr, len, data);
  if(cpu.cr0.paging){
	  if((addr&0xfff)+len-1>0xfff){
		  /*this is a specail case*/
		  assert(0);
	  }
	  else{
		  paddr_t paddr=page_translate(addr,0);
		  return paddr_write(paddr,len,data);
	  }
  }
  else
	  return paddr_write(addr,len,data);
}
