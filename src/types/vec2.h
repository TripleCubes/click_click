#ifndef TYPES__VEC2_H
#define TYPES__VEC2_H

struct Vec2i;

struct Vec2 {
	float x = 0;
	float y = 0;
};

Vec2 to_vec2(Vec2i veci);

Vec2 vec2_new(float x, float y);
Vec2 vec2_add(Vec2 vec_a, Vec2 vec_b);
Vec2 vec2_sub(Vec2 vec_a, Vec2 vec_b);
Vec2 vec2_mul(Vec2 vec, float f);
Vec2 vec2_div(Vec2 vec, float f);
Vec2 vec2_floor(Vec2 vec);

float vec2_length(Vec2 vec);
Vec2 vec2_normalized(Vec2 vec);

#endif
