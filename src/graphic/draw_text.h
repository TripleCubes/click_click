#ifndef GRAPHIC__DRAW_TEXT_H
#define GRAPHIC__DRAW_TEXT_H

#include <string>

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Color;

void draw_text(const GraphicStuff &gs, Vec2i fb_sz, const std::string &text,
	Vec2 pos, float box_w, int scale, Color color, bool flip_color);
void draw_text_with_bkg(const GraphicStuff &gs, Vec2i fb_sz,
	const std::string &text, Vec2 pos, float box_w, int scale,
	Color color, Vec2 margin, bool flip_color);

#endif
