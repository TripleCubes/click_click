#ifndef MAINLOOP_H
#define MAINLOOP_H

#include <vector>

struct GraphicStuff;
struct GameTime;
struct Input;
struct Tab;
struct Btn;

void update(GraphicStuff &gs,
	std::vector<Tab> &tab_list,
	std::vector<Btn> &btn_list,
	const GameTime &game_time,
	const Input &input);
void draw(const GraphicStuff &gs,
	const std::vector<Tab> &tab_list,
	const std::vector<Btn> &btn_list,
	const GameTime &game_time,
	const Input &input);

#endif
