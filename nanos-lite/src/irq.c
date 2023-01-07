#include <common.h>

extern Context* schedule(Context *prev);

// static void do_syscall(Context* c) {

//   switch (c -> GPR1)
//   {
//   case 0:         // exit
//     sys_exit(c -> GPR2);
//     break;
  
//   case 1:        // yield
//     sys_yield();
//     break;
  
//   case 4:        // write
//     c -> GPRx = sys_write((int)(c -> GPR2), (void*)(c -> GPR3), (size_t)(c -> GPR4));
//     break;
  
//   case 9:       // brk
//     c -> GPRx = sys_brk();
//     break;

//   default: printf("syscall#%d has not been implemented.\n", c -> mcause);
//     break;
//   }
// }
void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {

    case EVENT_YIELD: schedule(c); break;

    case EVENT_SYSCALL: do_syscall(c); break;  
     
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}


