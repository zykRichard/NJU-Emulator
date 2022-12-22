#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static uint8_t sdl_keystate[256] = {0};

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64] = {0};
  if(NDL_PollEvent(buf, sizeof(buf)) == 0){
    ev -> type = SDL_USEREVENT;
    ev -> key.keysym.sym = 0;
    return 0;
  }
  buf[strlen(buf) - 1] = 0;
  if(buf[1] == 'u') ev -> type = SDL_KEYUP;
  else if(buf[1] == 'd') ev -> type = SDL_KEYDOWN;
  
  for(int i = 1; i < sizeof(keyname) / sizeof(char*); i++)
    if(strcmp(keyname[i], &buf[3]) == 0) {
      ev -> key.keysym.sym = i;
      switch(ev -> type) {
        case SDL_KEYUP: sdl_keystate[i] = 0; break;
        case SDL_KEYDOWN: sdl_keystate[i] = 1; break;
      }
      return 1;
    }
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[64] = {0};
  while(NDL_PollEvent(buf, sizeof(buf)) == 0);
  printf("buf is %s\n", buf);
  buf[strlen(buf) - 1] = 0;
  if(buf[1] == 'u') event -> type = SDL_KEYUP;
  else if(buf[1] == 'd') event -> type = SDL_KEYDOWN;
  
  for(int i = 1; i < sizeof(keyname) / sizeof(char*); i++)
    if(strcmp(keyname[i], &buf[3]) == 0) {
      event -> key.keysym.sym = i;
      switch(event -> type) {
        case SDL_KEYUP: sdl_keystate[i] = 0; break;
        case SDL_KEYDOWN: sdl_keystate[i] = 1; break;
      }
      return 1;
    }
  assert(0);
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  SDL_Event ev;
  if(SDL_PollEvent(&ev)){
    switch (ev.type)
    {
    case SDL_KEYUP:
      sdl_keystate[ev.key.keysym.sym] = 0;
      break;
    
    case SDL_KEYDOWN:
      sdl_keystate[ev.key.keysym.sym] = 1;
      break;
    }
  }
  return sdl_keystate;
}
