#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t file_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

/********************************File System********************************/

int fs_open(const char* pathname, int flag, int modes) {
  int file_num = sizeof(file_table) / sizeof(Finfo);

  for(int i = 0; i < file_num; i++)
    if(strcmp(pathname, file_table[i].name) == 0){
        file_table[i].file_offset = 0;
        return i;
    }
   

  panic("File does not exist.");
  return -1;
}


size_t fs_read(int fd, void *buf, size_t len){
  size_t file_off = file_table[fd].file_offset;

  file_table[fd].file_offset += len;
  if(file_table[fd].file_offset > file_table[fd].size) return 0;
  /***Reading len bytes from fd to buf***/
  ramdisk_read(buf, file_table[fd].disk_offset + file_off, len);
  return len;
}


size_t fs_write(int fd, const void *buf, size_t len) {
  size_t file_off = file_table[fd].file_offset;

  size_t wlen = (file_off + len > file_table[fd].file_offset) ? 
                file_table[fd].file_offset - file_off : len;
  file_table[fd].file_offset += wlen;

  ramdisk_write(buf, file_table[fd].disk_offset + file_off, wlen);
  return wlen;
}


off_t fs_lseek(int fd, off_t offset, int whence) {
  off_t tmp = file_table[fd].file_offset; 

  switch (whence)
  {
  case SEEK_SET:
    tmp = offset;
    break;
  
  case SEEK_CUR:
    tmp = file_table[fd].file_offset + offset;
    break;

  case SEEK_END:
    tmp = file_table[fd].size + offset;
    break;

  default: 
    Log("Invalid whence");
    break;
  }

  file_table[fd].file_offset = tmp;  

  return tmp;
}


int fs_close(int fd) {
  file_table[fd].file_offset = 0;

  return 0;
}