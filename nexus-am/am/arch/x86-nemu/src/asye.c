#include <am.h>
#include <x86.h>
#include<stdio.h>
static _RegSet* (*H)(_Event,_RegSet*) = NULL;

void vecsys();
void vecnull();
void vectrap();
void vectime();
_RegSet* irq_handle(_RegSet *tf) {
  _RegSet *next = tf;
  if (H) {
    _Event ev;
    switch (tf->irq) {
	  case 0x20:/*printf("in asye.c/irq/time\n");*/ev.event=_EVENT_IRQ_TIME;break;
	  case 0x81:ev.event = _EVENT_TRAP;break;
      case 0x80:printf("in asye.c/irq_handle\n"); ev.event = _EVENT_SYSCALL; break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = H(ev, tf);
    if (next == NULL) {
      next = tf;
    }
  }

  return next;
}

static GateDesc idt[NR_IRQ];

void _asye_init(_RegSet*(*h)(_Event, _RegSet*)) {
  // initialize IDT
  for (unsigned int i = 0; i < NR_IRQ; i ++) {
    idt[i] = GATE(STS_TG32, KSEL(SEG_KCODE), vecnull, DPL_KERN);
  }

  // -------------------- system call --------------------------
  idt[0x80] = GATE(STS_TG32, KSEL(SEG_KCODE), vecsys, DPL_USER);
  // -------------------- kernel trap --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectrap,DPL_USER);
  // -------------------- time --------------------------
  idt[0x81] = GATE(STS_TG32, KSEL(SEG_KCODE), vectime,DPL_USER);
  //printf("IDT[0x80]:%08x\n",idt[128]);
  set_idt(idt, sizeof(idt));

  // register event handler
  H = h;
}

_RegSet *_make(_Area stack, void *entry, void *arg) {
  return NULL;
}

void _trap() {
	printf("in asye.c/_trap\n");
	asm volatile("int $0x81");
}

int _istatus(int enable) {
  return 0;
}
