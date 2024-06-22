#ifndef GRAPHIC__DRAW_RECT_H
#define GRAPHIC__DRAW_RECT_H

struct GraphicStuff;
struct Vec2;
struct Color;

void draw_rect(GraphicStuff &gs, Vec2 pos, Vec2 sz, Color color);
void draw_rect_border(GraphicStuff &gs,
	Vec2 pos, Vec2 sz, Color color, float border_w);

#endif
