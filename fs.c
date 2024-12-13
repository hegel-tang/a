#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
  size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVEVT,FD_FBINFO,FD_FB};

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
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
  [FD_EVEVT]={"/dev/events", 0, 0, events_read, invalid_write},
  [FD_FBINFO] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
  [FD_FB] =  {"/dev/fb", 0, 0, invalid_read, fb_write},
#include "files.h"
};
extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
void init_fs() {
  // TODO: initialize the size of /dev/fb
  AM_GPU_CONFIG_T gpu = io_read(AM_GPU_CONFIG);
  file_table[FD_FB].size = gpu.width * gpu.height * sizeof(uint32_t);
}
int fs_open(const char *path, int flags, int mode){
	for (int i = 0; i < sizeof(file_table) / sizeof(Finfo); i++){
    
    if (strcmp(path, file_table[i].name) == 0){
      //printf("%d",1);
      //printf("%s",path);
      file_table[i].open_offset = 0;
      return i;
    }
  }
  return -1;
}

size_t fs_read(int fd, void *buf, size_t len){
  if(fd!=-1){
    if (file_table[fd].read){
    //printf("%d",1);
    len =file_table[fd].read(buf, file_table[fd].open_offset, len);
  }
    else{
      size_t d_offset=file_table[fd].disk_offset;
      size_t o_offset=file_table[fd].open_offset;
      size_t size=file_table[fd].size;
      if(o_offset + len > size)
        len = size - o_offset;
      size_t offset=o_offset+d_offset;
      ramdisk_read(buf,offset,len);
      file_table[fd].open_offset += len;   }
  }
  assert(fd!=-1);
  return len;
}

size_t fs_write(int fd, void *buf,size_t len){
  if(fd!=-1){

    if (file_table[fd].write){
    len =file_table[fd].write(buf, file_table[fd].open_offset, len);
  }
    else{ 
      size_t d_offset=file_table[fd].disk_offset;
      size_t o_offset=file_table[fd].open_offset;
      size_t size=file_table[fd].size;
      if(o_offset + len > size)
        len = size - o_offset;
      size_t offset=o_offset+d_offset;
      ramdisk_write(buf,offset,len);
      file_table[fd].open_offset += len;  }
  }
  assert(fd!=-1);
  return len;
}

size_t fs_lseek(int fd, size_t offset, int whence){

  switch(whence){
    case SEEK_CUR:
      file_table[fd].open_offset += offset;
      break;

    case SEEK_SET:
      
      file_table[fd].open_offset = offset;
      break;

    case SEEK_END:
     
      file_table[fd].open_offset = file_table[fd].size + offset;
      break;

    default:
      assert(0);
    
  }

  return file_table[fd].open_offset;
}
int fs_close(int fd){
	file_table[fd].open_offset = 0;
	return 0;
}
