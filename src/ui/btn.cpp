#include "btn.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"
#include "../input.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../graphic/draw_text.h"

#include "../basic_math.h"
#include "../pos_convert.h"
#include "../consts.h"

Btn btn_new(Vec2 pos, Vec2 sz, Color color, const std::string &text) {
	Btn btn;
	btn.pos = pos;
	btn.sz = sz;
	btn.color = color;
	btn.text = text;

	return btn;
}

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input,
Vec2 parent_pos, bool show) {
	btn.hovered = false;
	btn.clicked = false;

	if (!show) {
		btn.holding = false;
		return;
	}

	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, btn.pos);

	if (input.left_release) {
		btn.holding = false;
	}

	if (!in_rect(mouse_pos, vec2_new(0, 0), to_vec2(main_fb_sz))) {
		return;
	}

	if (!in_rect(mouse_pos, pos, btn.sz)) {
		return;
	}

	btn.hovered = true;

	if (input.left_click) {
		btn.holding = true;
		btn.clicked = true;
	}
}

void btn_draw(const Btn &btn, GraphicStuff &gs, Vec2 parent_pos,bool selected){
	Vec2 pos = vec2_add(parent_pos, btn.pos);
	Color color = btn.color;
	
	bool flip_color = false;

	if (btn.holding || btn.hovered || selected) {
		flip_color = true;
	}

	auto _draw_icon = [&gs, color, flip_color, pos](int icon) {
		draw_icon(
			gs,
			icon,
			vec2_add(pos, vec2_new(3, 3)),
			TEXT_SCALE,
			color,
			vec2_new(3, 3),
			flip_color
		);
	};

	if (btn.text == "ICON_PLUS") {
		_draw_icon(ICON_PLUS);
	}
	else if (btn.text == "ICON_X") {
		_draw_icon(ICON_X);
	}
	else if (btn.text == "ICON_MINUS") {
		_draw_icon(ICON_MINUS);
	}
	else if (btn.text == "ICON_CURVE") {
		_draw_icon(ICON_CURVE);
	}
	else if (btn.text == "ICON_FILL") {
		_draw_icon(ICON_FILL);
	}
	else if (btn.text == "ICON_PEN") {
		_draw_icon(ICON_PEN);
	}
	else if (btn.text == "ICON_SELECT") {
		_draw_icon(ICON_SELECT);
	}
	else {
		draw_text(
			gs,
			btn.text,
			vec2_add(pos, vec2_new(4, 3)),
			btn.sz.x - 8,
			TEXT_SCALE,
			color,
			vec2_new(4, 3),
			flip_color
		);
	}
}
