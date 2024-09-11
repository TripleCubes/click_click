#ifndef TAB__SELECT_H
#define TAB__SELECT_H

#include <vector>
#include "../types/vec2.h"
#include "../types/vec2i.h"

const int SELECT_RECT = 0;
const int SELECT_FREE = 1;
const int SELECT_MAGIC_WAND = 2;

struct Tab;
struct GraphicStuff;
struct GameTime;
struct Input;

struct Selection {
	std::vector<Vec2> draw_preview_list;
	std::vector<Vec2i> scan_list;

	std::vector<Vec2i> full_preview_list;
	std::vector<unsigned char> fill_checked_map;

	std::vector<unsigned char> map;
	std::vector<Vec2i> border_hint_list;

	Vec2i rect_point_1 = vec2i_new(-1, -1);
	Vec2i rect_point_2 = vec2i_new(-1, -1);
};

void selection_init(Selection &selection, Vec2i canvas_sz);

void selection_all(Selection &selection, Vec2i sz);
void selection_clear(Selection &selection, Vec2i sz);

void select_tool_preview_update(Tab &tab, GraphicStuff &gs,
	const GameTime &game_time);

void select_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, bool subtract,
	int select_type, Vec2 parent_pos);

#endif
