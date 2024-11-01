#ifndef TAB__HISTORY_H
#define TAB__HISTORY_H

#include <vector>

const int HISTORY_CHUNK_W = 8;

struct Layer;
struct Vec2i;
struct Tab;
struct GraphicStuff;
struct Input;
struct GameTime;
struct Settings;
struct TabCommands;

struct HistoryPtr {
	int time_pos = 0;

	int point_to = 0;
	int at_page = 0;
};

struct HistoryLayer {
	int id = 0;
	std::vector<std::vector<std::vector<HistoryPtr>>> ptr_2d_list;
};

HistoryPtr history_ptr_new(int time_pos, int point_to, int at_page);

struct History {
	int time_pos_min = 0;
	int time_pos_max = 0;
	int time_pos_current = 0;

	int at_page = 0;
	int data_0_n = 1;
	int data_1_n = 0;

	int data_0_start_time_pos = 0;
	int data_1_start_time_pos = 0;

	std::vector<unsigned char> data_0;
	std::vector<unsigned char> data_1;

	std::vector<HistoryLayer> layer_list;
};

void history_layer_add(History &history, Layer &layer);

void history_init(History &history);

void history_commit_prepare(History &history, TabCommands &tab_commands);

void history_commit_layer(History &history, HistoryLayer &history_layer,
	const Layer &layer);

bool history_undo_prepare(History &history);

bool history_redo_prepare(History &history);

void history_undo(History &history, Tab &tab, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, const Settings &settings);

void history_redo(History &history, Tab &tab, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, const Settings &settings);

void history_roll_back_layer(History &history, HistoryLayer &history_layer,
	Layer &layer, int time_pos);

void history_release(History &history);

Vec2i get_history_ptr_2d_list_sz(Vec2i layer_sz);

#endif
