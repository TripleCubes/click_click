#include "layer_bar.h"

#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../types/color.h"

namespace {

const float BTN_HEIGHT = 12;
const Color BTN_COLOR = color_new(125/255.0f, 240/255.0f, 154/255.0f, 1);

}

LayerBar layer_bar_new(Vec2 pos, Vec2 sz) {
	LayerBar layer_bar;
	layer_bar.pos = pos;
	layer_bar.sz = sz;
	
	layer_bar.add_btn = btn_new(
		vec2_new(pos.x + 10, pos.y + sz.y - BTN_HEIGHT - 10),
		vec2_new(BTN_HEIGHT - 1, BTN_HEIGHT),
		BTN_COLOR,
		"A"
	);
	
	return layer_bar;
}

void layer_bar_update(LayerBar &layer_bar, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool show) {
	btn_update(layer_bar.add_btn, gs, input, parent_pos, show);
}

void layer_bar_draw(const LayerBar &layer_bar, GraphicStuff &gs,
Vec2 parent_pos) {
	btn_draw(layer_bar.add_btn, gs, parent_pos, false);
}
