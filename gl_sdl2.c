// SDL2 backend for mukaGL
// Double buffered using SDL_Renderer

#include <err.h>
#include <stdio.h>
#include <unistd.h>

#include <SDL.h>

#include "matrix.h"
#include "vector.h"

static struct {
	SDL_Window *window;
	SDL_Renderer *renderer;
	int x, y;
	int w, h;
	int quit;
	SDL_Color color;
	SDL_Color clear_color;
} sdl = {
	.x = 0,
	.y = 0,
	.w = 300,
	.h = 300,
};

static void (*display_func)(void);
static void (*idle_func)(void);
static void (*keyboard_func)(unsigned char key, int x, int y);
static void (*reshape_func)(int width, int height);

int gl_view_width(void)
{
	return sdl.w;
}

int gl_view_height(void)
{
	return sdl.h;
}

void gl_raster_point(int x, int y)
{
	SDL_RenderDrawPoint(sdl.renderer, x, y);
}

void gl_color(float r, float g, float b, float a)
{
	sdl.color.r = r * 255;
	sdl.color.g = g * 255;
	sdl.color.b = b * 255;
	sdl.color.a = a * 255;

	SDL_SetRenderDrawColor(sdl.renderer, sdl.color.r, sdl.color.g,
			sdl.color.b, sdl.color.a);
}

void gl_clear_color(float r, float g, float b, float a)
{
	sdl.clear_color.r = r * 255;
	sdl.clear_color.g = g * 255;
	sdl.clear_color.b = b * 255;
	sdl.clear_color.a = a * 255;
}

void gl_clear_color_buffer(void)
{
	SDL_SetRenderDrawColor(sdl.renderer, sdl.clear_color.r, sdl.clear_color.g,
			sdl.clear_color.b, sdl.clear_color.a);

	SDL_RenderClear(sdl.renderer);

	SDL_SetRenderDrawColor(sdl.renderer, sdl.color.r, sdl.color.g,
			sdl.color.b, sdl.color.a);
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

void glutPostRedisplay(void)
{
	if (display_func)
		display_func();
}

void glutSwapBuffers(void)
{
	SDL_RenderPresent(sdl.renderer);
}

void glutInit(int *pargc, char **argv)
{
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
		errx(1, "gl_sdl2: SDL_Init: %s", SDL_GetError());
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
	if (!sdl.window)
		errx(1, "gl_sdl2: SDL_CreateWindow: %s", SDL_GetError());

	sdl.renderer = SDL_CreateRenderer(sdl.window, -1, SDL_RENDERER_ACCELERATED);
	if (!sdl.renderer)
		errx(1, "gl_sdl2: SDL_CreateRenderer: %s", SDL_GetError());
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
