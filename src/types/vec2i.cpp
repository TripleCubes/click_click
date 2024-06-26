#include "vec2i.h"

#include "vec2.h"
#include <cmath>

Vec2i vec2i_new(int x, int y) {
	Vec2i veci;
	veci.x = x;
	veci.y = y;
	return veci;
}

Vec2i to_vec2i(Vec2 vec) {
	return vec2i_new(std::floor(vec.x), std::floor(vec.y));
}

bool vec2i_equals(Vec2i vec_a, Vec2i vec_b) {
	return vec_a.x == vec_b.x && vec_a.y == vec_b.y;
}

Vec2i vec2i_add(Vec2i vec_a, Vec2i vec_b) {
	return vec2i_new(vec_a.x + vec_b.x, vec_a.y + vec_b.y);
}

Vec2i vec2i_sub(Vec2i vec_a, Vec2i vec_b) {
	return vec2i_new(vec_a.x - vec_b.x, vec_a.y - vec_b.y);
}

Vec2i vec2i_div_div(Vec2i vec, int num) {
	return vec2i_new(vec.x/num, vec.y/num);
}

Vec2i vec2i_mul(Vec2i vec, int num) {
	return vec2i_new(vec.x*num, vec.y*num);
}
