#ifndef UI__TEXTAREA_H
#define UI__TEXTAREA_H

#include <string>
#include "../types/vec2.h"
#include "../types/color.h"

struct GraphicStuff;
struct Input;
struct GameTime;

struct TextArea {
	Vec2 pos;
	Vec2 sz;
	Color color;
	int cursor_at = 0;
	float cursor_moved_at = 0;
	std::string text;
	std::string defl_text;
	bool clicked = false;
	bool hovered = false;

	bool num_only = false;
};

TextArea textarea_new(Vec2 pos, Vec2 sz, Color color,
	const std::string &defl_text, bool num_only = false);

void textarea_update(TextArea &textarea, const GraphicStuff &gs,
	const GameTime &game_time, const Input &input, Vec2 parent_pos,
	bool active, bool show);

void textarea_draw(const TextArea &textarea, GraphicStuff &gs,
	const GameTime &game_time, Vec2 parent_pos, bool active, bool selected);

void textarea_cursor_reset(TextArea &textarea, const GameTime &game_time);

#endif
