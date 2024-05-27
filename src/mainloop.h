#ifndef MAINLOOP_H
#define MAINLOOP_H

struct GraphicStuff;
struct GameTime;

void update(const GameTime &game_time);
void draw(const GraphicStuff &gs, const GameTime &game_time);

#endif
