#ifndef TAB__PX_PERFECT_BRUSH_H
#define TAB__PX_PERFECT_BRUSH_H

struct Tab;
struct GraphicStuff;
struct Input;
struct Vec2;

void px_perfect_brush_tool_preview_update(Tab &tab, GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

bool px_perfect_brush_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

#endif
