#include "vec2.h"

#include "vec2i.h"

Vec2 vec2_new(float x, float y) {
	Vec2 vec;
	vec.x = x;
	vec.y = y;
	return vec;
}

Vec2 vec2_add(Vec2 vec_a, Vec2 vec_b) {
	return vec2_new(vec_a.x + vec_b.x, vec_a.y + vec_b.y);
}

Vec2 vec2_sub(Vec2 vec_a, Vec2 vec_b) {
	return vec2_new(vec_a.x - vec_b.x, vec_a.y - vec_b.y);
}

Vec2 vec2_mul(Vec2 vec, float f) {
	return vec2_new(vec.x * f, vec.y * f);
}

Vec2 vec2_div(Vec2 vec, float f) {
	return vec2_new(vec.x / f, vec.y / f);
}

Vec2 to_vec2(Vec2i veci) {
	return vec2_new(veci.x, veci.y);
}
