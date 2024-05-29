#include "tab.h"

#include "../types/vec2.h"
#include "../types/color.h"

void px(Tab &tab, Vec2i pos, Color color) {
	int index = tab.sz.y * pos.y * 4 + pos.x * 4;
	tab.data[index    ] = color.r * 255;
	tab.data[index + 1] = color.g * 255;
	tab.data[index + 2] = color.b * 255;
	tab.data[index + 3] = color.a * 255;
}
