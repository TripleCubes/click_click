#include "tab_commands.h"

#include "tab.h"
#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../game_time.h"
#include "../settings.h"
#include "../basic_math.h"
#include "history.h"

namespace {

int tab_commands_layer_add(Tab &tab, const std::string &layer_name,
GraphicStuff &gs, int history_layer_index, int layer_order_list_index) {
	tab.num_layer_created++;
	tab.layer_order_list_index = layer_order_list_index;

	int layer_index = tab_layer_new(
		tab,
		tab.layer_order_list_index,
		layer_name,
		gs,
		history_layer_index
	);

	return layer_index;
}

int get_layer_index(const Tab &tab) {
	return tab.layer_order_list[tab.layer_order_list_index];
}

void tab_commands_layer_delete(Tab &tab, GraphicStuff &gs,
int layer_order_list_index) {
	tab.num_layer_created--;
	tab.layer_order_list_index = layer_order_list_index;

	layer_release(tab.layer_list, gs, get_layer_index(tab));
	tab.layer_order_list.erase(
		tab.layer_order_list.begin() + tab.layer_order_list_index
	);

	tab.layer_order_list_index = clampi(
		tab.layer_order_list_index,
		0,
		(int)tab.layer_order_list.size() - 1
	);
}

void layer_swap_up(Tab &tab, int to_layer_order_list_index) {
	int swap = tab.layer_order_list[to_layer_order_list_index];
	tab.layer_order_list[to_layer_order_list_index]
		= tab.layer_order_list[to_layer_order_list_index + 1];
	tab.layer_order_list[to_layer_order_list_index + 1] = swap;

	tab.layer_order_list_index = to_layer_order_list_index;
}

void layer_swap_down(Tab &tab, int to_layer_order_list_index) {
	int swap = tab.layer_order_list[to_layer_order_list_index];
	tab.layer_order_list[to_layer_order_list_index]
		= tab.layer_order_list[to_layer_order_list_index - 1];
	tab.layer_order_list[to_layer_order_list_index - 1] = swap;

	tab.layer_order_list_index = to_layer_order_list_index;
}

void layer_swap_up_undo(Tab &tab, int to_layer_order_list_index) {
	int swap = tab.layer_order_list[to_layer_order_list_index];
	tab.layer_order_list[to_layer_order_list_index]
		= tab.layer_order_list[to_layer_order_list_index + 1];
	tab.layer_order_list[to_layer_order_list_index + 1] = swap;

	tab.layer_order_list_index = to_layer_order_list_index + 1;
}

void layer_swap_down_undo(Tab &tab, int to_layer_order_list_index) {
	int swap = tab.layer_order_list[to_layer_order_list_index];
	tab.layer_order_list[to_layer_order_list_index]
		= tab.layer_order_list[to_layer_order_list_index - 1];
	tab.layer_order_list[to_layer_order_list_index - 1] = swap;

	tab.layer_order_list_index = to_layer_order_list_index - 1;
}

}

Command command_new(int time_pos, int type, int a, int b, int c,
Vec2i va, Vec2i vb, Vec2i vc, Vec2i vd,
const std::string &str, const std::string &str_1) {
	Command command;
	
	command.time_pos = time_pos;
	command.type = type;

	command.a = a;
	command.b = b;
	command.c = c;
	command.va = va;
	command.vb = vb;
	command.vc = vc;
	command.vd = vd;
	command.str = str;
	command.str_1 = str_1;
	
	return command;
}

void tab_commands_init(TabCommands &tab_commands) {
	Command command = command_new(0, COMMAND_NOTHING, 0, 0, 0,
		vec2i_new(0, 0), vec2i_new(0, 0), vec2i_new(0, 0), vec2i_new(0, 0),
		"", "");
	tab_commands.command_list.push_back(command);
}

void rm_all_forward_commands(TabCommands &tab_commands, int time_pos) {
	for (int i = (int)tab_commands.command_list.size() - 1; i >= 0; i--) {
		if (tab_commands.command_list[i].time_pos <= time_pos) {
			break;
		}
		tab_commands.command_list.pop_back();
	}
}

int tab_commands_do_and_add(TabCommands &tab_commands,
const Command &command, History &history, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings,
Tab& tab) {
	tab_commands.command_list.push_back(command);
	return tab_commands_do_command(
		command, gs, input, game_time, settings, tab
	);
}

