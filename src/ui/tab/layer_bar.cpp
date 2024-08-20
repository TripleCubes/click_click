#include "layer_bar.h"

#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../types/color.h"

namespace {

const float BTN_HEIGHT = 12;
const Color BTN_COLOR = color_new(0.2, 0.2, 0.2, 1);

}

LayerBar layer_bar_new(Vec2 pos, Vec2 sz) {
	LayerBar layer_bar;
	layer_bar.pos = pos;
	layer_bar.sz = sz;
	
	layer_bar.add_btn = btn_new(
		vec2_new(0, sz.y - BTN_HEIGHT),
		vec2_new(BTN_HEIGHT - 1, BTN_HEIGHT),
		BTN_COLOR,
		"ICON_PLUS"
	);
	
	layer_bar.up_btn = btn_new(
		vec2_new(13, sz.y - BTN_HEIGHT),
		vec2_new(BTN_HEIGHT - 1, BTN_HEIGHT),
		BTN_COLOR,
		"ICON_UP"
	);

	layer_bar.down_btn = btn_new(
		vec2_new(24, sz.y - BTN_HEIGHT),
		vec2_new(BTN_HEIGHT - 1, BTN_HEIGHT),
		BTN_COLOR,
		"ICON_DOWN"
	);

	return layer_bar;
}

void layer_bar_update(LayerBar &layer_bar, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos, bool show) {
	Vec2 pos = vec2_add(layer_bar.pos, parent_pos);
	btn_update(layer_bar.add_btn, gs, input, pos, show);
	btn_update(layer_bar.up_btn, gs, input, pos, show);
	btn_update(layer_bar.down_btn, gs, input, pos, show);
}

void layer_bar_draw(const LayerBar &layer_bar, GraphicStuff &gs,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(layer_bar.pos, parent_pos);
	btn_draw(layer_bar.add_btn, gs, pos, false);
	btn_draw(layer_bar.up_btn, gs, pos, false);
	btn_draw(layer_bar.down_btn, gs, pos, false);
}
