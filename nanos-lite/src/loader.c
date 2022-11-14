#include <proc.h>
#include <elf.h>

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

  size_t base = 0;

  // read ELF header:
  ramdisk_read((void *)(&ELFheader), base, sizeof(Elf_Ehdr));
  
  // Judge:
  assert((uint32_t)ELFheader.e_ident == 0x464c457f);

  uintptr_t ret_addr = ELFheader.e_entry;
  size_t pro_off = ELFheader.e_phoff;
  size_t pro_num = ELFheader.e_phnum;
  size_t pro_size = ELFheader.e_phentsize;

  // read Segment headers and LOAD:
  for(int i = 0; i < pro_num; i++){

    ramdisk_read((void *)(&PROheader), base + pro_off + i * pro_size, pro_size);
    if(PROheader.p_type == PT_LOAD)
      ramdisk_read((void *)(PROheader.p_vaddr), base + PROheader.p_offset, PROheader.p_memsz);
    
  }  

  return ret_addr;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

