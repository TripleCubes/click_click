#include "btn.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"
#include "../input.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"

#include "../basic_math.h"
#include "../pos_convert.h"

Btn btn_new(Vec2 pos, Vec2 sz, Color color, const std::string &text) {
	Btn btn;
	btn.pos = pos;
	btn.sz = sz;
	btn.color = color;
	btn.text = text;

	return btn;
}

void btn_update(Btn &btn, const GraphicStuff &gs, const Input &input) {
	btn.hovered = false;
	btn.clicked = false;
	btn.released = false;

	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	if (input.left_release) {
		if (btn.holding) {
			btn.released = true;
		}
		btn.holding = false;
	}

	if (!in_rect(mouse_pos, vec2_new(0, 0), to_vec2(main_fb_sz))) {
		return;
	}

	if (!in_rect(mouse_pos, btn.pos, btn.sz)) {
		return;
	}

	btn.hovered = true;

	if (input.left_click) {
		btn.holding = true;
		btn.clicked = true;
	}
}

void btn_draw(const Btn &btn, const GraphicStuff &gs) {
	Color color = btn.color;
	if (btn.holding) {
		color = color_sub(color, color_new(0.1, 0.1, 0.1, 0));
	}
	else if (btn.hovered) {
		color = color_add(color, color_new(0.1, 0.1, 0.1, 0));
	}

	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	draw_rect_sz(
		gs,
		main_fb_sz,
		btn.pos,
		btn.sz,
		color
	);
}
