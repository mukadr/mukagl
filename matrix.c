#include <math.h>
#include <stdio.h>
#include <string.h>

#include "matrix.h"
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

void vec4_mul_mat44(vec4 a, mat44 m)
{
	int i;
	vec4 ret;

	for (i = 0; i < 4; i++)
		ret[i] = (a[0] * m[0][i]) + (a[1] * m[1][i]) + (a[2] * m[2][i]) + (a[3] * m[3][i]);

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
	float co = cosf(M_PI/180 * angle);
	float si = sinf(M_PI/180 * angle);

	rot[0][0] = 1.0f;

	rot[1][1] = co;
	rot[1][2] = si;

	rot[2][1] = -si;
	rot[2][2] = co;

	rot[3][3] = 1.0f;

	mat44_mul(m, rot);
}

void mat44_roty(mat44 mat, float angle)
{
	mat44 rot = {};
	float co = cosf(M_PI/180 * angle);
	float si = sinf(M_PI/180 * angle);

	rot[0][0] = co;
	rot[0][2] = -si;

	rot[1][1] = 1.0f;

	rot[2][0] = si;
	rot[2][2] = co;

	rot[3][3] = 1.0f;

	mat44_mul(mat, rot);
}

void mat44_rotz(mat44 mat, float angle)
{
	mat44 rot = {};
	float co = cosf(M_PI/180 * angle);
	float si = sinf(M_PI/180 * angle);

	rot[0][0] = co;
	rot[0][1] = si;

	rot[1][0] = -si;
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

	trans[3][0] = x;
	trans[3][1] = y;
	trans[3][2] = z;

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
