#ifndef UI__COLOR_PALLETE_H
#define UI__COLOR_PALLETE_H

#include <array>
#include "../types/vec2.h"
#include "../types/color.h"

const int COLOR_PALLETE_NUM_COLUMN = 8;
const int COLOR_PALLETE_NUM_ROW = 2;
const int COLOR_PALLETE_NUM_PAGE = 4;
const float COLOR_PALLETE_SPACING = 1;
const float COLOR_PALLETE_COLOR_CLICK_SZ = 10;

struct GraphicStuff;
struct Input;

struct ColorPallete {
	Vec2 pos;
	std::array<Color, COLOR_PALLETE_NUM_COLUMN *
	                  COLOR_PALLETE_NUM_ROW *
	                  COLOR_PALLETE_NUM_PAGE> color_list;
	int selected_index = 0;
	bool selection_changed = false;
	int at_page = 0;
};

ColorPallete color_pallete_new(Vec2 pos);
void color_pallete_update(ColorPallete &color_pallete,
	const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show);
void color_pallete_draw(const ColorPallete &color_pallete,
	const GraphicStuff &gs, Vec2 parent_pos);

#endif
