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
#include "../game_time.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../consts.h"

#include "../draw_tool/draw_tool_px.h"
#include "../draw_tool/draw_tool_line.h"

#include <cmath>

namespace {

const int PALLETE_DRAW = 0;
const int PALLETE_TOOL_PREVIEW = 1;

const float LAYER_TEXTAREA_LIST_LINE_HEIGHT = 12;

int get_blank_index(const std::vector<Tab> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

Vec2 get_tex_draw_mouse_pos(const Tab &tab, Vec2 main_fb_tab_pos,
Vec2 main_fb_mouse_pos) {
	Vec2 result = vec2_sub(main_fb_mouse_pos, main_fb_tab_pos);
	result = vec2_div(result, tab.px_scale);
	return result;
}

void pallete_data_color(Tab &tab, int pallete_index, Color color) {
	int index = pallete_index * 4;
	tab.pallete_data[index    ] = color.r * 255;
	tab.pallete_data[index + 1] = color.g * 255;
	tab.pallete_data[index + 2] = color.b * 255;
	tab.pallete_data[index + 3] = color.a * 255;
}

void tool_preview_pallete_data_color(Tab &tab, int pallete_index, Color color){
	int index = pallete_index * 4;
	tab.tool_preview_pallete_data[index    ] = color.r * 255;
	tab.tool_preview_pallete_data[index + 1] = color.g * 255;
	tab.tool_preview_pallete_data[index + 2] = color.b * 255;
	tab.tool_preview_pallete_data[index + 3] = color.a * 255;
}

void layer_texture_draw(const Tab &tab, const GraphicStuff &gs,
int texture_index, int pallete, Vec2 pos) {
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
		"u_draw_texture", 0, texture_get_id(gs, texture_index));
	if (pallete == PALLETE_DRAW) {
		set_uniform_texture(gs, SHADER_LAYER_DRAW,
			"u_pallete_texture", 1,
			texture_get_id(gs, tab.pallete_texture_index));
	}
	else if (pallete == PALLETE_TOOL_PREVIEW) {
		set_uniform_texture(gs, SHADER_LAYER_DRAW,
			"u_pallete_texture", 1,
			texture_get_id(gs, tab.tool_preview_pallete_texture_index));
	}

	mesh_draw(gs, MESH_RECT);
}

void layer_list_draw(const Tab &tab, GraphicStuff &gs, Vec2 pos) {
	for (int i = (int)tab.layer_order_list.size() - 1; i >= 0; i--) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
		layer_texture_draw(tab, gs, layer.texture_index, PALLETE_DRAW,
			vec2_floor(pos));
	}

	layer_texture_draw(tab, gs, tab.tool_preview_texture_index,
		PALLETE_TOOL_PREVIEW, pos);
}

//void tool_preview_data_update(Tab &tab, GraphicStuff &gs, const Input &input,
//Vec2 parent_pos) {
//	Vec2 pos = vec2_add(parent_pos, tab.pos);
//	
//	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
//	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
//	Vec2 tex_draw_mouse_pos
//		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
//
//	if (!in_rect(main_fb_mouse_pos, vec2_new(0, 0), main_fb_sz)) {
//		return;
//	}
//
//	if (input.left_down && input.mouse_move) {
//		draw_tool_line(
//			tab.tool_preview_data,
//			tab.sz,
//			get_tex_draw_mouse_pos(tab, pos, tab.mouse_click_pos),
//			tex_draw_mouse_pos,
//			1
//		);
//		texture_data_red(gs, tab.tool_preview_texture_index,
//			tab.sz, tab.tool_preview_data);
//		draw_tool_line(
//			tab.tool_preview_data,
//			tab.sz,
//			get_tex_draw_mouse_pos(tab, pos, tab.mouse_click_pos),
//			tex_draw_mouse_pos,
//			0
//		);
//	}
//}

int get_layer_index(const Tab &tab) {
	return tab.layer_order_list[tab.layer_order_list_index];
}

