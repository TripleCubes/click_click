#ifndef GRAPHIC__GRAPHIC_H
#define GRAPHIC__GRAPHIC_H

struct GraphicStuff;
struct Vec2i;

const int MESH_RECT = 0;

const int SHADER_COLOR_RECT = 0;
const int SHADER_TEXTURE_RECT = 1;
const int SHADER_COLOR_PICKER = 2;

const int FRAMEBUFFER_MAIN = 0;

const int TEXTURE_FONT = 0;
const int TEXTURE_DRAW = 1;

bool graphic_init(GraphicStuff &graphic_stuff);
void graphic_resize(GraphicStuff &graphic_stuff, Vec2i sz);

#endif
