#include "hue_slider.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../input.h"

#include "../types/color.h"
#include "../basic_math.h"
#include "../pos_convert.h"

HueSlider hue_slider_new(Vec2 pos, Vec2 sz) {
	HueSlider hue_slider;
	hue_slider.pos = pos;
	hue_slider.sz = sz;
	return hue_slider;
}

void hue_slider_update(HueSlider &hue_slider,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, hue_slider.pos);

	if (input.left_release) {
		hue_slider.sliding = false;
	}

	if (!show) {
		hue_slider.sliding = false;
		return;
	}

	if (hue_slider.sliding) {
		hue_slider.slide_percentage
			= (mouse_pos.x - pos.x) / hue_slider.sz.x;
		hue_slider.slide_percentage = clampf(hue_slider.slide_percentage, 0,1);
	}

	if (!in_rect(mouse_pos, vec2_new(0, 0), to_vec2(main_fb_sz))) {
		return;
	}

	if (!in_rect(mouse_pos, pos, hue_slider.sz)) {
		return;
	}

	if (input.left_click) {
		hue_slider.sliding = true;
	}
}

void hue_slider_draw(const HueSlider &hue_slider,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, hue_slider.pos);

	draw_rect(
		gs,
		vec2_new(
			pos.x + hue_slider.slide_percentage * hue_slider.sz.x,
			pos.y
		),
		vec2_new(1, hue_slider.sz.y),
		color_new(0, 0, 0, 1)
	);
}
