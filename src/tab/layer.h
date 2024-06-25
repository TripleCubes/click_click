#ifndef TAB__LAYER_H
#define TAB__LAYER_H

#include <string>
#include <vector>
#include "../types/vec2i.h"
#include "../ui/btn.h"

struct GraphicStuff;

struct Layer {
	bool running = true;
	Vec2i sz;
	std::string name;
	std::vector<unsigned char> data;
	int texture_index = 0;

	Btn btn;
};

int layer_new(std::vector<Layer> &layer_list, GraphicStuff &gs,
	const std::string &name, Vec2i sz, const std::vector<unsigned char> &data);

void layer_btn_update(Layer &layer, const GraphicStuff &gs, const Input &input,
	Vec2 parent_pos, bool show);

void layer_btn_draw(const Layer &layer, GraphicStuff &gs, Vec2 parent_pos,
	bool selected);

void layer_data(Layer &layer, Vec2i pos, unsigned char pallete_index);

void layer_set_texture_data(const Layer &layer, GraphicStuff &gs);

void layer_release(std::vector<Layer> &layer_list, GraphicStuff &gs,int index);

#endif
