#ifndef GRAPHIC_TYPES__TEXTURE_H
#define GRAPHIC_TYPES__TEXTURE_H

#include <string>
#include <vector>

struct GraphicStuff;
struct Vec2i;

int texture_new(GraphicStuff &gs, const std::string &path);
int texture_blank_new(GraphicStuff &gs, int w, int h);
int texture_blank_new_red(GraphicStuff &gs, int w, int h);
unsigned int texture_get_id(const GraphicStuff &gs, int index);
Vec2i texture_get_sz(const GraphicStuff &gs, int index);
void texture_data(GraphicStuff &gs, int index, Vec2i sz,
	const std::vector<unsigned char> &data);
void texture_data_red(GraphicStuff &gs, int index, Vec2i sz,
	const std::vector<unsigned char> &data);
void texture_release(GraphicStuff &gs, int index);

#endif
