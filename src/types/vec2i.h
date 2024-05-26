#ifndef TYPES__VEC2I_H
#define TYPES__VEC2I_H

struct Vec2;

struct Vec2i {
	int x = 0;
	int y = 0;
};

Vec2i vec2i_new(int x, int y);
Vec2i to_vec2i(Vec2 vec);

#endif
