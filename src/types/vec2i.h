#ifndef TYPES__VEC2I_H
#define TYPES__VEC2I_H

struct Vec2;

struct Vec2i {
	int x = 0;
	int y = 0;
};

Vec2i vec2i_new(int x, int y);
Vec2i to_vec2i(Vec2 vec);

bool vec2i_equals(Vec2i vec_a, Vec2i vec_b);
Vec2i vec2i_add(Vec2i vec_a, Vec2i vec_b);
Vec2i vec2i_sub(Vec2i vec_a, Vec2i vec_b);
Vec2i vec2i_div_div(Vec2i vec, int num);
Vec2i vec2i_mul(Vec2i vec, int num);

#endif
