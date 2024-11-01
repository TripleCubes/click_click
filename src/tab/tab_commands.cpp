#include "tab_commands.h"

// TEST
//#include <iostream>

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

}

Command command_new(int time_pos, int type, int a, int b, int c,
const std::string &str) {
	Command command;
	
	command.time_pos = time_pos;
	command.type = type;

	command.a = a;
	command.b = b;
	command.c = c;
	command.str = str;
	
	return command;
}

void tab_commands_init(TabCommands &tab_commands) {
	Command command = command_new(0, COMMAND_NOTHING, 0, 0, 0, "");
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

	// a: history layer index, b: layer order list index, str: tab name
	else if (command.type == COMMAND_LAYER_NEW) {
		return tab_commands_layer_add(tab, command.str, gs,
			command.a, command.b);
	}

	// a: history layer index, b: layer order list index
	else if (command.type == COMMAND_LAYER_DELETE) {
		tab_commands_layer_delete(tab, gs, command.b);
	}

	return 0;
}

int tab_commands_undo_command(const Command &command, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings,
Tab& tab) {
	if (command.type == COMMAND_LAYER_SELECT) {
		tab.layer_order_list_index = command.b;
	}

	else if (command.type == COMMAND_LAYER_NEW) {
		tab_commands_layer_delete(tab, gs, command.b);
	}

	else if (command.type == COMMAND_LAYER_DELETE) {
		return tab_commands_layer_add(tab, command.str, gs,
			command.a, command.b);
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
