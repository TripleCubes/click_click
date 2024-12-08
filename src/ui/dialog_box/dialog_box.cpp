#include "dialog_box.h"

#include <cmath>

#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/framebuffer.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../graphic/draw_text.h"
#include "../../types/vec2.h"
#include "../../types/vec2i.h"
#include "../../types/color.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../consts.h"
#include "../../file/file_extension.h"

namespace {

const Vec2 MARGIN = vec2_new(3, 3);
const Vec2 BTN_SZ = vec2_new(32, 12);

}

void dialog_box_init(DialogBox &dialog_box) {
	dialog_box_set(dialog_box, DIALOG_BOX_WEB_FIRST_RAN_NOTICE);
}

void dialog_box_update(DialogBox &dialog_box, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = std::floor((main_fb_sz.x - dialog_box.sz.x) / 2);
	const float Y = std::floor((main_fb_sz.y - dialog_box.sz.y) / 2);
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_update(dialog_box.ok_btn, gs, input, pos, show);
	btn_update(dialog_box.cancel_btn, gs, input, pos,
		dialog_box.show_cancel_btn);

	if (!show) {
		return;
	}

	gs.draw_thirdlayer_ui = true;
}

void dialog_box_draw(const DialogBox &dialog_box, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = std::floor((main_fb_sz.x - dialog_box.sz.x) / 2);
	const float Y = std::floor((main_fb_sz.y - dialog_box.sz.y) / 2);
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_draw(dialog_box.ok_btn, gs, pos, false);
	if (dialog_box.show_cancel_btn) {
		btn_draw(dialog_box.cancel_btn, gs, pos, false);
	}

	Vec2 title_box_sz = draw_text(
		gs,
		dialog_box.title_text,
		vec2_new(X + MARGIN.x + 5,
		         Y + MARGIN.y + 5),
		dialog_box.sz.x - MARGIN.x * 2 - 9,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);
	Vec2 text_0_sz = draw_text(
		gs,
		dialog_box.text,
		vec2_new(X + MARGIN.x + 5,
		         Y + MARGIN.y + title_box_sz.y + 13),
		dialog_box.sz.x - MARGIN.x * 2 - 9,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);
	draw_text(
		gs,
		dialog_box.text_1,
		vec2_new(X + MARGIN.x + 5,
		         Y + MARGIN.y + title_box_sz.y + 13 + text_0_sz.y + 8),
		dialog_box.sz.x - MARGIN.x * 2 - 9,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);
}

void dialog_box_bkg_draw(const DialogBox &dialog_box, GraphicStuff &gs,
Vec2 parent_pos) {
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

	const float X = std::floor((main_fb_sz.x - dialog_box.sz.x) / 2);
	const float Y = std::floor((main_fb_sz.y - dialog_box.sz.y) / 2);
	draw(vec2_new(X, Y), vec2_new(dialog_box.sz.x, dialog_box.sz.y));
}

void dialog_box_set(DialogBox &dialog_box, int dialog_type) {
	dialog_box.dialog_type = dialog_type;

	std::string ok_btn_text = "ok defl";
	std::string cancel_btn_text = "cancel defl";

	dialog_box.sz.x = 160;
	dialog_box.sz.y = 80;
	dialog_box.title_text = "title defl";
	dialog_box.text = "text defl";
	dialog_box.text_1 = "";
	dialog_box.show_cancel_btn = true;

	#ifdef __EMSCRIPTEN__
	if (dialog_box.dialog_type == DIALOG_BOX_WEB_DELETE_FILE) {
		dialog_box.ok_btn.text = "delete";
		dialog_box.title_text = "delete \"" + dialog_box.web_delete_file_name
		                        + "\" ?";
		dialog_box.text = "this file will be permanently deleted";

		ok_btn_text = "delete";
		cancel_btn_text = "cancel";
	}

	if (dialog_box.dialog_type == DIALOG_BOX_WEB_FIRST_RAN_NOTICE) {
		dialog_box.ok_btn.text = "delete";
		dialog_box.show_cancel_btn = false;

		dialog_box.sz.x = 220;
		dialog_box.sz.y = 140;

		dialog_box.title_text = "IMPORTANT";
		dialog_box.text = "this app is very experimental. I haven't tested "
		                  "this app much at all. there will be bugs, and data "
						  "lost may happen. so "
		                  "use the app at your own risk";
		                  
		dialog_box.text_1 = "the app is available under the MIT license. "
		                    "source code available at "
		                    "https://github.com/triplecubes/click_click_test";
	
		ok_btn_text = "okay";
	}
	#endif

	if (dialog_box.dialog_type == DIALOG_BOX_OVERRIDE_FILE) {
		std::string dot_extension;
		if (is_dot_click(dialog_box.override_file_path)) {
			dot_extension = DOT_CLICK;
		}
		else {
			dot_extension = DOT_PNG;
		}

		dialog_box.ok_btn.text = "okay";
		dialog_box.title_text = "override \"" + dialog_box.override_file_name
		                        + dot_extension + "\" ?";
		dialog_box.text = dialog_box.override_file_name + " already exist. "
		                  + "do you want to override?";

		ok_btn_text = "yes";
		cancel_btn_text = "no";
	}

	dialog_box.ok_btn = btn_new(
		vec2_new(dialog_box.sz.x - BTN_SZ.x - MARGIN.x,
		         dialog_box.sz.y - BTN_SZ.y - MARGIN.y),
		BTN_SZ,
		BTN_TEXTAREA_COLOR,
		ok_btn_text
	);

	dialog_box.cancel_btn = btn_new(
		vec2_new(dialog_box.sz.x - BTN_SZ.x * 2 - 1 - MARGIN.x,
		         dialog_box.sz.y - BTN_SZ.y - MARGIN.y),
		BTN_SZ,
		BTN_TEXTAREA_COLOR,
		cancel_btn_text
	);
}
