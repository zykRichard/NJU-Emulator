#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <NDL.h>
#include <BMP.h>

int main() {
  NDL_Init(0);
  int w, h;
  void *bmp = BMP_Load("/share/pictures/projectn.bmp", &w, &h);
  assert(bmp);
  printf("width is %d, height is %d", w, h);
  NDL_OpenCanvas(&w, &h);
  NDL_DrawRect(bmp, 0, 0, w, h);
  free(bmp);
  NDL_Quit();
  printf("Test ends! Spinning...\n");
  execve("/bin/pal", NULL, NULL);
  while (1);
  return 0;
}
