#include <common.h>
#include "syscall.h"
#include <fs.h>
#include <time.h>
int sys_yield(){
	yield();
	return 0;
}
int sys_exit(Context *c){
	halt(0);
  //naive_uload(NULL, "/bin/nterm");
	return 0;
}
int sys_write(Context *c){
	// if (c->GPR2 == 1 || c->GPR2 == 2){
  //   for (int i = 0; i < c->GPR4; ++i){
  //     putch(*(((char *)c->GPR3) + i));
  //   }
  //   return c->GPR4;}
  // else{
    return fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  // }
}
int sys_brk(Context *c){
  // for (uint32_t i=0; i<(int32_t)(c->GPR3); i++) 
  // {
  //   *(uint32_t*)(c->GPR2 + i) = 0;
  // }
  return 0;
}
int sys_open(Context *c) {
  return fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
}

int sys_read(Context *c){
  return fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
}

int sys_close(Context *c){
  return fs_close(c->GPR2);
}

int sys_lseek(Context *c){
  return fs_lseek(c->GPR2, c->GPR3, c->GPR4);
}
int sys_gettimeofday(Context *c){
  struct timeval *tv = (struct timeval*)c->GPR2;
  __uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_usec = time % 1000000;
  tv->tv_sec = time / 1000000;
	return 0;
}
void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;
  switch (a[0]) {
    case 0://SYS_exit
      c->GPRx = sys_exit(c);
      break;
    case 1://SYS_yield
      c->GPRx = sys_yield();
      break;
    case 4://SYS_write
      c->GPRx = sys_write(c);
      break;
    case 9://SYS_brk
      c->GPRx = sys_brk(c);
      break;
    case 2://SYS_open
      c->GPRx = sys_open(c);
      break;
    case 7://SYS_close
      c->GPRx = sys_close(c);
      break;
    case 3://SYS_read
      c->GPRx = sys_read(c);
      break;
    case 8://SYS_lseek
      c->GPRx = sys_lseek(c);
      break;
    case 19://SYS_gettimeofday
      c->GPRx = sys_gettimeofday(c);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}