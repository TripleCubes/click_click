#ifndef TAB__FILL_H
#define TAB__FILL_H

struct Tab;
struct Input;
struct GraphicStuff;
struct Vec2;

void fill_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

#endif
