#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;

struct timeval boot;
struct timeval systime;

static int fb_w = 0, fb_h = 0;
static int cv_w = 0, cv_h = 0;
static int ord_x = 0, ord_y = 0;


uint32_t NDL_GetTicks() {
  
  gettimeofday(&systime, NULL);
  return (systime.tv_sec - boot.tv_sec) * 1000 +
         (systime.tv_usec - boot.tv_usec) / 1000;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = open("/dev/events", O_RDONLY, 0);
  int retlen = read(fd, buf, len);
  return retlen;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }

  /* init time */
  gettimeofday(&boot, NULL);

  /* init fb */
  int fd = open("/proc/dispinfo", O_RDONLY, 0);
  char buf[64] = {0};
  read(fd, buf, sizeof(buf));
  printf("%s\n", buf);
  char *cur = buf;
  int flag = 1;
  fb_w = 0, fb_h = 0;
  while(*cur != 0) {
    if(*cur >= '0' && *cur <= '9'){
      if(flag) fb_w = fb_w * 10 + (*cur - '0');
      else fb_h = fb_h * 10 + (*cur - '0');
      printf("now width is %d, height is %d\n", fb_w, fb_h);
    }
    else if(*cur = '\n')
      flag = 0;
    
    cur++;
    //printf("width is %d, height is %d\n", fb_w, fb_h);
  }

  printf("width is %d, height is %d\n", fb_w, fb_h);
  return 0;
}

void NDL_Quit() {
  boot.tv_sec = boot.tv_usec = 0;
}
