#ifndef TAB__LAYER_H
#define TAB__LAYER_H

#include <string>
#include <vector>
#include "../types/vec2i.h"
#include "../ui/btn.h"

struct GraphicStuff;
struct Input;

struct Layer {
	bool running = true;
	Vec2i sz;
	std::string name;
	std::vector<unsigned char> data;
	int texture_index = 0;

	Btn btn;
	Btn delete_btn;
};

int layer_new(std::vector<Layer> &layer_list, GraphicStuff &gs,
	const std::string &name, Vec2i sz, const std::vector<unsigned char> &data);

void layer_btn_update(Layer &layer, const GraphicStuff &gs, const Input &input,
	Vec2 parent_pos, bool show);

void layer_btn_draw(const Layer &layer, GraphicStuff &gs, Vec2 parent_pos,
	bool selected);

void layer_set_texture_data(const Layer &layer, GraphicStuff &gs);

void layer_release(std::vector<Layer> &layer_list, GraphicStuff &gs,int index);

#endif
