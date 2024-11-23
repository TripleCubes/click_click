#include "app_menu.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <cmath>

#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/framebuffer.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../graphic/draw_text.h"
#include "../../types/vec2.h"
#include "../../consts.h"
#include "../../settings.h"

namespace {

const float W = 200;
const float H = 160;
const Vec2 MARGIN = vec2_new(4, 4);
const Vec2 SIDE_BTN_SZ = vec2_new(50, 12);
const Vec2 TOGGLE_SZ = vec2_new(20, 12);

void per_settings_menu_handling(AppMenu &app_menu, Settings &settings
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	if (app_menu.use_hardware_cursor_toggle.clicked) {
		settings.use_hardware_cursor = !settings.use_hardware_cursor;

		#ifndef __EMSCRIPTEN__
		if (settings.use_hardware_cursor) {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else {
			glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		#else
		if (settings.use_hardware_cursor) {
			EM_ASM(
			document.body.style.cursor = 'default';
			);
		}
		else {
			EM_ASM(
			document.body.style.cursor = 'none';
			);
		}
		#endif
	}
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
		vec2_new(MARGIN.x, MARGIN.y + 13 * 4),
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

	app_menu.use_hardware_cursor_toggle = btn_new(
		vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 1, MARGIN.y + 13 * 2),
		TOGGLE_SZ,
		BTN_TEXTAREA_COLOR,
		"_"
	);
}

void app_menu_update(AppMenu &app_menu, Settings &settings, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
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

	bool b_ui_settings_menu
		= show && app_menu.selected_menu == APP_MENU_UI_SETTINGS_MENU_SELECTED;
	btn_update(app_menu.use_hardware_cursor_toggle, gs, input, pos,
		b_ui_settings_menu);

	if (!show) {
		return;
	}

	if (app_menu.file_btn.clicked) {
		app_menu.selected_menu = APP_MENU_FILE_MENU_SELECTED;
	}
	if (app_menu.ui_settings_btn.clicked) {
		app_menu.selected_menu = APP_MENU_UI_SETTINGS_MENU_SELECTED;
	}

	per_settings_menu_handling(app_menu, settings
	#ifndef __EMSCRIPTEN__
	,glfw_window
	#endif
	);

	gs.draw_secondlayer_ui = true;
}

void app_menu_ui_draw(const AppMenu &app_menu, const Settings &settings,
GraphicStuff &gs, const Input &input, const GameTime &game_time,
Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
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
	else if (app_menu.selected_menu == APP_MENU_UI_SETTINGS_MENU_SELECTED) {
		btn_draw(app_menu.use_hardware_cursor_toggle, gs, pos, false,
			settings.use_hardware_cursor? "on" : "off");
	}

	auto text = [&gs, pos](const std::string &text, Vec2 in_pos,
	Color color = BTN_TEXTAREA_COLOR) {
		draw_text(
			gs,
			text,
			vec2_add(in_pos, pos),
			9999,
			1,
			color,
			vec2_new(4, 3),
			false
		);
	};

	text(
		"settings",
		vec2_new(MARGIN.x + 4, MARGIN.y + 13 * 2 + 16),
		KEY_HINT_COLOR
	);

	if (app_menu.selected_menu == APP_MENU_UI_SETTINGS_MENU_SELECTED) {
		text(
			"> use hardware cursor",
			vec2_new(MARGIN.x + SIDE_BTN_SZ.x + 5, MARGIN.y + 13 * 1 + 3)
		);
	}
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
