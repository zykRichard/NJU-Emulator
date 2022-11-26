#include <common.h>

static void do_syscall(Context* c) {

  switch (c -> GPR1)
  {
  case 0:         // exit
    halt(c -> GPR2);
    break;
  
  case 1:        // yield
    yield();
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
