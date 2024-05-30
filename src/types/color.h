#ifndef TYPES__COLOR_H
#define TYPES__COLOR_H

struct Color {
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
};

Color color_new(float r, float g, float b, float a);
Color color_add(Color color_a, Color color_b);
Color color_sub(Color color_a, Color color_b);

#endif
