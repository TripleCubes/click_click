#ifndef UI__TEXTAREA_H
#define UI__TEXTAREA_H

#include <string>
#include "../types/vec2.h"
#include "../types/color.h"

struct GraphicStuff;
struct Input;

struct TextArea {
	Vec2 pos;
	Vec2 sz;
	Color color;
	std::string text;
	std::string defl_text;
	bool clicked = false;
	bool hovered = false;
};

TextArea textarea_new(Vec2 pos, Vec2 sz, Color color,
	const std::string &defl_text);

void textarea_update(TextArea &textarea, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos, bool active, bool show);

void textarea_draw(const TextArea &textarea, GraphicStuff &gs, Vec2 parent_pos,
	bool active);

#endif
