#include "top_left_menu.h"

#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../consts.h"

void top_left_menu_init(TopLeftMenu &top_left_menu, Vec2 pos) {
	top_left_menu.pos = pos;

	top_left_menu.hamburger_btn = btn_new(
		vec2_new(0, 0),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_HAMBURGER"
	);

	top_left_menu.command_btn = btn_new(
		vec2_new(12, 0),
		vec2_new(SIDE_BAR_W - 17, 12),
		BTN_TEXTAREA_COLOR,
		"commands"
	);
}

void top_left_menu_update(TopLeftMenu &top_left_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2 pos = vec2_add(parent_pos, top_left_menu.pos);

	btn_update(top_left_menu.hamburger_btn, gs, input, pos, show);
	btn_update(top_left_menu.command_btn, gs, input, pos, show);

	// if (!show) {
	// 	return;
	// }
}

void top_left_menu_draw(const TopLeftMenu &top_left_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, top_left_menu.pos);

	btn_draw(top_left_menu.hamburger_btn, gs, pos, false);
	btn_draw(top_left_menu.command_btn, gs, pos, false);
}