int tab_commands_do_command(const Command &command, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings,
Tab& tab) {
	// a: index, b: prev index
	if (command.type == COMMAND_LAYER_SELECT) {
		tab.layer_order_list_index = command.a;
	}

	// b: layer order list index, str: rename to, str_1: rename from
	else if (command.type == COMMAND_LAYER_RENAME) {
		tab.layer_order_list_index = command.b;
		int layer_index = get_layer_index(tab);
		tab.layer_list[layer_index].textarea.text = command.str;
	}

	// a: history layer index, b: layer order list index, str: tab name
	else if (command.type == COMMAND_LAYER_NEW) {
		return tab_commands_layer_add(tab, command.str, gs,
			command.a, command.b);
	}

	// a: history layer index, b: layer order list index
	else if (command.type == COMMAND_LAYER_DELETE) {
		tab_commands_layer_delete(tab, gs, command.b);
	}

	// b: layer order list index after move
	else if (command.type == COMMAND_LAYER_MOVE_UP) {
		layer_swap_up(tab, command.b);
	}

	// b: layer order list index after move
	else if (command.type == COMMAND_LAYER_MOVE_DOWN) {
		layer_swap_down(tab, command.b);
	}

	// va: new_pos_anchor, vb: new_sz, vc: prev_pos_anchor, vd: prev_sz
	else if (command.type == COMMAND_RESIZE) {
		selection_clear(tab.selection, tab.sz);
		tab_resize(tab, gs, command.va, command.vb);
		for (int i = 0; i < (int)tab.layer_list.size(); i++) {
			Layer &layer = tab.layer_list[i];
			if (!layer.running) {
				continue;
			}

			HistoryLayer &history_layer
				= tab.history.layer_list[layer.history_layer_index];
			history_layer_size_up(tab.history, history_layer, command.vb);
		}
	}

	return 0;
}

int tab_commands_undo_command(const Command &command, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings,
Tab& tab) {
	if (command.type == COMMAND_LAYER_SELECT) {
		tab.layer_order_list_index = command.b;
	}

	else if (command.type == COMMAND_LAYER_RENAME) {
		tab.layer_order_list_index = command.b;
		int layer_index = get_layer_index(tab);
		tab.layer_list[layer_index].textarea.text = command.str_1;
	}

	else if (command.type == COMMAND_LAYER_NEW) {
		tab_commands_layer_delete(tab, gs, command.b);
	}

	else if (command.type == COMMAND_LAYER_DELETE) {
		return tab_commands_layer_add(tab, command.str, gs,
			command.a, command.b);
	}
	
	else if (command.type == COMMAND_LAYER_MOVE_UP) {
		layer_swap_up_undo(tab, command.b);
	}

	else if (command.type == COMMAND_LAYER_MOVE_DOWN) {
		layer_swap_down_undo(tab, command.b);
	}

	else if (command.type == COMMAND_RESIZE) {
		selection_clear(tab.selection, tab.sz);
		tab_resize(tab, gs, command.vc, command.vd);
	}

	return 0;
}

void tab_commands_time_pos_forward_to(TabCommands &tab_commands,
GraphicStuff &gs, const Input &input, const GameTime &game_time,
const Settings &settings, Tab& tab, int time_pos) {
	for (int i = 0; i < (int)tab_commands.command_list.size(); i++) {
		const Command &command = tab_commands.command_list[i];

		if (command.time_pos == time_pos) {
			tab_commands_do_command(
				command, gs, input, game_time, settings, tab
			);

			break;
		}
	}
}

void tab_commands_time_pos_backward_to(TabCommands &tab_commands,
GraphicStuff &gs, const Input &input, const GameTime &game_time,
const Settings &settings, Tab& tab, int time_pos) {
	for (int i = 0; i < (int)tab_commands.command_list.size(); i++) {
		const Command &command = tab_commands.command_list[i];

		if (command.time_pos - 1 == time_pos) {
			tab_commands_undo_command(
				command, gs, input, game_time, settings, tab
			);

			break;
		}
	}
}

void tab_commands_release(TabCommands &tab_commands) {
	tab_commands.command_list.clear();
}
