#include <common.h>

static void sys_exit(int code); 
static void sys_yield(); 
static int sys_write(int fd, void *buf, size_t count);
static int sys_brk();

static void do_syscall(Context* c) {

  switch (c -> GPR1)
  {
  case 0:         // exit
    sys_exit(c -> GPR2);
    break;
  
  case 1:        // yield
    sys_yield();
    break;
  
  case 4:        // write
    c -> GPRx = sys_write((int)(c -> GPR2), (void*)(c -> GPR3), (size_t)(c -> GPR4));
    break;
  
  case 9:       // brk
    c -> GPRx = sys_brk();
    break;

  default: printf("syscall#%d has not been implemented.\n", c -> mcause);
    break;
  }
}

static Context* do_event(Event e, Context* c) {
  switch (e.event) {

    case EVENT_YIELD: printf("Yield occurs!\n"); break;

    case EVENT_SYSCALL: do_syscall(c); break;  
     
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}



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