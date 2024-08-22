#ifndef DRAW_TOOL__DRAW_TOOL_FILL_H
#define DRAW_TOOL__DRAW_TOOL_FILL_H

#include <vector>

struct Vec2i;

void draw_tool_fill(std::vector<unsigned char> &data, Vec2i data_sz,
	Vec2i pos, unsigned char pallete_index);

#endif
