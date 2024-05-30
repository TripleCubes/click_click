#include "color.h"

namespace {

Color cap(Color color) {
	if (color.r < 0) {
		color.r = 0;
	}
	if (color.r > 1) {
		color.r = 1;
	}

	if (color.g < 0) {
		color.g = 0;
	}
	if (color.g > 1) {
		color.g = 1;
	}
	
	if (color.b < 0) {
		color.b = 0;
	}
	if (color.b > 1) {
		color.b = 1;
	}

	if (color.a < 0) {
		color.a = 0;
	}
	if (color.a > 1) {
		color.a = 1;
	}
	return color;
}

}

Color color_new(float r, float g, float b, float a) {
	Color color;
	color.r = r;
	color.g = g;
	color.b = b;
	color.a = a;
	return color;
}

Color color_add(Color color_a, Color color_b) {
	Color result;
	result.r = color_a.r + color_b.r;
	result.g = color_a.g + color_b.g;
	result.b = color_a.b + color_b.b;
	result.a = color_a.a + color_b.a;
	result = cap(result);
	return result;
}

Color color_sub(Color color_a, Color color_b) {
	Color result;
	result.r = color_a.r - color_b.r;
	result.g = color_a.g - color_b.g;
	result.b = color_a.b - color_b.b;
	result.a = color_a.a - color_b.a;
	result = cap(result);
	return result;
}
