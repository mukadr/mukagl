#include "math.h"

// Cross Product between two 2d vectors.
// Note: The X and Y components of a cross product between two vectors
// with Z=0 will result in 0, so we represent the result as an scalar (Z value).
static inline int cross_2i(int ax, int ay, int bx, int by)
{
	return ax*by - ay*bx;
}

// Cross Product between ab and ap
static inline int side_check(int px, int py, int ax, int ay, int bx, int by)
{
	return cross_2i(ax-bx, ay-by,  // ab
			ax-px, ay-py); // ap
}

// Special case when the point lies in the border of the triangle
static inline int __point_in_triangle_2i_special(int v1, int v2)
{
	// if the point lies exactly in one of the triangle vertexes, we are done
	if (v1 == 0 || v2 == 0)
		return 1;
	return (v1 < 0) == (v2 < 0);
}

// Verify if point is inside 2d triangle by checking the cross products
// between side-side and side-point. Sign of resulting Z must be the same in
// all tests (the point must be equally to the left or to the right on all sides)
// However, if the point lies in one (or two) of the sides (border),
// Z will be 0 (same angle). This is treated as a special case.
int point_in_triangle_2i(int px, int py, int ax, int ay,
			 int bx, int by, int cx, int cy)
{
	int ab = side_check(px, py, ax, ay, bx, by);
	int bc = side_check(px, py, bx, by, cx, cy);
	int ca = side_check(px, py, cx, cy, ax, ay);

	if (ab == 0)
		return __point_in_triangle_2i_special(bc, ca);
	if (bc == 0)
		return __point_in_triangle_2i_special(ab, ca);
	if (ca == 0)
		return __point_in_triangle_2i_special(ab, bc);

	return ((ab < 0) == (bc < 0)) && ((ab < 0) == (ca < 0));
}
