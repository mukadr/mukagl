#include <stdio.h>
#include <string.h>

#include "gl.h"
#include "gl_sdl2.h"
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

// textures
struct texinfo {
	int used;
	int min_filter;
	int mag_filter;
	char *data;
};

static GLuint texnr;

#define TEXTURE_MAX 1024

static struct texinfo textures[TEXTURE_MAX];

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

// FIXME: glRotatef should rotate around vector [x,y,z]
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
	update_transform();
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

void glEnable(int cap)
{
	switch (cap) {
	case GL_DEPTH_TEST:
		if (!(sdl.mode & GLUT_DEPTH)) {
			fprintf(stderr, "gl: GLUT_DEPTH must be specified during glutInit in order to enable buffer\n");
			break;
		}
		sdl.caps |= GL_DEPTH_TEST;
		break;
	}
}

void glDisable(int cap)
{
	switch (cap) {
	case GL_DEPTH_TEST:
		sdl.caps &= ~GL_DEPTH_TEST;
		break;
	}
}

void glClear(int opts)
{
	if (opts & GL_COLOR_BUFFER_BIT)
		gl_clear_color_buffer();
	if (opts & GL_DEPTH_BUFFER_BIT)
		gl_clear_depth_buffer();
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
	gl_raster_point(x, y, v[2]);
}

struct vecinfo {
	vec3 v;
	float s, t;
};

static struct vecinfo tribuf[3];
static int tripos;
static float scoord, tcoord;

