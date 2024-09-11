#include "select.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "../game_time.h"
#include "../basic_math.h"
#include "tab.h"
#include "../pos_convert.h"
#include "../draw_tool/draw_tool_px.h"

#include <cmath>
#include <algorithm>
#include <array>

//#include <iostream>

namespace {

struct QueuePos {
	Vec2i pos;
	unsigned char n = 0;
};

std::vector<QueuePos> border_queue;
std::vector<Vec2i> magic_wand_queue;

void point(Selection &selection, Vec2i sz, Vec2 pos, bool subtract) {
	if (pos.x < 0) { pos.x = 0; }
	if (pos.y < 0) { pos.y = 0; }
	if (pos.x >= sz.x) { pos.x = sz.x - 1; }
	if (pos.y >= sz.y) { pos.y = sz.y - 1; }

	int list_sz = (int)selection.draw_preview_list.size();
	Vec2 prev_pos = selection.draw_preview_list[list_sz - 1];
	Vec2i prev_pos_i = to_vec2i(prev_pos);
	Vec2i pos_i = to_vec2i(pos);

	if (pos_i.x == prev_pos_i.x && pos_i.y == prev_pos_i.y) {
		return;
	}

	selection.draw_preview_list.push_back(pos);
	
	if (pos_i.y < prev_pos_i.y) {
		selection.scan_list.push_back(pos_i);
		if (!subtract) {
			selection.border_hint_list.push_back(pos_i);
		}
		else {
			selection.border_hint_list.push_back(vec2i_new(pos_i.x,
			                                               pos_i.y - 1));
			selection.border_hint_list.push_back(vec2i_new(pos_i.x,
			                                               pos_i.y + 1));
			selection.border_hint_list.push_back(vec2i_new(pos_i.x - 1,
			                                               pos_i.y));
			selection.border_hint_list.push_back(vec2i_new(pos_i.x + 1,
			                                               pos_i.y));
		}
	}
	else if (prev_pos_i.y < pos_i.y) {
		selection.scan_list.push_back(prev_pos_i);
		if (!subtract) {
			selection.border_hint_list.push_back(prev_pos_i);
		}
		else {
			selection.border_hint_list.push_back(vec2i_new(prev_pos_i.x,
			                                               prev_pos_i.y - 1));
			selection.border_hint_list.push_back(vec2i_new(prev_pos_i.x,
			                                               prev_pos_i.y + 1));
			selection.border_hint_list.push_back(vec2i_new(prev_pos_i.x - 1,
			                                               prev_pos_i.y));
			selection.border_hint_list.push_back(vec2i_new(prev_pos_i.x + 1,
			                                               prev_pos_i.y));
		}
	}
}

void line(Selection &selection, Vec2i sz, Vec2 pos_1, Vec2 pos_2,
bool subtract) {
	if (pos_1.x == pos_2.x && pos_1.y == pos_2.y) {
		return;
	}

	float dist = vec2_dist_sqr(pos_1, pos_2);
	Vec2 dir = vec2_mul(vec2_normalized(vec2_sub(pos_2, pos_1)), 0.5);
	Vec2 current_pos = pos_1;

	while (vec2_dist_sqr(pos_1, current_pos) <= dist) {
		point(selection, sz, current_pos, subtract);
		current_pos = vec2_add(current_pos, dir);
	}

	point(selection, sz, pos_2, subtract);
}

void px(std::vector<unsigned char> &map, Vec2i sz, Vec2i pos, unsigned char n){
	map[pos.y * sz.y + pos.x] = n;
}

unsigned char get_px(const std::vector<unsigned char> &map,Vec2i sz,Vec2i pos){
	return map[pos.y * sz.y + pos.x];
}

void update_map_rect(Selection &selection, Vec2i sz, bool subtract) {
	Vec2i tl = selection.rect_point_1;
	Vec2i br = selection.rect_point_2;

	if (tl.x > br.x) {
		int swap = tl.x;
		tl.x = br.x;
		br.x = swap;
	}

	if (tl.y > br.y) {
		int swap = tl.y;
		tl.y = br.y;
		br.y = swap;
	}

	for (int y = tl.y; y <= br.y; y++) {
	for (int x = tl.x; x <= br.x; x++) {
		px(selection.map, sz, vec2i_new(x, y), subtract? 0 : 1);
	}
	}
}

void update_map_free(Selection &selection, Vec2i sz, bool subtract) {
	for (int i = 0; i < (int)selection.draw_preview_list.size(); i++) {
		Vec2i pos = to_vec2i(selection.draw_preview_list[i]);
		px(selection.map, sz, pos, subtract? 0 : 1);
	}

	int i = 0;
	while (i + 1 < (int)selection.scan_list.size()) {
		Vec2i pos_1 = selection.scan_list[i];
		Vec2i pos_2 = selection.scan_list[i + 1];

		if (pos_1.y != pos_2.y) {
			i++;
			continue;
		}

		for (int x = pos_1.x; x <= pos_2.x; x++) {
			px(selection.map, sz, vec2i_new(x, pos_1.y), subtract? 0 : 1);
		}

		i += 2;
	}
}

const std::array<Vec2i, 4> DIRS4 = {
	vec2i_new( 0, -1),
	vec2i_new( 0,  1),
	vec2i_new( 1,  0),
	vec2i_new(-1,  0),
};

unsigned char get_data_px(const std::vector<unsigned char> &data,
Vec2i data_sz, Vec2i pos) {
	int data_index = data_sz.y * pos.y + pos.x;
	return data[data_index];
}

void magic_wand_fill(Selection &selection, Vec2i sz,
const std::vector<unsigned char> &data, Vec2i pos, unsigned char replace_index,
bool subtract) {
	magic_wand_queue.clear();
	int i = 0;
	magic_wand_queue.push_back(pos);

	while (i < (int)magic_wand_queue.size()) {
		Vec2i current_pos = magic_wand_queue[i];
		
		for (int j = 0; j < (int)DIRS4.size(); j++) {
			Vec2i next_pos = vec2i_add(current_pos, DIRS4[j]);
			
			if (next_pos.x < 0 || next_pos.y < 0
			|| next_pos.x >= sz.x || next_pos.y >= sz.y) {
				selection.border_hint_list.push_back(next_pos);
				continue;
			}
			
			if (get_data_px(data, sz, next_pos) != replace_index) {
				selection.border_hint_list.push_back(next_pos);
				continue;
			}

			unsigned char set_to = subtract? 0 : 1;
			if (get_px(selection.map, sz, next_pos) == set_to) {
				selection.border_hint_list.push_back(next_pos);
				continue;
			}

			px(selection.map, sz, next_pos, set_to);
			magic_wand_queue.push_back(next_pos);
		}

		i++;
	}
}

void update_map_magic_wand(Selection &selection, Vec2i sz,
const std::vector<unsigned char> &data, Vec2i pos, bool subtract) {
	unsigned char replace_index = get_px(data, sz, pos);
	magic_wand_fill(selection, sz, data, pos, replace_index, subtract);
}

const std::array<Vec2i, 8> DIRS8 = {
	vec2i_new( 0, -1),
	vec2i_new( 0,  1),
	vec2i_new(-1,  0),
	vec2i_new( 1,  0),
	vec2i_new( 1, -1),
	vec2i_new( 1,  1),
	vec2i_new(-1,  1),
	vec2i_new(-1, -1),
};

bool is_border(Selection &selection, Vec2i sz, Vec2i pos) {
	if (pos.x == 0) {
		return true;
	}

	if (get_px(selection.map, sz, vec2i_new(pos.x - 1, pos.y)) == 0) {
		return true;
	}


	if (pos.y == 0) {
		return true;
	}

	if (get_px(selection.map, sz, vec2i_new(pos.x, pos.y - 1)) == 0) {
		return true;
	}


	if (pos.x == sz.x - 1) {
		return true;
	}

	if (get_px(selection.map, sz, vec2i_new(pos.x + 1, pos.y)) == 0) {
		return true;
	}


	if (pos.y == sz.y - 1) {
		return true;
	}

	if (get_px(selection.map, sz, vec2i_new(pos.x, pos.y + 1)) == 0) {
		return true;
	}


	return false;
}

void border_fill(Selection &selection, Vec2i sz, Vec2i pos) {
	border_queue.clear();

	QueuePos queue_pos;
	queue_pos.pos = pos;
	border_queue.push_back(queue_pos);

	while (border_queue.size() > 0) {
		QueuePos current = border_queue[border_queue.size() - 1];
		bool skip = false;

		for (int i = current.n; i < (int)DIRS8.size(); i++) {
			Vec2i next_pos = vec2i_add(current.pos, DIRS8[i]);
			current.n++;

			if (next_pos.x < 0 || next_pos.y < 0
			|| next_pos.x >= sz.x || next_pos.y >= sz.y) {
				continue;
			}

			if (get_px(selection.fill_checked_map, sz, next_pos) == 1) {
				continue;
			}

			if (get_px(selection.map, sz, next_pos) == 0) {
				continue;
			}

			if (!is_border(selection, sz, next_pos)) {
				continue;
			}

			selection.full_preview_list.push_back(next_pos);
			px(selection.fill_checked_map, sz, next_pos, 1);
			
			QueuePos queue_pos;
			queue_pos.pos = next_pos;
			border_queue.push_back(queue_pos);
			
			skip = true;
		}

		if (skip) {
			continue;
		}

		border_queue.pop_back();
	}
}

void set_full_preview_list(Selection &selection, Vec2i sz) {
	selection.full_preview_list.clear();
	std::fill(
		selection.fill_checked_map.begin(),
		selection.fill_checked_map.end(),
		0
	);

	for (int i = 0; i < (int)selection.border_hint_list.size(); i++) {
		Vec2i pos = selection.border_hint_list[i];

		if (get_px(selection.fill_checked_map, sz, pos) == 1) {
			continue;
		}

		if (get_px(selection.map, sz, pos) == 0) {
			continue;
		}

		if (!is_border(selection, sz, pos)) {
			continue;
		}

		border_fill(selection, sz, pos);
	}
}

void select_tool_rect_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract,
Vec2 parent_pos) {
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
		tab.selection.rect_point_1 = pos_clamped;
		tab.selection.rect_point_2 = pos_clamped;
	}

	if (input.left_down && input.mouse_move) {
		tab.selection.rect_point_2 = pos_clamped;
	}

	if (input.left_release) {
		Vec2i pos = tab.selection.rect_point_1;

		if (!subtract) {
			tab.selection.border_hint_list.push_back(pos);
		}
		else {
			tab.selection.border_hint_list.push_back(vec2i_new(pos.x,
			                                                   pos.y + 1));
			tab.selection.border_hint_list.push_back(vec2i_new(pos.x,
			                                                   pos.y - 1));
			tab.selection.border_hint_list.push_back(vec2i_new(pos.x + 1,
			                                                   pos.y));
			tab.selection.border_hint_list.push_back(vec2i_new(pos.x - 1,
			                                                   pos.y));
		}

		update_map_rect(tab.selection, tab.sz, subtract);
		set_full_preview_list(tab.selection, tab.sz);

		tab.selection.rect_point_1 = vec2i_new(-1, -1);
		tab.selection.rect_point_2 = vec2i_new(-1, -1);
	}
}

