#include "app_menu_handling.h"

#include "app_menu.h"
#include "../app_ui.h"
#include "../file_picker/file_picker_handling.h"
#include "../new_tab_menu/new_tab_menu_handling.h"
#include "../resize_menu/resize_menu_handling.h"
#include "../../states.h"
#include "../../tab/tab.h"
#include "../../input.h"
#include "../../file/save_project.h"

void app_menu_handling(States &states, AppUI &app_ui, Tab &tab,
GraphicStuff &gs, const Input &input) {
	AppMenu &app_menu = app_ui.app_menu;
	FilePicker &file_picker = app_ui.file_picker;
	NewTabMenu &new_tab_menu = app_ui.new_tab_menu;
	ResizeMenu &resize_menu = app_ui.resize_menu;

	if (!menu_opening(states) && map_press(input, MAP_APP_MENU)) {
		states.app_menu_opening = true;
	}
	else if (states.app_menu_opening
	&& (app_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.app_menu_opening = false;
	}


	if (app_menu.new_tab_btn.clicked) {
		states.app_menu_opening = false;
		new_tab_menu_open(states, new_tab_menu);
	}

	if (app_menu.open_file_btn.clicked) {
		states.app_menu_opening = false;
		file_picker_type_open_file_open(states, file_picker, tab);
	}

	if (app_menu.save_btn.clicked) {
		states.app_menu_opening = false;

		if (tab.path.length() == 0) {
			file_picker_type_save_file_open(states, file_picker, tab);
		}
		else {
			save_project(tab.path, tab);
		}
	}

	if (app_menu.save_as_btn.clicked) {
		states.app_menu_opening = false;
		file_picker_type_save_file_open(states, file_picker, tab);
	}

	if (app_menu.resize_btn.clicked) {
		states.app_menu_opening = false;
		resize_menu_open(states, resize_menu, tab);
	}
}
