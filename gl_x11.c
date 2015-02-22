#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

#include <stdio.h>
#include <unistd.h>

#include "matrix.h"
#include "vector.h"

static struct {
	Display		*display;
	int		 screen;
	Window		 window;
	GC		 gc;
	int		 x, y;
	int		 w, h;
	int		 quit;
} x11 = {
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
	return x11.w;
}

int gl_view_height(void)
{
	return x11.h;
}

void gl_raster_point(int x, int y)
{
	XDrawPoint(x11.display, x11.window, x11.gc, x, y);
}

void glClear(int opts)
{
	XClearWindow(x11.display, x11.window);
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
	// empty
}

void glutInit(int *pargc, char **argv)
{
	// empty
}

void glutInitWindowSize(int width, int height)
{
	x11.w = width;
	x11.h = height;
}

void glutInitWindowPosition(int x, int y)
{
	x11.x = x;
	x11.y = y;
}

void glutInitDisplayMode(int mode)
{
	// empty
}

void glutCreateWindow(const char *title)
{
	unsigned long black, white;

	x11.display = XOpenDisplay(NULL);
	x11.screen = DefaultScreen(x11.display);

	black = BlackPixel(x11.display, x11.screen);
	white = WhitePixel(x11.display, x11.screen);

	x11.window = XCreateSimpleWindow(x11.display, DefaultRootWindow(x11.display),
			x11.x, x11.y, x11.w, x11.h, 5, white, black);

	XSetStandardProperties(x11.display, x11.window, title, "", None, NULL, 0, NULL);
	XSelectInput(x11.display, x11.window, ExposureMask | ButtonPressMask | KeyPressMask);

	x11.gc = XCreateGC(x11.display, x11.window, 0, 0);
	XSetBackground(x11.display, x11.gc, black);
	XSetForeground(x11.display, x11.gc, white);
	XClearWindow(x11.display, x11.window);
	XMapRaised(x11.display, x11.window);
}

void glutMainLoop(void)
{
	if (reshape_func)
		reshape_func(x11.w, x11.h);

	while (!x11.quit) {
		XEvent event;

		while (!XPending(x11.display)) {
			if (idle_func)
				idle_func();
			usleep(10);
		}

		XNextEvent(x11.display, &event);

		switch (event.type) {
		case Expose:
			if (!display_func)
				break;

			if (event.xexpose.count != 0)
				break;

			display_func();
			break;

		case KeyPress: {
			unsigned char key;

			if (!keyboard_func)
				break;

			if (XLookupString(&event.xkey, (char *)&key, sizeof(key), NULL, 0) != 1)
				break;

			keyboard_func(key, 0, 0);
			break;
		} 
		}
	}

	XFreeGC(x11.display, x11.gc);
	XDestroyWindow(x11.display, x11.window);
	XCloseDisplay(x11.display);
}

void glutLeaveMainLoop(void)
{
	x11.quit = 1;
}
