#ifndef UI__TAB__BTN_PANEL_H
#define UI__TAB__BTN_PANEL_H

#include "../../types/vec2.h"
#include "../btn.h"

const Vec2 BTN_PANEL_SZ = vec2_new(39, 11);

struct BtnPanel {
	Vec2 pos;
	
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
