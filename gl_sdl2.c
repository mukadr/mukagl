// SDL2 backend for mukaGL
// Double buffered using SDL_Renderer

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <SDL.h>

#include "matrix.h"
#include "vector.h"

#include "gl_sdl2.h"

struct gl_sdl2 sdl = {
	.x = 0,
	.y = 0,
	.w = 300,
	.h = 300,
};

static void (*display_func)(void);
static void (*idle_func)(void);
static void (*keyboard_func)(unsigned char key, int x, int y);
static void (*reshape_func)(int width, int height);

void gl_color(float r, float g, float b, float a)
{
	Uint32 ri = (Uint32)(r * 255);
	Uint32 gi = (Uint32)(g * 255);
	Uint32 bi = (Uint32)(b * 255);
	Uint32 ai = (Uint32)(a * 255);

	sdl.color = (ai << 24) | (bi << 16) | (gi << 8) | ri;
}

void gl_clear_color(float r, float g, float b, float a)
{
	Uint32 ri = (Uint32)(r * 255);
	Uint32 gi = (Uint32)(g * 255);
	Uint32 bi = (Uint32)(b * 255);
	Uint32 ai = (Uint32)(a * 255);

	sdl.clear_color = (ai << 24) | (bi << 16) | (gi << 8) | ri;
}

void gl_clear_color_buffer(void)
{
	int i;

	for (i = 0; i < sdl.w * sdl.h; i++)
		sdl.framebuf[i] = sdl.clear_color;
}

void glutDisplayFunc(void (*func)(void))
{
	display_func = func;
}

void glutIdleFunc(void (*func)(void))
{
	idle_func = func;
}

void glutKeyboardFunc(void (*func)(unsigned char key, int x, int y))
{
	keyboard_func = func;
}

void glutReshapeFunc(void (*func)(int width, int height))
{
	reshape_func = func;
}

#ifdef __MUKAGL_DEBUGFPS__
static inline void debug_fps(void)
{
	static Uint32 last_ticks;
	static int fps;
	Uint32 now;

	now = SDL_GetTicks();
	if (now - last_ticks >= 1000) {
		printf("%u fps\n", fps);
		last_ticks = now;
		fps = 0;
	}
	fps++;
}
#else
static inline void debug_fps(void)
{
}
#endif

void glutPostRedisplay(void)
{
	if (display_func)
		display_func();

	debug_fps();
}

void glutSwapBuffers(void)
{
	SDL_UpdateTexture(sdl.videobuf, NULL, sdl.framebuf, sdl.w * 4);
	SDL_RenderCopy(sdl.renderer, sdl.videobuf, NULL, NULL);
	SDL_RenderPresent(sdl.renderer);
}

void glutInit(int *pargc, char **argv)
{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		fprintf(stderr, "gl_sdl2: SDL_Init: %s\n", SDL_GetError());
		exit(1);
        }
}

void glutInitWindowSize(int width, int height)
{
	sdl.w = width;
	sdl.h = height;
}

void glutInitWindowPosition(int x, int y)
{
	sdl.x = x;
	sdl.y = y;
}

void glutInitDisplayMode(int mode)
{
	// empty
}

void glutCreateWindow(const char *title)
{
	sdl.window = SDL_CreateWindow(title, sdl.x, sdl.y, sdl.w, sdl.h, SDL_WINDOW_SHOWN);
	if (!sdl.window) {
		fprintf(stderr, "gl_sdl2: SDL_CreateWindow: %s\n", SDL_GetError());
		exit(1);
        }

	sdl.renderer = SDL_CreateRenderer(sdl.window, -1, SDL_RENDERER_ACCELERATED);
	if (!sdl.renderer) {
		fprintf(stderr, "gl_sdl2: SDL_CreateRenderer: %s\n", SDL_GetError());
		exit(1);
        }

	sdl.framebuf = malloc(sdl.w * sdl.h * 4);
	if (!sdl.framebuf) {
		fprintf(stderr, "gl_sdl2: out of memory\n");
		exit(1);
	}

	sdl.videobuf = SDL_CreateTexture(sdl.renderer, SDL_PIXELFORMAT_ARGB8888,
					 SDL_TEXTUREACCESS_STREAMING, sdl.w, sdl.h);
	if (!sdl.videobuf) {
		fprintf(stderr, "gl_sdl2: SDL_CreateTexture: %s\n", SDL_GetError());
		exit(1);
	}
}

void glutMainLoop(void)
{
	if (reshape_func)
		reshape_func(sdl.w, sdl.h);

	while (!sdl.quit) {
		SDL_Event ev;

		while (SDL_PollEvent(&ev)) {
			if (ev.type == SDL_QUIT) {
				sdl.quit = 1;
				break;
			}

			switch (ev.type) {
			case SDL_KEYDOWN:
				if (keyboard_func)
					keyboard_func(ev.key.keysym.sym, 0, 0);
				break;
			}
		}

		if (idle_func)
			idle_func();
	}

	SDL_DestroyRenderer(sdl.renderer);
	SDL_DestroyWindow(sdl.window);
	SDL_Quit();
}

void glutLeaveMainLoop(void)
{
	sdl.quit = 1;
}
