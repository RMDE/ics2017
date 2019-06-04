#include "common.h"

#define DEFAULT_ENTRY ((void *)0x4000000)

//void ramdisk_read(void*,off_t,size_t);
int fs_open(const char*,int,int);
ssize_t fs_read(int,void*,size_t);
int fs_close(int);
size_t fs_filesz(int);
//size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  printf("file:%s\n",filename);
  int num=fs_open(filename,0,0);
  size_t len=fs_filesz(num);
  fs_read(num,DEFAULT_ENTRY,len);
  fs_close(num);
  return (uintptr_t)DEFAULT_ENTRY;
}
