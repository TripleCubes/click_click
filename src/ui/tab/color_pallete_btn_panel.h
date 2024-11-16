#ifndef UI__TAB__COLOR_PALLETE_BTN_PANEL_H
#define UI__TAB__COLOR_PALLETE_BTN_PANEL_H

#include "../btn.h"
#include "../../types/vec2.h"

struct GraphicStuff;
struct Input;

extern const Vec2 COLOR_PALLETE_BTN_PANEL_SZ;

struct ColorPalleteBtnPanel {
	Vec2 pos;
	Btn copy_btn;
	Btn paste_btn;	
};

ColorPalleteBtnPanel color_pallete_btn_panel_new(Vec2 pos);

void color_pallete_btn_panel_update(ColorPalleteBtnPanel &panel,
	GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show);

void color_pallete_btn_panel_draw(const ColorPalleteBtnPanel &panel,
	GraphicStuff &gs, Vec2 parent_pos);

#endif
