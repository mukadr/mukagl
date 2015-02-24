// Usage of point_in_triangle_2i to perform triangle rasterization
#include <stdio.h>

#include "../math.h"

#define min(a, b) (a < b ? a : b)
#define max(a, b) (a > b ? a : b)

void raster_test(int ax, int ay, int bx, int by, int cx, int cy)
{
	int minx = min(ax, min(bx, cx));
	int maxx = max(ax, max(bx, cx));
	int miny = min(ay, min(by, cy));
	int maxy = max(ay, max(by, cy));
	int x, y;

	printf("[%d,%d] [%d,%d] [%d,%d]\n", ax, ay, bx, by, cx, cy);
	for (y = maxy; y >= miny; y--) {
		for (x = minx; x <= maxx; x++) {
			if (point_in_triangle_2i(x, y, ax, ay, bx, by, cx, cy))
				printf(".");
			else
				printf("*");
		}
		printf("\n");
	}
}

int main()
{
	raster_test(10, 10, 60, 70, 40, -10);
	return 0;
}
