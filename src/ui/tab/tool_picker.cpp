#include "tool_picker.h"

#include "../../consts.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../graphic/draw_text.h"

namespace {

}

ToolPicker tool_picker_new(Vec2 pos) {
	ToolPicker tool_picker;

	tool_picker.pos = pos;

	std::array<std::string, NUM_BTN> icon_str_list = {
		"ICON_PEN", "ICON_CURVE", "ICON_FILL", "ICON_SELECT"
	};
	for (int i = 0; i < (int)tool_picker.btn_list.size(); i++) {
		tool_picker.btn_list[i] = btn_new(
			vec2_new(i * 13, 10),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			icon_str_list[i]
		);
	}

	return tool_picker;
}

void tool_picker_update(ToolPicker &tool_picker, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool tool_key_allowed, bool show) {
	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);

	for (int i = 0; i < (int)tool_picker.btn_list.size(); i++) {
		Btn &btn = tool_picker.btn_list[i];
		btn_update(btn, gs, input, pos, show);
	}

	for (int i = 0; i < (int)tool_picker.btn_list.size(); i++) {
		const Btn &btn = tool_picker.btn_list[i];

		if (btn.clicked) {
			tool_picker.selected_index = i;
		}
	}

	if (tool_key_allowed) {
		if (input.key_list[KEY_B].press) {
			tool_picker.selected_index = 0;
		}
		else if (input.key_list[KEY_C].press) {
			tool_picker.selected_index = 1;
		}
		else if (input.key_list[KEY_F].press) {
			tool_picker.selected_index = 2;
		}
		else if (input.key_list[KEY_S].press) {
			tool_picker.selected_index = 3;
		}
	}
}

void tool_picker_draw(const ToolPicker &tool_picker, GraphicStuff &gs,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tool_picker.pos);

	for (int i = 0; i < (int)tool_picker.btn_list.size(); i++) {
		const Btn &btn = tool_picker.btn_list[i];
		btn_draw(btn, gs, pos, i == tool_picker.selected_index);
	}

	std::array<std::string, NUM_BTN> tool_key_list = {
		"b", "c", "f", "s"
	};
	for (int i = 0; i < NUM_BTN; i++) {
		draw_text(
			gs,
			tool_key_list[i],
			vec2_add(pos, vec2_new(i * 13 + 4, 1)),
			12,
			1,
			KEY_HINT_COLOR,
			vec2_new(0, 0),
			false
		);
	}
}
