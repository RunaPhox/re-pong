#include <stdio.h>
#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
	SDL_Window *w;
	SDL_Renderer *r;

	SDL_Event e;
} Game;

Game game;

void init();
void run();
static void close();

void
init()
{
	SDL_Init(SDL_INIT_VIDEO);
	game.w = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
}

void
run()
{
	int quit = 0;
	while (!quit) {
		while (SDL_PollEvent(&game.e)) {
			if (game.e.type == SDL_QUIT) {
				quit = 1;
			}
		}
	}
}

static void
close()
{
	SDL_Quit();
}

int
main()
{
	init();
	run();
	close();

	return 0;
}
