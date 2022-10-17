#include <am.h>
#include <nemu.h>
#include <stdio.h>

#include <stdint.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  while(1){
  for(volatile int i = 1; i <= 1000000; i++);
  uint32_t kcode = inl(KBD_ADDR);
  printf("kcode is 0x%x\n", kcode);
  }
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;
}
