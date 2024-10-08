#ifndef UI__RESIZE_MENU__RESIZE_MENU_H
#define UI__RESIZE_MENU__RESIZE_MENU_H

#include "../btn.h"
#include "../textarea.h"

struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;

enum ResizeMenuTaActive {
	RESIZE_MENU_TA_ACTIVE_NONE,
	RESIZE_MENU_TA_ACTIVE_W,
	RESIZE_MENU_TA_ACTIVE_H,


	RESIZE_MENU_TA_ACTIVE_MAX,
};

struct ResizeMenu {
	Btn close_btn;
	Btn resize_btn;
	TextArea w_ta;
	TextArea h_ta;
	ResizeMenuTaActive ta_active = RESIZE_MENU_TA_ACTIVE_W;
};

void resize_menu_init(ResizeMenu &resize_menu);

void resize_menu_update(ResizeMenu &resize_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show);

void resize_menu_ui_draw(const ResizeMenu &resize_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void resize_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

bool resize_menu_all_field_valid(const ResizeMenu &resize_menu);

Vec2i resize_menu_get_canvas_sz(const ResizeMenu &resize_menu);

#endif
