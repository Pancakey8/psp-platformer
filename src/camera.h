#ifndef CAMERA_H
#define CAMERA_H
#include "SDL2/SDL_rect.h"

typedef struct camera {
    float x;
    float y;
    float w;
    float h;
    float scale;
} camera_t;

camera_t camera_new(float w, float h);

SDL_FPoint world2camp(camera_t const *cam, float x, float y);
SDL_FPoint cam2worldp(camera_t const *cam, float x, float y);
float world2caml(camera_t const *cam, float l);
float cam2worldl(camera_t const *cam, float l);

#endif
