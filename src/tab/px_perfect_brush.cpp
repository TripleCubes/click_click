#include "px_perfect_brush.h"

#include <cmath>

#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "tab.h"
#include "layer.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../draw_tool/draw_tool_px.h"

#include <iostream>

namespace {

Vec2i prev_point = vec2i_new(-1, -1);
Vec2i prev_prev_point = vec2i_new(-1, -1);

bool px_near(Vec2i pos_a, Vec2i pos_b) {
	return std::abs(pos_a.x - pos_b.x) <= 1 && std::abs(pos_a.y - pos_b.y) <= 1;
}

void px(Vec2i pos, Vec2i canvas_sz, Tab &tab, Layer &layer,
unsigned char pallete_index) {
	auto draw = [&layer, &tab, pallete_index](Vec2i pos) {
		draw_tool_px(
			layer.data,
			tab.selection,
			tab.sz,
			pos,
			pallete_index,
			0
		);
	};
	
	if (pos.x < 0 || pos.y < 0 || pos.x >= canvas_sz.x || pos.y >= canvas_sz.y){
		draw(prev_point);
		draw(prev_prev_point);
		prev_point = vec2i_new(-1, -1);
		prev_prev_point = vec2i_new(-1, -1);
		return;
	}
	if (prev_point.x == -1) {
		prev_point = pos;
		return;
	}
	if (prev_prev_point.x == -1) {
		prev_prev_point = prev_point;
		draw(prev_prev_point);
		prev_point = pos;
		return;
	}
	if (px_near(pos, prev_prev_point)) {
		prev_point = pos;
		return;
	}
	prev_prev_point = prev_point;
	draw(prev_prev_point);
	prev_point = pos;
}

void line(Vec2i pos, Vec2i canvas_sz, Tab &tab, Layer &layer,
unsigned char pallete_index) {
	if (prev_point.x == -1) {
		px(pos, canvas_sz, tab, layer, pallete_index);
		return;
	}

	if (vec2i_equals(pos, prev_point)) {
		return;
	}

	Vec2 pos_f = vec2_add(to_vec2(pos), vec2_new(0.5, 0.5));
	Vec2 prev_point_f = vec2_add(to_vec2(prev_point), vec2_new(0.5, 0.5));
	Vec2 diff = vec2_sub(pos_f, prev_point_f);
	float dist_sqr = vec2_length_sqr(diff);
	Vec2 add = vec2_mul(vec2_normalized(diff), 0.5);
	Vec2 current = prev_point_f;

	while (vec2_dist_sqr(prev_point_f, current) < dist_sqr) {
		current = vec2_add(current, add);
		px(to_vec2i(current), canvas_sz, tab, layer, pallete_index);
	}
}

}

void px_perfect_brush_tool_preview_update(Tab &tab, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
	Vec2i px_pos = to_vec2i(tex_draw_mouse_pos);

	draw_tool_px(
		tab.tool_preview_data,
		tab.selection,
		tab.sz,
		px_pos,
		1,
		0
	);
	draw_tool_px(
		tab.tool_preview_data,
		tab.selection,
		tab.sz,
		prev_point,
		1,
		0
	);
	texture_data_red(gs, tab.tool_preview_texture_index,
		tab.sz, tab.tool_preview_data);
	draw_tool_px(
		tab.tool_preview_data,
		tab.selection,
		tab.sz,
		px_pos,
		0,
		0
	);
	draw_tool_px(
		tab.tool_preview_data,
		tab.selection,
		tab.sz,
		prev_point,
		0,
		0
	);
}

void px_perfect_brush_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	int pallete_index = tab.color_pallete.selected_index;
	Layer &layer = tab.layer_list[layer_index];
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
	Vec2i px_pos = to_vec2i(tex_draw_mouse_pos);

	if (input.left_down && input.mouse_move) {
		line(px_pos, tab.sz, tab, layer, pallete_index);
		layer_set_texture_data(layer, gs);
	}

	if (input.left_release) {
		if (prev_point.x != -1) {
			draw_tool_px(
				layer.data,
				tab.selection,
				tab.sz,
				px_pos,
				pallete_index,
				0
			);
		}
		draw_tool_px(
			layer.data,
			tab.selection,
			tab.sz,
			prev_point,
			pallete_index,
			0
		);
		layer_set_texture_data(layer, gs);
		prev_point = vec2i_new(-1, -1);
		prev_prev_point = vec2i_new(-1, -1);
	}
}