void select_tool_free_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.left_click) {
		Vec2 pos = tex_draw_mouse_pos;
		if (pos.x < 0) { pos.x = 0; }
		if (pos.y < 0) { pos.y = 0; }
		if (pos.x >= tab.sz.x) { pos.x = tab.sz.x - 1; }
		if (pos.y >= tab.sz.y) { pos.y = tab.sz.y - 1; }
		tab.selection.draw_preview_list.push_back(pos);
	}

	if (input.left_down && input.mouse_move) {
		int list_sz = (int)tab.selection.draw_preview_list.size();
		Vec2 prev_pos = tab.selection.draw_preview_list[list_sz - 1];
		line(tab.selection, tab.sz, prev_pos, tex_draw_mouse_pos,
			subtract);
	}

	if (input.left_release) {
		Vec2 first_pos = tab.selection.draw_preview_list[0];
		int list_sz = (int)tab.selection.draw_preview_list.size();
		Vec2 prev_pos = tab.selection.draw_preview_list[list_sz - 1];
		line(tab.selection, tab.sz, prev_pos, first_pos,
			subtract);

		auto sort = [](Vec2i a, Vec2i b) {
			if (a.y == b.y) {
				return a.x < b.x;
			}

			return a.y < b.y;
		};

		std::sort(
			tab.selection.scan_list.begin(),
			tab.selection.scan_list.end(),
			sort
		);

		update_map_free(tab.selection, tab.sz, subtract);
		set_full_preview_list(tab.selection, tab.sz);

		tab.selection.draw_preview_list.clear();
		tab.selection.scan_list.clear();
	}
}

