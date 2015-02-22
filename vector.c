#include <stdio.h>
#include <string.h>

#include "vector.h"

float vec3_normalize(vec3 a)
{
	float mag = vec3_mag(a);

	if (mag == 0.0f) {
		vec3_zero(a);
		return 0.0f;
	}
	vec3_div(a, mag);
	return mag;
}

void vec3_cross(vec3 r, const vec3 a, const vec3 b)
{
	vec3 ret;

	ret[0] = (a[1] * b[2]) - (a[2] * b[1]);
	ret[1] = (a[2] * b[0]) - (a[0] * b[2]);
	ret[2] = (a[0] * b[1]) - (a[1] * b[0]);

	memcpy(r, ret, sizeof(ret));
}

void vec3_dbg(const vec3 a)
{
	fprintf(stdout, "x:%f y:%f z:%f\n", a[0], a[1], a[2]);
}
