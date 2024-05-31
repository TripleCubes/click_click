#include "tab.h"

#include "../types/color.h"

namespace {

int get_blank_index(const std::vector<Tab> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int tab_new(std::vector<Tab> &tab_list, Vec2 pos, Vec2i sz, int px_scale) {
	int index = get_blank_index(tab_list);
	if (index == -1) {
		Tab new_tab;
		tab_list.push_back(new_tab);

		index = (int)tab_list.size() - 1;

		Tab &tab = tab_list[index];
		tab.color_picker = color_picker_new(vec2_new(50, 10));
		tab.color_pallete = color_pallete_new(vec2_new(300, 10));
	}
	Tab &tab = tab_list[index];

	tab.pos = pos;
	tab.sz = sz;
	tab.px_scale = px_scale;
	tab.data.resize(sz.x * sz.y * 4);

	return index;
}

void px(Tab &tab, Vec2i pos, Color color) {
	int index = tab.sz.y * pos.y * 4 + pos.x * 4;
	tab.data[index    ] = color.r * 255;
	tab.data[index + 1] = color.g * 255;
	tab.data[index + 2] = color.b * 255;
	tab.data[index + 3] = color.a * 255;
}

void tab_close(std::vector<Tab> &tab_list, int index) {
	Tab &tab = tab_list[index];
	tab.data.clear();
	tab.running = false;
}
