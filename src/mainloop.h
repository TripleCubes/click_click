#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct GraphicStuff;
struct GameTime;
struct Input;
struct TabBar;

void update(GraphicStuff &gs,
	TabBar &tab_bar,
	const GameTime &game_time,
	const Input &input);
void draw(GraphicStuff &gs,
	const TabBar &tab_bar,
	const GameTime &game_time,
	const Input &input);

#endif
