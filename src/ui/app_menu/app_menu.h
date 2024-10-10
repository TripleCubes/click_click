#ifndef UI__APP_MENU__APP_MENU_H
#define UI__APP_MENU__APP_MENU_H

#include "../btn.h"

struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;

struct AppMenu {
	
};

void app_menu_init(AppMenu &app_menu);

void app_menu_update(AppMenu &app_menu, GraphicStuff &gs, const Input &input,
	const GameTime &game_time, Vec2 parent_pos, bool show);

void app_menu_ui_draw(const AppMenu &app_menu, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void app_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

#endif
