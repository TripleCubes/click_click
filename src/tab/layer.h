#ifndef TAB__LAYER_H
#define TAB__LAYER_H

#include <string>
#include <vector>
#include "../types/vec2i.h"
#include "../ui/btn.h"
#include "../ui/textarea.h"

struct GraphicStuff;
struct Input;

struct Layer {
	bool running = true;
	Vec2i sz;
	std::string name;
	std::vector<unsigned char> data;
	int texture_index = 0;
	int history_layer_index = 0;

	TextArea textarea;
	Btn show_hide_btn;
	Btn lock_btn;

	bool hidden = false;
	bool locked = false;
};

int layer_new(std::vector<Layer> &layer_list, GraphicStuff &gs,
	const std::string &name, Vec2i sz,
	const std::vector<unsigned char> &data, int history_layer_index);

void layer_textarea_update(Layer &layer, const GraphicStuff &gs,
	const GameTime &game_time, const Input &input, Vec2 parent_pos,
	bool active, bool show);

void layer_textarea_draw(const Layer &layer, GraphicStuff &gs,
	const GameTime &game_time, Vec2 parent_pos, bool active, bool selected);

void layer_set_texture_data(const Layer &layer, GraphicStuff &gs);

void layer_release(std::vector<Layer> &layer_list, GraphicStuff &gs,int index);

#endif