void layer_list_data_update(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, Vec2 parent_pos) {
	if (input.key_list[KEY_SPACE].down) {
		return;
	}

	Vec2 pos = vec2_add(parent_pos, tab.pos);
	int pallete_index = tab.color_pallete.selected_index;
	
	Layer &layer = tab.layer_list[get_layer_index(tab)];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (!in_rect(main_fb_mouse_pos, vec2_new(0, 0), main_fb_sz)) {
		return;
	}

	bool drawn = false;

	if (input.left_click) {
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;
		draw_tool_px(layer.data, tab.sz,
			to_vec2i(tex_draw_mouse_pos), pallete_index);

		drawn = true;
	}

	if (input.left_down && input.mouse_move) {
		draw_tool_line(layer.data, tab.sz, tab.tex_draw_tag_pos,
			tex_draw_mouse_pos, pallete_index);
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;

		drawn = true;
	}

	if (drawn) {
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
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (in_rect(tex_draw_mouse_pos, vec2_new(0, 0), to_vec2(tab.sz))) {
		draw_rect(
			gs,
			vec2_new(
				std::floor(pos.x) + std::floor(tex_draw_mouse_pos.x) * tab.px_scale,
				std::floor(pos.y) + std::floor(tex_draw_mouse_pos.y) * tab.px_scale
			),
			vec2_new(tab.px_scale, tab.px_scale),
			color_new(0, 0, 0, 1)
		);
	}
}

void tab_layer_new(Tab &tab, GraphicStuff &gs) {
	std::vector<unsigned char> data;
	data.resize(tab.sz.x * tab.sz.y, 0);
	int index = layer_new(tab.layer_list, gs, "bkg", tab.sz, data);
	
	tab.layer_order_list.push_back(index);
}

void layer_textarea_list_update(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, const Input &input, Vec2 parent_pos, bool show) {
	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(tab.layer_bar.pos.x,
			tab.layer_bar.pos.y + LAYER_TEXTAREA_LIST_LINE_HEIGHT * i);
		layer_textarea_update(
			layer,
			gs,
			game_time,
			input,
			vec2_add(parent_pos, add),
			i == tab.layer_order_list_index && tab.tab_name_editing,
			show
		);
	}

	if (input.left_click) {
		tab.tab_name_editing = false;
	}

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
	
		if (layer.textarea.clicked) {
			if (tab.layer_order_list_index == i) {
				tab.tab_name_editing = true;
				gs.cursor_icon = CURSOR_TEXT;
				return;
			}

			tab.layer_order_list_index = i;
			return;
		}

		if (layer.delete_btn.clicked && tab.layer_order_list.size() > 1) {
			layer_release(tab.layer_list, gs, index);
			tab.layer_order_list.erase(tab.layer_order_list.begin() + i);

			if (tab.layer_order_list_index > 0
			&& i <= tab.layer_order_list_index) {
				tab.layer_order_list_index--;
			}

			return;
		}

		if (layer.textarea.hovered && tab.layer_order_list_index == i) {
			gs.cursor_icon = CURSOR_TEXT;
			return;
		}
	}
}

void layer_textarea_list_draw(const Tab &tab, GraphicStuff &gs,
const GameTime &game_time, Vec2 parent_pos) {
	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(tab.layer_bar.pos.x,
			tab.layer_bar.pos.y + LAYER_TEXTAREA_LIST_LINE_HEIGHT * i);
		layer_textarea_draw(layer, gs, game_time,
			vec2_add(parent_pos, add),
			i == tab.layer_order_list_index && tab.tab_name_editing,
			i == tab.layer_order_list_index);
	}
}

