#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
	SDL_Window *w;
	SDL_Renderer *r;

	SDL_Event e;
	SDL_Rect rct;
} Game;

Game game;

typedef struct {
	int bs, bx, by;
	float bspdx, bspdy;

	int px, py, pw, ph, oy;

	int mup, mdown;
} Stage;

Stage stage;

/* initialization */
void init();
void resetStage();

/* game loop */
void run();
void handleEvents();
void update();
void draw();

/* clean up */
static void close();

void
init()
{
	SDL_Init(SDL_INIT_VIDEO);
	game.w = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	game.r = SDL_CreateRenderer(game.w, -1, SDL_RENDERER_ACCELERATED);

	resetStage();
}

void
resetStage()
{
	stage.bs = 30;
	stage.bx = SCREEN_WIDTH/2 - stage.bs/2; stage.by = SCREEN_HEIGHT/2 - stage.bs/2;

	stage.bspdx = stage.bspdy = 0;

	stage.pw = 15; stage.ph = 100;
	stage.px = 20; stage.py = stage.oy = SCREEN_HEIGHT/2 - stage.ph/2;

	stage.mup = stage.mdown = 0;
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
		update();
		draw();
	}
}

void
update()
{
}

void
draw()
{
	SDL_SetRenderDrawColor(game.r, 0x28, 0x28, 0x28, 0xff);
	SDL_RenderClear(game.r);
	SDL_RenderPresent(game.r);
}

static void
close()
{
	SDL_Quit();
}
