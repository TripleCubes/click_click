#include "color_pallete.h"

#include <string>

#include "../../graphic_types/graphic_types.h"
#include "../../graphic/draw_rect.h"
#include "../../graphic/draw_text.h"
#include "../../input.h"
#include "../../input_map.h"

#include "../../basic_math.h"
#include "../../pos_convert.h"

#include "../../consts.h"

namespace {

int get_index(int page, int row, int column) {
	return page *
	       COLOR_PALLETE_NUM_ROW *
	       COLOR_PALLETE_NUM_COLUMN +
	       row *
	       COLOR_PALLETE_NUM_COLUMN +
	       column;
}

void page_index_set(ColorPallete &color_pallete, int page, int index) {
	color_pallete.prev_selected_index = color_pallete.selected_index;
	color_pallete.prev_at_page = color_pallete.at_page;

	color_pallete.at_page = page;
	color_pallete.selected_index = index;
	color_pallete.selection_changed = true;
}

void mouse_update(ColorPallete &color_pallete,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	color_pallete.selection_changed = false;

	Vec2 cp_pos = vec2_add(parent_pos, color_pallete.pos);

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		btn_update(color_pallete.page_btn_list[i], gs, input, cp_pos, show);
	}

	if (!show) {
		return;
	}

	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		const Btn &btn = color_pallete.page_btn_list[i];
		if (btn.clicked) {
			int row = color_pallete.selected_index / COLOR_PALLETE_NUM_COLUMN %
				COLOR_PALLETE_NUM_ROW;
			int column = color_pallete.selected_index %
				COLOR_PALLETE_NUM_COLUMN;
			page_index_set(color_pallete, i, get_index(i, row, column));
		}
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
			page_index_set(color_pallete, color_pallete.at_page, index);
			return;
		}
	}
	}
}

void kb_update(ColorPallete &color_pallete, const Input &input, bool show) {
	if (!show) {
		return;
	}

	for (int i = MAP_COLOR_1; i <= MAP_COLOR_8; i++) {
		if (map_press(input, (MappedKeyIndex)i)) {
			int next_index = i - MAP_COLOR_1;
			if (next_index == color_pallete.selected_index) {
				page_index_set(color_pallete, color_pallete.at_page,
					next_index + 8);
			}
			else {
				page_index_set(color_pallete, color_pallete.at_page,
					next_index);
			}
		}
	}

	for (int i = MAP_COLOR_PAGE_1; i <= MAP_COLOR_PAGE_4; i++) {
		if (map_press(input, (MappedKeyIndex)i)) {
			int page = i - MAP_COLOR_PAGE_1;

			int row = color_pallete.selected_index / COLOR_PALLETE_NUM_COLUMN %
				COLOR_PALLETE_NUM_ROW;
			int column = color_pallete.selected_index %
				COLOR_PALLETE_NUM_COLUMN;

			page_index_set(color_pallete, page, get_index(page, row, column));
		}
	}
}

std::string get_key_str(const Input &input, MappedKeyIndex index) {
	int key = input.input_map.key_list[index].key;
	int modifier = input.input_map.key_list[index].modifier;

	bool shift = modifier == MODIFIER_LEFT_SHIFT
		|| modifier == MODIFIER_RIGHT_SHIFT
		|| modifier == MODIFIER_BOTH_SHIFT;

	return std::string(1, key_get_char(key, shift));
}

}

ColorPallete color_pallete_new(Vec2 pos) {
	ColorPallete color_pallete;
	color_pallete.pos = pos;

	color_pallete.color_list[0] = color_new(0, 0, 0, 0);

	for (int i = 0; i < (int)color_pallete.page_btn_list.size(); i++) {
		color_pallete.page_btn_list[i] = btn_new(
			vec2_new(
				-24,
				i * COLOR_PALLETE_PAGE_BTN_SZ.y
			),
			COLOR_PALLETE_PAGE_BTN_SZ,
			BTN_TEXTAREA_COLOR,
			std::to_string(i + 1)
		);
	}

	return color_pallete;
}

void color_pallete_update(ColorPallete &color_pallete,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool kb_allowed,
bool show) {
	mouse_update(color_pallete, gs, input, parent_pos, show);
	
	if (kb_allowed) {
		kb_update(color_pallete, input, show);
	}
}

void color_pallete_draw(const ColorPallete &color_pallete,
GraphicStuff &gs, const Input &input, Vec2 parent_pos) {
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
		BTN_TEXTAREA_COLOR,
		COLOR_PALLETE_SPACING * 2
	);

	for (int i = 1; i <= COLOR_PALLETE_NUM_COLUMN; i++) {
		draw_text(
			gs,
			get_key_str(input, (MappedKeyIndex)(MAP_COLOR_1 + i - 1)),
			vec2_new(
				cp_pos.x - 8,
				cp_pos.y + (COLOR_PALLETE_COLOR_CLICK_SZ) * (i - 1) + 2
			),
			12,
			1,
			KEY_HINT_COLOR,
			vec2_new(0, 0),
			false
		);
	}
}

void color_pallete_toggle_eraser(ColorPallete &color_pallete) {
	if (color_pallete.at_page != 0 || color_pallete.selected_index != 0) {
		page_index_set(color_pallete, 0, 0);
	}
	else {
		page_index_set(color_pallete, color_pallete.prev_at_page,
			color_pallete.prev_selected_index);
	}
}
