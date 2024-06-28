#include "basic_math.h"

#include "types/vec2.h"
#include <cmath>

float floor2(float a, float b) {
	return std::floor(a / b) * b;
}

float mod2(float a, float b) {
	return a - std::floor(a / b) * b;
}

bool in_rect(Vec2 pos, Vec2 rect_pos, Vec2 rect_sz) {
	if (pos.x < rect_pos.x) {
		return false;
	}

	if (pos.y < rect_pos.y) {
		return false;
	}

	if (pos.x >= rect_pos.x + rect_sz.x) {
		return false;
	}

	if (pos.y >= rect_pos.y + rect_sz.y) {
		return false;
	}

	return true;
}

float clampf(float f, float min, float max) {
	if (f < min) { f = min; }
	else if (f > max) { f = max; }

	return f;
}

float clampi(int n, int min, int max) {
	if (n < min) { n = min; }
	else if (n > max) { n = max; }

	return n;
}

float max3f(float a, float b, float c) {
	if (a >= b && a >= c) {
		return a;
	}
	if (b >= c && b >= a) {
		return b;
	}
	return c;
}
