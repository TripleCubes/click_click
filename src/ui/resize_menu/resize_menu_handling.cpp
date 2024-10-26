#include "resize_menu_handling.h"

#include "resize_menu.h"
#include "../../states.h"
#include "../../tab/tab.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../types/vec2i.h"

void resize_menu_open(States &states, ResizeMenu &resize_menu, Tab &tab) {
	states.resize_menu_opening = true;
	resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_W;
	resize_menu.w_ta.text = std::to_string(tab.sz.x);
	resize_menu.h_ta.text = std::to_string(tab.sz.y);
}

void resize_menu_close(States &states) {
	states.resize_menu_opening = false;
}

void resize_menu_handling(States &states, ResizeMenu &resize_menu,
Tab &tab, GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states) && map_press(input, MAP_RESIZE_CANVAS)) {
		resize_menu_open(states, resize_menu, tab);
	}
	
	if (states.resize_menu_opening
	&& (resize_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		resize_menu_close(states);
	}

	if (states.resize_menu_opening
	&& (resize_menu.resize_btn.clicked || map_press(input, MAP_ENTER))
	&& resize_menu_all_field_valid(resize_menu)) {
		states.resize_menu_opening = false;

		Vec2i prev_sz = tab.sz;
		Vec2i new_sz = resize_menu_get_canvas_sz(resize_menu);
		Vec2i new_pos = vec2i_new(0, 0);

		switch (resize_menu.align_btn_selected) {
		case 0:
			new_pos.x = 0;
			new_pos.y = 0;
			break;
		case 1:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = 0;
			break;
		case 2:
			new_pos.x = prev_sz.x - new_sz.x;
			new_pos.y = 0;
			break;
		case 3:
			new_pos.x = 0;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 4:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 5:
			new_pos.x = prev_sz.x - new_sz.x;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 6:
			new_pos.x = 0;
			new_pos.y = prev_sz.y - new_sz.y;
			break;
		case 7:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = prev_sz.y - new_sz.y;
			break;
		case 8:
			new_pos.y = prev_sz.y - new_sz.y;
			new_pos.x = prev_sz.x - new_sz.x;
			break;
		}

		selection_clear(tab.selection, tab.sz);
		tab_resize(tab, gs, new_pos, new_sz);
	}
}
