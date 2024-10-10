#include "app_menu.h"

#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/framebuffer.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../graphic/draw_text.h"
#include "../../types/vec2.h"
#include "../../consts.h"

#include <cmath>

namespace {

const float W = 200;
const float H = 160;
const Vec2 MARGIN = vec2_new(4, 4);
const Vec2 SIDE_BTN_SZ = vec2_new(50, 12);

void per_menu_handling(AppMenu &app_menu, const Input &input) {
	// if (app_menu.) {

	// }
}

}

void app_menu_init(AppMenu &app_menu) {
	app_menu.close_btn = btn_new(
		vec2_new(W - 11 - MARGIN.x, MARGIN.y),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_X"
	);

	app_menu.file_btn = btn_new(
		vec2_new(MARGIN.x, MARGIN.y + 13),
		SIDE_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"file"
	);
	app_menu.ui_settings_btn = btn_new(
		vec2_new(MARGIN.x, MARGIN.y + 13 + 6 + 13 * 2),
		SIDE_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"ui"
	);

	const Vec2 CONTENT_BTN_SZ
		= vec2_new(W - SIDE_BTN_SZ.x - MARGIN.x * 2 + 1, 12);
	app_menu.new_tab_btn = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 1),
		CONTENT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"new tab"
	);
	app_menu.open_file_btn = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 2),
		CONTENT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"open file"
	);
	app_menu.save_btn = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 3),
		CONTENT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"save"
	);
	app_menu.save_as_btn = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 4),
		CONTENT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"save as"
	);
	app_menu.resize_btn = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 5),
		CONTENT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"resize canvas"
	);
}

void app_menu_update(AppMenu &app_menu, GraphicStuff &gs, const Input &input,
const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_update(app_menu.close_btn, gs, input, pos, show);
	btn_update(app_menu.file_btn, gs, input, pos, show);
	btn_update(app_menu.ui_settings_btn, gs, input, pos, show);

	bool b_file_menu
		= show && app_menu.selected_menu == APP_MENU_FILE_MENU_SELECTED;
	btn_update(app_menu.new_tab_btn, gs, input, pos, b_file_menu);
	btn_update(app_menu.open_file_btn, gs, input, pos, b_file_menu);
	btn_update(app_menu.save_btn, gs, input, pos, b_file_menu);
	btn_update(app_menu.save_as_btn, gs, input, pos, b_file_menu);
	btn_update(app_menu.resize_btn, gs, input, pos, b_file_menu);


	if (!show) {
		return;
	}

	if (app_menu.file_btn.clicked) {
		app_menu.selected_menu = APP_MENU_FILE_MENU_SELECTED;
	}
	if (app_menu.ui_settings_btn.clicked) {
		app_menu.selected_menu = APP_MENU_UI_SETTINGS_MENU_SELECTED;
	}

	per_menu_handling(app_menu, input);

	gs.draw_secondlayer_ui = true;
}

void app_menu_ui_draw(const AppMenu &app_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_draw(app_menu.close_btn, gs, pos, false);
	btn_draw(app_menu.file_btn, gs, pos,
		app_menu.selected_menu == APP_MENU_FILE_MENU_SELECTED);
	btn_draw(app_menu.ui_settings_btn, gs, pos,
		app_menu.selected_menu == APP_MENU_UI_SETTINGS_MENU_SELECTED);

	if (app_menu.selected_menu == APP_MENU_FILE_MENU_SELECTED) {
		btn_draw(app_menu.new_tab_btn, gs, pos, false);
		btn_draw(app_menu.open_file_btn, gs, pos, false);
		btn_draw(app_menu.save_btn, gs, pos, false);
		btn_draw(app_menu.save_as_btn, gs, pos, false);
		btn_draw(app_menu.resize_btn, gs, pos, false);
	}

	draw_text(
		gs,
		"settings",
		vec2_add(pos, vec2_new(MARGIN.x + 4, MARGIN.y + 13 * 2 + 8)),
		9999,
		1,
		KEY_HINT_COLOR,
		vec2_new(4, 3),
		false
	);
}

void app_menu_bkg_draw(GraphicStuff &gs, Vec2 parent_pos) {
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

	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
	draw(vec2_new(X, Y), vec2_new(W, H));
}
