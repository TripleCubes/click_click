#include "color.h"

Color color_new(float r, float g, float b, float a) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}
