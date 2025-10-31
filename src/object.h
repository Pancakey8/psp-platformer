#ifndef OBJECT_H
#define OBJECT_H
#include "SDL2/SDL_render.h"
#include <math.h>
#include <stdlib.h>
#include "camera.h"

typedef struct rect {
  float x;
  float y;
  float w;
  float h;
} rect_t;

char collides_rectnrect(rect_t a, rect_t b);

float collides_rectnrect_s(rect_t body, rect_t solid, float vx, float vy, float *nx, float *ny);

typedef struct shape {
  enum { COL_RECT } kind;
  union {
    rect_t rect;
  } data;
} shape_t;

shape_t shape_make_rect(float x, float y, float w, float h);

typedef struct object {
  shape_t shape;
} object_t;

object_t *object_new(shape_t shape);

void object_render(object_t *object, SDL_Renderer *renderer, camera_t const *camera);

void object_free(object_t *object);
#endif
