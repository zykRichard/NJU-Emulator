#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *args;
  uintptr_t *argv_start = args + 1;
  char *argv[argc];
  for(int i = 0; i < argc; i++)
    argv[i] = (char *)(*(argv_start + i));
  
  char *empty[] =  {NULL };
  environ = empty;
  exit(main(argc, argv, empty));
  assert(0);
}
