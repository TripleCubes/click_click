#ifndef BASIC_MATH_H
#define BASIC_MATH_H

struct Vec2;

float floor2(float a, float b);
bool in_rect(Vec2 pos, Vec2 rect_pos, Vec2 rect_sz);
float clampf(float n, float min, float max);
float clampi(float n, float min, float max);

#endif