void select_tool_magic_wand_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, const GameTime &game_time, bool subtract,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Layer &layer = tab.layer_list[layer_index];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (!in_rect(tex_draw_mouse_pos, vec2_new(0, 0), to_vec2(tab.sz))) {
		return;
	}

	if (input.left_click) {
		update_map_magic_wand(tab.selection, tab.sz,
			layer.data, to_vec2i(tex_draw_mouse_pos), subtract);
		set_full_preview_list(tab.selection, tab.sz);
	}
}

bool ink_cond(int ink) {
	return ink % 10 < 4;
}

void rect_preview_update(Tab &tab,
const GameTime &game_time, unsigned char pallete_index) {
	if (tab.selection.rect_point_1.x == -1) {
		return;
	}

	int ink = game_time.time_since_start * 10;

	auto draw = [&tab, &ink, pallete_index](int x, int y){
		ink++;
		if (!ink_cond(ink)) {
			return;
		}

		draw_tool_px_no_selection(
			tab.selection_preview_data,
			tab.selection,
			tab.sz,
			vec2i_new(x, y),
			pallete_index,
			0
		);
	};

	Vec2i tl = tab.selection.rect_point_1;
	Vec2i br = tab.selection.rect_point_2;

	if (tl.x > br.x) {
		int swap = tl.x;
		tl.x = br.x;
		br.x = swap;
	}

	if (tl.y > br.y) {
		int swap = tl.y;
		tl.y = br.y;
		br.y = swap;
	}

	for (int x = tl.x; x <= br.x; x++) {
		draw(x, tl.y);
	}
	for (int y = tl.y; y <= br.y; y++) {
		draw(br.x, y);
	}
	for (int x = br.x; x >= tl.x; x--) {
		draw(x, br.y);
	}
	for (int y = br.y; y >= tl.y; y--) {
		draw(tl.x, y);
	}
}

