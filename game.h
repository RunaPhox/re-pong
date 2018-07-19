#include <SDL2/SDL.h>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

typedef struct {
	SDL_Window *w;
	SDL_Renderer *r;

	SDL_Event e;

	unsigned int newTick, oldTick;

	int quit;
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

	game.oldTick = game.newTick = 0;
	game.quit = 0;

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
	while (!game.quit) {
		handleEvents();
		update();
		draw();
	}
}

void
handleEvents()
{
	while (SDL_PollEvent(&game.e)) {
		if (game.e.type == SDL_QUIT) {
			game.quit = 1;
		}

		if (game.e.type == SDL_KEYDOWN) {
			switch (game.e.key.keysym.sym) {
			case SDLK_r:
				resetStage();
				break;
			case SDLK_SPACE:
				stage.bspdx=-10;
				stage.bspdy=10;
				break;
			case SDLK_UP:
				stage.mup = 1;
				break;
			case SDLK_DOWN:
				stage.mdown = 1;
				break;
			default:
				break;
			}
		}

		if (game.e.type == SDL_KEYUP) {
			switch (game.e.key.keysym.sym) {
			case SDLK_UP:
				stage.mup = 0;
				break;
			case SDLK_DOWN:
				stage.mdown = 0;
				break;
			default:
				break;
			}
		}
	}
}

void
update()
{
	game.newTick = SDL_GetTicks();
	if (game.newTick > game.oldTick + 50) {
		stage.py += (stage.py != 0) && stage.mup ? -10:0;
		stage.py += (stage.py != SCREEN_HEIGHT - stage.ph) && stage.mdown ? 10 : 0;

		stage.oy=(stage.by<SCREEN_HEIGHT - stage.ph)?stage.by :stage.oy;

		stage.by+=stage.bspdy;
		stage.bspdy=(stage.by<=0 || stage.by>=SCREEN_HEIGHT - stage.bs)?-stage.bspdy:stage.bspdy;

		if (stage.bx <= 0 || stage.bx + stage.bs >= SCREEN_WIDTH) {
			resetStage();
		}

		stage.bx+=stage.bspdx;
		stage.bspdx=   (stage.bx>=stage.px && stage.bx<=stage.pw + stage.px && stage.py<=stage.by + stage.bs && stage.by<=stage.py+stage.ph || 
				stage.bx>=SCREEN_WIDTH - stage.px - stage.pw - stage.bs && stage.bx<=stage.pw + SCREEN_WIDTH - stage.px - stage.pw - stage.bs && stage.oy<=stage.by + stage.bs && stage.by<=stage.oy+stage.ph
				)?-stage.bspdx:stage.bspdx;
		game.oldTick=game.newTick;
	}
}

void
draw()
{
	SDL_SetRenderDrawColor(game.r, 0x28, 0x28, 0x28, 0xff);
	SDL_RenderClear(game.r);

	SDL_SetRenderDrawColor(game.r, 0xee, 0xee, 0xee, 0xff);
	SDL_RenderDrawLine(game.r, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

	SDL_SetRenderDrawColor(game.r, 0xee, 0xee, 0xee, 0xff);
	SDL_Rect paddleL = {stage.px, stage.py, stage.pw, stage.ph};
	SDL_RenderFillRect(game.r, &paddleL);

	SDL_SetRenderDrawColor(game.r, 0xee, 0xee, 0xee, 0xff);
	SDL_Rect paddleR = {SCREEN_WIDTH - stage.px - stage.pw, stage.oy, stage.pw, stage.ph};
	SDL_RenderFillRect(game.r, &paddleR);

	SDL_SetRenderDrawColor(game.r, 0xee, 0xee, 0xee, 0xff);
	SDL_Rect ball = {stage.bx, stage.by, stage.bs, stage.bs};
	SDL_RenderFillRect(game.r, &ball);

	SDL_RenderPresent(game.r);
}

static void
close()
{
	SDL_Quit();
}
