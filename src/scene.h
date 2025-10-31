#ifndef SCENE_H
#define SCENE_H
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"

typedef struct scene {
  SDL_Window *window;
  SDL_Renderer *renderer;
  void *data;

  void (*init)(struct scene *scene);
  void (*event)(struct scene *scene, SDL_Event const *event);
  void (*update)(struct scene *scene, float dt);
  void (*render)(struct scene *scene);
  void (*destroy)(struct scene *scene);
} scene_t;

scene_t *scene_new_direct(scene_t scene);

void scene_free(scene_t *scene);

extern scene_t *scene_game_level1;

extern scene_t *current_scene;
void scenes_register(SDL_Window *window, SDL_Renderer *renderer);
#endif
