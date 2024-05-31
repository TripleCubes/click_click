#ifndef TYPES__COLOR_H
#define TYPES__COLOR_H

struct Color {
	float r = 1;
	float g = 1;
	float b = 1;
	float a = 1;
};

Color color_new(float r, float g, float b, float a);
Color color_add(Color color_a, Color color_b);
Color color_sub(Color color_a, Color color_b);
Color hue_to_rgb(float hue);
Color hsv_to_rgb(Color hsv);

#endif
