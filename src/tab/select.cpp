#include "select.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "../game_time.h"
#include "tab.h"
#include "../pos_convert.h"
#include "../draw_tool/draw_tool_px.h"

#include <cmath>
#include <algorithm>
#include <array>

#include <iostream>

namespace {

void set_map_area(Selection &selection, Vec2 pos) {
	if (pos.x < selection.top_left.x) {
		selection.top_left.x = pos.x;
	}
	if (pos.y < selection.top_left.y) {
		selection.top_left.y = pos.y;
	}
	if (pos.x > selection.bottom_right.x) {
		selection.bottom_right.x = pos.x;
	}
	if (pos.y > selection.bottom_right.y) {
		selection.bottom_right.y = pos.y;
	}
}

void set_map_area_all(Selection &selection, Vec2 pos) {
	if (pos.x < selection.top_left_all.x) {
		selection.top_left_all.x = pos.x;
	}
	if (pos.y < selection.top_left_all.y) {
		selection.top_left_all.y = pos.y;
	}
	if (pos.x > selection.bottom_right_all.x) {
		selection.bottom_right_all.x = pos.x;
	}
	if (pos.y > selection.bottom_right_all.y) {
		selection.bottom_right_all.y = pos.y;
	}
}

void add_point(Selection &selection, Vec2i canvas_sz,Vec2 pos){
	int sz = (int)selection.point_list.size();
	SelectPoint prev_point = selection.point_list[sz - 1];

	if (pos.x < 0) {
		pos.x = 0;
	}
	if (pos.x >= canvas_sz.x) {
		pos.x = canvas_sz.x - 1;
	}
	if (pos.y < 0) {
		pos.y = 0;
	}
	if (pos.y >= canvas_sz.y) {
		pos.y = canvas_sz.y - 1;
	}

	if (std::floor(prev_point.pos.x) != std::floor(pos.x)
	|| std::floor(prev_point.pos.y) != std::floor(pos.y)) {
		SelectPoint point;
		point.pos = pos;
		selection.point_list.push_back(point);

		set_map_area(selection, point.pos);
		set_map_area_all(selection, point.pos);
	}
}

void line(Selection &selection, Vec2i canvas_sz, Vec2 pos_1, Vec2 pos_2) {
	if (pos_1.x == pos_2.x && pos_1.y == pos_2.y) {
		return;
	}

	float pos_dist_sqr = vec2_dist_sqr(pos_1, pos_2);
	Vec2 add = vec2_mul(vec2_normalized(vec2_sub(pos_2, pos_1)), 0.5);
	Vec2 current_pos = vec2_add(pos_1, add);

	while (vec2_dist_sqr(pos_1, current_pos) <= pos_dist_sqr) {
		add_point(selection, canvas_sz, current_pos);
		current_pos = vec2_add(current_pos, add);
	}
}

bool ink_cond(int ink) {
	if (ink % 10 < 6) {
		return false;
	}
	return true;
};

void preview_rect(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, unsigned char pallete_index) {
	int ink = game_time.time_since_start * 10;
	if (tab.selection.rect_top_left.x != -1) {
		Vec2i top_left = tab.selection.rect_top_left;
		Vec2i bottom_right = tab.selection.rect_bottom_right;
		if (top_left.x > bottom_right.x) {
			int swap = top_left.x;
			top_left.x = bottom_right.x;
			bottom_right.x = swap;
		}
		if (top_left.y > bottom_right.y) {
			int swap = top_left.y;
			top_left.y = bottom_right.y;
			bottom_right.y = swap;
		}

		for (int x = top_left.x; x <= bottom_right.x; x++) {
			ink++;
			if (!ink_cond(ink)) {
				continue;
			}

			draw_tool_px_no_selection(
				tab.selection_preview_data,
				tab.selection,
				tab.sz,
				vec2i_new(x, top_left.y),
				pallete_index,
				0
			);
		}

		for (int y = top_left.y; y <= bottom_right.y; y++) {
			ink++;
			if (!ink_cond(ink)) {
				continue;
			}

			draw_tool_px_no_selection(
				tab.selection_preview_data,
				tab.selection,
				tab.sz,
				vec2i_new(bottom_right.x, y),
				pallete_index,
				0
			);
		}

		for (int x = bottom_right.x; x >= top_left.x; x--) {
			ink++;
			if (!ink_cond(ink)) {
				continue;
			}

			draw_tool_px_no_selection(
				tab.selection_preview_data,
				tab.selection,
				tab.sz,
				vec2i_new(x, bottom_right.y),
				pallete_index,
				0
			);
		}

		for (int y = bottom_right.y; y >= top_left.y; y--) {
			ink++;

			if (!ink_cond(ink)) {
				continue;
			}

			draw_tool_px_no_selection(
				tab.selection_preview_data,
				tab.selection,
				tab.sz,
				vec2i_new(top_left.x, y),
				pallete_index,
				0
			);
		}
	}
}

void preview_free(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, unsigned char pallete_index) {
	int ink = game_time.time_since_start * 10;
	for (int i = 0; i < (int)tab.selection.point_list.size(); i++) {
		ink++;
		if (!ink_cond(ink)) {
			continue;
		}

		SelectPoint point = tab.selection.point_list[i];
		draw_tool_px_no_selection(
			tab.selection_preview_data,
			tab.selection,
			tab.sz,
			to_vec2i(point.pos),
			pallete_index,
			0
		);
	}
}

void preview_selected(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, unsigned char pallete_index) {
	int ink = game_time.time_since_start * 10;
	for (int i = 0; i < (int)tab.selection.point_list_2.size(); i++) {
		ink++;
		if (!ink_cond(ink)) {
			continue;
		}

		SelectPoint point = tab.selection.point_list_2[i];
		draw_tool_px_no_selection(
			tab.selection_preview_data,
			tab.selection,
			tab.sz,
			to_vec2i(point.pos),
			pallete_index,
			0
		);
	}
}

void preview_update(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, unsigned char pallete_index) {
	preview_rect(tab, gs, game_time, pallete_index);
	preview_free(tab, gs, game_time, pallete_index);
	preview_selected(tab, gs, game_time, pallete_index);
}

bool check(Vec2i pos, Vec2i pos_1, Vec2i pos_2) {
	if (pos_1.y == pos_2.y) {
		return false;
	}

	if (pos_1.y < pos_2.y && (pos_1.x != pos.x || pos_1.y != pos.y)) {
		return false;
	}

	if (pos_1.y > pos_2.y && (pos_2.x != pos.x || pos_2.y != pos.y)) {
		return false;
	}

	return true;
}

void px(std::vector<unsigned char> &map, Vec2i map_sz, Vec2i pos,
unsigned char val) {
	int i = pos.y * map_sz.y + pos.x;
	map[i] = val;
}

unsigned char get_px(const std::vector<unsigned char> &map, Vec2i map_sz,
Vec2i pos){
	int i = pos.y * map_sz.y + pos.x;
	return map[i];
}

void update_map_rect(Selection &selection, Vec2i map_sz, bool subtract) {
	Vec2i top_left = selection.rect_top_left;
	Vec2i bottom_right = selection.rect_bottom_right;
	if (top_left.x > bottom_right.x) {
		int swap = top_left.x;
		top_left.x = bottom_right.x;
		bottom_right.x = swap;
	}
	if (top_left.y > bottom_right.y) {
		int swap = top_left.y;
		top_left.y = bottom_right.y;
		bottom_right.y = swap;
	}

	for (int y = top_left.y; y <= bottom_right.y; y++) {
	for (int x = top_left.x; x <= bottom_right.x; x++) {
		px(selection.map, map_sz, vec2i_new(x, y), subtract? 0 : 1);
	}
	}
}

void update_map_free(Selection &selection, Vec2i map_sz, bool subtract) {
	Vec2i top_left_i = to_vec2i(selection.top_left);
	Vec2i bottom_right_i = to_vec2i(selection.bottom_right);

	for (int y = top_left_i.y; y <= bottom_right_i.y; y++) {
	bool on = false;
	for (int x = top_left_i.x; x <= bottom_right_i.x; x++) {
		Vec2i pos = vec2i_new(x, y);

		SelectPoint first_point = selection.point_list[0];
		px(selection.map, map_sz, to_vec2i(first_point.pos), subtract? 0 : 1);
		for (int i = 1; i < (int)selection.point_list.size(); i++) {
			SelectPoint point = selection.point_list[i];
			SelectPoint prev_point = selection.point_list[i - 1];
			Vec2i pos_1 = to_vec2i(prev_point.pos);
			Vec2i pos_2 = to_vec2i(point.pos);

			px(selection.map, map_sz, pos_2, subtract? 0 : 1);

			if (check(pos, pos_1, pos_2)) {
				on = !on;
			}
		}

		if (on) {
			px(selection.map, map_sz, pos, subtract? 0 : 1);
		}
	}
	}
}

const std::array<Vec2i, 4> DIRS = {
	vec2i_new(-1,  0),
	vec2i_new( 1,  0),
	vec2i_new( 0, -1),
	vec2i_new( 0,  1),
};

const std::array<Vec2i, 8> DIRS8 = {
	vec2i_new(-1,  0),
	vec2i_new( 1,  0),
	vec2i_new( 0, -1),
	vec2i_new( 0,  1),
	vec2i_new(-1,  1),
	vec2i_new( 1, -1),
	vec2i_new(-1, -1),
	vec2i_new( 1,  1),
};

void fill(const std::vector<unsigned char> &map,
std::vector<unsigned char> &map_2, Vec2i map_sz, Vec2i pos) {
	bool border = false;

	for (int i = 0; i < (int)DIRS.size(); i++) {
		Vec2i next_pos = vec2i_add(pos, DIRS[i]);

		if (next_pos.x < 0 || next_pos.y < 0
		|| next_pos.x >= map_sz.x || next_pos.y >= map_sz.y) {
			border = true;
		}

		if (get_px(map, map_sz, next_pos) == 0) {
			border = true;
		}
	}

	if (border) {
		px(map_2, map_sz, pos, 2);
	}
	else {
		px(map_2, map_sz, pos, 1);
	}

	for (int i = 0; i < (int)DIRS.size(); i++) {
		Vec2i next_pos = vec2i_add(pos, DIRS[i]);
		
		if (next_pos.x < 0 || next_pos.y < 0
		|| next_pos.x >= map_sz.x || next_pos.y >= map_sz.y) {
			continue;
		}

		if (get_px(map_2, map_sz, next_pos) == 0
		&& get_px(map, map_sz, next_pos) == 1) {
			fill(map, map_2, map_sz, next_pos);
		}
	}
}

void fill_border(std::vector<unsigned char> &map_2,
std::vector<SelectPoint> &point_list_2, Vec2i map_sz, Vec2i pos) {
	px(map_2, map_sz, pos, 3);
	
	SelectPoint point;
	point.pos = to_vec2(pos);
	point_list_2.push_back(point);

	for (int i = 0; i < (int)DIRS8.size(); i++) {
		Vec2i next_pos = vec2i_add(pos, DIRS8[i]);

		if (next_pos.x < 0 || next_pos.y < 0
		|| next_pos.x >= map_sz.x || next_pos.y >= map_sz.y) {
			continue;
		}

		if (get_px(map_2, map_sz, next_pos) == 2) {
			fill_border(map_2, point_list_2, map_sz, next_pos);
		}
	}
}

void update_point_list_2(Selection &selection, Vec2i map_sz) {
	std::fill(selection.map_2.begin(), selection.map_2.end(), 0);
	selection.point_list_2.clear();

	Vec2i bottom_right = to_vec2i(selection.bottom_right_all);
	Vec2i top_left = to_vec2i(selection.top_left_all);

	for (int x = top_left.x; x <= bottom_right.x; x++) {
	for (int y = top_left.y; y <= bottom_right.y; y++) {
		Vec2i pos = vec2i_new(x, y);

		if (get_px(selection.map_2, map_sz, pos) != 0) {
			continue;
		}

		if (get_px(selection.map, map_sz, pos) == 0) {
			continue;
		}

		fill(selection.map, selection.map_2, map_sz, pos);
	}
	}

	for (int x = top_left.x; x <= bottom_right.x; x++) {
	for (int y = top_left.y; y <= bottom_right.y; y++) {
		Vec2i pos = vec2i_new(x, y);

		if (get_px(selection.map_2, map_sz, pos) != 2) {
			continue;
		}

		fill_border(selection.map_2, selection.point_list_2, map_sz, pos);

		SelectPoint point;
		point.pos = to_vec2(pos);
		selection.point_list_2.push_back(point);
	}
	}
}

void select_tool_rect_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract, Vec2 parent_pos){
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
	Vec2i tex_draw_mouse_pos_i = to_vec2i(tex_draw_mouse_pos);
	Vec2i pos_clamped = tex_draw_mouse_pos_i;
	if (pos_clamped.x < 0) {
		pos_clamped.x = 0;
	}
	if (pos_clamped.y < 0) {
		pos_clamped.y = 0;
	}
	if (pos_clamped.x >= tab.sz.x) {
		pos_clamped.x = tab.sz.x - 1;
	}
	if (pos_clamped.y >= tab.sz.y) {
		pos_clamped.y = tab.sz.y - 1;
	}

	if (input.left_click) {
		tab.selection.rect_top_left = pos_clamped;
		tab.selection.rect_bottom_right = pos_clamped;
		set_map_area_all(tab.selection, to_vec2(pos_clamped));
	}

	if (input.left_down) {
		tab.selection.rect_bottom_right = pos_clamped;
		set_map_area_all(tab.selection, to_vec2(pos_clamped));
	}

	if (input.left_release) {
		update_map_rect(tab.selection, tab.sz, subtract);
		update_point_list_2(tab.selection, tab.sz);

		tab.selection.rect_top_left = vec2i_new(-1, -1);
		tab.selection.rect_bottom_right = vec2i_new(-1, -1);
	}
}

