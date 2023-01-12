#include <proc.h>
#include <elf.h>
#include <fs.h>
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);


static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ELFheader;
  Elf_Phdr PROheader;
  memset(ELFheader.e_ident, 0xff, 4);

  int fd = fs_open(filename, 0, 0); 
  size_t base = file_load(fd);
  //Log("base is %d", base);
  // read ELF header:
  ramdisk_read((void *)(&ELFheader), base, sizeof(Elf_Ehdr));
  //char *p = (char *)(&ELFheader);
  //printf("first 4 bytes:%x %x %x %x", p[0], p[1], p[2], p[3]);
  //printf("magic is %x\n", *(uint32_t *)(ELFheader.e_ident)); 
  // Judge:
  assert(*(uint32_t*)(ELFheader.e_ident) == 0x464c457f);

  uintptr_t ret_addr = ELFheader.e_entry;
  //printf("ret addr is %p\n", ret_addr);
  size_t pro_off = ELFheader.e_phoff;
  //printf("Segment offset is %x\n", pro_off);
  size_t pro_num = ELFheader.e_phnum;
  //printf("Segment number is %x\n", pro_num);
  size_t pro_size = ELFheader.e_phentsize;
  //printf("Segment entry size is %x\n", pro_size);

  // read Segment headers and LOAD:
  for(int i = 0; i < pro_num; i++){

    ramdisk_read((void *)(&PROheader), base + pro_off + i * pro_size, pro_size);
    if(PROheader.p_type == PT_LOAD){
      //printf("In proheader %d, offset is %d, size is %d\n", i, PROheader.p_offset, PROheader.p_memsz);
      ramdisk_read((void *)(PROheader.p_vaddr), base + PROheader.p_offset, PROheader.p_filesz);
      memset((void*)(PROheader.p_vaddr + PROheader.p_filesz), 0, PROheader.p_memsz - PROheader.p_filesz);
    }    
  }  

  return ret_addr;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void(*func)(void *), void *arg) {
  Area kstack;
  kstack.start = pcb -> stack;
  kstack.end = pcb -> stack + STACK_SIZE;
  Context *ctx = kcontext(kstack, func, arg);
  pcb -> cp = ctx;
  printf("kload over\n");
}

void context_uload(PCB *pcb, char *filename, char * argv[], char * envp[]) {
  Area kstack;
  kstack.start = pcb -> stack;
  kstack.end = pcb -> stack + STACK_SIZE;
  protect(&(pcb -> as));
  // kernel stack
  uintptr_t entry = loader(pcb, filename);
  Context *ctx = ucontext(&(pcb -> as), kstack, (void *)entry);

  pcb -> cp = ctx;
  // user stack
  pcb -> cp -> GPRx = (uintptr_t)heap.end;

  // load arguments
  void *upf = new_page(8);
  uintptr_t ustack_end = (uintptr_t)upf + 8 * PGSIZE;
  uintptr_t str_start = ustack_end - STRING_AREA_SIZE;
  uintptr_t ustack_start = ustack_end - USTACK_SIZE;
  printf("ustack_end is %p, str_start is %p, ustack_stack is %p\n", ustack_end, str_start, ustack_start);
  printf("arguments loading over\n");
  // get argc
  int argc = 0;
  int envc = 0;
  if(argv){ 
  while(argv[argc])
    argc ++ ;
  printf("argc %d loading over\n", argc);
  }

  if(envp){
  while(envp[envc])
    envc ++ ;
  printf("envc loading over\n");
  }
  // load argc 
  int *where_argc = (int *)ustack_start;
  *where_argc = argc;
  printf("argc loading over\n");
  // load argv 
  uintptr_t * where_argv = (uintptr_t *)(ustack_start + sizeof(uintptr_t));
  for(int i = 0; i < argc; i++) {
    uintptr_t where_copy = str_start + i * SZ_PER_STR;
    *where_argv = where_copy;
    where_argv ++;
    strcpy((void *)where_copy, argv[i]);
  }
  printf("argv loading over\n");
  // load envp 
  if(envp){
  uintptr_t * where_envp = (uintptr_t *)(ustack_start + argc * sizeof(uintptr_t));
  for(int j = 0; j < envc; j++) {
    uintptr_t where_copy = str_start + (j + argc) * SZ_PER_STR;
    *where_envp = where_copy;
    where_envp ++;
    strcpy((void *)where_copy, envp[j]);
  }
  printf("envp loading over\n");
  }
  pcb -> cp -> GPRx = ustack_start;
}