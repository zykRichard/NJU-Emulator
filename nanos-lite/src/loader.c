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
  Log("base is %d", base);
  // read ELF header:
  ramdisk_read((void *)(&ELFheader), base, sizeof(Elf_Ehdr));
  //char *p = (char *)(&ELFheader);
  //printf("first 4 bytes:%x %x %x %x", p[0], p[1], p[2], p[3]);
  //printf("magic is %x\n", *(uint32_t *)(ELFheader.e_ident)); 
  // Judge:
  assert(*(uint32_t*)(ELFheader.e_ident) == 0x464c457f);

  uintptr_t ret_addr = ELFheader.e_entry;
  printf("ret addr is %p\n", ret_addr);
  size_t pro_off = ELFheader.e_phoff;
  printf("Segment offset is %x\n", pro_off);
  size_t pro_num = ELFheader.e_phnum;
  printf("Segment number is %x\n", pro_num);
  size_t pro_size = ELFheader.e_phentsize;
  printf("Segment entry size is %x\n", pro_size);

  // read Segment headers and LOAD:
  for(int i = 0; i < pro_num; i++){

    ramdisk_read((void *)(&PROheader), base + pro_off + i * pro_size, pro_size);
    if(PROheader.p_type == PT_LOAD){
      printf("In proheader %d, offset is %d, size is %d\n", i, PROheader.p_offset, PROheader.p_memsz);
      ramdisk_read((void *)(PROheader.p_vaddr), base + PROheader.p_offset, PROheader.p_memsz);
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

