#include <am.h>
#include <nemu.h>
#include <klib.h>
#include <stdint.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

static int max_w ;
static int max_h ;

void __am_gpu_init() {
  int i;
  int w = inl(VGACTL_ADDR) >> 16;
  int h = inl(VGACTL_ADDR) & (0xffff);
  max_w = w;
  max_h = h;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for(i = 0; i < w * h; i++) fb[i] = i;
  outl(SYNC_ADDR, 1);  
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inl(VGACTL_ADDR) >> 16, .height = inl(VGACTL_ADDR) & (0xffff),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  int w = ctl -> w;
  int h = ctl -> h;
  //int block_sz = block_w * block_h;
  int x = ctl -> x, y = ctl -> y;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *pixels = (uint32_t *)(ctl -> pixels);

  int start_id = max_w * y + x;
  int x_bound = (x + w > max_w) ? max_w : x + w;
  int y_bound = (y + h > max_h) ? max_h : y + h;
    
  for(int i = 0; i < y_bound - y; i++) {
    memcpy(fb + start_id + i * max_w, pixels, x_bound - x);
    pixels += w; 
  }

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
