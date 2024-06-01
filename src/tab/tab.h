#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../ui/color_picker.h"
#include "../ui/color_pallete.h"

struct Color;
struct GraphicStuff;
struct Input;

struct Tab {
	bool running = true;
	std::vector<unsigned char> data;
	Vec2 pos;
	Vec2i sz;
	int px_scale;

	ColorPicker color_picker;
	ColorPallete color_pallete;

	int texture_index = 0;
};

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
	Vec2 pos, Vec2i sz, int px_scale);
void px(Tab &tab, Vec2i pos, Color color);
void tab_update(Tab &tab, GraphicStuff &gs, const Input &input);
void tab_draw(const Tab &tab, const GraphicStuff &gs, const Input &input);
void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index);

#endif
