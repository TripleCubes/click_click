#include "layer.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../types/vec2.h"
#include "../types/color.h"
#include "../input.h"
#include "../consts.h"

namespace {

const Vec2 TEXTAREA_SZ = vec2_new(81, 12);
const Vec2 DELETE_BTN_SZ = vec2_new(11, 12);
const Color TEXTAREA_COLOR = BTN_TEXTAREA_COLOR;

int get_blank_index(const std::vector<Layer> &layer_list) {
	for (int i = 0; i < (int)layer_list.size(); i++) {
		if (!layer_list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int layer_new(std::vector<Layer> &layer_list,
GraphicStuff &gs, const std::string &name, Vec2i sz,
const std::vector<unsigned char> &data) {
	int index = get_blank_index(layer_list);
	if (index == -1) {
		Layer new_layer;
		layer_list.push_back(new_layer);

		index = (int)layer_list.size() - 1;
	}
	
	Layer &layer = layer_list[index];

	layer.name = name;
	layer.sz = sz;
	layer.data = data;

	layer.texture_index = texture_blank_new_red(gs, sz.x, sz.y);
	texture_data_red(gs, layer.texture_index, sz, layer.data);

	layer.textarea = textarea_new(
		vec2_new(0, 0),
		TEXTAREA_SZ,
		TEXTAREA_COLOR,
		name
	);
	layer.show_hide_btn = btn_new(
		vec2_new(TEXTAREA_SZ.x, 0),
		DELETE_BTN_SZ,
		TEXTAREA_COLOR,
		"ICON_UP"
	);
	layer.lock_btn = btn_new(
		vec2_new(TEXTAREA_SZ.x + 12, 0),
		DELETE_BTN_SZ,
		TEXTAREA_COLOR,
		"ICON_UNLOCKED"
	);

	layer.running = true;

	return index;
}

void layer_textarea_update(Layer &layer, const GraphicStuff &gs,
const GameTime &game_time, const Input &input, Vec2 parent_pos,
bool active, bool show) {
	textarea_update(layer.textarea,gs,game_time,input,parent_pos,active,show);
	btn_update(layer.show_hide_btn, gs, input, parent_pos, show);
	btn_update(layer.lock_btn, gs, input, parent_pos, show);

	if (!show) {
		return;
	}

	if (layer.show_hide_btn.clicked) {
		layer.hidden = !layer.hidden;

		if (layer.hidden) {
			layer.show_hide_btn.text = "ICON_MINUS";
		}
		else {
			layer.show_hide_btn.text = "ICON_UP";
		}
	}
	
	if (layer.lock_btn.clicked) {
		layer.locked = !layer.locked;

		if (layer.locked) {
			layer.lock_btn.text = "ICON_LOCKED";
		}
		else {
			layer.lock_btn.text = "ICON_UNLOCKED";
		}
	}
}

void layer_textarea_draw(const Layer &layer, GraphicStuff &gs,
const GameTime &game_time, Vec2 parent_pos, bool active, bool selected) {
	textarea_draw(layer.textarea, gs, game_time, parent_pos, active, selected);
	btn_draw(layer.show_hide_btn, gs, parent_pos, false);
	btn_draw(layer.lock_btn, gs, parent_pos, false);
}

void layer_set_texture_data(const Layer &layer, GraphicStuff &gs) {
	texture_data_red(gs, layer.texture_index, layer.sz, layer.data);
}

void layer_release(std::vector<Layer> &layer_list, GraphicStuff &gs,int index){
	Layer &layer = layer_list[index];
	layer.name = "";
	layer.data.clear();
	texture_release(gs, layer.texture_index);
	layer.running = false;
}
