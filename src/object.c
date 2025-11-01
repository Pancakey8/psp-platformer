#include "object.h"
#include "SDL2/SDL_render.h"
#include "camera.h"
#include "textures.h"

char collides_rectnrect(rect_t a, rect_t b) {
  return (a.x < b.x + b.w && a.x + a.w > b.x && a.y < b.y + b.h &&
          a.y + a.h > b.y);
}

float collides_rectnrect_s(rect_t body, rect_t solid, float vx, float vy,
                           float *nx, float *ny) {
  float x_inv_ent, y_inv_ent;
  float x_inv_ex, y_inv_ex;

  if (vx > 0.0f) {
    x_inv_ent = solid.x - (body.x + body.w);
    x_inv_ex = (solid.x + solid.w) - body.x;
  } else {
    x_inv_ent = (solid.x + solid.w) - body.x;
    x_inv_ex = solid.x - (body.x + body.w);
  }

  if (vy > 0.0f) {
    y_inv_ent = solid.y - (body.y + body.h);
    y_inv_ex = (solid.y + solid.h) - body.y;
  } else {
    y_inv_ent = (solid.y + solid.h) - body.y;
    y_inv_ex = solid.y - (body.y + body.h);
  }

  float x_ent, y_ent;
  float x_ex, y_ex;

  if (vx == 0.0f) {
    x_ent = -FLT_MAX;
    x_ex = FLT_MAX;
  } else {
    x_ent = x_inv_ent / vx;
    x_ex = x_inv_ex / vx;
  }

  if (vy == 0.0f) {
    y_ent = -FLT_MAX;
    y_ex = FLT_MAX;
  } else {
    y_ent = y_inv_ent / vy;
    y_ex = y_inv_ex / vy;
  }

  float entry = fmaxf(x_ent, y_ent);
  float exit = fminf(x_ex, y_ex);

  if (entry > exit || x_ent < 0.0f && y_ent < 0.0f || x_ent > 1.0f ||
      y_ent > 1.0f) {
    *nx = 0.0f;
    *ny = 0.0f;
    return 1.0f;
  }

  if (x_ent > y_ent) {
    *nx = (x_inv_ent < 0.0f) ? 1.0f : -1.0f;
    *ny = 0.0f;
  } else {
    *nx = 0.0f;
    *ny = (y_inv_ent < 0.0f) ? 1.0f : -1.0f;
  }

  return entry;
}

shape_t shape_make_rect(float x, float y, float w, float h) {
  return (shape_t){.kind = COL_RECT,
                   .data.rect = (rect_t){.x = x, .y = y, .w = w, .h = h}};
}

void object_render(object_t *object, SDL_Renderer *renderer,
                   camera_t const *camera) {
  switch (object->shape.kind) {
  case COL_RECT: {
    rect_t rect = object->shape.data.rect;
    SDL_FPoint pos = world2camp(camera, rect.x, rect.y);
    float w = world2caml(camera, rect.w), h = world2caml(camera, rect.h);
    SDL_RenderCopy(renderer, object->tex, NULL, &(SDL_Rect){.x = pos.x, .y = pos.y, .w = w, .h = h});
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_RenderFillRectF(renderer,
    //                     &(SDL_FRect){.x = pos.x, .y = pos.y, .w = w, .h = h});
  } break;
  }
}

object_t *object_make_ground(SDL_Renderer *renderer, int x, int y, int w,
                             int h) {
  SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, w, h);
  SDL_SetRenderTarget(renderer, tex);
  for (int i = 0; i < w; i += 32) {
    for (int j = 0; j < h; j += 32) {
      tmap_render(renderer, i, j, TI_GROUND_TC);
    }
  }
  SDL_SetRenderTarget(renderer, NULL);

  object_t *obj = calloc(1, sizeof(object_t));
  *obj = (object_t){.shape.kind = COL_RECT,
                  .shape.data.rect = (rect_t){.x = x, .y = y, .w = w, .h = h},
                  .tex = tex};
  return obj;
}

void object_free(object_t *object) { free(object); }
