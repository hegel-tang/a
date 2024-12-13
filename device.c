#include <common.h>
#include <stdio.h>
#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};

size_t serial_write(const void *buf, size_t offset, size_t len) {
  char *str = (char *)buf;
  for (int i = 0; i < len; i++){
    putch(str[i]);
  }
  return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);
  if (kbd.keycode == AM_KEY_NONE) {
    //printf("%d",1);
    *(char*)buf = '\0';
    return 0;
  }
  int ret = snprintf(buf, len, "%s %s\n", kbd.keydown?"kd":"ku", keyname[kbd.keycode]);
  return ret;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  if (offset > 0){
    return 0;
  }
  int ret = snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", io_read(AM_GPU_CONFIG).width, io_read(AM_GPU_CONFIG).height);
  if (ret >= len){
    assert(0);
  }
  return ret;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  uint32_t w = io_read(AM_GPU_CONFIG).width;
	uint32_t h = io_read(AM_GPU_CONFIG).height;
  uint32_t x = offset%w;
  uint32_t y = offset/w;
  if(offset+len > w*h) len = w*h- offset;
  io_write(AM_GPU_FBDRAW, x, y, (uint32_t*)buf, len, 1, true);
  return len;
}


void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
