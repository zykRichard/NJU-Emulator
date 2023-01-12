#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
int main(int argc, char *argv[], char *envp[]);
extern char **environ;
void call_main(uintptr_t *args) {
  int argc = *args;
  uintptr_t *argv_start = args + 1;
  char *argv[argc];
  for(int i = 0; i < argc; i++)
    argv[i] = (char *)(*(argv_start + i));
  //printf("That's call_main\n");
  char *envp[] = {NULL};
  // char **envp = (char **)args + 1 + argc; 
  // for(int j = 0; ;j++){
  //   if(envp[j] != NULL)
  //     printf("envp %d : %s\n", j, envp[j]);
  // } 
  environ = envp;
  exit(main(argc, argv, envp));
  assert(0);
}
