#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../ui/color_picker.h"

struct Color;

struct Tab {
	bool running = true;
	std::vector<unsigned char> data;
	Vec2 pos;
	Vec2i sz;
	int px_scale;

	ColorPicker color_picker;
};

int tab_new(std::vector<Tab> &tab_list, Vec2 pos, Vec2i sz, int px_scale);
void px(Tab &tab, Vec2i pos, Color color);
void tab_close(std::vector<Tab> &tab_list, int index);

#endif
