#include <am.h>
#include <nemu.h>

#include <stdint.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t k_code = inl(KBD_ADDR);
  if(k_code & KEYDOWN_MASK) kbd -> keydown = 1;
  else kbd -> keydown = 0;

  kbd -> keycode = k_code & (~ KEYDOWN_MASK);
  // kbd->keydown = 0;
  // kbd->keycode = AM_KEY_NONE;
}
