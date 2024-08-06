#ifndef GRAPHIC__GRAPHIC_H
#define GRAPHIC__GRAPHIC_H

struct GraphicStuff;
struct Vec2i;

const int MESH_RECT = 0;
const int MESH_BASIC_DRAW = 1;

const int SHADER_BASIC_DRAW = 0;
const int SHADER_FB_MAIN = 1;
const int SHADER_LAYER_DRAW = 2;
const int SHADER_COLOR_PICKER_POS_SELECT = 3;
const int SHADER_COLOR_PICKER_HUE_SLIDER = 4;
const int SHADER_BLUR = 5;

const int FB_MAIN = 0;
const int FB_BLUR_0 = 1;
const int FB_BLUR_1 = 2;

const int TEXTURE_FONT = 0;
const int TEXTURE_ICONS = 1;

bool graphic_init(GraphicStuff &graphic_stuff);
void graphic_resize(GraphicStuff &graphic_stuff, Vec2i sz);

#endif
