#include <iostream>
#include <SDL2/SDL.h>

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cout << "Error : " << SDL_GetError() << std::endl;
    return -1;
  }

  SDL_Window* win =
      SDL_CreateWindow("SDL2 Example", SDL_WINDOWPOS_UNDEFINED,
                       SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_SHOWN);

  SDL_Renderer* renderer =
      SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

  SDL_Event e;

  while (1) {
    SDL_PollEvent(&e);
    if (e.type == SDL_QUIT) break;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderPresent(renderer);
  }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(win);
  SDL_Quit();
  return 0;
}