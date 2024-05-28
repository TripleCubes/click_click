#ifndef MAINLOOP_H
#define MAINLOOP_H

struct GraphicStuff;
struct GameTime;
struct Input;

void update(const GraphicStuff &gs, const GameTime &game_time,
	const Input &input);
void draw(const GraphicStuff &gs, const GameTime &game_time);

#endif
