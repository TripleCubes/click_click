#ifndef TAB__SELECT_H
#define TAB__SELECT_H

#include <vector>
#include "../types/vec2.h"

struct Tab;
struct GraphicStuff;
struct GameTime;
struct Input;

struct SelectPoint {
	Vec2 pos;
};

struct Selection {
	std::vector<SelectPoint> point_list;
	std::vector<SelectPoint> point_list_2;
	std::vector<unsigned char> map;
	std::vector<unsigned char> map_2;
	Vec2 top_left = vec2_new(9999999, 9999999);
	Vec2 bottom_right = vec2_new(-9999999, -9999999);
	Vec2 top_left_all = vec2_new(9999999, 9999999);
	Vec2 bottom_right_all = vec2_new(-9999999, -9999999);
};

void select_tool_preview_update(Tab &tab, GraphicStuff &gs,
	const GameTime &game_time);

void select_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, bool subtract,
	Vec2 parent_pos);

#endif
