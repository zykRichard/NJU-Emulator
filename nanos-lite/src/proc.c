#include <proc.h>

#define MAX_NR_PROC 4
extern void naive_uload(PCB *pcb, const char *filename);
extern void context_kload(PCB *pcb, void(*func)(void *), void *arg); 
extern void context_uload(PCB *pcb, char *filename, char *argv[], char *envp[]);

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;
int fg_pcb = 2;

void switch_boot_pcb() {
  printf("switch to boot\n");
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
  char * argv3[] = {"/bin/bird", NULL};
  char * envp3[] = {NULL};
  context_uload(&pcb[3], "/bin/bird", argv3, envp3);

  char * argv2[] = {"/bin/nterm", NULL};
  char * envp2[] = {NULL};
  context_uload(&pcb[2], "/bin/nterm", argv2, envp2);
  //char * argv0[] = {"/bin/pal", "--skip", NULL};
  //char * envp0[] = {NULL};
  //context_uload(&pcb[1], "/bin/nterm", argv1, envp1);
  //context_uload(&pcb[0], "/bin/pal", argv0, envp0);
  context_kload(&pcb[0], hello_fun, (void *)'2');
  //context_uload(&pcb[0], "/bin/nterm");
  //context_uload(&pcb[1], "/bin/pal");
  char *argv1[] = {"/bin/pal", "--skip", NULL};
  char *envp1[] = {NULL};
  context_uload(&pcb[1], "/bin/pal", argv1, envp1);
  switch_boot_pcb();
  
  Log("Initializing processes...");
  yield(); 
  // load program here
  //naive_uload(NULL, "/bin/exec-test");
}

Context* schedule(Context *prev) {
  //printf("ready to schedule\n");
 
  current -> cp = prev;
  current = (current == &pcb[0]) ? &pcb[fg_pcb] : &pcb[0];
  //current = &pcb[0];
  return current -> cp;
}
