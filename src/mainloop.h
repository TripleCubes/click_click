#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct States;
struct GraphicStuff;
struct GameTime;
struct Input;
struct AppUI;

void update(
	States &states,
	GraphicStuff &gs,
	const GameTime &game_time,
	const Input &input,
	AppUI &app_ui
);
void draw(
	const States &states,
	GraphicStuff &gs,
	const GameTime &game_time,
	const Input &input,
	const AppUI &app_ui
);

#endif
