#ifndef GRAPHIC_TYPES__TEXTURE_H
#define GRAPHIC_TYPES__TEXTURE_H

#include <string>
#include <vector>

struct GraphicStuff;
struct Vec2i;

bool texture_new(GraphicStuff &gs, const std::string &path);
unsigned int texture_get_id(const GraphicStuff &gs, int index);
Vec2i texture_get_sz(const GraphicStuff &gs, int index);
void texture_data(GraphicStuff &gs, int index, Vec2i sz,
	const std::vector<unsigned char> &data);

#endif
