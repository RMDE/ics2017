#include "common.h"
#include "syscall.h"
#include "fs.h"

int mm_brk(uint32_t new_brk);
static inline uintptr_t sys_none(){
	return 1;
}

static inline uintptr_t sys_open(uintptr_t pathname, uintptr_t flags, uintptr_t mode) {
  TODO();
  return 1;
}

static inline uintptr_t sys_write(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  //TODO();
  if(fd==1||fd==2){
	  char* p=(char*)buf;
	  int i;
	  for(i=0;i<len;i++)
		  _putc(*p);
	  return i;
  }
  return -1;
}

static inline uintptr_t sys_read(uintptr_t fd, uintptr_t buf, uintptr_t len) {
  TODO();
  return 1;
}

static inline uintptr_t sys_lseek(uintptr_t fd, uintptr_t offset, uintptr_t whence) {
  return fs_lseek(fd, offset, whence);
}

static inline uintptr_t sys_close(uintptr_t fd) {
  TODO();
  return 1;
}

static inline uintptr_t sys_brk(uintptr_t new_brk) {
  TODO();
  return 1;
}
static inline uintptr_t sys_exit(int code){
	_halt(code);
	return 1;
}

_RegSet* do_syscall(_RegSet *r) {
  uintptr_t a[4];
  a[0] = SYSCALL_ARG1(r);
  a[1] = SYSCALL_ARG2(r);
  a[2] = SYSCALL_ARG3(r);
  a[3] = SYSCALL_ARG4(r);
  printf("%d %d %d %d\n",a[0],a[1],a[2],a[3]);
  switch (a[0]) {
	case 0: SYSCALL_ARG1(r)=sys_none();break;
	//case 3: SYSCALL_ARG1(r)=sys_write();break;
	case 4: SYSCALL_ARG1(r)=sys_exit(0);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}
