#include "draw_tool_px.h"

#include "../types/vec2i.h"

void draw_tool_px(std::vector<unsigned char> &data, Vec2i data_sz, Vec2i pos,
unsigned char pallete_index) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= data_sz.x || pos.y >= data_sz.y) {
		return;
	}

	int data_index = data_sz.y * pos.y + pos.x;
	data[data_index] = pallete_index;
}
