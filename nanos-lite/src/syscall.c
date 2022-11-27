#include <common.h>
#include "syscall.h"

static void sys_exit(int code); 
static void sys_yield(); 
static int sys_write(int fd, void *buf, size_t count);
static int sys_brk();

void do_syscall(Context *c) {
  uintptr_t arg[4];
  arg[0] = c->GPR1;
  arg[1] = c->GPR2;
  arg[2] = c->GPR3;
  arg[3] = c->GPR4;

  switch (arg[0]) {
      
      case SYS_exit:
        sys_exit(arg[1]);
        break;

      case SYS_yield:
        sys_yield();
        break;

      case SYS_write:
        c->GPRx = sys_write((int)arg[1], (void*)arg[2], (size_t)arg[3]);
        break;

      case SYS_brk:
        c->GPRx = sys_brk();
        break;

      default: printf("syscall#%d has not been implemented.\n", c -> mcause);
        break;
  }
}

/****************************syscall********************************/

static void sys_exit(int code) { 
  Log("sys_exit occurs");

  halt(code); 
}

static void sys_yield() { 
  Log("sys_yield occurs");

  yield(); 
}

static int sys_write(int fd, void *buf, size_t count) {
  Log("sys_write occurs");

  char *buf_wr = (char *)buf;
  if(fd == 1 || fd == 2)
  for(int i = 0; i < count; i++)
    putch(buf_wr[i]);

  return count;
}

static int sys_brk() {
  Log("sys_brk occurs");

  return 0;
}