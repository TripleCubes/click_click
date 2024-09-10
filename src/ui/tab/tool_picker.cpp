#include "tool_picker.h"

#include "../../consts.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../input_map.h"
#include "../../graphic/draw_text.h"

namespace {

void main_btns_update(ToolPicker &tool_picker, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool tool_key_allowed, bool show) {
	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);

	for (int i = 0; i < NUM_BTN; i++) {
		Btn &btn = tool_picker.btn_list[i];
		btn_update(btn, gs, input, pos, show);
	}

	for (int i = 0; i < NUM_BTN; i++) {
		const Btn &btn = tool_picker.btn_list[i];

		if (btn.clicked) {
			tool_picker.selected_index = i;
		}
	}

	if (tool_key_allowed) {
		if (map_press(input, MAP_TOOL_BRUSH)) {
			tool_picker.selected_index = 0;
		}
		else if (map_press(input, MAP_TOOL_CURVE)) {
			tool_picker.selected_index = 1;
		}
		else if (map_press(input, MAP_TOOL_FILL)) {
			tool_picker.selected_index = 2;
		}
		else if (map_press(input, MAP_TOOL_SELECT)) {
			tool_picker.selected_index = 3;
		}
	}
}

void other_btns_update(ToolPicker &tool_picker, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool tool_key_allowed, bool show) {
	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);
	
	bool cond_0 = show
	&& (tool_picker.selected_index == 0 || tool_picker.selected_index == 1);
	bool cond_1 = show && tool_picker.selected_index == 2;
	bool cond_2 = show && tool_picker.selected_index == 3;
	bool cond_3 = show && tool_picker.selected_index == 3;

	for (int i = 0; i < NUM_SZ_BTN; i++) {
		btn_update(tool_picker.sz_btn_list[i], gs, input, pos, cond_0);
	}

	for (int i = 0; i < NUM_FILL_BTN; i++) {
		btn_update(tool_picker.fill_btn_list[i], gs, input, pos, cond_1);
	}
	
	for (int i = 0; i < NUM_SELECT_BTN; i++) {
		btn_update(tool_picker.select_btn_list[i], gs, input, pos, cond_2);
	}
	
	for (int i = 0; i < NUM_SELECT_SECOND_BTN; i++) {
		btn_update(tool_picker.select_second_btn_list[i], gs,input,pos,cond_3);
	}


	if (!show) { return; }

	if (tool_picker.selected_index == TOOL_BRUSH) {
		for (int i = 0; i < NUM_SZ_BTN; i++) {
			if (tool_picker.sz_btn_list[i].clicked) {
				tool_picker.brush_selected_index = i;
			}
		}

		if (tool_key_allowed) {
			if (map_press(input, MAP_TOOL_BRUSH)) {
				tool_picker.brush_selected_index += 1;
				if (tool_picker.brush_selected_index >= NUM_SZ_BTN) {
					tool_picker.brush_selected_index = 0;
				}
			}
		}
	}

	if (tool_picker.selected_index == TOOL_CURVE) {
		for (int i = 0; i < NUM_SZ_BTN; i++) {
			if (tool_picker.sz_btn_list[i].clicked) {
				tool_picker.curve_selected_index = i;
			}
		}

		if (tool_key_allowed) {
			if (map_press(input, MAP_TOOL_CURVE)) {
				tool_picker.curve_selected_index += 1;
				if (tool_picker.curve_selected_index >= NUM_SZ_BTN) {
					tool_picker.curve_selected_index = 0;
				}
			}
		}
	}

	if (tool_picker.selected_index == TOOL_FILL) {
		for (int i = 0; i < NUM_FILL_BTN; i++) {
			if (tool_picker.fill_btn_list[i].clicked) {
				tool_picker.fill_selected_index = i;
			}
		}

		if (tool_key_allowed) {
			if (map_press(input, MAP_TOOL_FILL)) {
				tool_picker.fill_selected_index += 1;
				if (tool_picker.fill_selected_index >= NUM_FILL_BTN) {
					tool_picker.fill_selected_index = 0;
				}
			}
		}
	}
	
	if (tool_picker.selected_index == TOOL_SELECT) {
		for (int i = 0; i < NUM_SELECT_BTN; i++) {
			if (tool_picker.select_btn_list[i].clicked) {
				tool_picker.select_selected_index = i;
			}
		}

		for (int i = 0; i < NUM_SELECT_SECOND_BTN; i++) {
			if (tool_picker.select_second_btn_list[i].clicked) {
				tool_picker.select_second_selected_index = i;
			}
		}

		if (tool_key_allowed) {
			if (map_press(input, MAP_TOOL_SELECT)) {
				tool_picker.select_selected_index += 1;
				if (tool_picker.select_selected_index >= NUM_SELECT_BTN) {
					tool_picker.select_selected_index = 0;
				}
			}

			if (map_press(input, MAP_TOOL_SELECT_MODE)) {
				tool_picker.select_second_selected_index += 1;
				if (tool_picker.select_second_selected_index
				>= NUM_SELECT_SECOND_BTN) {
					tool_picker.select_second_selected_index = 0;
				}
			}
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

void main_btns_draw(const ToolPicker &tool_picker, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {

	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);

	for (int i = 0; i < NUM_BTN; i++) {
		const Btn &btn = tool_picker.btn_list[i];
		btn_draw(btn, gs, pos, i == tool_picker.selected_index);
	}

	std::array<std::string, NUM_BTN> tool_key_list = {
		get_key_str(input, MAP_TOOL_BRUSH),
		get_key_str(input, MAP_TOOL_CURVE),
		get_key_str(input, MAP_TOOL_FILL),
		get_key_str(input, MAP_TOOL_SELECT),
	};
	for (int i = 0; i < NUM_BTN; i++) {
		draw_text(
			gs,
			tool_key_list[i],
			vec2_add(pos, vec2_new(i * 13 + 4, 2)),
			12,
			1,
			KEY_HINT_COLOR,
			vec2_new(0, 0),
			false
		);
	}
}

void other_btns_draw(const ToolPicker &tool_picker, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);
	std::string tool_key_str;

	if (tool_picker.selected_index == 0 || tool_picker.selected_index == 1) {
		for (int i = 0; i < NUM_SZ_BTN; i++) {
			bool cond = false;
			if (tool_picker.selected_index == 0) {
				cond = tool_picker.brush_selected_index == i;
			}
			else {
				cond = tool_picker.curve_selected_index == i;
			}

			btn_draw(tool_picker.sz_btn_list[i], gs, pos, cond);
		}
	}

	if (tool_picker.selected_index == 2) {
		for (int i = 0; i < NUM_FILL_BTN; i++) {
			btn_draw(tool_picker.fill_btn_list[i], gs, pos,
				tool_picker.fill_selected_index == i);
		}
	}
	
	if (tool_picker.selected_index == 3) {
		for (int i = 0; i < NUM_SELECT_BTN; i++) {
			btn_draw(tool_picker.select_btn_list[i], gs, pos,
				tool_picker.select_selected_index == i);
		}
	}
	
	if (tool_picker.selected_index == 3) {
		for (int i = 0; i < NUM_SELECT_SECOND_BTN; i++) {
			btn_draw(tool_picker.select_second_btn_list[i], gs, pos,
				tool_picker.select_second_selected_index == i);
		}
	}

	if (tool_picker.selected_index == 0) {
		tool_key_str = get_key_str(input, MAP_TOOL_BRUSH);
	}
	else if (tool_picker.selected_index == 1) {
		tool_key_str = get_key_str(input, MAP_TOOL_CURVE);
	}
	else if (tool_picker.selected_index == 2) {
		tool_key_str = get_key_str(input, MAP_TOOL_FILL);
	}
	else if (tool_picker.selected_index == 3) {
		tool_key_str = get_key_str(input, MAP_TOOL_SELECT);
	}
	draw_text(
		gs,
		tool_key_str,
		vec2_add(pos, vec2_new(4, 13 * 2 + 1)),
		12,
		1,
		KEY_HINT_COLOR,
		vec2_new(0, 0),
		false
	);

	if (tool_picker.selected_index == 3) {
		draw_text(
			gs,
			get_key_str(input, MAP_TOOL_SELECT_MODE),
			vec2_add(pos, vec2_new(4 + 13 * 4, 13 * 2 + 1)),
			12,
			1,
			KEY_HINT_COLOR,
			vec2_new(0, 0),
			false
		);
	}
}

}

