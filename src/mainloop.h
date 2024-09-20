#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct States;
struct GraphicStuff;
struct GameTime;
struct Input;
struct TabBar;
struct FilePicker;

void update(
	States &states,
	GraphicStuff &gs,
	TabBar &tab_bar,
	FilePicker &file_picker,
	const GameTime &game_time,
	const Input &input
);
void draw(
	const States &states,
	GraphicStuff &gs,
	const TabBar &tab_bar,
	const FilePicker &file_picker,
	const GameTime &game_time,
	const Input &input
);

#endif
