#include "tab.h"

#include "../types/color.h"

#include "../graphic_types/texture.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../graphic/draw_texture.h"

#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../pos_convert.h"
#include "../basic_math.h"

namespace {

int get_blank_index(const std::vector<Tab> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
Vec2 pos, Vec2i sz, int px_scale) {
	int index = get_blank_index(tab_list);
	if (index == -1) {
		Tab new_tab;
		tab_list.push_back(new_tab);

		index = (int)tab_list.size() - 1;
	}
	Tab &tab = tab_list[index];

	tab.pos = pos;
	tab.sz = sz;
	tab.px_scale = px_scale;
	tab.data.resize(sz.x * sz.y * 4);
	tab.color_picker = color_picker_new(vec2_new(50, 10));
	tab.color_pallete = color_pallete_new(vec2_new(300, 10));
	tab.texture_index = texture_blank_new(gs, sz.x, sz.y);

	return index;
}

void px(Tab &tab, Vec2i pos, Color color) {
	int index = tab.sz.y * pos.y * 4 + pos.x * 4;
	tab.data[index    ] = color.r * 255;
	tab.data[index + 1] = color.g * 255;
	tab.data[index + 2] = color.b * 255;
	tab.data[index + 3] = color.a * 255;
}

void tab_update(Tab &tab, GraphicStuff &gs, const Input &input) {
	color_picker_update(tab.color_picker, gs, input);
	color_pallete_update(tab.color_pallete, gs, input);
	
	Color rgb = color_picker_get_rgb(tab.color_picker);

	if (tab.color_picker.color_changed) {
		int index = tab.color_pallete.selected_index;
		tab.color_pallete.color_list[index] = rgb;
	}
	if (tab.color_pallete.selection_changed) {
		int index = tab.color_pallete.selected_index;
		Color color = tab.color_pallete.color_list[index];
		color_picker_set_rgb(tab.color_picker, color);
	}


	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2i tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, main_fb_mouse_pos);

	if (in_rect(to_vec2(tex_draw_mouse_pos),vec2_new(0, 0),to_vec2(tab.sz))) {
		if ((input.left_down && input.mouse_move) || input.left_click) {
			px(tab, tex_draw_mouse_pos, rgb);
			texture_data(gs, tab.texture_index, tab.sz, tab.data);
		}
	}
}

void tab_draw(const Tab &tab, const GraphicStuff &gs, const Input &input) {
	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	Vec2i tex_draw_sz = texture_get_sz(gs, tab.texture_index);

	draw_texture(
		gs,
		main_fb_sz,
		tex_draw_sz,
		
		vec2_new(0, 0),
		to_vec2(tex_draw_sz),
		tab.pos,
		vec2_mul(
			to_vec2(tex_draw_sz), tab.px_scale
		),
		
		texture_get_id(gs, tab.texture_index),
		true
	);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2i tex_draw_mouse_pos = get_tex_draw_mouse_pos(tab, main_fb_mouse_pos);

	if (in_rect(to_vec2(tex_draw_mouse_pos),vec2_new(0, 0),to_vec2(tab.sz))) {
		draw_rect_sz(
			gs,
			main_fb_sz,
			vec2_new(
				floor2(main_fb_mouse_pos.x, tab.px_scale),
				floor2(main_fb_mouse_pos.y, tab.px_scale)
			),
			vec2_new(tab.px_scale, tab.px_scale),
			color_new(0, 0, 0, 1)
		);
	}

	color_picker_draw(tab.color_picker, gs);
	color_pallete_draw(tab.color_pallete, gs);
}

void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index) {
	Tab &tab = tab_list[index];
	tab.data.clear();
	texture_release(gs, tab.texture_index);
	tab.running = false;
}
