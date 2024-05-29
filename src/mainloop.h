#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct GraphicStuff;
struct GameTime;
struct Input;
struct Tab;

void update(const GraphicStuff &gs,
	std::vector<Tab> &tab_list,
	const GameTime &game_time,
	const Input &input);
void draw(const GraphicStuff &gs,
	const std::vector<Tab> &tab_list,
	const GameTime &game_time);

#endif
