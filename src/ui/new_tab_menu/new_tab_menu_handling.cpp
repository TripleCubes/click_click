#include "new_tab_menu_handling.h"

#include "new_tab_menu.h"
#include "../../states.h"
#include "../tab_bar/tab_bar.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"

void new_tab_menu_open(States &states, NewTabMenu &new_tab_menu) {
	states.new_tab_menu_opening = true;
	new_tab_menu.ta_active = NEW_TAB_MENU_TA_ACTIVE_W;
}

void new_tab_menu_close(States &states) {
	states.new_tab_menu_opening = false;
}

void new_tab_menu_handling(States &states, NewTabMenu &new_tab_menu,
TabBar &tab_bar, GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states)
	&& (map_press(input, MAP_NEW_PROJECT)
		|| map_press(input, MAP_NEW_PROJECT_1))) {
		new_tab_menu_open(states, new_tab_menu);
	}
	
	if (states.new_tab_menu_opening
	&& (new_tab_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		new_tab_menu_close(states);
	}

	if (states.new_tab_menu_opening
	&& (new_tab_menu.new_tab_btn.clicked || map_press(input, MAP_ENTER))
	&& new_tab_menu_all_field_valid(new_tab_menu)) {
		states.new_tab_menu_opening = false;

		Vec2i canvas_sz = new_tab_menu_get_canvas_sz(new_tab_menu);

		tab_bar_tab_new(tab_bar, gs, tab_bar.order_index + 1,
			vec2_new(0, 0), canvas_sz, 2);
		tab_bar.order_index++;

		int index = tab_bar.tab_order_list[tab_bar.order_index];
		tab_center_canvas(tab_bar.tab_list[index], gs);
	}
}
