#ifndef TAB__TAB_COMMANDS_H
#define TAB__TAB_COMMANDS_H

#include <vector>
#include <string>

const int COMMAND_NOTHING         = 0;

const int COMMAND_LAYER_SELECT    = 1;
const int COMMAND_LAYER_RENAME    = 2;
const int COMMAND_LAYER_NEW       = 3;
const int COMMAND_LAYER_DELETE    = 4;
const int COMMAND_LAYER_MOVE_UP   = 5;
const int COMMAND_LAYER_MOVE_DOWN = 6;

const int COMMAND_RESIZE          = 7;

struct Tab;
struct GraphicStuff;
struct Input;
struct GameTime;
struct Settings;
struct History;

struct Command {
	int time_pos = 0;
	int type = 0;

	int a = 0;
	int b = 0;
	int c = 0;
	std::string str;
	std::string str_1;
};

struct TabCommands {
	std::vector<Command> command_list;
};

Command command_new(int time_pos, int type, int a, int b, int c,
	const std::string &str, const std::string &str_1);

void tab_commands_init(TabCommands &tab_commands);

void rm_all_forward_commands(TabCommands &tab_commands, int time_pos);

int tab_commands_do_and_add(TabCommands &tab_commands,
	const Command &command, History &history, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, const Settings &settings,
	Tab& tab);

int tab_commands_do_command(const Command &command, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, const Settings &settings,
	Tab& tab);

int tab_commands_undo_command(const Command &command, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, const Settings &settings,
	Tab& tab);

void tab_commands_time_pos_forward_to(TabCommands &tab_commands,
	GraphicStuff &gs, const Input &input, const GameTime &game_time,
	const Settings &settings, Tab& tab, int time_pos);

void tab_commands_time_pos_backward_to(TabCommands &tab_commands,
	GraphicStuff &gs, const Input &input, const GameTime &game_time,
	const Settings &settings, Tab& tab, int time_pos);

void tab_commands_release(TabCommands &tab_commands);

#endif
