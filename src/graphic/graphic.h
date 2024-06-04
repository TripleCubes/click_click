#ifndef GRAPHIC__GRAPHIC_H
#define GRAPHIC__GRAPHIC_H

struct GraphicStuff;
struct Vec2i;

const int MESH_RECT = 0;

const int SHADER_COLOR_RECT = 0;
const int SHADER_TEXTURE_RECT = 1;
const int SHADER_COLOR_PICKER_POS_SELECT = 2;
const int SHADER_COLOR_PICKER_HUE_SLIDER = 3;
const int SHADER_TAB_DRAW = 4;

const int FRAMEBUFFER_MAIN = 0;

const int TEXTURE_FONT = 0;

bool graphic_init(GraphicStuff &graphic_stuff);
void graphic_resize(GraphicStuff &graphic_stuff, Vec2i sz);

#endif
