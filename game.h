#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

/* window resolution */
#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

/* frames per second */
#define FPS 30

/* game independent logic and resources */
typedef struct {
	SDL_Window *w;
	SDL_Renderer *r;

	/* for all the in-game events */
	SDL_Event e;

	/* delta time for consistent execution speed */
	unsigned int frameTicks;

	int quit;
} Game;
Game game;

/* game dependent assets */
typedef struct {
	/* ball's size and coordinates */
	int bs, bx, by;

	/* ball's x and y direction speeds */
	float bspdx, bspdy;

	/* paddle's width, height, x and y position */
	int px, py, pw, ph;

	/* opponent's paddle y coordinate */
	int oy;

	/* movement toggle */
	int mup, mdown;
} Stage;
Stage stage;

/* basic debug mode controller */
typedef struct {
	/* debug square size and x coordinate */
	int ss, sx;

	/* for toggling of debug functionality */
	int toggleDraw;
} Debug;
Debug debug;

/* initialization */
void init();
void prepareDebug();
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
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		fprintf(stderr, "could not initialize SDL. SDL Error: %s", SDL_GetError());
		exit(1);
	}

	game.w = SDL_CreateWindow("Title", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	if (!game.w) {
		fprintf(stderr, "could not create window. SDL Error: %s", SDL_GetError());
		exit(1);
	}

	game.r = SDL_CreateRenderer(game.w, -1, SDL_RENDERER_ACCELERATED);
	if (!game.r) {
		fprintf(stderr, "could not create renderer. SDL Error: %s", SDL_GetError());
		exit(1);
	}

	game.frameTicks = 0;
	game.quit = 0;

	prepareDebug();
	resetStage();
}

void
prepareDebug()
{
	debug.toggleDraw = debug.ss = debug.sx = 0;
}

void
resetStage()
{
	/* ball size */
	stage.bs = 30;

	/* middle y coordinate for the ball */
	stage.bx = SCREEN_WIDTH/2 - stage.bs/2;
	/* middle x coordinate for the ball */
	stage.by = SCREEN_HEIGHT/2 - stage.bs/2;

	/* no speed at start */
	stage.bspdx = stage.bspdy = 0;

	stage.pw = 15;
	stage.ph = 100;
	stage.px = 20;

	/* middle y coordinate for the paddles */
	stage.py = stage.oy = SCREEN_HEIGHT/2 - stage.ph/2;

	/* the user has not iniciated movement */
	stage.mup = stage.mdown = 0;
}

void
run()
{
	/* game loop */
	while (!game.quit) {
		game.frameTicks = SDL_GetTicks();
		handleEvents();
		update();
		draw();
		if (SDL_GetTicks() - game.frameTicks < 1000 / FPS) {
			SDL_Delay((1000 / FPS) - (SDL_GetTicks() - game.frameTicks));
		}
	}
}

