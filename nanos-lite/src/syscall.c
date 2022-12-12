#include <common.h>
#include <fs.h>
#include "syscall.h"

static void sys_exit(int code); 
static void sys_yield(); 
static int sys_write(int fd, void *buf, size_t count);
static int sys_brk();
static int sys_open(char* pathname, int flags, int modes);
static size_t sys_read(int fd, void *buf, size_t count);
static off_t sys_lseek(int fd, off_t offset, int whence);
static int sys_close(int fd);

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

      case SYS_open:
        c->GPRx = sys_open((char *)arg[1], (int)arg[2], (int)arg[3]);
        break;

      case SYS_read:
        c->GPRx = sys_read((int)arg[1], (void*)arg[2], (size_t)arg[3]);
        Log("arg3 is %d", arg[3]);
        break;

      case SYS_lseek:
        c->GPRx = sys_lseek((int)arg[1], (off_t)arg[2], (int)arg[3]);
        break;

      case SYS_close:
        c->GPRx = sys_close((int)arg[1]);
        break;

      default: panic("syscall#%d has not been implemented.\n", c -> mcause);
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
  //Log("sys_write occurs");
  //Log("writing %d bytes", count);
  char *buf_wr = (char *)buf;
  if(fd == 1 || fd == 2)
  for(int i = 0; i < count; i++)
    putch(buf_wr[i]);

  else count = fs_write(fd, buf, count);

  return count;
}

static int sys_brk() {
  Log("sys_brk occurs");

  return 0;
}

static int sys_open(char* pathname, int flags, int modes) {
  Log("sys_open occurs");
  
  return fs_open(pathname, flags, modes);
}

static size_t sys_read(int fd, void *buf, size_t count) {
  Log("sys_read occurs");
  //Log("reading %p bytes", count); 
  size_t byte_read = fs_read(fd, buf, count);
  if(byte_read <= 0) {
    Log("file#%d : end of the file", fd);
    return 0;
  }
  return byte_read;
}

static off_t sys_lseek(int fd, off_t offset, int whence) {
  Log("sys_lseek occurs");

  off_t ret = fs_lseek(fd, offset, whence);
  
  return ret;
}

static int sys_close(int fd) {
  fs_close(fd);
  return 0;
}