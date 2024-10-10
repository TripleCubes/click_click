#include "app_menu.h"

#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/framebuffer.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../types/vec2.h"

#include <cmath>

namespace {

const float W = 320;
const float H = 220;

}

void app_menu_init(AppMenu &app_menu) {

}

void app_menu_update(AppMenu &app_menu, GraphicStuff &gs, const Input &input,
const GameTime &game_time, Vec2 parent_pos, bool show) {
	if (!show) {
		return;
	}

	gs.draw_secondlayer_ui = true;
}

void app_menu_ui_draw(const AppMenu &app_menu, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {

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
