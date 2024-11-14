#include "resize_menu_handling.h"

#include "resize_menu.h"
#include "../../states.h"
#include "../../tab/tab.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../settings.h"
#include "../../types/vec2i.h"
#include "../../tab/tab_commands.h"

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
Tab &tab, GraphicStuff &gs, const Input &input, const GameTime &game_time,
const Settings &settings) {
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
		resize_menu_close(states);

		Vec2i prev_sz = tab.sz;
		Vec2i new_sz = resize_menu_get_canvas_sz(resize_menu);
		Vec2i new_pos_anchor = vec2i_new(0, 0);

		switch (resize_menu.align_btn_selected) {
		case 0:
			new_pos_anchor.x = 0;
			new_pos_anchor.y = 0;
			break;
		case 1:
			new_pos_anchor.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos_anchor.y = 0;
			break;
		case 2:
			new_pos_anchor.x = prev_sz.x - new_sz.x;
			new_pos_anchor.y = 0;
			break;
		case 3:
			new_pos_anchor.x = 0;
			new_pos_anchor.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 4:
			new_pos_anchor.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos_anchor.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 5:
			new_pos_anchor.x = prev_sz.x - new_sz.x;
			new_pos_anchor.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 6:
			new_pos_anchor.x = 0;
			new_pos_anchor.y = prev_sz.y - new_sz.y;
			break;
		case 7:
			new_pos_anchor.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos_anchor.y = prev_sz.y - new_sz.y;
			break;
		case 8:
			new_pos_anchor.y = prev_sz.y - new_sz.y;
			new_pos_anchor.x = prev_sz.x - new_sz.x;
			break;
		}

		history_commit_prepare(tab.history, tab.tab_commands);
		Command command = command_new(
			tab.history.time_pos_current,
			COMMAND_RESIZE,
			0,
			0,
			0,
			new_pos_anchor,
			new_sz,
			vec2i_mul(new_pos_anchor, -1),
			tab.sz,
			"",
			""
		);
		tab_commands_do_and_add(
			tab.tab_commands,
			command,
			tab.history,
			gs,
			input,
			game_time,
			settings,
			tab
		);

		for (int i = 0; i < (int)tab.layer_list.size(); i++) {
			Layer &layer = tab.layer_list[i];
			if (!layer.running) {
				continue;
			}

			HistoryLayer &history_layer
				= tab.history.layer_list[layer.history_layer_index];
			history_commit_layer(tab.history, history_layer, layer);
		}
	}
}
