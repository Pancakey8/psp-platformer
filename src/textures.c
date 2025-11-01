#include "textures.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include <SDL2/SDL_image.h>

SDL_Texture *s_texture_map = NULL;
tex_pos_t s_tex_pos[TI_COUNT] = {
    [TI_GROUND_TL] = tpos_make_tile(560, 96),
    [TI_GROUND_TC] = tpos_make_tile(576, 96),
    [TI_GROUND_TR] = tpos_make_tile(592, 96),
    [TI_GROUND_CL] = tpos_make_tile(560, 112),
    [TI_GROUND_MID] = tpos_make_tile(576, 112),
    [TI_GROUND_CR] = tpos_make_tile(592, 112),
    [TI_GROUND_BL] = tpos_make_tile(560, 128),
    [TI_GROUND_BC] = tpos_make_tile(576, 128),
    [TI_GROUND_BR] = tpos_make_tile(592, 128),
    [TI_PLAYER_IDLE] = (tex_pos_t){.x = 464, .y = 208, .w = 16, .h = 24},
    [TI_PLAYER_WALK1] = (tex_pos_t){.x = 480, .y = 208, .w = 16, .h = 24},
    [TI_PLAYER_WALK2] = (tex_pos_t){.x = 496, .y = 208, .w = 16, .h = 24},
    [TI_PLAYER_WALK3] = (tex_pos_t){.x = 512, .y = 208, .w = 16, .h = 24},
    [TI_PLAYER_WALK4] = (tex_pos_t){.x = 528, .y = 208, .w = 16, .h = 24},
};

void tmap_init(SDL_Renderer *renderer) {
  SDL_Surface *surf = IMG_Load("res/textures/monkeylad_sprites.png");
  s_texture_map = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_FreeSurface(surf);
}

void tmap_render(SDL_Renderer *renderer, int x, int y, tex_index_t index) {
  tex_pos_t pos = s_tex_pos[index];
  SDL_RenderCopy(renderer, s_texture_map,
                 &(SDL_Rect){.x = pos.x, .y = pos.y, .w = pos.w, .h = pos.h},
                 &(SDL_Rect){.x = x, .y = y, .w = pos.w * 2, .h = pos.h * 2});
}