void canvas_move_update(Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	if (input.key_list[KEY_SPACE].down && input.left_down && input.mouse_move){
		Vec2 pos = vec2_add(parent_pos, tab.pos);

		Vec2 main_fb_mouse_pos
			= get_main_fb_mouse_pos(gs, input.mouse_pos);
		Vec2 prev_main_fb_mouse_pos
			= get_main_fb_mouse_pos(gs, input.prev_mouse_pos);
		Vec2 tex_draw_mouse_pos
			= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
		Vec2 prev_tex_draw_mouse_pos
			= get_tex_draw_mouse_pos(tab, pos, prev_main_fb_mouse_pos);

		Vec2 mouse_diff = vec2_sub(
			tex_draw_mouse_pos,
			prev_tex_draw_mouse_pos
		);
		tab.pos = vec2_add(tab.pos, vec2_mul(mouse_diff, tab.px_scale));
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

	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	tab.pos = pos;
	tab.sz = sz;
	tab.px_scale = px_scale;

	tab.color_picker = color_picker_new(vec2_new(SIDE_BAR_W + 9, 207));
	tab.color_pallete = color_pallete_new(vec2_new(SIDE_BAR_W + 32, 8));
	tab.layer_bar = layer_bar_new(vec2_new(4, main_fb_sz.y - 100 - 4),
	                              vec2_new(100, 100));
	tab.tool_picker = tool_picker_new(vec2_new(SIDE_BAR_W + 63, 8));

	tab.pallete_data.resize(16 * 16, 1);
	pallete_data_color(tab, 0, color_new(0, 0, 0, 0));
	tab.pallete_texture_index = texture_blank_new(gs, 16, 16);
	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);

	tab.tool_preview_pallete_data.resize(16 * 16, 1);
	tool_preview_pallete_data_color(tab, 0, color_new(0, 0, 0, 0));
	tool_preview_pallete_data_color(tab, 1, color_new(0, 0, 0, 1));
	tab.tool_preview_pallete_texture_index = texture_blank_new(gs, 16, 16);
	texture_data(gs, tab.tool_preview_pallete_texture_index,
		vec2i_new(16, 16), tab.tool_preview_pallete_data);

	tab.tool_preview_data.resize(sz.x * sz.y, 0);
	tab.tool_preview_texture_index = texture_blank_new_red(gs, sz.x, sz.y);
	texture_data_red(gs, tab.tool_preview_texture_index, sz,
		tab.tool_preview_data);

	tab_layer_new(tab, gs);

	tab.running = true;

	return index;
}

void tab_update(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, Vec2 parent_pos, bool show) {
	color_picker_update(tab.color_picker, gs, input, parent_pos, show);
	color_pallete_update(tab.color_pallete, gs, input, parent_pos, show);
	layer_bar_update(tab.layer_bar, gs, input, parent_pos, show);
	tool_picker_update(tab.tool_picker, gs, input, parent_pos,
		!tab.tab_name_editing, show);

	layer_textarea_list_update(tab, gs, game_time, input, parent_pos, show);

	if (!show) {
		return;
	}

	if (tab.layer_bar.add_btn.clicked) {
		tab_layer_new(tab, gs);
	}

	canvas_move_update(tab, gs, input, parent_pos);

	color_picker_color_pallete_data_update(tab, gs);

	layer_list_data_update(tab, gs, input, game_time, parent_pos);
}

void tab_blur_rects_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos) {
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

	draw(
		vec2_sub(tab.color_pallete.pos, vec2_new(24 + 3, 3)),
		vec2_add(COLOR_PALLETE_SZ, vec2_new(24 + 6, 6))
	);
	draw(
		vec2_sub(tab.color_picker.pos, vec2_new(4, 4)),
		vec2_add(color_picker_get_sz(tab.color_picker), vec2_new(8, 8))
	);
	draw(
		vec2_sub(tab.tool_picker.pos, vec2_new(3, 3)),
		vec2_add(TOOL_PICKER_SZ, vec2_new(5, 6))
	);
}

void tab_canvas_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	layer_list_draw(tab, gs, pos);
	px_cursor_draw(tab, gs, input, parent_pos);
}

void tab_ui_draw(const Tab &tab, GraphicStuff &gs, const GameTime &game_time,
Vec2 parent_pos) {
	layer_textarea_list_draw(tab, gs, game_time, parent_pos);

	color_picker_draw(tab.color_picker, gs, parent_pos);
	color_pallete_draw(tab.color_pallete, gs, parent_pos);
	layer_bar_draw(tab.layer_bar, gs, parent_pos);
	tool_picker_draw(tab.tool_picker, gs, parent_pos);
}

void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index) {
	Tab &tab = tab_list[index];
	
	tab.pallete_data.clear();
	texture_release(gs, tab.pallete_texture_index);
	tab.tool_preview_pallete_data.clear();
	texture_release(gs, tab.tool_preview_pallete_texture_index);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		if (tab.layer_list[i].running) {
			layer_release(tab.layer_list, gs, i);
		}
	}

	tab.running = false;
}
