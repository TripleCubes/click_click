#ifndef TAB__CURVE_H
#define TAB__CURVE_H

struct Tab;
struct GraphicStuff;
struct Input;
struct Vec2;

void curve_tool_preview_update(Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos);

void curve_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

#endif
