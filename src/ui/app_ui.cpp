#include "app_ui.h"

#include "../graphic_types/graphic_types.h"
#include "../consts.h"

void app_ui_init(AppUI &app_ui, GraphicStuff &gs) {
	tab_bar_init(app_ui.tab_bar, gs, vec2_new(SIDE_BAR_W + 3 + 3, 3 + 3));
	file_picker_init(app_ui.file_picker);
	new_tab_menu_init(app_ui.new_tab_menu);
	resize_menu_init(app_ui.resize_menu);
	top_left_menu_init(app_ui.top_left_menu, vec2_new(3, 3));
	app_menu_init(app_ui.app_menu);

	dialog_box_init(app_ui.dialog_box);
}

void app_ui_release(AppUI &app_ui, GraphicStuff &gs) {
	tab_bar_release(app_ui.tab_bar, gs);
}
