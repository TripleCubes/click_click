#include "history.h"

#include <algorithm>
#include <array>

// TEST
//#include <iostream>

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "layer.h"
#include "tab.h"
#include "tab_commands.h"
#include "../graphic_types/graphic_types.h"

namespace {

const int HISTORY_DATA_SZ = 2 * 1024 * 1024;

int div_1(int a, int b) {
	if (a % b == 0) {
		return a / b;
	}
	else {
		return a / b + 1;
	}
}

unsigned char get_history_px(const History &history,
const HistoryPtr &history_ptr, int i_add) {
	const std::vector<unsigned char> *history_data;
	if (history_ptr.at_page == 0) {
		history_data = &history.data_0;
	}
	else {
		history_data = &history.data_1;
	}

	return (*history_data)[
		history_ptr.point_to*HISTORY_CHUNK_W*HISTORY_CHUNK_W + i_add];
}

bool data_equals(const History &history, const HistoryPtr &history_ptr,
const Layer &layer, Vec2i layer_data_pos) {
	const std::vector<unsigned char> *history_data;
	if (history_ptr.at_page == 0) {
		history_data = &history.data_0;
	}
	else {
		history_data = &history.data_1;
	}

	for (int y = 0; y < HISTORY_CHUNK_W; y++) {
	for (int x = 0; x < HISTORY_CHUNK_W; x++) {
		int history_data_i
			= (history_ptr.point_to * HISTORY_CHUNK_W * HISTORY_CHUNK_W)
			+ (y * HISTORY_CHUNK_W + x);
		unsigned char history_px = (*history_data)[history_data_i];

		unsigned char layer_px = 0;
		int layer_x = layer_data_pos.x + x;
		int layer_y = layer_data_pos.y + y;
		if (layer_x < layer.sz.x && layer_y < layer.sz.y) {
			layer_px = layer.data[layer_y * layer.sz.x + layer_x];
		}

		if (history_px != layer_px) {
			return false;
		}
	}
	}

	return true;
}

HistoryPtr add_history_data(History &history, const Layer &layer,
Vec2i layer_data_pos) {
	int num_chunk_per_page
		= history.data_0.size() / (HISTORY_CHUNK_W * HISTORY_CHUNK_W);
	
	// the data vector's size must be made to always be bigger than data
	// of 1 layer, so that a data page always contain more than 1 time pos
	if (history.at_page == 0 && history.data_0_n == num_chunk_per_page - 1) {
		history.at_page = 1;
		
		history.time_pos_min = history.data_1_start_time_pos + 1;
		history.data_0_start_time_pos = history.time_pos_current;
	}
	if (history.at_page == 1 && history.data_1_n == num_chunk_per_page - 1) {
		history.at_page = 0;

		history.time_pos_min = history.data_0_start_time_pos + 1;
		history.data_1_start_time_pos = history.time_pos_current;
	}

	int history_data_start_i = 0;
	int history_ptr_point_to = 0;
	if (history.at_page == 0) {
		history.data_0_n++;
		history_ptr_point_to = history.data_0_n;
		history_data_start_i
			= history.data_0_n * HISTORY_CHUNK_W * HISTORY_CHUNK_W;
	}
	else {
		history.data_1_n++;
		history_ptr_point_to = history.data_1_n;
		history_data_start_i
			= history.data_1_n * HISTORY_CHUNK_W * HISTORY_CHUNK_W;
	}
	int i_add = 0;
	for (int y = layer_data_pos.y; y < layer_data_pos.y+HISTORY_CHUNK_W; y++) {
	for (int x = layer_data_pos.x; x < layer_data_pos.x+HISTORY_CHUNK_W; x++) {
		unsigned char px = 0;
		if (y < layer.sz.y && x < layer.sz.x) {
			int layer_data_i = y * layer.sz.x + x;
			px = layer.data[layer_data_i];
		}

		if (history.at_page == 0) {
			history.data_0[history_data_start_i + i_add] = px;
		}

		i_add++;
	}
	}

	return history_ptr_new(
		history.time_pos_current,
		history_ptr_point_to,
		history.at_page
	);
}

void copy_to_layer_data(const History &history,
const HistoryPtr &history_ptr, Layer &layer, Vec2i layer_data_pos) {
	int i_add = 0;
	for (int y = layer_data_pos.y; y < layer_data_pos.y+HISTORY_CHUNK_W; y++) {
	for (int x = layer_data_pos.x; x < layer_data_pos.x+HISTORY_CHUNK_W; x++) {
		if (x < layer.sz.x && y < layer.sz.y) {
			int data_i = y * layer.sz.x + x;
			layer.data[data_i]
				= get_history_px(history, history_ptr, i_add);
		}

		i_add++;
	}
	}
}

void rm_all_forward_histories(History &history, int history_pos) {
	for (int layer_i = 0; layer_i < (int)history.layer_list.size(); layer_i++){
	HistoryLayer &history_layer = history.layer_list[layer_i];
	for (int y = 0; y < (int)history_layer.ptr_2d_list.size(); y++) {
	for (int x = 0; x < (int)history_layer.ptr_2d_list[y].size(); x++) {
	for (int i = (int)history_layer.ptr_2d_list[y][x].size() - 1; i >= 0; i--){
		HistoryPtr &history_ptr = history_layer.ptr_2d_list[y][x][i];
		if (history_ptr.time_pos <= history_pos) {
			break;
		}

		history_layer.ptr_2d_list[y][x].pop_back();
	}
	}
	}
	}
}

}

