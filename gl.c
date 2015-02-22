// OpenGL like implementation based on mukagl

#include <string.h>

#include "gl.h"
#include "gl_internal.h"
#include "matrix.h"
#include "util.h"
#include "vector.h"

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

void glClearColor(float r, float g, float b, float a)
{

}

void glBegin(int mode)
{

}

void glEnd(void)
{

}

void glVertex3f(float x, float y, float z)
{
	vec4 a = { x, y, z, 1.0f };
	int w = gl_view_width();
	int h = gl_view_height();
	int xx;
	int yy;

	mat44_mul_vec4(transform, a);

	if (a[3] < 0.0001f)
		return;

	// w division
	a[0] /= a[3];
	a[1] /= a[3];
	a[2] /= a[3];

	if (a[0] < -1.0f || a[0] > 1.0f ||
	    a[1] < -1.0f || a[1] > 1.0f ||
	    a[2] < -1.0f || a[2] > 1.0f)
		return;

	xx = a[0]*(w/2) + w/2;
	yy = -a[1]*(h/2) + h/2;

	gl_raster_point(xx, yy);
}
