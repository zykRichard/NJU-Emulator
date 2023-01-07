#include <proc.h>

#define MAX_NR_PROC 4
extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void(*func)(void *), void *arg); 
extern void context_uload(PCB *pcb, char *filename);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite with arg '%c' for the %dth time!", (uintptr_t)arg, j);
    j ++;
    yield();
  }
}

void init_proc() {
  context_kload(&pcb[0], hello_fun, (void *)'1');
  //context_kload(&pcb[1], hello_fun, (void *)'2');
  context_uload(&pcb[1], "/bin/nterm");
  switch_boot_pcb();
  
  Log("Initializing processes...");
  //yield(); 
  // load program here
  //naive_uload(NULL, "/bin/pal");
}

Context* schedule(Context *prev) {
  printf("ready to schedule\n"); 
  current -> cp = prev;
  current = (current == &pcb[0]) ? &pcb[1] : &pcb[0];

  return current -> cp;
}