static inline void swap2i(int *a, int *b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

static inline void swap2f(float *a, float *b)
{
	float tmp = *a;
	*a = *b;
	*b = tmp;
}

struct cursor {
	int x, y;
	int x1, y1;
	int dx, dy;
	int err;
	int incr;
	float z;
	float zstep;
	void (*walk_fn)(struct cursor *);
};

static void cursor_walk81(struct cursor *c)
{
	c->x++;
	c->err += c->dy;
	if (2*c->err >= c->dx) {
		c->y += c->incr;
		c->err -= c->dx;
	}
}

static void cursor_walk23(struct cursor *c)
{
	c->y++;
	c->err += c->dx;
	if (2*c->err >= c->dy) {
		c->x += c->incr;
		c->err -= c->dy;
	}
}

static void cursor_walk45(struct cursor *c)
{
	c->x--;
	c->err += c->dy;
	if (2*c->err >= c->dx) {
		c->y += c->incr;
		c->err -= c->dx;
	}
}

static void cursor_walk67(struct cursor *c)
{
	c->y--;
	c->err += c->dx;
	if (2*c->err >= c->dy) {
		c->x += c->incr;
		c->err -= c->dy;
	}
}

// given two points p0 and p1, initialize cursor to walk from p0 to p1
static void cursor_init(struct cursor *cur, int x0, int y0, float z0, int x1, int y1, float z1)
{
	int dx = abs(x1 - x0);
	int dy = abs(y1 - y0);
	int steps;

	if (x0 < x1) {
		if (dx >= dy) {
			cur->walk_fn = cursor_walk81;
			cur->incr = (y0 < y1) ? 1 : -1;
			steps = dx;
		} else {
			if (y0 < y1)
				cur->walk_fn = cursor_walk23;
			else
				cur->walk_fn = cursor_walk67;
			cur->incr = 1;
			steps = dy;
		}
	} else {
		if (dx >= dy) {
			cur->walk_fn = cursor_walk45;
			cur->incr = (y0 < y1) ? 1 : -1;
			steps = dx;
		} else {
			if (y0 < y1)
				cur->walk_fn = cursor_walk23;
			else
				cur->walk_fn = cursor_walk67;
			cur->incr = -1;
			steps = dy;
		}
	}

	cur->x = x0;
	cur->y = y0;
	cur->x1 = x1;
	cur->y1 = y1;
	cur->dx = dx;
	cur->dy = dy;
	cur->err = 0;
	cur->zstep = (z1 - z0)/(steps + 2); // linear interpolation of Z coord
	cur->z = z0 + cur->zstep;
}

static inline void cursor_update(struct cursor *cur)
{
	cur->walk_fn(cur);
	cur->z += cur->zstep;
}

static void raster_triangle(const vec3 v)
{
	int tx[3], ty[3];
	float tz[3];
	int i, j;
	int x, y;
	struct cursor cur0, cur1;

	memcpy(tribuf[tripos].v, v, sizeof(vec3));
	tribuf[tripos].s = scoord;
	tribuf[tripos].t = tcoord;
	tripos++;
	if (tripos < 3)
		return;
	tripos = 0;

	// Do not raster triangles outside view frustrum
	if (tribuf[0].v[0] > 1.0f && tribuf[1].v[0] > 1.0f && tribuf[2].v[0] > 1.0f) // right
		return;
	if (tribuf[0].v[0] < -1.0f && tribuf[1].v[0] < -1.0f && tribuf[2].v[0] < -1.0f) // left
		return;
	if (tribuf[0].v[1] > 1.0f && tribuf[1].v[1] > 1.0f && tribuf[2].v[1] > 1.0f) // top
		return;
	if (tribuf[0].v[1] < -1.0f && tribuf[1].v[1] < -1.0f && tribuf[2].v[1] < -1.0f) // bottom
		return;
	if (tribuf[0].v[2] > 1.0f && tribuf[1].v[2] > 1.0f && tribuf[2].v[2] > 1.0f) // after
		return;
	if (tribuf[0].v[2] < -1.0f && tribuf[1].v[2] < -1.0f && tribuf[2].v[2] < -1.0f) // before
		return;

	clip_to_screen(tribuf[0].v, tx+0, ty+0);
	clip_to_screen(tribuf[1].v, tx+1, ty+1);
	clip_to_screen(tribuf[2].v, tx+2, ty+2);

	for (i = 0; i < 3; i++)
		tz[i] = tribuf[i].v[2];

	// sort by ascending Y
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (ty[j+1] < ty[j]) {
				swap2i(tx+j, tx+j+1);
				swap2i(ty+j, ty+j+1);
				swap2f(tz+j, tz+j+1);
			}
		}
	}

	// sort by ascending X (when Y is the same)
	for (i = 0; i < 2; i++) {
		for (j = 0; j < 2; j++) {
			if (ty[j+1] == ty[j] && tx[j+1] < tx[j]) {
				swap2i(tx+j, tx+j+1);
				swap2i(ty+j, ty+j+1);
				swap2f(tz+j, tz+j+1);
			}
		}
	}

	// cur0 walks from A to B
	// cur1 walks from A to C
	cursor_init(&cur0, tx[0], ty[0], tz[0], tx[1], ty[1], tz[1]);
	cursor_init(&cur1, tx[0], ty[0], tz[0], tx[2], ty[2], tz[2]);

	for (y = ty[0]; y <= ty[2]; y++) {
		int x0, x1;
		float z, z0, z1, zstep;
		int prev;

		if (y == ty[1]) {
			// cur0 walks from B to C
			cursor_init(&cur0, tx[1], ty[1], tz[1], tx[2], ty[2], tz[2]);
		}

		if (cur0.x < cur1.x) {
			x0 = cur0.x;
			z0 = cur0.z;
			x1 = cur1.x;
			z1 = cur1.z;
		} else {
			x0 = cur1.x;
			z0 = cur1.z;
			x1 = cur0.x;
			z1 = cur0.z;
		}

		zstep = (z1 - z0)/(x1 - x0 + 2);

		// fill line between cur0 and cur1
		for (x = x0, z = z0; x <= x1; x++) {
			z += zstep;
			gl_raster_point(x, y, z);
		}

		prev = cur0.y;
		cursor_update(&cur0);
		if (cur0.y != cur0.y1) {
			while (prev == cur0.y)
				cursor_update(&cur0);
		}

		prev = cur1.y;
		cursor_update(&cur1);
		if (cur1.y != cur1.y1) {
			while (prev == cur1.y)
				cursor_update(&cur1);
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

	if (a[3] < 0.1f)
		a[3] = 0.1f;

	// w division
	b[0] = a[0]/a[3];
	b[1] = a[1]/a[3];
	b[2] = a[2]/a[3];

	if (primitive_fn)
		primitive_fn(b);
}

void glGenTextures(GLsizei n, GLuint *texture)
{
	GLuint idx = 0;

	while (n > 0) {
		while (textures[idx].used) {
			idx++;
			if (idx == TEXTURE_MAX) {
				fprintf(stderr, "glGenTextures: Too many textures\n");
				exit(1);
			}
		}
		textures[idx].used = 1;
		*texture++ = idx;
		n--;
	}
}

void glBindTexture(GLenum target, GLuint texture)
{
	if (target != GL_TEXTURE_2D) {
		fprintf(stderr, "glBindTexture: Only GL_TEXTURE_2D supported\n");
		return;
	}
	if (texture >= TEXTURE_MAX || !textures[texture].used) {
		fprintf(stderr, "gl: Invalid texture id\n");
		exit(1);
	}
	texnr = texture;
}

void glTexParameteri(GLenum target, GLenum pname, GLint param)
{
	struct texinfo *ti = &textures[texnr];

	switch (pname) {
	case GL_TEXTURE_MIN_FILTER:
		ti->min_filter = param;
		break;
	case GL_TEXTURE_MAG_FILTER:
		ti->mag_filter = param;
		break;
	default:
		fprintf(stderr, "gl: glTexParameteri: Unsupported parameter %d\n", pname);
		break;
	}
}

void glPixelStorei(GLenum pname, GLint param)
{
	if (pname != GL_UNPACK_ALIGNMENT) {
		fprintf(stderr, "glPixelStorei: Unsupported parameter %d\n", pname);
		return;
	}
	sdl.unpack_align = param;
}

void glTexImage2D(GLenum target, GLint level, GLint internal_fmt,
		     GLsizei width, GLsizei height, GLint border, GLenum format,
		     GLenum type, const GLvoid *pixels)
{
	struct texinfo *ti = &textures[texnr];
	const char *src = pixels;
	char *dst;
	GLsizei h;

	if (target != GL_TEXTURE_2D) {
		fprintf(stderr, "glTexImage2D: Only GL_TEXTURE_2D supported\n");
		return;
	}

	if (level != 0) {
		fprintf(stderr, "glTexImage2D: Mipmap not supported\n");
		return;
	}

	if (internal_fmt != GL_RGB || format != GL_RGB) {
		fprintf(stderr, "glTexImage2D: Only GL_RGB supported\n");
		return;
	}

	if (border != 0) {
		fprintf(stderr, "glTexImage2D: No border support\n");
		return;
	}

	if (type != GL_UNSIGNED_BYTE) {
		fprintf(stderr, "glTexImage2D: Only GL_UNSIGNED_BYTE supported\n");
		return;
	}

	if (width <= 0 || height <= 0) {
		fprintf(stderr, "glTexImage2D: Invalid image size\n");
	}

	ti->data = malloc(width * height * 3);
	if (!ti->data) {
		fprintf(stderr, "glTexImage2D: Out of memory\n");
		exit(1);
	}

	dst = ti->data;
	for (h = 0; h < height; h++) {
		// align to next row of pixels
		src = (const char *)(((uintptr_t)src + sdl.unpack_align - 1) & ~(sdl.unpack_align - 1));

		memcpy(dst, src, width * 3);

		dst += width * 3;
		src += width * 3;
	}
}

void glTexEnvf(GLenum target, GLenum pname, GLfloat param)
{
}

void glTexCoord2f(GLfloat s, GLfloat t)
{
	scoord = s;
	tcoord = t;
}
