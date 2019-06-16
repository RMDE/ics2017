#include "common.h"

#define DEFAULT_ENTRY ((void *)0x8048000)

//void ramdisk_read(void*,off_t,size_t);
int fs_open(const char*,int,int);
ssize_t fs_read(int,void*,size_t);
int fs_close(int);
size_t fs_filesz(int);
void* new_page(void);
//size_t get_ramdisk_size();
uintptr_t loader(_Protect *as, const char *filename) {
  //TODO();
  printf("file:%s\n",filename);
  /*nt num=fs_open(filename,0,0);
  size_t len=fs_filesz(num);
  fs_read(num,DEFAULT_ENTRY,len);
  fs_close(num);*/
  uint32_t offset=0;
  void *pa;
  void *va=DEFAULT_ENTRY;
  int num=fs_open(filename,0,0);
  uint32_t page=0;
  uint32_t len=fs_filesz(num);
  uint32_t pagesize=1<<12;
  uint32_t page_num=len>>12;//需要的总内存除以每一页的空间大小
  if(len&0x00000fff)
	  page_num+=1;//最后剩余内容不足一页，但是也要另开辟一页
  while(page<page_num){
	  pa=new_page();
	  if(offset+pagesize<=len)
		  fs_read(num,pa,pagesize);
	  else
		  fs_read(num,pa,len-offset);//最后不足一页部分
	  Log("Map va to pa:0x%08x t0 0x%08x",va,pa);
	  _map(as,va,pa);
	  page++;
	  offset+=pagesize;//当最后不足一页，由于直接就会跳出循环，所以不影响
	  va+=pagesize;
  }
  fs_close(num);
  return (uintptr_t)DEFAULT_ENTRY;
}
