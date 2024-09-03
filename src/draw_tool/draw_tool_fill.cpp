#include "draw_tool_fill.h"

#include <array>
#include "../types/vec2i.h"
#include "draw_tool_px.h"

namespace {

const std::array<Vec2i, 4> dirs = {
	vec2i_new(0, -1),
	vec2i_new(0,  1),
	vec2i_new(-1, 0),
	vec2i_new( 1, 0),
};

unsigned char get_px(const std::vector<unsigned char> &data, Vec2i data_sz,
Vec2i pos) {
	int data_index = data_sz.y * pos.y + pos.x;
	return data[data_index];
}

void fill(std::vector<unsigned char> &data, Vec2i data_sz, Vec2i pos,
unsigned char replace_index, unsigned char pallete_index) {
	draw_tool_px(data, data_sz, pos, pallete_index, 0);

	for (int i = 0; i < (int)dirs.size(); i++) {
		Vec2i next_pos = vec2i_add(dirs[i], pos);
		
		unsigned char px = get_px(data, data_sz, next_pos);
		
		if (px == replace_index
		&& next_pos.x >= 0 && next_pos.y >= 0
		&& next_pos.x < data_sz.x && next_pos.y < data_sz.y) {
			fill(data, data_sz, next_pos, replace_index, pallete_index);
		}
	}
}

}

void draw_tool_fill(std::vector<unsigned char> &data, Vec2i data_sz,
Vec2i pos, unsigned char pallete_index) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= data_sz.x || pos.y >= data_sz.y) {
		return;
	}

	unsigned char px = get_px(data, data_sz, pos);
	if (px == pallete_index) {
		return;
	}

	fill(data, data_sz, pos, px, pallete_index);
}
