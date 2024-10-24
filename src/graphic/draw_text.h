#ifndef GRAPHIC__DRAW_TEXT_H
#define GRAPHIC__DRAW_TEXT_H

#include <string>

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Color;

const int ICON_PLUS            = 0;
const int ICON_X               = 1;
const int ICON_MINUS           = 2;
const int ICON_CURVE           = 3;
const int ICON_FILL            = 4;
const int ICON_PEN             = 5;
const int ICON_SELECT          = 6;
const int ICON_UP              = 7;
const int ICON_DOWN            = 8;
const int ICON_SZ_1            = 9;

const int ICON_SZ_2            = 10;
const int ICON_SZ_3            = 11;
const int ICON_SZ_3_5          = 12;
const int ICON_SZ_4            = 13;
const int ICON_FILL_ALL        = 14;
const int ICON_FILL_DITHERED   = 15;
const int ICON_FREE_SELECT     = 16;
const int ICON_MAGIC_WAND      = 17;
const int ICON_SELECT_ADD      = 18;
const int ICON_SELECT_SUBTRACT = 19;

const int ICON_ARROW_UP        = 20;
const int ICON_FILE            = 21;
const int ICON_FOLDER          = 22;
const int ICON_IMAGE           = 23;
const int ICON_SZ_4_5          = 24;
const int ICON_PIN             = 25;
const int ICON_CIRCLE          = 26;

const int ICON_HAMBURGER       = 27;
const int ICON_ERASER          = 28;
const int ICON_EYE             = 29;

const int ICON_UNLOCKED        = 30;
const int ICON_EYE_CLOSED      = 31;
const int ICON_LOCKED          = 32;

const int ICON_TRASH           = 33;

const int ICON_COUNT           = 34;

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
