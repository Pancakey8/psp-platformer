#ifndef TEXTURES_H
#define TEXTURES_H
#include "SDL2/SDL_render.h"
typedef enum tex_index {
  TI_GROUND_TL,
  TI_GROUND_TC,
  TI_GROUND_TR,
  TI_GROUND_CL,
  TI_GROUND_MID,
  TI_GROUND_CR,
  TI_GROUND_BL,
  TI_GROUND_BC,
  TI_GROUND_BR,
  TI_PLAYER_IDLE,
  TI_PLAYER_WALK1,
  TI_PLAYER_WALK2,
  TI_PLAYER_WALK3,
  TI_PLAYER_WALK4,
  TI_COUNT
} tex_index_t;

typedef struct tex_pos {
  int x;
  int y;
  int w;
  int h;
} tex_pos_t;

#define tpos_make_tile(x_, y_) \
  ((tex_pos_t){.x = x_, .y = y_, .w = 16, .h = 16})

extern tex_pos_t s_tex_pos[TI_COUNT];

extern SDL_Texture *s_texture_map;

void tmap_init(SDL_Renderer *renderer);

void tmap_render(SDL_Renderer *renderer, int x, int y, tex_index_t index);
#endif
