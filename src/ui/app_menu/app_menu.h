#ifndef UI__APP_MENU__APP_MENU_H
#define UI__APP_MENU__APP_MENU_H

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#endif

#include "../btn.h"

struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;
struct Settings;

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

	Btn use_hardware_cursor_toggle;
	Btn px_scale_btn;

	int selected_menu = 0;
};

void app_menu_init(AppMenu &app_menu);

void app_menu_update(AppMenu &app_menu, Settings &settings, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
);

void app_menu_ui_draw(const AppMenu &app_menu, const Settings &settings,
	GraphicStuff &gs, const Input &input, const GameTime &game_time,
	Vec2 parent_pos);

void app_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

#endif
