#ifndef UI__BTN_H
#define UI__BTN_H

#include <vector>
#include <string>
#include "../types/vec2.h"
#include "../types/color.h"

struct GraphicStuff;
struct Input;

struct Btn {
	bool running = true;
	bool show = false;
	Vec2 pos;
	Vec2 sz;
	Color color;
	std::string text;
	bool hovered = false;
	bool clicked = false;
	bool holding = false;
	bool released = false;
};

int btn_new(std::vector<Btn> &btn_list, Vec2 pos, Vec2 sz, Color color,
	const std::string &text, bool show);

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input);

void btn_draw(const Btn &btn, const GraphicStuff &gs, const Input &input);

void btn_list_update(std::vector<Btn> &btn_list,
	const GraphicStuff &gs, const Input &input);

void btn_list_draw(const std::vector<Btn> &btn_list,
	const GraphicStuff &gs, const Input &input);

bool btn_hovered(const std::vector<Btn> &btn_list, int index);
bool btn_clicked(const std::vector<Btn> &btn_list, int index);
bool btn_holding(const std::vector<Btn> &btn_list, int index);
bool btn_released(const std::vector<Btn> &btn_list, int index);

#endif
