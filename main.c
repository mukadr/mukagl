#include <X11/Xlib.h>
#include <X11/Xos.h>
#include <X11/Xutil.h>

#include <err.h>
#include <errno.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "matrix.h"
#include "vector.h"

struct {
	Display		*display;
	int		 screen;
	Window		 window;
	GC		 gc;
} x11;

#define WND_WIDTH	1024.0f
#define WND_HEIGHT	768.0f

struct {
	float x, y, z;
	float ax, ay, az;
	float ms; // move speed
} player = {
	.z = -2.0f,
	.ms = 0.5f,
};

static inline float deg2rad(float deg)
{
	return deg * (M_PI/180);
}

void draw_vec4(const vec4 v)
{
	// avoid division by zero when trying to clip vertex with w=0
	// this means that we should use w to check against near and far planes
	// instead of checking clipped z, but due to my lazyness i will add this quick fix.
	if (v[3] < 0.001f)
		return;

	vec3 clipped = {
		v[0]/v[3],
		v[1]/v[3],
		v[2]/v[3],
	};

	// ignore vertexes outside clipping box (view box..)
	if (clipped[0] < -1.0f || clipped[0] > 1.0f ||
	    clipped[1] < -1.0f || clipped[1] > 1.0f ||
	    clipped[2] < -1.0f || clipped[2] > 1.0f)
		return;

	int x = clipped[0]*(WND_WIDTH/2) + WND_WIDTH/2;
	int y = -clipped[1]*(WND_HEIGHT/2) + WND_HEIGHT/2;

	XDrawPoint(x11.display, x11.window, x11.gc, x, y);
}

void setup_camera(mat44 cam,
		  float x, float y, float z,
		  float ax, float ay, float az)
{
	mat44_identity(cam);

	mat44_rotx(cam, -ax);
	mat44_roty(cam, -ay);
	mat44_rotz(cam, -az);
	mat44_translate(cam, -x, -y, -z);
}

// Setups a clip matrix. objects visible in the view frustrum will appear inside
// a box between -1 and 1 (all axis) after the division by w (old z).
//
// A zoom of 1.0 results in a FOV of 90 degrees. since dividing x and y by z will result
// in 1 or -1 (limit of the frustrum) when the components are equal(disconsidering sign),
// this fact becomes clear:
//
//
//                      z          . (x==z)
//                      .  (x<z) .
//                      .      .
//                      .    .         (x>z) out of clipping zone (result >1)
//                      .  .
//                      ..
//                      ..............x
//
// The straight line where the division results in 1 is half a quadrant (45 deg).
// There is also another 45 degrees at the negative side, so we have a FOV of 90 degrees when dividing
// x/z (the same applies to y/z..) considering that we scale from -1 to 1 on the viewport.
//
// Since the zoom increases as fov decreases, the formula to convert between zoom and fov is given as
// 1/tan(fov/2)
//
// To maintain correct aspect ratio, lets specify only the Xfov and scale Yfov
// depending on the aspect ration (gluPerspective let you work with Yfov, but I think it is much more
// intuitive to specify the horizontal FOV)
//
void setup_projection(mat44 clip, float fovx, float aspect, float near, float far)
{
	float zoomx = 1.0f / tanf(deg2rad(fovx/2));
	float zoomy = zoomx * aspect;

	mat44_identity(clip);

	clip[0][0] = zoomx;
	clip[1][1] = zoomy;

	clip[2][2] = (far+near)/(far-near);
	clip[3][2] = 1.0f;

	clip[2][3] = (-2*near*far)/(far-near);
	clip[3][3] = 0.0f;
}

void redraw(void)
{
	mat44 camera;
	mat44 clip;
	mat44 world_camera;
	static int angle;

	mat44_identity(world_camera);

	/* clipping transform */
	setup_projection(clip, 90.0f, WND_WIDTH/WND_HEIGHT, 0.1f, 30.0f);
	mat44_mul(world_camera, clip);

	/* camera transform */
	setup_camera(camera, player.x, player.y, player.z, player.ax, player.ay, player.az);
	mat44_mul(world_camera, camera);

	/* world transform */
	mat44_roty(world_camera, angle);

	XClearWindow(x11.display, x11.window);

	float v4[4] = { 1.0f, 0.0f, 0.0f, 1.0f };

	mat44_mul_vec4(world_camera, v4);

	draw_vec4(v4);

	if (++angle == 360)
		angle = 0;
}

int main(int argc, char **argv)
{
	unsigned long black, white;

	x11.display = XOpenDisplay(NULL);
	x11.screen = DefaultScreen(x11.display);

	black = BlackPixel(x11.display, x11.screen);
	white = WhitePixel(x11.display, x11.screen);

	x11.window = XCreateSimpleWindow(x11.display, DefaultRootWindow(x11.display),
			0, 0, WND_WIDTH, WND_HEIGHT, 5, white, black);

	XSetStandardProperties(x11.display, x11.window, "3D Render", "3D", None, NULL, 0, NULL);
	XSelectInput(x11.display, x11.window, ExposureMask | ButtonPressMask | KeyPressMask);

	x11.gc = XCreateGC(x11.display, x11.window, 0, 0);
	XSetBackground(x11.display, x11.gc, black);
	XSetForeground(x11.display, x11.gc, white);
	XClearWindow(x11.display, x11.window);
	XMapRaised(x11.display, x11.window);

	for (;;) {		
		XEvent event;

		if (!XPending(x11.display)) {
			usleep(10000);
			redraw();
			continue;
		}

		XNextEvent(x11.display, &event);

		if (event.type == KeyPress) {
			char key;

			if (XLookupString(&event.xkey, &key, sizeof(key), NULL, 0) != 1)
				continue;

			if (key == 27) // ESC
				break;

			switch (key) {
			case 'w':
				player.x += sinf(deg2rad(player.ay)) * player.ms;
				player.z += cosf(deg2rad(player.ay)) * player.ms;
				break;
			case 's':
				player.x += -sinf(deg2rad(player.ay)) * player.ms;
				player.z += -cosf(deg2rad(player.ay)) * player.ms;
				break;
			case 'a':
				player.x += -cosf(deg2rad(player.ay)) * player.ms;
				player.z += sinf(deg2rad(player.ay)) * player.ms;
				break;
			case 'd':
				player.x += cosf(deg2rad(player.ay)) * player.ms;
				player.z += -sinf(deg2rad(player.ay)) * player.ms;
				break;
			case 'q':
				player.ay--;
				break;
			case 'e':
				player.ay++;
				break;
			}
		} 
	}

	XFreeGC(x11.display, x11.gc);
	XDestroyWindow(x11.display, x11.window);
	XCloseDisplay(x11.display);

	return 0;
}

