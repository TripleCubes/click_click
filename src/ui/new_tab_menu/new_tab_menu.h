#ifndef UI__NEW_TAB_NENU__NEW_TAB_MENU_H
#define UI__NEW_TAB_NENU__NEW_TAB_MENU_H

#include "../btn.h"
#include "../textarea.h"

struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;

enum NewTabMenuTaActive {
	NEW_TAB_MENU_TA_ACTIVE_NONE,
	NEW_TAB_MENU_TA_ACTIVE_W,
	NEW_TAB_MENU_TA_ACTIVE_H,


	NEW_TAB_MENU_TA_ACTIVE_MAX,
};

struct NewTabMenu {
	Btn close_btn;
	Btn new_tab_btn;
	TextArea w_ta;
	TextArea h_ta;
	NewTabMenuTaActive ta_active = NEW_TAB_MENU_TA_ACTIVE_NONE;
};

void new_tab_menu_init(NewTabMenu &new_tab_menu);

void new_tab_menu_update(NewTabMenu &new_tab_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show);

void new_tab_menu_ui_draw(const NewTabMenu &new_tab_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void new_tab_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

bool new_tab_menu_all_field_valid(const NewTabMenu &new_tab_menu);

Vec2i new_tab_menu_get_canvas_sz(const NewTabMenu &new_tab_menu);

#endif
