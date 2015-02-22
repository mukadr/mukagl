#include <math.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"
#include "util.h"
#include "vector.h"

void mat44_identity(mat44 m)
{
	int i, j;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			m[i][j] = (i == j) ? 1.0f : 0.0f;
}

void mat44_mul(mat44 m, mat44 n)
{
	int i, j;
	mat44 ret;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			ret[i][j] = m[i][0]*n[0][j] + m[i][1]*n[1][j] + m[i][2]*n[2][j] + m[i][3]*n[3][j];

	memcpy(m, ret, sizeof(mat44));
}

void mat44_mul_vec4(mat44 m, vec4 a)
{
	int i;
	vec4 ret;

	for (i = 0; i < 4; i++)
		ret[i] = (a[0] * m[i][0]) + (a[1] * m[i][1]) + (a[2] * m[i][2]) + (a[3] * m[i][3]);

	memcpy(a, ret, sizeof(mat44));
}

void mat44_transpose(mat44 m)
{
	int i, j;
	mat44 ret;

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			ret[i][j] = m[j][i];

	memcpy(m, ret, sizeof(mat44));
}

void mat44_rotx(mat44 m, float angle)
{
	mat44 rot = {};
	float co = cosf(deg2radf(angle));
	float si = sinf(deg2radf(angle));

	rot[0][0] = 1.0f;

	rot[1][1] = co;
	rot[2][1] = si;

	rot[1][2] = -si;
	rot[2][2] = co;

	rot[3][3] = 1.0f;

	mat44_mul(m, rot);
}

void mat44_roty(mat44 mat, float angle)
{
	mat44 rot = {};
	float co = cosf(deg2radf(angle));
	float si = sinf(deg2radf(angle));

	rot[0][0] = co;
	rot[2][0] = -si;

	rot[1][1] = 1.0f;

	rot[0][2] = si;
	rot[2][2] = co;

	rot[3][3] = 1.0f;

	mat44_mul(mat, rot);
}

void mat44_rotz(mat44 mat, float angle)
{
	mat44 rot = {};
	float co = cosf(deg2radf(angle));
	float si = sinf(deg2radf(angle));

	rot[0][0] = co;
	rot[1][0] = si;

	rot[0][1] = -si;
	rot[1][1] = co;

	rot[2][2] = 1.0f;

	rot[3][3] = 1.0f;

	mat44_mul(mat, rot);
}

void mat44_scale(mat44 m, float sx, float sy, float sz)
{
	mat44 scale = {};

	scale[0][0] = sx;
	scale[1][1] = sy;
	scale[2][2] = sz;
	scale[3][3] = 1.0f;

	mat44_mul(m, scale);
}

void mat44_translate(mat44 m, float x, float y, float z)
{
	mat44 trans;

	mat44_identity(trans);

	trans[0][3] = x;
	trans[1][3] = y;
	trans[2][3] = z;

	mat44_mul(m, trans);
}

void mat44_dbg(mat44 m)
{
	int i, j;

	for (i = 0; i < 4; i++) {
		for (j = 0; j < 4; j++)
			printf("%f ", m[i][j]);
		printf("\n");
	}
}
