#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>

static Context* (*user_handler)(Event, Context*) = NULL;

extern void __am_get_cur_as(Context *c);
extern void __am_switch(Context *c);

#define SYSCALL_EVENT ((c->mcause >= 0) && (c -> mcause < 20))

Context* __am_irq_handle(Context *c) {
  __am_get_cur_as(c);
  printf("go into irq_handler\n");
  if (user_handler) {
    Event ev = {0};
    // printf("mcause is %p\n", c -> mcause);
    // printf("mstatus is %p\n", c -> mstatus);
    // printf("mepc is %p\n", c -> mepc);
    
    if(SYSCALL_EVENT) ev.event = EVENT_SYSCALL;
    else 
    switch (c->mcause) {
      case -1 : 
        ev.event = EVENT_YIELD;
        break;

      case 0x80000007 :
        ev.event = EVENT_IRQ_TIMER;
        break; 

      default: ev.event = EVENT_ERROR; break;
    }

    c = user_handler(ev, c);
    assert(c != NULL);
  }
  __am_switch(c);
  printf("return from irq_handler\n");
  return c;
}

extern void __am_asm_trap(void);

bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  Context *ctx = (Context *)(kstack.end - sizeof(Context));
  ctx -> mepc = (uintptr_t)entry;
  ctx -> pdir = NULL;
  ctx -> mstatus |= 0x80;
  ctx -> mscratch = 0; // kernel
  ctx -> GPRs = (uintptr_t)(kstack.end);
  asm volatile("csrw mscratch, %0" : : "r"((uintptr_t)(0)));
  if(arg) {
    //printf("arg create by %p\n", arg);
    ctx -> GPR2 = (uintptr_t)arg;
  }
  return ctx;
}

void yield() {
  asm volatile("li a7, -1; ecall");
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