void select_tool_free_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract, Vec2 parent_pos){
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.left_click) {
		tab.selection.top_left = vec2_new(9999999, 9999999);
		tab.selection.bottom_right = vec2_new(-9999999, -9999999);

		SelectPoint point;
		point.pos = tex_draw_mouse_pos;

		if (point.pos.x < 0) {
			point.pos.x = 0;
		}
		if (point.pos.x >= tab.sz.x) {
			point.pos.x = tab.sz.x - 1;
		}
		if (point.pos.y < 0) {
			point.pos.y = 0;
		}
		if (point.pos.y >= tab.sz.y) {
			point.pos.y = tab.sz.y - 1;
		}

		tab.selection.point_list.push_back(point);
		set_map_area(tab.selection, point.pos);
		set_map_area_all(tab.selection, point.pos);
	}

	if (input.left_down) {
		int sz = (int)tab.selection.point_list.size();
		SelectPoint prev_point = tab.selection.point_list[sz - 1];
		line(tab.selection, tab.sz, prev_point.pos, tex_draw_mouse_pos);
	}

	if (input.left_release) {
		int sz = (int)tab.selection.point_list.size();
		SelectPoint prev_point = tab.selection.point_list[sz - 1];
		SelectPoint first_point = tab.selection.point_list[0];
		line(tab.selection, tab.sz, prev_point.pos, first_point.pos);
	
		update_map_free(tab.selection, tab.sz, subtract);
		update_point_list_2(tab.selection, tab.sz);

		tab.selection.point_list.clear();
	}
}

}

void select_tool_preview_update(Tab &tab, GraphicStuff &gs,
const GameTime &game_time) {
	preview_update(tab, gs, game_time, 1);
	texture_data_red(gs, tab.selection_preview_texture_index,
		tab.sz, tab.selection_preview_data);
	preview_update(tab, gs, game_time, 0);
}

void select_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract,
int select_type, Vec2 parent_pos){
	if (select_type == SELECT_RECT) {
		select_tool_rect_update(tab, layer_index, gs, input, game_time,
			subtract, parent_pos);
	}

	else if (select_type == SELECT_FREE) {
		select_tool_free_update(tab, layer_index, gs, input, game_time,
			subtract, parent_pos);
	}

	if (input.key_list[KEY_P].press) {
		std::cout << "--------------------------" << std::endl;
		for (int i = 0; i < tab.sz.y; i++) {
			std::cout << std::endl;
		for (int j = 0; j < tab.sz.x; j++) {
			std::cout << (int)tab.selection.map_2[i * tab.sz.y + j];
		}
		}
	}
}
