#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)


void __am_gpu_init() {
  int i;
  int w = inl(VGACTL_ADDR) >> 16;
  int h = inl(VGACTL_ADDR) & (0xffff);
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
  int block_w = ctl -> w;
  int block_h = ctl -> h;
  //int block_sz = block_w * block_h;
  int fx = ctl -> x, fy = ctl -> y;
  uint32_t **fb = (uint32_t **)(uintptr_t)FB_ADDR;
  uint32_t *pixels = (uint32_t *)(ctl -> pixels);

  for(int i = 0; i < block_w; i++)
    for(int j = 0; j < block_h; j++)
      fb[fx + i][fy + j] = pixels[i * j];

  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
