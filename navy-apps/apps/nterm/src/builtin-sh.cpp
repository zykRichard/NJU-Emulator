#include <nterm.h>
#include <stdarg.h>
#include <unistd.h>
#include <SDL.h>

char handle_key(SDL_Event *ev);
static char split[2] = " ";

static void sh_printf(const char *format, ...) {
  static char buf[256] = {};
  va_list ap;
  va_start(ap, format);
  int len = vsnprintf(buf, 256, format, ap);
  va_end(ap);
  term->write(buf, len);
}

static void sh_banner() {
  sh_printf("Built-in Shell in NTerm (NJU Terminal)\n\n");
}

static void sh_prompt() {
  sh_printf("sh> ");
}

static void sh_handle_cmd(const char *cmd) {
  int MAX_ARG = 16;
  char cmd_buf[32], fname[16];
  strcpy(cmd_buf, cmd);
  // eliminate '\n':
  cmd_buf[strlen(cmd) - 1] = '\0';

  int argc = -1;
  char *argv[MAX_ARG] = {};

  char *token;
  token = strtok(cmd_buf, split);
  while(token != NULL) {
    if(argc == -1){
      strcpy(fname, token);
      argc = 0;
      token = strtok(NULL, split);
    }
    else {
      argv[argc ++] = token;
      token = strtok(NULL, split);
    }
  }
  execvp(fname, argv);
}


void builtin_sh_run() {
  sh_banner();
  sh_prompt();
  
  setenv("PATH", "/bin", 0);
  while (1) {
    SDL_Event ev;
    if (SDL_PollEvent(&ev)) {
      if (ev.type == SDL_KEYUP || ev.type == SDL_KEYDOWN) {
        const char *res = term->keypress(handle_key(&ev));
        if (res) {
          sh_handle_cmd(res);
          sh_prompt();
        }
      }
    }
    refresh_terminal();
  }
}

