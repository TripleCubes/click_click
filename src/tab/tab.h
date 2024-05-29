#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"

struct Color;

struct Tab {
	std::vector<unsigned char> data;
	Vec2i sz;
};

void px(Tab &tab, Vec2i pos, Color color);

#endif
