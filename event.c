#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[64];
  if (NDL_PollEvent(buf, sizeof(buf))!= 0) {
    char name[64], keydown;
    sscanf(buf, "k%c %s", &keydown, name);

    ev->type = keydown=='d' ? SDL_KEYDOWN : SDL_KEYUP;

    for (int i=0; i<sizeof(keyname) / sizeof(char *); i++) {
      if (strcmp(name, keyname[i]) == 0) {
        ev->key.keysym.sym = i;
        break;
      }
    }
    return 1;
  }
  else { ev->key.keysym.sym = 0; return 0;}
}

int SDL_WaitEvent(SDL_Event *event) {
  while(1) {
    char buf[64];
    if (NDL_PollEvent(buf, sizeof(buf))) {
      char name[64], keydown;
      sscanf(buf, "k%c %s", &keydown, name);
      event->type = keydown=='d' ? SDL_KEYDOWN : SDL_KEYUP;
      for (int i=0; i<sizeof(keyname) / sizeof(char *); i++) {
        if (strcmp(name, keyname[i]) == 0) {
          event->key.keysym.sym = i;
          break;
        }
      }
      return 1;
    }
}
}
int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
