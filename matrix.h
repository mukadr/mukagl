#ifndef MUKAGL_MATRIX_H
#define MUKAGL_MATRIX_H

#include "vector.h"

typedef float mat44[4][4];

void mat44_identity(mat44 m);
void mat44_mul(mat44 m, mat44 n);
void mat44_transpose(mat44 m);
void mat44_rotx(mat44 m, float angle);
void mat44_roty(mat44 m, float angle);
void mat44_rotz(mat44 m, float angle);
void mat44_scale(mat44 m, float sx, float sy, float sz);
void mat44_translate(mat44 m, float x, float y, float z);
void mat44_dbg(mat44 m);

void vec4_mul_mat44(vec4 a, mat44 m);

#endif /* MUKAGL_MATRIX_H */