ToolPicker tool_picker_new(Vec2 pos) {
	ToolPicker tool_picker;

	tool_picker.pos = pos;

	std::array<std::string, NUM_BTN> icon_str_list = {
		"ICON_PEN", "ICON_CURVE", "ICON_FILL", "ICON_SELECT",
	};
	for (int i = 0; i < NUM_BTN; i++) {
		tool_picker.btn_list[i] = btn_new(
			vec2_new(i * 13, 11),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			icon_str_list[i]
		);
	}

	std::array<std::string, NUM_SZ_BTN> sz_icon_str_list = {
		"ICON_SZ_1", "ICON_SZ_2", "ICON_SZ_3", "ICON_SZ_3_5", "ICON_SZ_4",
		"ICON_SZ_4_5",
	};
	for (int i = 0; i < NUM_SZ_BTN; i++) {
		tool_picker.sz_btn_list[i] = btn_new(
			vec2_new(i * 13 + 13, 11 + 13),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			sz_icon_str_list[i]
		);
	}

	std::array<std::string, NUM_FILL_BTN> fill_icon_str_list = {
		"ICON_FILL_ALL", "ICON_FILL_DITHERED",
	};
	for (int i = 0; i < NUM_FILL_BTN; i++) {
		tool_picker.fill_btn_list[i] = btn_new(
			vec2_new(i * 13 + 13, 11 + 13),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			fill_icon_str_list[i]
		);
	}

	std::array<std::string, NUM_SELECT_BTN> select_icon_str_list = {
		"ICON_SELECT", "ICON_FREE_SELECT", "ICON_MAGIC_WAND",
	};
	for (int i = 0; i < NUM_SELECT_BTN; i++) {
		tool_picker.select_btn_list[i] = btn_new(
			vec2_new(i * 13 + 13, 11 + 13),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			select_icon_str_list[i]
		);
	}

	std::array<std::string, NUM_SELECT_SECOND_BTN> select_second_icon_str_list
	= {
		"ICON_SELECT_ADD", "ICON_SELECT_SUBTRACT",
	};
	for (int i = 0; i < NUM_SELECT_SECOND_BTN; i++) {
		tool_picker.select_second_btn_list[i] = btn_new(
			vec2_new(i * 13 + 13 * 5, 11 + 13),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			select_second_icon_str_list[i]
		);
	}

	return tool_picker;
}

void tool_picker_update(ToolPicker &tool_picker, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool tool_key_allowed, bool show) {
	other_btns_update(tool_picker, gs, input, parent_pos,
		tool_key_allowed, show);
	main_btns_update(tool_picker, gs, input, parent_pos,
		tool_key_allowed, show);
}

void tool_picker_draw(const ToolPicker &tool_picker, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	main_btns_draw(tool_picker, gs, input, parent_pos);
	other_btns_draw(tool_picker, gs, input, parent_pos);
}
