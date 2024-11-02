#ifndef TAB__BRUSH_H
#define TAB__BRUSH_H

struct GraphicStuff;
struct Input;
struct Tab;
struct Vec2;

bool brush_tool_update(Tab &tab, int layer_index, int brush_sz,
	GraphicStuff &gs, const Input &input, Vec2 parent_pos);

#endif
