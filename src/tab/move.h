#ifndef TAB__MOVE_H
#define TAB__MOVE_H

#include <vector>

#include "../types/vec2.h"
#include "../types/vec2i.h"

struct Tab;
struct GraphicStuff;
struct Input;

struct Move {
	std::vector<unsigned char> data;
	Vec2 pos;
	Vec2i sz;
	bool moving = false;
};

void move_tool_prepare(Move &move);

void move_tool_start(Move &move, Tab &tab, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

void move_tool_commit(Move &move, Tab &tab, GraphicStuff &gs);

void move_tool_discard(Move &move, Tab &tab, GraphicStuff &gs);

void move_tool_preview_update(Move &move, Tab &tab,
	GraphicStuff &gs, const Input &input, Vec2 parent_pos);

void move_tool_update(Move &move, Tab &tab, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

#endif
