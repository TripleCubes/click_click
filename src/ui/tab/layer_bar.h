#ifndef UI__TAB__LAYER_BAR_H
#define UI__TAB__LAYER_BAR_H

#include "../btn.h"
#include "../../types/vec2.h"

struct LayerBar {
	Btn add_btn;
	Btn up_btn;
	Btn down_btn;
	Btn delete_layer_btn;
	Vec2 pos;
	Vec2 sz;
};

LayerBar layer_bar_new(Vec2 pos, Vec2 sz);

void layer_bar_update(LayerBar &layer_bar, const GraphicStuff &gs,
	const Input &input, Vec2 parent_pos, bool show);

void layer_bar_draw(const LayerBar &layer_bar, GraphicStuff &gs,
	Vec2 parent_pos);

#endif
