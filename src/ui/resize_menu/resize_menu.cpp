#include "resize_menu.h"

//#include "../../tab/tab.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../input_map.h"
#include "../../game_time.h"
#include "../../graphic_types/framebuffer.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../graphic/draw_text.h"
#include "../../types/vec2.h"
#include "../../types/vec2i.h"
#include "../../consts.h"

#include <string>

namespace {

const float W = 160;
const float H = 120;
const Vec2 RESIZE_BTN_SZ = vec2_new(36, 12);
const Vec2 MARGIN = vec2_new(3, 3);
const Vec2 WH_TA_SZ = vec2_new(W/2 - MARGIN.x - 2, 12);

void ta_hover_handle(const ResizeMenu &resize_menu, GraphicStuff &gs) {
	if (resize_menu.w_ta.hovered || resize_menu.h_ta.hovered) {
		gs.cursor_icon = CURSOR_TEXT;
	}
}

void tab_pressed_handle(ResizeMenu &resize_menu, const GameTime &game_time) {
	resize_menu.ta_active = (ResizeMenuTaActive)(resize_menu.ta_active + 1);

	if (resize_menu.ta_active == RESIZE_MENU_TA_ACTIVE_MAX) {
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_W;
	}

	textarea_cursor_reset(resize_menu.w_ta, game_time);
	textarea_cursor_reset(resize_menu.h_ta, game_time);
}

void shift_tab_pressed_handle(ResizeMenu &resize_menu,
const GameTime &game_time) {
	resize_menu.ta_active = (ResizeMenuTaActive)(resize_menu.ta_active - 1);

	if (resize_menu.ta_active <= RESIZE_MENU_TA_ACTIVE_NONE) {
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_H;
	}

	textarea_cursor_reset(resize_menu.w_ta, game_time);
	textarea_cursor_reset(resize_menu.h_ta, game_time);
}

void ta_active_handle(ResizeMenu &resize_menu, const Input &input,
const GameTime &game_time) {
	if (input.left_click || map_press(input, MAP_ESC)) {
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_NONE;
	}

	if (resize_menu.w_ta.clicked) {
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_W;
	}
	else if (resize_menu.h_ta.clicked) {
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_H;
	}

	if (map_press(input, MAP_TAB)) {
		tab_pressed_handle(resize_menu, game_time);
	}
	else if (map_press(input, MAP_SHIFT_TAB)) {
		shift_tab_pressed_handle(resize_menu, game_time);
	}
}

}

void resize_menu_init(ResizeMenu &resize_menu) {
	resize_menu.close_btn = btn_new(
		vec2_new(W - 12 - MARGIN.x, MARGIN.y + 1),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_X"
	);

	resize_menu.resize_btn = btn_new(
		vec2_new(W - RESIZE_BTN_SZ.x - MARGIN.x,
		         H - RESIZE_BTN_SZ.y - MARGIN.y),
		RESIZE_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"resize"
	);

	resize_menu.w_ta = textarea_new(
		vec2_new(MARGIN.x + 1, MARGIN.y + 23),
		WH_TA_SZ,
		BTN_TEXTAREA_COLOR,
		"32",
		true
	);
	resize_menu.h_ta = textarea_new(
		vec2_new(2 + W / 2, MARGIN.y + 23),
		WH_TA_SZ,
		BTN_TEXTAREA_COLOR,
		"32",
		true
	);
}

void resize_menu_update(ResizeMenu &resize_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_update(resize_menu.resize_btn, gs, input, pos, show);
	btn_update(resize_menu.close_btn, gs, input, pos, show);
	textarea_update(resize_menu.w_ta, gs, game_time, input, pos,
		resize_menu.ta_active == RESIZE_MENU_TA_ACTIVE_W, show);
	textarea_update(resize_menu.h_ta, gs, game_time, input, pos,
		resize_menu.ta_active == RESIZE_MENU_TA_ACTIVE_H, show);

	ta_hover_handle(resize_menu, gs);
	ta_active_handle(resize_menu, input, game_time);

	gs.draw_secondlayer_ui = true;
}

void resize_menu_ui_draw(const ResizeMenu &resize_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_floor(vec2_new(X, Y)));

	btn_draw(resize_menu.resize_btn, gs, pos, false);
	btn_draw(resize_menu.close_btn, gs, pos, false);
	textarea_draw(resize_menu.w_ta, gs, game_time, pos,
		resize_menu.ta_active == RESIZE_MENU_TA_ACTIVE_W, true);
	textarea_draw(resize_menu.h_ta, gs, game_time, pos,
		resize_menu.ta_active == RESIZE_MENU_TA_ACTIVE_H, true);

	draw_text(
		gs,
		"width",
		vec2_floor(vec2_new(X + MARGIN.x + 3, Y + MARGIN.y + 15)),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(0, 0),
		false
	);
	draw_text(
		gs,
		"height",
		vec2_floor(vec2_new(X + MARGIN.x + W / 2 + 1, Y + MARGIN.y + 15)),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(0, 0),
		false
	);

	if (!resize_menu_all_field_valid(resize_menu)) {
		draw_text(
			gs,
			"invalid canvas size",
			vec2_new(X + MARGIN.x + 3, Y + MARGIN.y + 40),
			100,
			1,
			WARNING_TEXT_COLOR,
			vec2_new(0, 0),
			false
		);
	}
}

void resize_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	auto draw = [&gs, main_fb_sz, parent_pos](Vec2 pos, Vec2 sz) -> void {
		draw_texture(
			gs,
			main_fb_sz,
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			color_new(0, 0, 0, 0),
			false
		);
	};

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	draw(vec2_new(X, Y), vec2_new(W, H));
}

bool resize_menu_all_field_valid(const ResizeMenu &resize_menu) {
	if (resize_menu.w_ta.text.length() == 0
	||  resize_menu.h_ta.text.length() == 0) {
		return false;
	}

	if (std::stoi(resize_menu.w_ta.text) == 0
	||  std::stoi(resize_menu.h_ta.text) == 0) {
		return false;
	}

	return true;
}

Vec2i resize_menu_get_canvas_sz(const ResizeMenu &resize_menu) {
	return vec2i_new(std::stoi(resize_menu.w_ta.text),
	                 std::stoi(resize_menu.h_ta.text));
}
