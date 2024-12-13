#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include <assert.h>
static int evtdev = 3;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
uint32_t NDL_GetTicks() {
  struct timeval tv;
  gettimeofday(&tv , NULL);
  return tv.tv_usec / 1000 + tv.tv_sec*1000;
}

int NDL_PollEvent(char *buf, int len) {
  return read(evtdev,buf,len);
}

void NDL_OpenCanvas(int *w, int *h) {
  char readbuf[64];
  read(4, readbuf, 64);
  sscanf(readbuf, "WIDTH:%d\nHEIGHT:%d\n", &screen_w, &screen_h);
  if (*w>screen_w || *w==0) { *w = screen_w; }
  if (*h>screen_h || *h==0) { *h = screen_h; }
  canvas_w = *w; 
  canvas_h = *h;
  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int offset_w = (screen_w - canvas_w)/2;
  int offset_h = (screen_h - canvas_h)/2;
  if(h == 0||h > canvas_h)h = canvas_h;
  if(w == 0||w > canvas_w)w = canvas_w; 
  for (uint32_t i=0; i<h; i++) {
    uint32_t off = ((i+offset_h)*screen_w+offset_w);
    lseek(5, off, SEEK_SET);
    write(5, pixels +i*w, w);
  }
}
void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  return 0;
}

void NDL_Quit() {
}
