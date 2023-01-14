#include <am.h>
#include <nemu.h>
#include <klib.h>

static AddrSpace kas = {};
static void* (*pgalloc_usr)(int) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static Area segments[] = {      // Kernel memory mappings
  NEMU_PADDR_SPACE
};

#define USER_SPACE RANGE(0x40000000, 0x80000000)

static inline void set_satp(void *pdir) {
  //printf("set satp at %p\n", (uintptr_t)pdir >> 12);
  uintptr_t mode = 1ul << (__riscv_xlen - 1);
  asm volatile("csrw satp, %0" : : "r"(mode | ((uintptr_t)pdir >> 12)));
}

static inline uintptr_t get_satp() {
  uintptr_t satp;
  asm volatile("csrr %0, satp" : "=r"(satp));
  return satp << 12;
}

bool vme_init(void* (*pgalloc_f)(int), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  kas.ptr = pgalloc_f(PGSIZE);

  int i;
  for (i = 0; i < LENGTH(segments); i ++) {
    void *va = segments[i].start;
    for (; va < segments[i].end; va += PGSIZE) {
      map(&kas, va, va, 0);
    }
  }
  //printf("set satp\n");
  set_satp(kas.ptr);
  vme_enable = 1;

  return true;
}

void protect(AddrSpace *as) {
  PTE *updir = (PTE*)(pgalloc_usr(PGSIZE));
  as->ptr = updir;
  as->area = USER_SPACE;
  as->pgsize = PGSIZE;
  // map kernel space
  memcpy(updir, kas.ptr, PGSIZE);
}

void unprotect(AddrSpace *as) {
}

void __am_get_cur_as(Context *c) {
  c->pdir = (vme_enable ? (void *)get_satp() : NULL);
}

void __am_switch(Context *c) {
  if (vme_enable && c->pdir != NULL) {
    set_satp(c->pdir);
  }

}

void map(AddrSpace *as, void *va, void *pa, int prot) {
  //printf("map begin\n");
  //printf("map %p to %p\n", va, pa);
  
  uint32_t vpn1 = ((uintptr_t) va) >> 22;
  uint32_t vpn0 = (((uintptr_t) va) >> 12) & 0x3ff;

  PTE* PDE = as -> ptr;
  //printf("base is %p\n", as -> ptr);
  assert(PDE);
  PTE entry = PDE[vpn1];
  
  // valid bit detection:
  if((entry & 0x1) == 0) {
    PTE updir = (PTE)(pgalloc_usr(PGSIZE)) | 0x1;
    PDE[vpn1] = updir;
  }
  //printf("PTE alloc OK\n");
  uintptr_t PTE_base = ((uintptr_t)PDE[vpn1] >> 12) << 12;
  PTE* PTEdir = (PTE*)(PTE_base);
  PTEdir[vpn0] = (((uintptr_t)pa >> 12) << 12) | 0x1;
  
}

Context *ucontext(AddrSpace *as, Area kstack, void *entry) {
  Context *ctx = (Context *)(kstack.end - sizeof(Context));
  ctx -> mepc = (uintptr_t)entry;
  ctx -> mstatus |= 0x88;
  ctx -> pdir = as -> ptr;
  return ctx;
}
