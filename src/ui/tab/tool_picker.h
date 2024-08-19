#ifndef UI__TAB__TOOL_PICKER_H
#define UI__TAB__TOOL_PICKER_H

#include "../../types/vec2.h"
#include "../btn.h"
#include <array>

const int TOOL_BRUSH = 0;
const int TOOL_CURVE = 1;
const int TOOL_FILL = 2;
const int TOOL_SELECT = 3;

const Vec2 TOOL_PICKER_SZ = vec2_new(12*4 + 3, 21);
const int NUM_BTN = 4;

struct GraphicStuff;
struct Input;

struct ToolPicker {
	Vec2 pos;
	int selected_index = 0;

	std::array<Btn, NUM_BTN> btn_list;
};

ToolPicker tool_picker_new(Vec2 pos);

void tool_picker_update(ToolPicker &tool_picker, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos, bool tool_key_allowed, bool show);

void tool_picker_draw(const ToolPicker &tool_picker, GraphicStuff &gs,
	Vec2 parent_pos);

#endif
