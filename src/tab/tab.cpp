#include "tab.h"

#include "../types/color.h"

#include "../graphic_types/texture.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/mesh.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../graphic/draw_texture.h"

#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../pos_convert.h"
#include "../basic_math.h"

namespace {

const Vec2 LAYER_BTN_LIST_POS = vec2_new(10, 100);
const float LAYER_BTN_LIST_LINE_HEIGHT = 13;

int get_blank_index(const std::vector<Tab> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

void pallete_data_color(Tab &tab, int pallete_index, Color color) {
	int index = pallete_index * 4;
	tab.pallete_data[index    ] = color.r * 255;
	tab.pallete_data[index + 1] = color.g * 255;
	tab.pallete_data[index + 2] = color.b * 255;
	tab.pallete_data[index + 3] = color.a * 255;
}

void draw_layer(const Tab &tab, const Layer &layer, const GraphicStuff &gs,
Vec2 pos) {
	Vec2 main_fb_sz_f = to_vec2(fb_get_sz(gs, FB_MAIN));
	Vec2 sz_f = to_vec2(tab.sz);
	Vec2 pos_normalized = vec2_new(
		pos.x / main_fb_sz_f.x,
		pos.y / main_fb_sz_f.y
	);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));
	Vec2 sz_normalized = vec2_new(
		sz_f.x / main_fb_sz_f.x * 2 * tab.px_scale,
		sz_f.y / main_fb_sz_f.y * 2 * tab.px_scale
	);
	pos_normalized.y = - pos_normalized.y - sz_normalized.y;

	use_shader(gs, SHADER_LAYER_DRAW);
	set_uniform_vec2(gs, SHADER_LAYER_DRAW, "u_pos", pos_normalized);
	set_uniform_vec2(gs, SHADER_LAYER_DRAW, "u_sz", sz_normalized);
	set_uniform_texture(gs, SHADER_LAYER_DRAW,
		"u_draw_texture", 0, texture_get_id(gs, layer.texture_index));
	set_uniform_texture(gs, SHADER_LAYER_DRAW,
		"u_pallete_texture", 1, texture_get_id(gs, tab.pallete_texture_index));

	mesh_draw(gs, MESH_RECT);
}

void layer_list_draw(const Tab &tab, GraphicStuff &gs, Vec2 pos) {
	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		draw_layer(tab, tab.layer_list[i], gs, pos);
	}
}

int get_layer_index(const Tab &tab) {
	return tab.layer_order_list[tab.layer_order_list_index];
}

void layer_list_data_update(Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	int pallete_index = tab.color_pallete.selected_index;
	
	Layer &layer = tab.layer_list[get_layer_index(tab)];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
	Vec2i tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (!in_rect(main_fb_mouse_pos, vec2_new(0, 0), main_fb_sz)) {
		return;
	}

	if (!in_rect(to_vec2(tex_draw_mouse_pos),vec2_new(0, 0),to_vec2(tab.sz))) {
		return;
	}

	if ((input.left_down && input.mouse_move) || input.left_click) {
		layer_data(layer, tex_draw_mouse_pos, pallete_index);
		layer_set_texture_data(layer, gs);
	}
}

void color_picker_color_pallete_data_update(Tab &tab, GraphicStuff &gs) {
	int pallete_index = tab.color_pallete.selected_index;

	if (tab.color_picker.color_changed) {
		Color color = color_picker_get_rgb(tab.color_picker);
		tab.color_pallete.color_list[pallete_index] = color;

		pallete_data_color(tab, pallete_index, color);
		texture_data(gs, tab.pallete_texture_index,
			vec2i_new(16, 16), tab.pallete_data);
	}

	if (tab.color_pallete.selection_changed) {
		Color color = tab.color_pallete.color_list[pallete_index];
		color_picker_set_rgb(tab.color_picker, color);
	}
}

void px_cursor_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2i tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (in_rect(to_vec2(tex_draw_mouse_pos),vec2_new(0, 0),to_vec2(tab.sz))) {
		draw_rect(
			gs,
			vec2_new(
				floor2(main_fb_mouse_pos.x, tab.px_scale),
				floor2(main_fb_mouse_pos.y, tab.px_scale)
			),
			vec2_new(tab.px_scale, tab.px_scale),
			color_new(0, 0, 0, 1)
		);
	}
}

void tab_layer_new(Tab &tab, GraphicStuff &gs, Vec2i sz) {
	std::vector<unsigned char> data;
	data.resize(sz.x * sz.y, 0);
	int index = layer_new(tab.layer_list, gs, "bkg", sz, data);
	
	tab.layer_order_list.push_back(index);
}

void layer_btn_list_update(Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool show) {
	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(LAYER_BTN_LIST_POS.x,
			LAYER_BTN_LIST_POS.y + LAYER_BTN_LIST_LINE_HEIGHT * i);
		layer_btn_update(layer, gs, input,
			vec2_add(parent_pos, add), show);
	}

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
	
		if (layer.btn.clicked) {
			tab.layer_order_list_index = i;
			return;
		}
	}
}

void layer_btn_list_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos) {
	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(LAYER_BTN_LIST_POS.x,
		    LAYER_BTN_LIST_POS.y + LAYER_BTN_LIST_LINE_HEIGHT * i);
		layer_btn_draw(layer, gs,
			vec2_add(parent_pos, add),
			i == tab.layer_order_list_index);
	}
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
	tab.color_picker = color_picker_new(vec2_new(50, 10));
	tab.color_pallete = color_pallete_new(vec2_new(300, 10));

	tab.pallete_data.resize(16 * 16, 1);
	pallete_data_color(tab, 0, color_new(0, 0, 0, 0));
	tab.pallete_texture_index = texture_blank_new(gs, 16, 16);
	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);

	tab_layer_new(tab, gs, sz);
	tab_layer_new(tab, gs, sz);

	return index;
}

void tab_update(Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos, bool show) {
	color_picker_update(tab.color_picker, gs, input, parent_pos, show);
	color_pallete_update(tab.color_pallete, gs, input, parent_pos, show);

	layer_btn_list_update(tab, gs, input, parent_pos, show);

	if (!show) {
		return;
	}

	color_picker_color_pallete_data_update(tab, gs);
	layer_list_data_update(tab, gs, input, parent_pos);
}

void tab_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	layer_list_draw(tab, gs, pos);

	px_cursor_draw(tab, gs, input, parent_pos);

	layer_btn_list_draw(tab, gs, parent_pos);

	color_picker_draw(tab.color_picker, gs, parent_pos);
	color_pallete_draw(tab.color_pallete, gs, parent_pos);
}

void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index) {
	Tab &tab = tab_list[index];
	
	tab.pallete_data.clear();
	texture_release(gs, tab.pallete_texture_index);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		if (tab.layer_list[i].running) {
			layer_release(tab.layer_list, gs, i);
		}
	}

	tab.running = false;
}
