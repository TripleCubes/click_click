#ifndef UI__BTN_H
#define UI__BTN_H

#include <vector>
#include <string>
#include "../types/vec2.h"
#include "../types/color.h"

struct GraphicStuff;
struct Input;

struct Btn {
	Vec2 pos;
	Vec2 sz;
	Color color;
	std::string text;
	bool hovered = false;
	bool clicked = false;
	bool holding = false;
};

Btn btn_new(Vec2 pos, Vec2 sz, Color color, const std::string &text);

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input,
	Vec2 parent_pos, bool show);

void btn_draw(const Btn &btn, GraphicStuff &gs, Vec2 parent_pos);

#endif
