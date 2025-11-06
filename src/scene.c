#include "scene.h"
#include "SDL2/SDL_blendmode.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_gamecontroller.h"
#include "SDL2/SDL_keycode.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_surface.h"
#include "camera.h"
#include "object.h"
#include "player.h"
#include "textures.h"
#include <SDL2/SDL_ttf.h>
#include <string.h>

scene_t *scene_new_direct(scene_t scene) {
  scene_t *scn = malloc(sizeof(scene_t));
  *scn = scene;
  return scn;
}

void scene_free(scene_t *scene) {
  scene->destroy(scene);
  if (scene->data) {
    free(scene->data);
  }
  free(scene);
}

// GAME LEVEL 1
typedef struct level_data {
  camera_t camera;
  player_t *player;
  object_t **objects;
  size_t objects_count;
} level_data_t;

void game_level1_init(scene_t *scene) {
  int w, h;
  SDL_GetRendererOutputSize(scene->renderer, &w, &h);
  level_data_t *data = scene->data;
  data->camera = camera_new(w, h);
  object_t *objs[] = {
    object_make_ground(scene->renderer, 0, 0, 58 * 32, 5 * 32, 0),
    object_make_ground(scene->renderer, 9 * 32, -3 * 32, 2 * 32, 1 * 32, 0),
    object_make_ground(scene->renderer, 13 * 32, -5 * 32, 2 * 32, 5 * 32,
                       OBJECT_IGNORE_BOT),
    object_make_ground(scene->renderer, 21 * 32, -5 * 32, 2 * 32, 4 * 32, 0),
    object_make_text(scene->renderer, 5 * 32, -2 * 32, 20, "Jump (X)"),
    object_make_text(scene->renderer, 17 * 32 - 30, -2 * 32, 20,
                     "Crawl (Square)"),
    object_make_snake(scene->renderer, 27 * 32, -1 * 32),
    object_make_snake(scene->renderer, 28 * 32, -1 * 32)};
  data->objects_count = sizeof(objs) / sizeof(object_t *);
  data->objects = calloc(data->objects_count, sizeof(object_t *));
  memcpy(data->objects, objs, data->objects_count * sizeof(object_t *));
  data->player = player_new(&data->camera, 0, -2 * 32);
  data->player->objects = data->objects;
  data->player->objects_count = data->objects_count;
}
void game_level1_event(scene_t *scene, SDL_Event const *event) {
  level_data_t *data = scene->data;
  player_event(data->player, event);
}
void game_level1_update(scene_t *scene, float dt) {
  level_data_t *data = scene->data;
  player_update(data->player, dt);
  if (!data->player->is_alive) {
    scene_switch(scene_main_menu);
  }
}
void game_level1_render(scene_t *scene) {
  SDL_SetRenderDrawColor(scene->renderer, 0, 170, 255, 255);
  SDL_RenderClear(scene->renderer);
  level_data_t *data = scene->data;
  player_render(data->player, scene->renderer);
  for (size_t i = 0; i < data->objects_count; ++i) {
    object_render(data->objects[i], scene->renderer, &data->camera);
  }
}
void game_level1_destroy(scene_t *scene) {
  level_data_t *data = scene->data;
  free(data->player);
  for (size_t i = 0; i < data->objects_count; ++i) {
    object_free(data->objects[i]);
  }
  free(data->objects);
}

// MAIN MENU
typedef struct main_menu_data {
  TTF_Font *font_s;
  TTF_Font *font_l;
  int w, h;
  // title
  SDL_Rect title_rect;
  SDL_Texture *title_texture;
  // press start
  SDL_Rect start_rect;
  SDL_Texture *start_texture;
} main_menu_data_t;

