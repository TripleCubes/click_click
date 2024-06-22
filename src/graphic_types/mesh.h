#ifndef GRAPHIC_TYPES__MESH_H
#define GRAPHIC_TYPES__MESH_H

#include <vector>
#include <array>

const int VERT_SZ = 9;

struct GraphicStuff;

int mesh_new(GraphicStuff &graphic_stuff);
void mesh_add(GraphicStuff &gs, int index, const std::vector<float> &add);
void mesh_add_arr(GraphicStuff &gs, int index,
	const std::array<float, VERT_SZ * 6> &add);
void mesh_clear(GraphicStuff &gs, int index);
void mesh_set(GraphicStuff &gs, int index);
void mesh_draw(const GraphicStuff &graphic_stuff, int index);
void mesh_release(GraphicStuff &gs, int index);

#endif