void free_preview_update(Tab &tab,
const GameTime &game_time, unsigned char pallete_index) {
	int ink = game_time.time_since_start * 10;
	for (int i = 0; i < (int)tab.selection.draw_preview_list.size(); i++) {
		ink++;
		if (!ink_cond(ink)) {
			continue;
		}

		Vec2 pos = tab.selection.draw_preview_list[i];
		draw_tool_px_no_selection(
			tab.selection_preview_data,
			tab.selection,
			tab.sz,
			to_vec2i(pos),
			pallete_index,
			0
		);
	}
}

void full_preview_update(Tab &tab,
const GameTime &game_time, unsigned char pallete_index) {
	int ink = game_time.time_since_start * 10;
	for (int i = 0; i < (int)tab.selection.full_preview_list.size(); i++) {
		ink++;
		if (!ink_cond(ink)) {
			continue;
		}

		Vec2i pos = tab.selection.full_preview_list[i];
		draw_tool_px_no_selection(
			tab.selection_preview_data,
			tab.selection,
			tab.sz,
			pos,
			pallete_index,
			0
		);
	}
}

void preview_update(Tab &tab,
const GameTime &game_time, unsigned char pallete_index) {
	rect_preview_update(tab, game_time, pallete_index);
	free_preview_update(tab, game_time, pallete_index);
	full_preview_update(tab, game_time, pallete_index);
}

}

void selection_init(Selection &selection, Vec2i canvas_sz) {
	selection.map.resize(canvas_sz.x * canvas_sz.y, 0);
	selection.fill_checked_map.resize(canvas_sz.x * canvas_sz.y, 0);
}

void selection_all(Selection &selection, Vec2i sz) {
	std::fill(
		selection.map.begin(),
		selection.map.end(),
		1
	);
	selection.border_hint_list.clear();
	selection.border_hint_list.push_back(vec2i_new(0, 0));
	set_full_preview_list(selection, sz);
}

void selection_clear(Selection &selection, Vec2i sz) {
	std::fill(
		selection.map.begin(),
		selection.map.end(),
		0
	);
	selection.border_hint_list.clear();
	set_full_preview_list(selection, sz);
}

void select_tool_preview_update(Tab &tab, GraphicStuff &gs,
const GameTime &game_time) {
	preview_update(tab, game_time, 1);
	texture_data_red(gs, tab.selection_preview_texture_index,
		tab.sz, tab.selection_preview_data);
	preview_update(tab, game_time, 0);
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

	else if (select_type == SELECT_MAGIC_WAND) {
		select_tool_magic_wand_update(tab, layer_index, gs, input, game_time,
			subtract, parent_pos);
	}

//	if (input.key_list[KEY_P].press) {
//		std::cout << "--------------------------------" << std::endl;
//		for (int y = 0; y < tab.sz.y; y++) {
//		for (int x = 0; x < tab.sz.x; x++) {
//			std::cout << (int)tab.selection.map[y * tab.sz.y + x];
//		}
//		std::cout << " " << y << std::endl;
//		}
//	}
}
