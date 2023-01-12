#include <memory.h>

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *ret = pf;
  pf = (void *)((char *)pf + nr_page * PGSIZE);

  return ret;
}

#ifdef HAS_VME
static void* pg_alloc(int n) {
  size_t nr_page = n / PGSIZE;
  void *ret = new_page(nr_page);
  memset(ret, 0, n);
  //Log("alloc page at address %p", ret);
  return ret;
}
#endif

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
