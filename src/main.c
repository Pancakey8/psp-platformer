#include "SDL2/SDL_log.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_stdinc.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_ttf.h"
#include "object.h"
#include "player.h"
#include "scene.h"
#include "textures.h"
#include <SDL2/SDL.h>

void logger(void *, int, SDL_LogPriority, const char *msg) {
  printf("%s\n", msg);
}

int main(int, char *[]) {
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
  TTF_Init();

  SDL_Window *window = SDL_CreateWindow("window", SDL_WINDOWPOS_UNDEFINED,
                                        SDL_WINDOWPOS_UNDEFINED, 480, 272, 0);

  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  tmap_init(renderer);

#ifdef LINUX
  SDL_LogSetOutputFunction(logger, NULL);
#endif

  scenes_register(window, renderer);

  int running = 1;
  Uint64 last = SDL_GetPerformanceCounter();
  SDL_Event event;
  while (running) {
    Uint64 now = SDL_GetPerformanceCounter();
    float delta = (float)(now - last) / (float)SDL_GetPerformanceFrequency();
    last = now;
    current_scene->update(current_scene, delta);

    if (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT: {
        running = 0;
      } break;
      case SDL_CONTROLLERDEVICEADDED: {
        s_player_controller = SDL_GameControllerOpen(event.cdevice.which);
      } break;
      default: {
        current_scene->event(current_scene, &event);
      } break;
      }
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
    current_scene->render(current_scene);
    SDL_RenderPresent(renderer);
  }

  scene_free(scene_game_level1);

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  TTF_Quit();

  return 0;
}
