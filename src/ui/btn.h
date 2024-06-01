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
	bool released = false;
};

Btn btn_new(Vec2 pos, Vec2 sz, Color color, const std::string &text);

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input);

void btn_draw(const Btn &btn, const GraphicStuff &gs);

#endif
