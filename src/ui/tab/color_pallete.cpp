#include "color_pallete.h"

#include <string>

#include "../../graphic_types/graphic_types.h"
#include "../../graphic/draw_rect.h"
#include "../../input.h"

#include "../../basic_math.h"
#include "../../pos_convert.h"

namespace {

int get_index(int page, int row, int column) {
	return page *
	       COLOR_PALLETE_NUM_ROW *
	       COLOR_PALLETE_NUM_COLUMN +
	       row *
	       COLOR_PALLETE_NUM_COLUMN +
	       column;
}

}

ColorPallete color_pallete_new(Vec2 pos) {
	ColorPallete color_pallete;
	color_pallete.pos = pos;

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		color_pallete.page_btn_list[i] = btn_new(
			vec2_new(
				(COLOR_PALLETE_COLOR_CLICK_SZ + COLOR_PALLETE_SPACING * 2) *
					COLOR_PALLETE_NUM_ROW,
				i * COLOR_PALLETE_PAGE_BTN_SZ.y
			),
			COLOR_PALLETE_PAGE_BTN_SZ,
			color_new(0.2, 0.2, 0.2, 1),
			std::to_string(i)
		);
	}

	return color_pallete;
}

void color_pallete_update(ColorPallete &color_pallete,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	color_pallete.selection_changed = false;

	Vec2 cp_pos = vec2_add(parent_pos, color_pallete.pos);

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		btn_update(color_pallete.page_btn_list[i], gs, input, cp_pos, show);
	}

	if (!show) {
		return;
	}

	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		const Btn &btn = color_pallete.page_btn_list[i];
		if (btn.clicked) {
			color_pallete.at_page = i;

			int row = color_pallete.selected_index / COLOR_PALLETE_NUM_COLUMN %
				COLOR_PALLETE_NUM_ROW;
			int column = color_pallete.selected_index %
				COLOR_PALLETE_NUM_COLUMN;
			color_pallete.selected_index = get_index(i, row, column);
			color_pallete.selection_changed = true;
		}
	}


	if (!in_rect(mouse_pos, vec2_new(0, 0), main_fb_sz)) {
		return;
	}

	if (!input.left_click) {
		return;
	}

	for (int row = 0; row < COLOR_PALLETE_NUM_ROW; row++) {
	for (int column = 0; column < COLOR_PALLETE_NUM_COLUMN; column++) {
		int index = get_index(color_pallete.at_page, row, column);
		
		if (in_rect(
			mouse_pos,
			vec2_new(
				cp_pos.x + COLOR_PALLETE_COLOR_CLICK_SZ * row,
				cp_pos.y + COLOR_PALLETE_COLOR_CLICK_SZ * column
			),
			vec2_new(COLOR_PALLETE_COLOR_CLICK_SZ,COLOR_PALLETE_COLOR_CLICK_SZ)
		)) {
			color_pallete.selected_index = index;
			color_pallete.selection_changed = true;
			return;
		}
	}
	}
}

void color_pallete_draw(const ColorPallete &color_pallete,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 cp_pos = vec2_add(parent_pos, color_pallete.pos);
	float click_sz = COLOR_PALLETE_COLOR_CLICK_SZ;

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		btn_draw(color_pallete.page_btn_list[i], gs,
		         cp_pos, i == color_pallete.at_page);
	}

	for (int row = 0; row < COLOR_PALLETE_NUM_ROW; row++) {
	for (int column = 0; column < COLOR_PALLETE_NUM_COLUMN; column++) {
		int index = get_index(color_pallete.at_page, row, column);

		draw_rect(
			gs,
			vec2_new(
				cp_pos.x + row * click_sz + COLOR_PALLETE_SPACING,
				cp_pos.y + column * click_sz + COLOR_PALLETE_SPACING
			),
			vec2_new(
				click_sz - 2 * COLOR_PALLETE_SPACING,
				click_sz - 2 * COLOR_PALLETE_SPACING
			),
			color_pallete.color_list[index]
		);
	}
	}

	int row = color_pallete.selected_index %
		(COLOR_PALLETE_NUM_ROW * COLOR_PALLETE_NUM_COLUMN) /
		COLOR_PALLETE_NUM_COLUMN;
	int column = color_pallete.selected_index %
		(COLOR_PALLETE_NUM_ROW * COLOR_PALLETE_NUM_COLUMN) %
		COLOR_PALLETE_NUM_COLUMN;

	draw_rect_border(
		gs,
		vec2_new(
			cp_pos.x + row * click_sz - COLOR_PALLETE_SPACING,
			cp_pos.y + column * click_sz - COLOR_PALLETE_SPACING
		),
		vec2_new(
			click_sz + COLOR_PALLETE_SPACING * 2,
			click_sz + COLOR_PALLETE_SPACING * 2
		),
		color_new(0, 0, 0, 1),
		COLOR_PALLETE_SPACING * 2
	);
}
