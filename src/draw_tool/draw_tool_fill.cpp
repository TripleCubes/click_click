#include "draw_tool_fill.h"

#include <array>
#include <vector>
#include "../types/vec2i.h"
#include "../tab/select.h"
#include "draw_tool_px.h"

namespace {

std::vector<Vec2i> queue;

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

void fill(std::vector<unsigned char> &data, const Selection &selection,
Vec2i data_sz, Vec2i pos,
unsigned char replace_index, unsigned char pallete_index) {
	queue.clear();
	int i = 0;
	queue.push_back(pos);
	draw_tool_px(data, selection, data_sz, pos, pallete_index, 0);

	while (i < (int)queue.size()) {
		Vec2i current_pos = queue[i];

		for (int j = 0; j < (int)dirs.size(); j++) {
			Vec2i next_pos = vec2i_add(current_pos, dirs[j]);

			if (next_pos.x < 0 || next_pos.y < 0
			|| next_pos.x >= data_sz.x || next_pos.y >= data_sz.y) {
				continue;
			}
			
			int index = next_pos.y * data_sz.y + next_pos.x;
			bool selection_cond = selection.full_preview_list.size() == 0
				|| selection.map[index] == 1;

			if (!selection_cond) {
				continue;
			}

			unsigned char px = get_px(data, data_sz, next_pos);

			if (px != replace_index) {
				continue;
			}

			draw_tool_px(data, selection, data_sz, next_pos,
				pallete_index, 0);
			queue.push_back(next_pos);
		}

		i++;
	}
}

}

void draw_tool_fill(std::vector<unsigned char> &data, 
const Selection &selection, Vec2i data_sz,
Vec2i pos, unsigned char pallete_index) {
	if (pos.x < 0 || pos.y < 0 || pos.x >= data_sz.x || pos.y >= data_sz.y) {
		return;
	}

	unsigned char px = get_px(data, data_sz, pos);
	if (px == pallete_index) {
		return;
	}

	fill(data, selection, data_sz, pos, px, pallete_index);
}
