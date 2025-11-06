#ifndef PLAYER_H
#define PLAYER_H
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_render.h"
#include "camera.h"
#include "object.h"
#include "textures.h"

extern SDL_GameController *s_player_controller;

typedef struct player {
  float x;
  float y;
  float w;
  float h;
  float g;
  float vx;
  float vy;
  float delta;
  char is_jumping;
  float jump_held_for;
  camera_t *camera;
  object_t **objects;
  size_t objects_count;
  tex_index_t sprite;
  float anim_counter;
  int direction;
  char is_alive;
} player_t;

player_t *player_new(camera_t *camera, float x, float y);
void player_free(player_t *player);

void player_update(player_t *player, float delta);
void player_event(player_t *player, const SDL_Event *event);
void player_render(player_t *player, SDL_Renderer *renderer);

shape_t player_get_shape(player_t *player);
#endif