void
handleEvents()
{
	while (SDL_PollEvent(&game.e)) {
		/* user wants to close the window */
		if (game.e.type == SDL_QUIT) {
			game.quit = 1;
		}

		/* user presses a key */
		if (game.e.type == SDL_KEYDOWN) {
			switch (game.e.key.keysym.sym) {
			case SDLK_d:
				debug.toggleDraw = debug.toggleDraw ? 0 : 1;
				break;
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

		/* user releases a key */
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
	/* movement boundaries */
	stage.py += (stage.py != 0) && stage.mup? -10: 0;
	stage.py += (stage.py != SCREEN_HEIGHT - stage.ph) && stage.mdown? 10: 0;

	/* CPU opponent */
	stage.oy = (stage.by < SCREEN_HEIGHT - stage.ph/2 - stage.bs/2 && stage.by > stage.ph/2 - stage.bs/2)? stage.by + stage.bs/2 - stage.ph/2: stage.oy;

	/* reset when the ball pass any paddle */
	if (stage.bx <= 0 || stage.bx + stage.bs >= SCREEN_WIDTH) {
		resetStage();
	}

	/* ball reflection by velocity on the y axis */
	stage.by += stage.bspdy;
	stage.bspdy = (stage.by <= 0 || /* top limit */
		       stage.by >= SCREEN_HEIGHT - stage.bs || /* bottom limit */
		       /* left paddle limit */
		       !(stage.bx > stage.px + stage.pw -10) && /* start x limit interval */
		       !(stage.bx + stage.bs < stage.px) &&     /* finish x limit interval */
		              (/* left paddle top limit */
			       stage.by + stage.bs >= stage.py &&     /* start y limit interval */
			       stage.by + stage.bs <= stage.py +10 || /* finish y limit interval */
			       /* left paddle bottom limit */
			       stage.by <= stage.py + stage.ph &&  /* start y limit interval */
			       stage.by >= stage.py + stage.ph -10 /* finish y limit interval */
			      ) ||
		       /* rigth paddle limit */
		       !(stage.bx > SCREEN_WIDTH - stage.px) && /* start x limit interval */
		       !(stage.bx + stage.bs < SCREEN_WIDTH - stage.px - stage.pw +10) && /* finish x limit interval */
		              (/* rigth paddle top limit */
			       stage.by + stage.bs >= stage.oy  &&    /* start y limit interval */
			       stage.by + stage.bs <= stage.oy +10 || /* finish y limit interval */
			       /* rigth paddle bottom limit */
			       stage.by <= stage.oy + stage.ph &&  /* start y limit interval */
			       stage.by >= stage.oy + stage.ph -10 /* finish y limit interval */
			      )			     
		      )
		      ?-stage.bspdy: stage.bspdy;

	/* ball reflection by velocity on the x axis */
	stage.bx += stage.bspdx;
	stage.bspdx = (/* left paddle limit */
		       stage.bx >= stage.px + stage.pw -10 && /* start x limit interval */
		       stage.bx <= stage.px + stage.pw &&     /* finish x limit interval */
		       stage.by <= stage.py + stage.ph +5 &&     /* start y limit interval */
		       stage.by + stage.bs >= stage.py -5 ||     /* finish y limit interval */
		       /* rigth paddle limit */
		       stage.bx + stage.bs >= SCREEN_WIDTH - stage.px - stage.pw &&     /* start x limit interval */
		       stage.bx + stage.bs <= SCREEN_WIDTH - stage.px - stage.pw +10 && /* finish x limit interval */
		       stage.by <= stage.oy + stage.ph +5 && /* start y limit interval */
		       stage.by + stage.bs >= stage.oy -5    /* finish y limit interval */
		      )
		      ?-stage.bspdx: stage.bspdx;
}

void
draw()
{
	SDL_SetRenderDrawColor(game.r, 0x28, 0x28, 0x28, 0xff);
	SDL_RenderClear(game.r);

	if (debug.toggleDraw) {
		SDL_SetRenderDrawColor(game.r, 0xff, 0xff, 0xff, 0xff);
		SDL_Rect ds;

		ds.x = (SCREEN_WIDTH - SCREEN_HEIGHT) / 2;
		ds.y = 0;
		ds.w = ds.h = SCREEN_HEIGHT;

		SDL_RenderDrawRect(game.r, &ds);

		SDL_RenderDrawLine(game.r, ds.x, 0, ds.x + ds.w, ds.h);
		SDL_RenderDrawLine(game.r, ds.x + ds.w, 0, ds.x, ds.h);

		SDL_RenderDrawLine(game.r, 0, SCREEN_HEIGHT/2, SCREEN_WIDTH, SCREEN_HEIGHT/2);

		int ballheight = stage.by + stage.bs/2;
		if (ballheight >= stage.oy && ballheight <= stage.oy + stage.ph) {
			SDL_SetRenderDrawColor(game.r, 0x00, 0x00, 0xff, 0xff);
			SDL_RenderDrawLine(game.r, 0, ballheight, SCREEN_WIDTH, ballheight);
		}
	}

	SDL_SetRenderDrawColor(game.r, 0xee, 0xee, 0xee, 0xff);
	SDL_RenderDrawLine(game.r, SCREEN_WIDTH/2, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT);

	SDL_Rect paddleL = {stage.px, stage.py, stage.pw, stage.ph};
	SDL_RenderFillRect(game.r, &paddleL);

	SDL_Rect paddleR = {SCREEN_WIDTH - stage.px - stage.pw, stage.oy, stage.pw, stage.ph};
	SDL_RenderFillRect(game.r, &paddleR);

	SDL_Rect ball = {stage.bx, stage.by, stage.bs, stage.bs};
	SDL_RenderFillRect(game.r, &ball);

	SDL_RenderPresent(game.r);
}

static void
close()
{
	SDL_Quit();
}
