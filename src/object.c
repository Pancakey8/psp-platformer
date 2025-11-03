#include "object.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_ttf.h"
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
    SDL_RenderCopy(renderer, object->tex, NULL,
                   &(SDL_Rect){.x = pos.x, .y = pos.y, .w = w, .h = h});
    // SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    // SDL_RenderDrawRectF(renderer,
    //                     &(SDL_FRect){.x = pos.x, .y = pos.y, .w = w, .h = h});
  } break;
  }
}

object_t *object_make_ground(SDL_Renderer *renderer, int x, int y, int w, int h,
                             int sides) {
  SDL_Texture *tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                                       SDL_TEXTUREACCESS_TARGET, w, h);
  SDL_SetRenderTarget(renderer, tex);

  char ignore_top = sides & OBJECT_IGNORE_TOP;
  char ignore_left = sides & OBJECT_IGNORE_LEFT;
  char ignore_right = sides & OBJECT_IGNORE_RIGHT;
  char ignore_bot = sides & OBJECT_IGNORE_BOT;

  int tiles_x = w / 32;
  int tiles_y = h / 32;

  for (int ty = 0; ty < tiles_y; ty++) {
    for (int tx = 0; tx < tiles_x; tx++) {
      int i = tx * 32;
      int j = ty * 32;

      int top = (ty == 0);
      int bot = (ty == tiles_y - 1);
      int left = (tx == 0);
      int right = (tx == tiles_x - 1);

      int tile = TI_GROUND_MID;

      if (top && !ignore_top) {
        if (left && !ignore_left)
          tile = TI_GROUND_TL;
        else if (right && !ignore_right)
          tile = TI_GROUND_TR;
        else
          tile = TI_GROUND_TC;
      } else if (bot && !ignore_bot) {
        if (left && !ignore_left)
          tile = TI_GROUND_BL;
        else if (right && !ignore_right)
          tile = TI_GROUND_BR;
        else
          tile = TI_GROUND_BC;
      } else if (left && !ignore_left)
        tile = TI_GROUND_CL;
      else if (right && !ignore_right)
        tile = TI_GROUND_CR;

      tmap_render(renderer, i, j, tile);
    }
  }

  SDL_SetRenderTarget(renderer, NULL);

  object_t *obj = calloc(1, sizeof(object_t));
  *obj = (object_t){.shape.kind = COL_RECT,
                    .shape.data.rect = (rect_t){.x = x, .y = y, .w = w, .h = h},
                    .tex = tex};
  return obj;
}

object_t *object_make_text(SDL_Renderer *renderer, int x, int y, int f_size,
                           char *text) {
  TTF_Font *font = TTF_OpenFont("res/fonts/Finesse-Oblique.otf", f_size);
  SDL_Surface *surf = TTF_RenderText_Blended(
      font, text, (SDL_Color){.r = 0, .g = 0, .b = 0, .a = 255});
  SDL_Texture *tex = SDL_CreateTextureFromSurface(renderer, surf);
  int w = surf->w, h = surf->h;
  SDL_FreeSurface(surf);
  object_t *obj = calloc(1, sizeof(object_t));
  obj->shape.kind = COL_RECT;
  obj->shape.data.rect = (rect_t){.x = x, .y = y, .w = w, .h = h};
  obj->shape.passthrough = 1;
  obj->tex = tex;
  TTF_CloseFont(font);
  return obj;
}

void object_free(object_t *object) { free(object); }
