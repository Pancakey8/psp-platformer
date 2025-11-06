#include "textures.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL_image.h>

SDL_Texture *s_texture_map = NULL;
tex_pos_t s_tex_pos[TI_COUNT] = {
    [TI_GROUND_TL] = tpos_make_tile(0, 32),
    [TI_GROUND_TC] = tpos_make_tile(16, 32),
    [TI_GROUND_TR] = tpos_make_tile(32, 32),
    [TI_GROUND_CL] = tpos_make_tile(0, 144),
    [TI_GROUND_MID] = tpos_make_tile(16, 96),
    [TI_GROUND_CR] = tpos_make_tile(32, 144),
    [TI_GROUND_BL] = tpos_make_tile(0, 160),
    [TI_GROUND_BC] = tpos_make_tile(16, 160),
    [TI_GROUND_BR] = tpos_make_tile(32, 160),
    [TI_SNAKE1] = tpos_make_tile(0, 272),
    [TI_PLAYER_IDLE] = (tex_pos_t){.x = 16, .y = 192, .w = 16, .h = 24},
    [TI_PLAYER_WALK1] = (tex_pos_t){.x = 32, .y = 192, .w = 16, .h = 24},
    [TI_PLAYER_WALK2] = (tex_pos_t){.x = 48, .y = 192, .w = 16, .h = 24},
    [TI_PLAYER_WALK3] = (tex_pos_t){.x = 64, .y = 192, .w = 16, .h = 24},
    [TI_PLAYER_WALK4] = (tex_pos_t){.x = 80, .y = 192, .w = 16, .h = 24},
    [TI_PLAYER_CRAWL1] = (tex_pos_t){.x = 24, .y = 224, .w = 24, .h = 16},
    [TI_PLAYER_CRAWL2] = (tex_pos_t){.x = 48, .y = 224, .w = 24, .h = 16},
    [TI_PLAYER_CRAWL3] = (tex_pos_t){.x = 72, .y = 224, .w = 24, .h = 16}
};

void tmap_init(SDL_Renderer *renderer) {
  SDL_Surface *surf = IMG_Load("res/textures/sprites.png");
  if (!surf)
    SDL_Log("Err: %s\n", SDL_GetError());
  s_texture_map = SDL_CreateTextureFromSurface(renderer, surf);
  if (!s_texture_map)
    SDL_Log("Err: %s\n", SDL_GetError());
  SDL_FreeSurface(surf);
}

void tmap_render(SDL_Renderer *renderer, int x, int y, tex_index_t index) {
  tex_pos_t pos = s_tex_pos[index];
  SDL_RenderCopy(renderer, s_texture_map,
                 &(SDL_Rect){.x = pos.x, .y = pos.y, .w = pos.w, .h = pos.h},
                 &(SDL_Rect){.x = x, .y = y, .w = pos.w * 2, .h = pos.h * 2});
}
