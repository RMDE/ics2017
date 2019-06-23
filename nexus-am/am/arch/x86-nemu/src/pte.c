#include <x86.h>
#include<stdio.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN;
static PTE kptabs[PMEM_SIZE / PGSIZE] PG_ALIGN;
static void* (*palloc_f)();
static void (*pfree_f)(void*);

_Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0,          .end = (void*)PMEM_SIZE}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

void _pte_init(void* (*palloc)(), void (*pfree)(void*)) {
  palloc_f = palloc;
  pfree_f = pfree;

  int i;

  // make all PDEs invalid
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = (uintptr_t)ptab | PTE_P;

      // fill PTE
      PTE pte = PGADDR(pdir_idx, 0, 0) | PTE_P;
      PTE pte_end = PGADDR(pdir_idx + 1, 0, 0) | PTE_P;
      for (; pte < pte_end; pte += PGSIZE) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_cr3(kpdirs);
  set_cr0(get_cr0() | CR0_PG);
}

void _protect(_Protect *p) {
  PDE *updir = (PDE*)(palloc_f());
  p->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
	  updir[i] = kpdirs[i];
  }

  p->area.start = (void*)0x8000000;
  p->area.end = (void*)0xc0000000;
}

void _release(_Protect *p) {
}

void _switch(_Protect *p) {
  set_cr3(p->ptr);
}

void _map(_Protect *p, void *va, void *pa) {
  PDE *pt = (PDE*)p->ptr;
  PDE *pde = &pt[PDX(va)];
  if (!(*pde & PTE_P)) {
    *pde = PTE_P | PTE_W | PTE_U | (uint32_t)palloc_f();
  }
  PTE *pte = &((PTE*)PTE_ADDR(*pde))[PTX(va)];
  if (!(*pte & PTE_P)) {
    *pte = PTE_P | PTE_W | PTE_U | (uint32_t)pa;
  }
}

void _unmap(_Protect *p, void *va) {
}

_RegSet *_umake(_Protect *p, _Area ustack, _Area kstack, void *entry, char *const argv[], char *const envp[]) {
    /**(uint32_t*)(ustack.end-4)=0; //envp
	*(uint32_t*)(ustack.end - 8) = 0; // argv
	*(uint32_t*)(ustack.end - 12) = 0; // argc
	*(uint32_t*)(ustack.end - 16) = 0; // retaddr
	*(uint32_t*)(ustack.end - 20) = 0x00000202; // eflags
	*(uint32_t*)(ustack.end - 24) = 8; // cs
	*(uint32_t*)(ustack.end - 28) = (uint32_t)entry; // eip
	*(uint32_t*)(ustack.end - 32) = 0; // error_code
	*(uint32_t*)(ustack.end - 36) = 0x81; // irq
	printf("in pte.c/_umake\n");
	return (_RegSet*)(ustack.end-68);*/
	uint32_t *stack=(uint32_t*)ustack.end;stack--;
	//stack frame of _start()
	*stack=0;stack--; //argc
	*stack=0;stack--; //argv
	*stack=0;stack--; //envp
	/*/trap frame
	*stack=0x2;stack--; //eflags
	*stack=8;stack--; //cs
	*stack=(uint32_t)entry;stack--; //eip
	*stack=0;stack--; //error_code
	*stack=0x81;stack--; //irq;
	*stack = 0x0; stack--;  //eax
	*stack = 0x0; stack--;  //ecx
	*stack = 0x0; stack--;  //edx
	*stack = 0x0; stack--;  //ebx
	*stack = 0x0; stack--;  //esp
	*stack = (uint32_t)ustack.end; stack--;  //ebp
	*stack = 0x0; stack--;  //esi
	*stack = 0x0; stack--;  //edi
	printf("in pte.c/_umake\n");
    return (_RegSet*)stack;*/
	struct{_RegSet *tf;}*pcb=ustack.start;
	pcb->tf=(void*)(stack-sizeof(_RegSet));
	pcb->tf->eflags=0x2;//|(1<<9);
	pcb->tf->cs=8;
	pcb->tf->eip=(uintptr_t)entry;
	return pcb->tf;	
}
