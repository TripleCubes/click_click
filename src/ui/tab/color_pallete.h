#ifndef UI__TAB__COLOR_PALLETE_H
#define UI__TAB__COLOR_PALLETE_H

#include <array>
#include "../../types/vec2.h"
#include "../../types/color.h"
#include "../btn.h"

const int COLOR_PALLETE_NUM_COLUMN = 8;
const int COLOR_PALLETE_NUM_ROW = 2;
const int COLOR_PALLETE_NUM_PAGE = 4;
const float COLOR_PALLETE_SPACING = 1;
const float COLOR_PALLETE_COLOR_CLICK_SZ = 10;
const Vec2 COLOR_PALLETE_PAGE_BTN_SZ = vec2_new(12, 12);
const Vec2 COLOR_PALLETE_SZ = vec2_new(
	COLOR_PALLETE_NUM_ROW * COLOR_PALLETE_COLOR_CLICK_SZ
	+ COLOR_PALLETE_PAGE_BTN_SZ.x + 4,
	COLOR_PALLETE_NUM_COLUMN * COLOR_PALLETE_COLOR_CLICK_SZ
);

struct GraphicStuff;
struct Input;

struct ColorPallete {
	Vec2 pos;
	std::array<Color, COLOR_PALLETE_NUM_COLUMN *
	                  COLOR_PALLETE_NUM_ROW *
	                  COLOR_PALLETE_NUM_PAGE> color_list;
	int selected_index = 1;
	bool selection_changed = false;
	int at_page = 0;

	std::array<Btn, COLOR_PALLETE_NUM_PAGE> page_btn_list;
};

ColorPallete color_pallete_new(Vec2 pos);
void color_pallete_update(ColorPallete &color_pallete,
	const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show);
void color_pallete_draw(const ColorPallete &color_pallete,
	GraphicStuff &gs, Vec2 parent_pos);

#endif
