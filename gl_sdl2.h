#ifndef MUKAGL_GL_INTERNAL_H
#define MUKAGL_GL_INTERNAL_H

#include <SDL.h>

extern struct gl_sdl2 {
	SDL_Window *window;
	SDL_Renderer *renderer;
	Uint32 *framebuf;
	SDL_Texture *videobuf;
	int x, y;
	int w, h;
	int quit;
	Uint32 color;
	Uint32 clear_color;
} sdl;

// Viewport
static inline int gl_view_width(void)
{
	return sdl.w;
}

static inline int gl_view_height(void)
{
	return sdl.h;
}


// Rasterization
static inline void gl_raster_point(int x, int y)
{
	sdl.framebuf[x + y*sdl.w] = sdl.color;
}

void	gl_clear_color_buffer(void);
void	gl_clear_color(float r, float g, float b, float a);
void	gl_color(float r, float g, float b, float a);


#endif // MUKAGL_GL_INTERNAL_H
