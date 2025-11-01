#include "player.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_log.h"
#include "SDL2/SDL_rect.h"
#include "SDL2/SDL_render.h"
#include "camera.h"
#include "object.h"
#include "textures.h"
#include <math.h>

SDL_GameController *s_player_controller = NULL;

static const float player_velo = 150.0f;
static const float player_width = 32, player_height = 48;
static const float player_jump_max = 0.4f;

int is_controller_down(SDL_GameControllerButton button) {
  if (!s_player_controller)
    return 0;
  return SDL_GameControllerGetButton(s_player_controller, button) != 0;
}

player_t *player_new(camera_t *camera) {
  player_t *p = calloc(1, sizeof(player_t));
  *p = (player_t){.x = 30,
                  .y = 30,
                  .camera = camera,
                  .g = 500,
                  .direction = 1,
                  .objects = NULL,
                  .objects_count = 0,
                  .sprite = TI_PLAYER_IDLE};
  return p;
}

void player_free(player_t *player) { free(player); }

void player_update(player_t *player, float delta) {
  player->delta = delta;

#ifdef LINUX
  SDL_PumpEvents();
  const Uint8 *kb = SDL_GetKeyboardState(NULL);
  float dirx = kb[SDL_SCANCODE_D] - kb[SDL_SCANCODE_A];
#else
  float dirx = is_controller_down(SDL_CONTROLLER_BUTTON_DPAD_RIGHT) -
               is_controller_down(SDL_CONTROLLER_BUTTON_DPAD_LEFT);
#endif

  player->vx += dirx * player_velo * delta;

  if (dirx != 0)
    player->direction = dirx;

#ifdef LINUX
  char jump_held = kb[SDL_SCANCODE_W];
#else
  char jump_held = is_controller_down(SDL_CONTROLLER_BUTTON_DPAD_UP) ||
                   is_controller_down(SDL_CONTROLLER_BUTTON_X);
#endif
  // SDL_Log("%f,%b", dirx, jump_held);

  rect_t p_rect = player_get_shape(player).data.rect;
  p_rect.y += 0.1;
  char grounded = 0;
  for (size_t i = 0; i < player->objects_count; ++i) {
    object_t *obj = player->objects[i];
    switch (obj->shape.kind) {
    case COL_RECT: {
      if (collides_rectnrect(p_rect, obj->shape.data.rect)) {
        grounded = 1;
        break;
      }
    } break;
    }
  }
  p_rect.y -= 0.1;

  if (jump_held && player->is_jumping) {
    player->jump_held_for += delta;
    float fall_off = 1 - player->jump_held_for / player_jump_max;
    player->vy -= fall_off * (player->g + 900) * delta;
    if (player->jump_held_for >= player_jump_max) {
      player->vy = 0.1f;
      player->is_jumping = 0;
    }
  } else if (jump_held && !player->is_jumping) {
    if (grounded) {
      SDL_Log("Jumping");
      player->is_jumping = 1;
      player->jump_held_for = 0;
      player->vy = -(player->g + 1400) * delta;
    }
  } else if (!jump_held && player->is_jumping) {
    player->vy = 0.1f;
    player->is_jumping = 0;
  }

  if (!grounded)
    player->vy += player->g * delta;

  if (player->vy != 0)
    player->vx += 0.0001;

  float nx = 0, ny = 0;
  float time = 1.0f;
  for (size_t i = 0; i < player->objects_count; ++i) {
    object_t *obj = player->objects[i];
    switch (obj->shape.kind) {
    case COL_RECT: {
      float nx_, ny_;
      float tm = collides_rectnrect_s(p_rect, obj->shape.data.rect, player->vx,
                                      player->vy, &nx_, &ny_);
      if (tm < time) {
        time = tm;
        nx = nx_;
        ny = ny_;
      }
    } break;
    }
  }

  SDL_Log("vx=%f,vy=%f", player->vx, player->vy);
  SDL_Log("nx=%f,ny=%f,t=%f,gnd=%b", nx, ny, time, grounded);

  player->x += player->vx * time;
  player->y += player->vy * time;

  // push:
  float mag = hypotf(player->vx, player->vy) * (1 - time);
  float dot = player->vx * ny + player->vy * nx;

  if (dot > 0)
    dot = 1;
  else if (dot < 0)
    dot = -1;

  player->vx = dot * ny * mag;
  player->vy = dot * nx * mag;
  player->x += nx * 0.01;
  player->y += ny * 0.01;

  // slide:
  // float dot = (player->vx * ny + player->vy * nx) * (1 - time);
  // player->vx = dot * ny;
  // player->vy = dot * nx;

  float dx = player->x - player->camera->x;
  player->camera->x += dx * delta / 0.3f;
  float dy = player->y - player->camera->y;
  player->camera->y += dy * delta / 0.3f;

  if (dirx != 0) {
    static tex_index_t walk_cycle[] = {TI_PLAYER_WALK1, TI_PLAYER_WALK2,
                                       TI_PLAYER_WALK3, TI_PLAYER_WALK4};
    static size_t sz = sizeof(walk_cycle) / sizeof(tex_index_t);
    if (player->anim_counter >= 0.2) {
      player->anim_counter = 0;
      if (player->sprite == TI_PLAYER_IDLE)
        player->sprite = TI_PLAYER_WALK1;
      else {
        for (size_t i = 0; i < sz; ++i) {
          if (player->sprite == walk_cycle[i]) {
            player->sprite = walk_cycle[(i + 1) % sz];
            break;
          }
        }
      }
    }
  } else {
    player->sprite = TI_PLAYER_IDLE;
  }
  player->anim_counter += delta;
}

void player_event(player_t *player, const SDL_Event *event) {
  // switch (event->type) {
  // case SDL_CONTROLLERBUTTONDOWN: {
  //   if (event->cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
  //     player->x += player_velo * player->delta;
  //   }
  // }
  // }
}

void player_render(player_t *player, SDL_Renderer *renderer) {
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);

  // SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  SDL_FPoint pos = world2camp(player->camera, player->x, player->y);
  tex_pos_t tpos = s_tex_pos[player->sprite];
  SDL_RenderCopyEx(
      renderer, s_texture_map,
      &(SDL_Rect){.x = tpos.x, .y = tpos.y, .w = tpos.w, .h = tpos.h},
      &(SDL_Rect){.x = pos.x, .y = pos.y, .w = tpos.w * 2, .h = tpos.h * 2},
      0, NULL, player->direction == 1 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL);
  SDL_Log("Drawing as %d\n", player->sprite);
  // SDL_RenderFillRectF(
  //     renderer, &(SDL_FRect){.x = pos.x,
  //                            .y = pos.y,
  //                            .w = world2caml(player->camera, player_width),
  //                            .h = world2caml(player->camera,
  //                            player_height)});
}

shape_t player_get_shape(player_t *player) {
  return shape_make_rect(player->x, player->y, player_width, player_height);
}
