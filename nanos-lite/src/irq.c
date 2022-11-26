#include <common.h>

static void sys_exit(int code) { halt(code); }
static void sys_yield() { yield(); }
static int sys_write(int fd, void *buf, size_t count);


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

static int sys_write(int fd, void *buf, size_t count) {
  char *buf_wr = (char *)buf;
  if(fd == 1 || fd == 2)
  for(int i = 0; i < count; i++)
    putch(buf_wr[i]);

  return count;
}