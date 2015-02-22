#ifndef MUKAGL_VECTOR_H
#define MUKAGL_VECTOR_H

#include <math.h>

typedef float vec3[3];
typedef float vec4[4];

static inline void vec3_zero(vec3 a)
{
	a[0] = 0.0f;
	a[1] = 0.0f;
	a[2] = 0.0f;
}

static inline int vec3_eq(const vec3 a, const vec3 b)
{
	return a[0] == b[0] &&
	       a[1] == b[1] &&
	       a[2] == b[2];
}

static inline void vec3_add(vec3 a, const vec3 b)
{
	a[0] += b[0];
	a[1] += b[1];
	a[2] += b[2];
}

static inline void vec3_sub(vec3 a, const vec3 b)
{
	a[0] -= b[0];
	a[1] -= b[1];
	a[2] -= b[2];
}

static inline void vec3_mul(vec3 a, float f)
{
	a[0] *= f;
	a[1] *= f;
	a[2] *= f;
}

static inline void vec3_div(vec3 a, float f)
{
	a[0] /= f;
	a[1] /= f;
	a[2] /= f;
}

static inline void vec3_neg(vec3 a)
{
	a[0] = -a[0];
	a[1] = -a[1];
	a[2] = -a[2];
}

static inline float vec3_mag(const vec3 a)
{
	return sqrtf((a[0] * a[0]) +
		     (a[1] * a[1]) +
		     (a[2] * a[2]));
}

static inline float vec3_dot(const vec3 a, const vec3 b)
{
	return a[0] * b[0] +
	       a[1] * b[1] +
	       a[2] * b[2];
}

float	vec3_normal(vec3 a);
void	vec3_cross(vec3 r, const vec3 a, const vec3 b);
void	vec3_dbg(const vec3 a);

#endif /* MUKAGL_VECTOR_H */
