#include "SDL2/SDL_log.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_timer.h"
#include "object.h"
#include "player.h"
#include <SDL2/SDL.h>

void logger(void *, int, SDL_LogPriority, const char *msg) {
  printf("%s\n", msg);
}

int main(int, char *[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);

  SDL_Window *window = SDL_CreateWindow("window", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 480, 272, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  int w, h;
  SDL_GetRendererOutputSize(renderer, &w, &h);

#ifdef LINUX
  SDL_LogSetOutputFunction(logger, NULL);
#endif

  camera_t camera = camera_new(w, h);
  object_t *objects[] = {
      object_new(shape_make_rect(20, 220, 200, 80)),
      object_new(shape_make_rect(380, 120, 100, 40)),
  };
  player_t *player = player_new(&camera);
  player->objects = objects;
  player->objects_count = sizeof(objects) / sizeof(object_t *);

  int running = 1;
  Uint64 last = SDL_GetPerformanceCounter();
  SDL_Event event;
  while (running) {
    Uint64 now = SDL_GetPerformanceCounter();
    float delta = (float)(now - last) / (float)SDL_GetPerformanceFrequency();
    last = now;
    player_update(player, delta);

    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        running = 0;
      } break;
      case SDL_CONTROLLERDEVICEADDED: {
        s_player_controller = SDL_GameControllerOpen(event.cdevice.which);
      } break;
      default: {
        player_event(player, &event);
      } break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    player_render(player, renderer);
    for (int i = 0; i < sizeof(objects) / sizeof(object_t *); ++i) {
      object_render(objects[i], renderer, &camera);
    }
    SDL_RenderPresent(renderer);
  }

  player_free(player);
  for (int i = 0; i < sizeof(objects) / sizeof(object_t *); ++i) {
    object_free(objects[i]);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();

  return 0;
}
