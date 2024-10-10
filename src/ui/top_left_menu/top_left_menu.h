#ifndef UI__TOP_LEFT_MENU__TOP_LEFT_MENU_H
#define UI__TOP_LEFT_MENU__TOP_LEFT_MENU_H

#include "../btn.h"
#include "../../types/vec2.h"

struct GraphicStuff;
struct Input;
struct GameTime;

struct TopLeftMenu {
	Vec2 pos;
	Btn hamburger_btn;
	Btn search_btn;
};

void top_left_menu_init(TopLeftMenu &top_left_menu, Vec2 pos);

void top_left_menu_update(TopLeftMenu &top_left_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show);

void top_left_menu_draw(const TopLeftMenu &top_left_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

#endif
