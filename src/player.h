#ifndef PLAYER_H
#define PLAYER_H
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_render.h"
#include "object.h"
#include "camera.h"

extern SDL_GameController *s_player_controller;

struct player {
    float x;
    float y;
    float g;
    float vx;
    float vy;
    float delta;
    char is_jumping;
    float jump_held_for;
    camera_t *camera;
    object_t **objects;
    size_t objects_count;
} typedef player_t;

player_t *player_new(camera_t *camera);
void player_free(player_t *player);

void player_update(player_t *player, float delta);
void player_event(player_t *player, const SDL_Event *event);
void player_render(player_t *player, SDL_Renderer *renderer);

shape_t player_get_shape(player_t *player);
#endif
