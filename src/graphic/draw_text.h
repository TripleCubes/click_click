#ifndef GRAPHIC__DRAW_TEXT_H
#define GRAPHIC__DRAW_TEXT_H

#include <string>

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Color;

const int ICON_PLUS = 0;
const int ICON_X = 1;

const int CHAR_W = 4;
const int REAL_CHAR_H = 6;
const int CHAR_H = 8;

Vec2 draw_text(GraphicStuff &gs,
	const std::string &text, Vec2 pos, float box_w, int scale,
	Color color, Vec2 margin, bool flip_color);

Vec2 draw_icon(GraphicStuff &gs,
	int icon, Vec2 pos, int scale, Color color, Vec2 bkg_margin,
	bool flip_color);

#endif
