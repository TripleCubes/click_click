#include "tab_bar.h"

#include "../consts.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../types/vec2i.h"
#include "../basic_math.h"

namespace {

const float BTN_SPACING = 2;

void reset_btn_pos(TabBar &tab_bar, const GraphicStuff &gs) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	tab_bar.new_tab_btn = btn_new(
		vec2_new(main_fb_sz.x - SIDE_BAR_W - 25, 0),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_PLUS"
	);
}

void window_resize_handle(TabBar &tab_bar, const GraphicStuff &gs) {
	reset_btn_pos(tab_bar, gs);
}

void _tab_new(TabBar &tab_bar, GraphicStuff &gs, int at,
Vec2 pos, Vec2i sz, int scale){
	int index = tab_new(tab_bar.tab_list, gs, pos, sz, scale);
	tab_bar.tab_order_list.insert(
		tab_bar.tab_order_list.begin() + at,
		index
	);
}

}

void tab_bar_init(TabBar &tab_bar, GraphicStuff &gs, Vec2 pos) {
	tab_bar.pos = pos;

	reset_btn_pos(tab_bar, gs);

	_tab_new(tab_bar, gs, 0, vec2_new(200, 50), vec2i_new(128, 128), 2);
}

void tab_bar_update(TabBar &tab_bar, GraphicStuff &gs,
const Input &input, bool show) {
	if (gs.just_resized) {
		window_resize_handle(tab_bar, gs);
	}

	btn_update(tab_bar.new_tab_btn, gs, input, tab_bar.pos, show);

	if (tab_bar.new_tab_btn.clicked) {
		_tab_new(tab_bar, gs, tab_bar.order_index + 1,
			vec2_new(200, 50), vec2i_new(128, 128), 2);
		tab_bar.order_index++;
	}
	
	Vec2 cursor = tab_bar.pos;
	for (int i = 0; i < (int)tab_bar.tab_order_list.size(); i++) {
		int tab_index = tab_bar.tab_order_list[i];
		
		Tab &tab = tab_bar.tab_list[tab_index];
	
		btn_update(tab.btn, gs, input, cursor, show);
		btn_update(tab.close_btn, gs, input, cursor, show);
		cursor.x += tab.btn.sz.x + 11 + BTN_SPACING;
	}

	for (int i = 0; i < (int)tab_bar.tab_order_list.size(); i++) {
		int tab_index = tab_bar.tab_order_list[i];
		
		Tab &tab = tab_bar.tab_list[tab_index];

		if (tab.btn.clicked) {
			tab_bar.order_index = i;

			break;
		}

		if (tab.close_btn.clicked && tab_bar.tab_order_list.size() > 1) {
			tab_close(tab_bar.tab_list, gs, tab_index);
			tab_bar.tab_order_list.erase(tab_bar.tab_order_list.begin() + i);
			
			if (i < tab_bar.order_index) {
				tab_bar.order_index--;
			}

			tab_bar.order_index = clampi(tab_bar.order_index,
			                             0,
			                             tab_bar.tab_order_list.size() - 1);

			break;
		}
	}
}

void tab_bar_draw(const TabBar &tab_bar, GraphicStuff &gs) {
	btn_draw(tab_bar.new_tab_btn, gs, tab_bar.pos, false);

	Vec2 cursor = tab_bar.pos;
	for (int i = 0; i < (int)tab_bar.tab_order_list.size(); i++) {
		int tab_index = tab_bar.tab_order_list[i];
		
		const Tab &tab = tab_bar.tab_list[tab_index];

		btn_draw(tab.btn, gs, cursor, i == tab_bar.order_index);
		btn_draw(tab.close_btn, gs, cursor, false);
		cursor.x += tab.btn.sz.x + 11 + BTN_SPACING;
	}
}

void tab_bar_release(TabBar &tab_bar, GraphicStuff &gs) {
	for (int i = 0; i < (int)tab_bar.tab_list.size(); i++) {
		if (!tab_bar.tab_list[i].running) {
			continue;
		}

		tab_close(tab_bar.tab_list, gs, i);
	}
}

int tab_bar_get_tab_index(const TabBar& tab_bar) {
	return tab_bar.tab_order_list[tab_bar.order_index];
}

