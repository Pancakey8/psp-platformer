#ifndef OBJECT_H
#define OBJECT_H
#include "SDL2/SDL_render.h"
#include "camera.h"
#include <math.h>
#include <stdlib.h>

typedef struct rect {
  float x;
  float y;
  float w;
  float h;
} rect_t;

char collides_rectnrect(rect_t a, rect_t b);

float collides_rectnrect_s(rect_t body, rect_t solid, float vx, float vy,
                           float *nx, float *ny);

typedef struct shape {
  enum { COL_RECT } kind;
  union {
    rect_t rect;
  } data;
} shape_t;

shape_t shape_make_rect(float x, float y, float w, float h);

typedef enum object_type { OBJECT_GROUND } object_type_t;
typedef enum object_sides {
  OBJECT_SIDE_ALL = 0,
  OBJECT_IGNORE_TOP = 1,
  OBJECT_IGNORE_LEFT = 2,
  OBJECT_IGNORE_RIGHT = 4,
  OBJECT_IGNORE_BOT = 8
} object_sides_t;

typedef struct object {
  shape_t shape;
  SDL_Texture *tex;
} object_t;

object_t *object_make_ground(SDL_Renderer *renderer, int x, int y, int w,
                             int h, int sides);

void object_render(object_t *object, SDL_Renderer *renderer,
                   camera_t const *camera);

void object_free(object_t *object);
#endif
