#include "camera.h"

camera_t camera_new(float w, float h) {
  return (camera_t){.x = 0, .y = 0, .w = w, .h = h, .scale = 1};
}

SDL_FPoint world2camp(camera_t const *cam, float x, float y) {
  return (SDL_FPoint){.x = x - cam->x + cam->w / 2,
                      .y = y - cam->y + cam->h / 2};
}
SDL_FPoint cam2worldp(camera_t const *cam, float x, float y) {
  return (SDL_FPoint){.x = x + cam->x - cam->w / 2,
                      .y = y + cam->y - cam->h / 2};
}
float world2caml(camera_t const *cam, float l) {
    return l * cam->scale;
}
float cam2worldl(camera_t const *cam, float l) {
    return l / cam->scale;
}
