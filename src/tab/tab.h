#ifndef TAB__TAB_H
#define TAB__TAB_H

#include <vector>
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../ui/tab/color_picker.h"
#include "../ui/tab/color_pallete.h"
#include "../ui/tab/layer_bar.h"
#include "layer.h"

struct Color;
struct GraphicStuff;
struct Input;
struct GameTime;

struct Tab {
	bool running = true;
	Vec2 pos;
	Vec2i sz;
	int px_scale = 1;

	Vec2 tex_draw_tag_pos;
	int pen_check_count = 0;

	ColorPicker color_picker;
	ColorPallete color_pallete;
	LayerBar layer_bar;

	std::vector<unsigned char> pallete_data;
	int pallete_texture_index = 0;
	std::vector<unsigned char> tool_preview_pallete_data;
	int tool_preview_pallete_texture_index = 0;

	std::vector<unsigned char> tool_preview_data;
	int tool_preview_texture_index = 0;

	std::vector<Layer> layer_list;
	std::vector<int> layer_order_list;
	int layer_order_list_index = 0;
};

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
	Vec2 pos, Vec2i sz, int px_scale);
void tab_update(Tab &tab, GraphicStuff &gs, const Input &input,
	const GameTime &game_time, Vec2 parent_pos, bool show);
void tab_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos);
void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index);

#endif
