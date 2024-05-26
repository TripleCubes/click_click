#include "vec2i.h"

#include "vec2.h"

Vec2i vec2i_new(int x, int y) {
	Vec2i veci;
	veci.x = x;
	veci.y = y;
	return veci;
}

Vec2i to_vec2i(Vec2 vec) {
	return vec2i_new(vec.x, vec.y);
}
