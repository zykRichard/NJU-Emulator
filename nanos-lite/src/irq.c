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

    case EVENT_YIELD: {//printf("yield detected\n");
    c = schedule(c); /*Log("yield occurs");*/ break;}

    case EVENT_SYSCALL: do_syscall(c);/* Log("syscall occurs");*/break;  

    case EVENT_IRQ_TIMER: c = schedule(c); /*Log("time interrupt occurs");*/break; 
    default: panic("Unhandled event ID = %d", e.event);
  }
  //Log("ctx at %p", c);
  //Log("mepc is %p, sp is %p, mscratch is %p at %p, np is %p", c -> mepc, c->GPRs, c->mscratch,&(c -> mscratch), c-> np);
  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}