void main_menu_init(struct scene *scene) {
  main_menu_data_t *data = scene->data;
  data->font_s = TTF_OpenFont("res/fonts/Finesse-Oblique.otf", 24);
  data->font_l = TTF_OpenFont("res/fonts/Finesse-Oblique.otf", 36);
  SDL_GetRendererOutputSize(scene->renderer, &data->w, &data->h);

  SDL_Color fg = {0, 0, 0, 255};
  SDL_Surface *surface =
      TTF_RenderUTF8_Blended(data->font_l, "PSP Platformer", fg);
  int gap = surface->h + 20;
  data->title_rect = (SDL_Rect){(data->w - surface->w) / 2, data->h / 3,
                                surface->w, surface->h};
  data->title_texture = SDL_CreateTextureFromSurface(scene->renderer, surface);
  SDL_FreeSurface(surface);
  surface = TTF_RenderUTF8_Blended(data->font_s, "Press start", fg);
  data->start_rect = (SDL_Rect){(data->w - surface->w) / 2, data->h / 3 + gap,
                                surface->w, surface->h};
  data->start_texture = SDL_CreateTextureFromSurface(scene->renderer, surface);
  SDL_FreeSurface(surface);
}
void main_menu_event(struct scene *scene, SDL_Event const *event) {
  switch (event->type) {
#ifdef LINUX
  case SDL_KEYDOWN: {
    if (event->key.keysym.sym == SDLK_RETURN) {
      scene_switch(scene_game_level1);
    }
  } break;
#else
  case SDL_CONTROLLERBUTTONDOWN: {
    if (event->cbutton.button == SDL_CONTROLLER_BUTTON_START) {
      scene_switch(scene_game_level1);
    }
  } break;
#endif
  }
}
void main_menu_update(struct scene *scene, float dt) {}
void main_menu_render(struct scene *scene) {
  main_menu_data_t *data = scene->data;
  SDL_RenderCopy(scene->renderer, data->title_texture, NULL, &data->title_rect);
  SDL_RenderCopy(scene->renderer, data->start_texture, NULL, &data->start_rect);
}
void main_menu_destroy(struct scene *scene) {
  main_menu_data_t *data = scene->data;
  TTF_CloseFont(data->font_s);
  TTF_CloseFont(data->font_l);
  SDL_DestroyTexture(data->title_texture);
}

scene_t *scene_game_level1 = NULL;
scene_t *scene_main_menu = NULL;
scene_t *current_scene = NULL;

void scenes_register(SDL_Window *window, SDL_Renderer *renderer) {
  scene_game_level1 =
      scene_new_direct((scene_t){.window = window,
                                 .renderer = renderer,
                                 .data = calloc(1, sizeof(level_data_t)),
                                 .data_size = sizeof(level_data_t),
                                 .init = &game_level1_init,
                                 .event = &game_level1_event,
                                 .update = &game_level1_update,
                                 .render = &game_level1_render,
                                 .destroy = &game_level1_destroy});
  scene_game_level1->direct_ref = &scene_game_level1;
  scene_game_level1->init(scene_game_level1);
  scene_main_menu =
      scene_new_direct((scene_t){.window = window,
                                 .renderer = renderer,
                                 .data = calloc(1, sizeof(main_menu_data_t)),
                                 .data_size = sizeof(main_menu_data_t),
                                 .init = &main_menu_init,
                                 .event = &main_menu_event,
                                 .update = &main_menu_update,
                                 .render = &main_menu_render,
                                 .destroy = &main_menu_destroy});
  scene_main_menu->direct_ref = &scene_main_menu;
  scene_main_menu->init(scene_main_menu);
  current_scene = scene_main_menu;
}

void scene_switch(scene_t *other) {
  scene_t restore = (scene_t){
      .window = current_scene->window,
      .renderer = current_scene->renderer,
      .data = calloc(1, current_scene->data_size),
      .data_size = current_scene->data_size,
      .init = current_scene->init,
      .event = current_scene->event,
      .update = current_scene->update,
      .render = current_scene->render,
      .destroy = current_scene->destroy,
  };
  restore.direct_ref = current_scene->direct_ref;
  scene_free(current_scene);
  *restore.direct_ref = scene_new_direct(restore);
  restore.init(*restore.direct_ref);
  current_scene = other;
}
