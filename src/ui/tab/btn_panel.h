#ifndef UI__TAB__BTN_PANEL_H
#define UI__TAB__BTN_PANEL_H

#include "../../types/vec2.h"
#include "../btn.h"

extern const Vec2 BTN_PANEL_SZ;

struct BtnPanel {
	Vec2 pos;
	
	Btn undo_btn;
	Btn redo_btn;
	Btn zoom_out_btn;
	Btn zoom_in_btn;
	Btn zoom_0_btn;
};

BtnPanel btn_panel_new(Vec2 pos);

void btn_panel_update(BtnPanel &btn_panel, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos, bool show);

void btn_panel_draw(const BtnPanel &btn_panel, GraphicStuff &gs,
	const Input &input, Vec2 parent_pos);

#endif