HistoryPtr history_ptr_new(int time_pos, int point_to, int at_page) {
	HistoryPtr history_ptr;
	
	history_ptr.time_pos = time_pos;

	history_ptr.point_to = point_to;
	history_ptr.at_page = at_page;

	return history_ptr;
}

void history_layer_add(History &history, Layer &layer) {
	history.layer_list.push_back(HistoryLayer());
	HistoryLayer &history_layer = history.layer_list.back();

	Vec2i ptr_2d_list_sz = get_history_ptr_2d_list_sz(layer.sz);
	history_layer.ptr_2d_list.resize(ptr_2d_list_sz.y);
	for (int i = 0; i < ptr_2d_list_sz.y; i++) {
		history_layer.ptr_2d_list[i].resize(ptr_2d_list_sz.x);
		for (int j = 0; j < ptr_2d_list_sz.x; j++) {
			HistoryPtr history_ptr = history_ptr_new(0, 0, 0);
			history_layer.ptr_2d_list[i][j].push_back(history_ptr);
		}
	}

	history_commit_layer(history, history_layer, layer);
}

void history_init(History &history) {
	history.data_0.resize(HISTORY_DATA_SZ, 0);
	history.data_1.resize(HISTORY_DATA_SZ, 0);
}

void history_commit_prepare(History &history, TabCommands &tab_commands) {
	if (history.time_pos_current < history.time_pos_max) {
		rm_all_forward_histories(history, history.time_pos_current);
		rm_all_forward_commands(tab_commands, history.time_pos_current);
	}

	history.time_pos_current++;
	history.time_pos_max = history.time_pos_current;
}

void history_commit_layer(History &history, HistoryLayer &history_layer,
const Layer &layer) {
	Vec2i ptr_2d_list_sz = get_history_ptr_2d_list_sz(layer.sz);
	for (int y = 0; y < ptr_2d_list_sz.y; y++) {
	for (int x = 0; x < ptr_2d_list_sz.x; x++) {
		std::vector<HistoryPtr> &ptr_list = history_layer.ptr_2d_list[y][x];
		Vec2i layer_data_pos
			= vec2i_new(x * HISTORY_CHUNK_W, y * HISTORY_CHUNK_W);

		if (!data_equals(history, ptr_list.back(), layer, layer_data_pos)) {
			HistoryPtr history_ptr = add_history_data(
				history,
				layer,
				layer_data_pos
			);
			ptr_list.push_back(history_ptr);
		}
	}
	}
}

void history_undo_prepare(History &history) {
	if (history.time_pos_current <= history.time_pos_min) {
		return;
	}

	history.time_pos_current--;
}

void history_redo_prepare(History &history) {
	if (history.time_pos_current >= history.time_pos_max) {
		return;
	}

	history.time_pos_current++;
}

void history_undo(History &history, Tab &tab, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings) {
	history_undo_prepare(history);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		Layer &layer = tab.layer_list[i];

		if (!layer.running) {
			continue;
		}

		history_roll_back_layer(
			history,
			history.layer_list[layer.history_layer_index],
			layer,
			history.time_pos_current
		);

		layer_set_texture_data(layer, gs);
	}

	tab_commands_time_pos_backward_to(
		tab.tab_commands,
		gs, input, game_time,
		settings, tab, history.time_pos_current
	);
}

void history_redo(History &history, Tab &tab, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings) {
	history_redo_prepare(history);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		Layer &layer = tab.layer_list[i];

		if (!layer.running) {
			continue;
		}

		history_roll_back_layer(
			history,
			history.layer_list[layer.history_layer_index],
			layer,
			history.time_pos_current
		);

		layer_set_texture_data(layer, gs);
	}

	tab_commands_time_pos_forward_to(
		tab.tab_commands,
		gs, input, game_time,
		settings, tab, history.time_pos_current);
}

void history_roll_back_layer(History &history, HistoryLayer &history_layer,
Layer &layer, int time_pos) {
	Vec2i ptr_2d_list_sz = get_history_ptr_2d_list_sz(layer.sz);
	for (int y = 0; y < ptr_2d_list_sz.y; y++) {
	for (int x = 0; x < ptr_2d_list_sz.x; x++) {
		HistoryPtr history_ptr;

		for (int i = (int)history_layer.ptr_2d_list[y][x].size() - 1;
		i >= 0; i--) {
			const HistoryPtr &v = history_layer.ptr_2d_list[y][x][i];
			if (time_pos >= v.time_pos) {
				history_ptr = v;
				break;
			}
		}

		copy_to_layer_data(
			history,
			history_ptr,
			layer,
			vec2i_new(x * HISTORY_CHUNK_W, y * HISTORY_CHUNK_W)
		);
	}
	}
}

void history_release(History &history) {
	history.time_pos_min = 0;
	history.time_pos_max = 0;
	history.time_pos_current = 0;
	
	history.at_page = 0;
	history.data_0_n = 1;
	history.data_1_n = 0;

	history.data_0_start_time_pos = 0;
	history.data_1_start_time_pos = 0;

	history.data_0.clear();
	history.data_1.clear();

	history.layer_list.clear();
}

Vec2i get_history_ptr_2d_list_sz(Vec2i layer_sz) {
	return vec2i_new(div_1(layer_sz.x, HISTORY_CHUNK_W),
	                 div_1(layer_sz.y, HISTORY_CHUNK_W));
}
