#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../ui/tab/color_picker.h"
#include "../ui/tab/color_pallete.h"
#include "layer.h"

struct Color;
struct GraphicStuff;
struct Input;

struct Tab {
	bool running = true;
	Vec2 pos;
	Vec2i sz;
	int px_scale = 1;

	ColorPicker color_picker;
	ColorPallete color_pallete;

	std::vector<unsigned char> pallete_data;
	int pallete_texture_index = 0;

	std::vector<Layer> layer_list;
	std::vector<int> layer_order_list;
	int layer_order_list_index = 0;
};

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
	Vec2 pos, Vec2i sz, int px_scale);
void tab_update(Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos, bool show);
void tab_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos);
void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index);

#endif
