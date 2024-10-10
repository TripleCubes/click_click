#ifndef UI__APP_MENU__APP_MENU_H
#define UI__APP_MENU__APP_MENU_H

#include "../btn.h"

struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;

const int APP_MENU_FILE_MENU_SELECTED = 0;
const int APP_MENU_UI_SETTINGS_MENU_SELECTED = 1;

struct AppMenu {
	Btn close_btn;
	
	Btn file_btn;
	Btn ui_settings_btn;

	Btn new_tab_btn;
	Btn open_file_btn;
	Btn save_btn;
	Btn save_as_btn;
	Btn resize_btn;

	int selected_menu = 0;
};

void app_menu_init(AppMenu &app_menu);

void app_menu_update(AppMenu &app_menu, GraphicStuff &gs, const Input &input,
	const GameTime &game_time, Vec2 parent_pos, bool show);

void app_menu_ui_draw(const AppMenu &app_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void app_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

#endif
