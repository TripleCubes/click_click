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
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 fb_sz_f = to_vec2(fb_get_sz(gs, FB_MAIN));
	Vec2 pos = vec2_add(color_picker.pos_select.pos, parent_pos);
	Vec2 sz = color_picker.pos_select.sz;
	
	Vec2 pos_normalized = vec2_new(pos.x / fb_sz_f.x, pos.y / fb_sz_f.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_new(sz.x / fb_sz_f.x, sz.y / fb_sz_f.y);
	sz_normalized = vec2_mul(sz_normalized, 2);

	pos_normalized.y = -pos_normalized.y - sz_normalized.y;

	float hue = color_picker.hue_slider.slide_percentage;
	Color hue_rgb = hue_to_rgb(hue);

	use_shader(gs, SHADER_COLOR_PICKER_POS_SELECT);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER_POS_SELECT,
		"u_pos",
		pos_normalized
	);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER_POS_SELECT,
		"u_sz",
		sz_normalized
	);
	set_uniform_color(
		gs,
		SHADER_COLOR_PICKER_POS_SELECT,
		"u_hue_rgb",
		hue_rgb
	);

	mesh_draw(gs, MESH_RECT);
}

void draw_hue_slider_bkg(const ColorPicker &color_picker,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 fb_sz_f = to_vec2(fb_get_sz(gs, FB_MAIN));
	Vec2 pos = vec2_add(color_picker.hue_slider.pos, parent_pos);
	Vec2 sz = color_picker.hue_slider.sz;
	
	Vec2 pos_normalized = vec2_new(pos.x / fb_sz_f.x, pos.y / fb_sz_f.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_new(sz.x / fb_sz_f.x, sz.y / fb_sz_f.y);
	sz_normalized = vec2_mul(sz_normalized, 2);

	pos_normalized.y = -pos_normalized.y - sz_normalized.y;

	use_shader(gs, SHADER_COLOR_PICKER_HUE_SLIDER);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER_HUE_SLIDER,
		"u_pos",
		pos_normalized
	);
	set_uniform_vec2(
		gs,
		SHADER_COLOR_PICKER_HUE_SLIDER,
		"u_sz",
		sz_normalized
	);

	mesh_draw(gs, MESH_RECT);
}

}

ColorPicker color_picker_new(Vec2 pos) {
	ColorPicker color_picker;
	color_picker.pos = pos;
	
	color_picker.pos_select = pos_select_new(
		vec2_add(pos, vec2_new(10, 10)),
		vec2_new(64, 64)
	);

	color_picker.hue_slider = hue_slider_new(
		vec2_add(pos, vec2_new(10, 84)),
		vec2_new(64, 10)
	);

	return color_picker;
}

void color_picker_update(ColorPicker &color_picker,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	Vec2 pos = vec2_add(color_picker.pos, parent_pos);
	pos_select_update(color_picker.pos_select, gs, input, pos, show);
	hue_slider_update(color_picker.hue_slider, gs, input, pos, show);

	color_picker.color_changed = false;

	if (!show) {
		return;
	}

	if (color_picker.pos_select.selected) {
		color_picker.color_changed = true;
	}
	if (color_picker.hue_slider.sliding) {
		color_picker.color_changed = true;
	}
}

void color_picker_draw(const ColorPicker &color_picker,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 pos = vec2_add(color_picker.pos, parent_pos);
	draw_pos_select_bkg(color_picker, gs, pos);
	draw_hue_slider_bkg(color_picker, gs, pos);

	pos_select_draw(color_picker.pos_select, gs, pos);
	hue_slider_draw(color_picker.hue_slider, gs, pos);
}

Color color_picker_get_hsv(const ColorPicker &color_picker) {
	Vec2 selected_pos = pos_select_get_selected(color_picker.pos_select);

	Color color;
	color.r = color_picker.hue_slider.slide_percentage; // h
	color.g = selected_pos.x;                           // s
	color.b = 1 - selected_pos.y;                       // v
	color.a = 1;

	return color;
}

Color color_picker_get_rgb(const ColorPicker &color_picker) {
	Color hsv = color_picker_get_hsv(color_picker);
	return hsv_to_rgb(hsv);
}

void color_picker_set_rgb(ColorPicker &color_picker, Color rgb) {
	Color hsv = rgb_to_hsv(rgb);

	color_picker.hue_slider.slide_percentage = hsv.r;
	color_picker.pos_select.selected_pos.x
		= hsv.g * color_picker.pos_select.sz.x;
	color_picker.pos_select.selected_pos.y
		= (1 - hsv.b) * color_picker.pos_select.sz.y;
}
