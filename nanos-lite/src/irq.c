#include "common.h"

_RegSet* do_syscall(_RegSet *r);

static _RegSet* do_event(_Event e, _RegSet* r) {
	printf("%d\n",e.event);
  switch (e.event) {
	case 8: do_syscall(r);break;
	case 4:do_syscall(r);break;
    default: panic("Unhandled event ID = %d", e.event);
  }
  printf("return\n");
  return NULL;
}

void init_irq(void) {
  _asye_init(do_event);
}
