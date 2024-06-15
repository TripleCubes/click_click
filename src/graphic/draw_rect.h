#ifndef GRAPHIC__DRAW_RECT_H
#define GRAPHIC__DRAW_RECT_H

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Color;

void draw_rect_sz_any_fb(const GraphicStuff &gs, Vec2i fb_sz,
	Vec2 pos, Vec2 sz,
	Color color);
void draw_rect_pos_any_fb(const GraphicStuff &gs, Vec2i fb_sz,
	Vec2 pos_a, Vec2 pos_b,
	Color color);
void draw_rect_border_sz_any_fb(const GraphicStuff &gs, Vec2i fb_sz, Vec2 pos,
	Vec2 sz, float border_w, Color color);

void draw_rect_sz(const GraphicStuff &gs, Vec2 pos, Vec2 sz, Color color);
void draw_rect_pos(const GraphicStuff &gs, Vec2 pos_a, Vec2 pos_b,Color color);
void draw_rect_border_sz(const GraphicStuff &gs, Vec2 pos, Vec2 sz,
	float border_w, Color color);

#endif
