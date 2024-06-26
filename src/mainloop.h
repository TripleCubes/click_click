#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct GraphicStuff;
struct GameTime;
struct Input;
struct Tab;

void update(GraphicStuff &gs,
	std::vector<Tab> &tab_list,
	const GameTime &game_time,
	const Input &input);
void draw(GraphicStuff &gs,
	const std::vector<Tab> &tab_list,
	const GameTime &game_time,
	const Input &input);

#endif
