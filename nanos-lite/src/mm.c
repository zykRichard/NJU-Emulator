#include <memory.h>
#include <proc.h>
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

extern PCB *current;
/* The brk() system call handler. */
int mm_brk(uintptr_t brk) {
  if(current -> max_brk == 0) {
    current -> max_brk = brk;
    if(brk % PGSIZE == 0){
      void *pa = new_page(1);
      map(&current -> as, (void *)current -> max_brk, pa, 0);
    }
  }

  if(brk > current -> max_brk){
    if(brk / PGSIZE > (current -> max_brk) / PGSIZE){
      // va align to next page
      void * va = (void *)(((current -> max_brk) | (PGSIZE - 1)) + 1);
      while(brk >= (uintptr_t)va){
        void *pa = new_page(1);
        map(&current -> as, va, pa, 0);
        va = (void *)((char *)va + PGSIZE);
      }
    }
    Log("brk from %p to %p", current -> max_brk, brk);
    current -> max_brk = brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)ROUNDUP(heap.start, PGSIZE);
  Log("free physical pages starting from %p", pf);

#ifdef HAS_VME
  vme_init(pg_alloc, free_page);
#endif
}
