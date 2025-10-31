#include "scene.h"
#include "camera.h"
#include "player.h"
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
      object_new(shape_make_rect(20, 220, 200, 80)),
      object_new(shape_make_rect(380, 120, 100, 40)),
  };
  data->objects_count = sizeof(objs) / sizeof(object_t*);
  data->objects = calloc(data->objects_count, sizeof(object_t*));
  memcpy(data->objects, objs, data->objects_count * sizeof(object_t*));
  data->player = player_new(&data->camera);
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
}
void game_level1_render(scene_t *scene) {
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

scene_t *scene_game_level1 = NULL;
scene_t *current_scene = NULL;

void scenes_register(SDL_Window *window, SDL_Renderer *renderer) {
  scene_game_level1 =
      scene_new_direct((scene_t){.window = window,
                                 .renderer = renderer,
                                 .data = calloc(1, sizeof(level_data_t)),
                                 .init = &game_level1_init,
                                 .event = &game_level1_event,
                                 .update = &game_level1_update,
                                 .render = &game_level1_render,
                                 .destroy = &game_level1_destroy});
  current_scene = scene_game_level1;
}
