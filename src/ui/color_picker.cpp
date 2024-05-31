#include "color_picker.h"

#include "../types/vec2.h"
#include "../types/color.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../input.h"

namespace {

void draw_pos_select_bkg(const ColorPicker &color_picker,
const GraphicStuff &gs) {
	Vec2 fb_sz_f = to_vec2(fb_get_sz(gs, FRAMEBUFFER_MAIN));
	Vec2 pos = color_picker.pos_select.pos;
	Vec2 sz = color_picker.pos_select.sz;
	
	Vec2 pos_normalized = vec2_new(pos.x / fb_sz_f.x, pos.y / fb_sz_f.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_new(sz.x / fb_sz_f.x, sz.y / fb_sz_f.y);
	sz_normalized = vec2_mul(sz_normalized, 2);

	pos_normalized.y = -pos_normalized.y - sz_normalized.y;

	Color hsv = color_picker_get_hsv(color_picker);
	Color hue_rgb = hue_to_rgb(hsv.r);

	use_shader(gs, SHADER_COLOR_PICKER);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER,
		"u_pos",
		pos_normalized
	);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER,
		"u_sz",
		sz_normalized
	);
	set_uniform_color(
		gs,
		SHADER_COLOR_PICKER,
		"u_hue_rgb",
		hue_rgb
	);

	draw_mesh(gs, MESH_RECT);
}

}

ColorPicker color_picker_new(Vec2 pos) {
	ColorPicker color_picker;
	color_picker.pos = pos;
	
	Vec2 pos_select_sz = vec2_new(64, 64);
	color_picker.pos_select = pos_select_new(
		vec2_add(pos, vec2_new(10, 10)),
		pos_select_sz
	);

	return color_picker;
}

void color_picker_update(ColorPicker &color_picker,
const GraphicStuff &gs, const Input &input) {
	pos_select_update(color_picker.pos_select, gs, input);
}

void color_picker_draw(const ColorPicker &color_picker,
const GraphicStuff &gs) {
	draw_pos_select_bkg(color_picker, gs);
	pos_select_draw(color_picker.pos_select, gs);
}

Color color_picker_get_hsv(const ColorPicker &color_picker) {
	Vec2 selected_pos = pos_select_get_selected(color_picker.pos_select);

	Color color;
	color.r = 0.5;                // h
	color.g = selected_pos.x;     // s
	color.b = 1 - selected_pos.y; // v
	color.a = 1;

	return color;
}

Color color_picker_get_rgb(const ColorPicker &color_picker) {
	Color hsv = color_picker_get_hsv(color_picker);
	return hsv_to_rgb(hsv);
}
