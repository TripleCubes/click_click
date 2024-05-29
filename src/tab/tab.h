#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"

struct Color;

struct Tab {
	std::vector<unsigned char> data;
	Vec2i sz;
	Vec2 pos;
	int px_scale;
};

void px(Tab &tab, Vec2i pos, Color color);

#endif
