#include <common.h>
#include <am.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

void __am_input_keybrd(AM_INPUT_KEYBRD_T *);
void __am_gpu_config(AM_GPU_CONFIG_T *);

static int fb_width = 0, fb_height = 0;

size_t serial_write(const void *buf, size_t offset, size_t len) {
  for(size_t i = 0; i < len; i++)
    putch(*((char *)buf + i));
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T kbd;
  __am_input_keybrd(&kbd);

  if(kbd.keycode == AM_KEY_NONE){
    *(char *)buf = 0;
    return 0;
  }  

  char str[64];
  memset(str, 0, sizeof(str));
  if(kbd.keydown) strcpy(str, "kd ");
  else strcpy(str, "ku ");

  strcpy(str + 3, keyname[kbd.keycode]);
  strncpy(buf, str, len);
  return len;

  return 0;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T cfg;
  __am_gpu_config(&cfg);
  fb_width = cfg.width;
  fb_height = cfg.height;
  int retlen = sprintf(buf, "WIDTH : %d\nHEIGHT : %d", fb_width, fb_height);
  return retlen;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
