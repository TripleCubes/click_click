#include "layer.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../types/vec2.h"
#include "../types/color.h"
#include "../input.h"

namespace {

const Vec2 BTN_SZ = vec2_new(60, 12);
const Color BTN_COLOR = color_new(125/255.0f, 240/255.0f, 154/255.0f, 1);

int get_blank_index(const std::vector<Layer> &layer_list) {
	for (int i = 0; i < (int)layer_list.size(); i++) {
		if (!layer_list[i].running) {
			return i;
		}
	}

	return -1;
}

}

int layer_new(std::vector<Layer> &layer_list, GraphicStuff &gs,
const std::string &name, Vec2i sz, const std::vector<unsigned char> &data) {
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

	layer.btn = btn_new(
		vec2_new(0, 0),
		BTN_SZ,
		BTN_COLOR,
		name
	);

	return index;
}

void layer_btn_update(Layer &layer, const GraphicStuff &gs, const Input &input,
Vec2 parent_pos, bool show) {
	btn_update(layer.btn, gs, input, parent_pos, show);
}

void layer_btn_draw(const Layer &layer, GraphicStuff &gs, Vec2 parent_pos,
bool selected) {
	btn_draw(layer.btn, gs, parent_pos, selected);
}

void layer_data(Layer &layer, Vec2i pos, unsigned char pallete_index) {
	int data_index = layer.sz.y * pos.y + pos.x;
	layer.data[data_index] = pallete_index;
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
