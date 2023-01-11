#include <common.h>
#include <fs.h>
#include <sys/time.h>
#include <proc.h>
#include "syscall.h"

static void sys_exit(int code); 
static void sys_yield(); 
static int sys_write(int fd, void *buf, size_t count);
static int sys_brk();
static int sys_open(char* pathname, int flags, int modes);
static size_t sys_read(int fd, void *buf, size_t count);
static off_t sys_lseek(int fd, off_t offset, int whence);
static int sys_close(int fd);
static int sys_gettimeofday(struct timeval *tv, struct timezone *tz);
static int sys_execve(const char* pathname, char *argv[], char *envp[]);

extern void naive_uload(PCB *pcb, const char *filename);
extern void context_uload(PCB *pcb, char *filename, char *const argv[], char *const envp[]);
extern void switch_boot_pcb();

void do_syscall(Context *c) {
  uintptr_t arg[4];
  arg[0] = c->GPR1;
  arg[1] = c->GPR2;
  arg[2] = c->GPR3;
  arg[3] = c->GPR4;

  switch (arg[0]) {
      
      case SYS_exit:
        Log("arg0 is %p", arg[0]);
        Log("arg1 is %p", arg[1]);
        Log("arg2 is %s", arg[2]);
        Log("arg3 is %s", arg[3]);
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
        //Log("arg3 is %d", arg[3]);
        break;

      case SYS_lseek:
        c->GPRx = sys_lseek((int)arg[1], (off_t)arg[2], (int)arg[3]);
        break;

      case SYS_close:
        c->GPRx = sys_close((int)arg[1]);
        break;

      case SYS_gettimeofday:
        c->GPRx = sys_gettimeofday((struct timeval *)arg[1], (struct timezone *)arg[2]);
        break;

      case SYS_execve:
        c->GPRx = sys_execve((char *)arg[1], (char **)arg[2], (char **)arg[3]);
        break;

      default: panic("syscall#%d has not been implemented.\n", c -> mcause);
        break;
  }
}

/****************************syscall********************************/

static void sys_exit(int code) { 
  Log("sys_exit occurs");
  

  //halt(code);
  //sys_execve("/bin/menu", NULL, NULL);
  char *argv[2] = {"/bin/exec-test", NULL};
  char *envp[1] = {NULL};
  sys_execve("/bin/exec-test", argv, envp);
  //context_uload(current, "/bin/exec-test", argv, NULL);
  switch_boot_pcb();
  yield();
}

static void sys_yield() { 
  //Log("sys_yield occurs");

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
  //Log("sys_brk occurs");

  return 0;
}

static int sys_open(char* pathname, int flags, int modes) {
  //Log("sys_open occurs");
  
  return fs_open(pathname, flags, modes);
}

static size_t sys_read(int fd, void *buf, size_t count) {
  //Log("sys_read occurs");
  //Log("reading %p bytes from file %d", count, fd); 
  size_t byte_read = fs_read(fd, buf, count);
  //printf("read bytes %d\n", byte_read);
  if(byte_read <= 0) {
    //Log("file#%d : end of the file", fd);
    return 0;
  }
  return byte_read;
}

static off_t sys_lseek(int fd, off_t offset, int whence) {
  //Log("sys_lseek occurs");

  off_t ret = fs_lseek(fd, offset, whence);
  
  return ret;
}

static int sys_close(int fd) {
  fs_close(fd);
  return 0;
}

static int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  uint64_t us = io_read(AM_TIMER_UPTIME).us;
  tv -> tv_sec = us / 1000000;
  tv -> tv_usec = us;
  return 0;

  /* always success, regardless tz*/
}

static int sys_execve(const char* pathname, char *argv[], char *envp[]) {
  Log("sys_execve occurs");
  if(fs_open(pathname, 0, 0) == -1) return -1;
  
  context_uload(current, (char *)pathname, argv, envp);
  switch_boot_pcb();
  yield();
  panic("should not reach here!");
  return -1;
}