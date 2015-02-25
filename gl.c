// OpenGL like implementation based on mukagl

#include <stdio.h>
#include <string.h>

#include "gl.h"
#include "gl_internal.h"
#include "math.h"
#include "matrix.h"
#include "util.h"
#include "vector.h"

#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b))

#define STACK_MAX	128

struct glmatrix {
	mat44 mat;
	mat44 stack[STACK_MAX];
	int pos;
};

// model-view matrix
static struct glmatrix model_view;

// matrix selector
static struct glmatrix *matp = &model_view;

// projection matrix
static struct glmatrix project;

// final model-view-proj matrix
static mat44 transform;

// selected primitive (GL_POINTS, GL_TRIANGLES...)
static void (*primitive_fn)(const vec3 v);

static void update_transform(void)
{
	mat44_identity(transform);
	mat44_mul(transform, project.mat);
	mat44_mul(transform, model_view.mat);
}

void glMatrixMode(int mode)
{
	switch (mode) {
	case GL_MODELVIEW:
		matp = &model_view;
		break;
	case GL_PROJECTION:
		matp = &project;
		break;
	}
}

void glTranslatef(float x, float y, float z)
{
	mat44_translate(matp->mat, x, y, z);
	update_transform();
}

void glRotatef(float angle, float x, float y, float z)
{
	mat44_rotx(matp->mat, x * angle);
	mat44_roty(matp->mat, y * angle);
	mat44_rotz(matp->mat, z * angle);
	update_transform();
}

void glScalef(float sx, float sy, float sz)
{
	mat44_scale(matp->mat, sz, sy, sz);
	update_transform();
}

void glLoadIdentity(void)
{
	mat44_identity(matp->mat);
	update_transform();
}

void glPushMatrix(void)
{
	if (matp->pos == STACK_MAX)
		return;

	memcpy(matp->stack[matp->pos], matp->mat, sizeof(mat44));
	matp->pos++;
}

void glPopMatrix(void)
{
	if (matp->pos == 0)
		return;

	matp->pos--;
	memcpy(matp->mat, matp->stack[matp->pos], sizeof(mat44));
}

void gluPerspective(float fovy, float aspect, float near, float far)
{
	float zoomy = 1.0f / tanf(deg2radf(fovy/2));
	float zoomx = zoomy / aspect;

	matp->mat[0][0] = zoomx;
	matp->mat[1][1] = zoomy;

	matp->mat[2][2] = -(far+near)/(far-near);
	matp->mat[3][2] = -1.0f;

	matp->mat[2][3] = (-2*near*far)/(far-near);
	matp->mat[3][3] = 0.0f;
}

void glClear(int opts)
{
	gl_clear_color_buffer();
}

void glClearColor(float r, float g, float b, float a)
{
	gl_clear_color(r, g, b, a);
}

void glColor3f(float r, float g, float b)
{
	gl_color(r, g, b, 1.0f);
}

static inline int is_visible(const vec3 v)
{
	return v[0] >= -1.0f && v[0] <= 1.0f &&
	       v[1] >= -1.0f && v[1] <= 1.0f &&
	       v[2] >= -1.0f && v[2] <= 1.0f;
}

static inline void clip_to_screen(const vec3 v, int *px, int *py)
{
	int w = gl_view_width();
	int h = gl_view_height();

	*px = v[0]*(w/2) + w/2;
	*py = -v[1]*(h/2) + h/2;
}

void raster_point(const vec3 v)
{
	int x, y;

	if (!is_visible(v))
		return;

	clip_to_screen(v, &x, &y);
	gl_raster_point(x, y);
}

static vec3 tribuf[3];
static int tripos;

void raster_triangle(const vec3 v)
{
	int x1, y1;
	int x2, y2;
	int x3, y3;
	int minx, maxx;
	int miny, maxy;
	int x, y;

	memcpy(tribuf[tripos++], v, sizeof(vec3));
	if (tripos < 3)
		return;

	tripos = 0;

	// if entire triangle is out of clipping zone, do nothing
	if (!is_visible(tribuf[0]) &&
	    !is_visible(tribuf[1]) &&
	    !is_visible(tribuf[2]))
		return;

	clip_to_screen(tribuf[0], &x1, &y1);
	clip_to_screen(tribuf[1], &x2, &y2);
	clip_to_screen(tribuf[2], &x3, &y3);

	minx = min(x1, min(x2, x3));
	maxx = max(x1, max(x2, x3));
	miny = min(y1, min(y2, y3));
	maxy = max(y1, max(y2, y3));

	// rasterize triangle
	for (y = miny; y <= maxy; y++) {
		for (x = minx; x <= maxx; x++) {
			if (point_in_triangle_2i(x, y, x1, y1, x2, y2, x3, y3))
				gl_raster_point(x, y);
		}
	}
}

void glBegin(int mode)
{
	switch (mode) {
	default:
		fprintf(stderr, "gl: Unsupported primitive %d, defaulting to GL_POINTS\n", mode);
	case GL_POINTS:
		primitive_fn = raster_point;
		break;
	case GL_TRIANGLES:
		primitive_fn = raster_triangle;
		break;
	}
}

void glEnd(void)
{
	primitive_fn = NULL;
}

void glVertex3f(float x, float y, float z)
{
	vec4 a = { x, y, z, 1.0f };
	vec3 b;

	mat44_mul_vec4(transform, a);

	// Avoid division by zero (near clip should always be greater than this)
	if (a[3] < 0.0001f)
		return;

	// w division
	b[0] = a[0]/a[3];
	b[1] = a[1]/a[3];
	b[2] = a[2]/a[3];

	if (primitive_fn)
		primitive_fn(b);
}
