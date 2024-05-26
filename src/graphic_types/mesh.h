#ifndef GRAPHIC_TYPES__MESH_H
#define GRAPHIC_TYPES__MESH_H

#include <vector>

struct GraphicStuff;

bool mesh_new(GraphicStuff &graphic_stuff,const std::vector<float> &verticies);
void draw_mesh(const GraphicStuff &graphic_stuff, int index);

#endif
