#ifndef GRAPHIC__DRAW_TEXTURE_H
#define GRAPHIC__DRAW_TEXTURE_H

struct GraphicStuff;
struct Vec2i;
struct Vec2;
struct Color;

void draw_texture(GraphicStuff &gs, Vec2i tex_sz,
Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz, Color color, bool flip_color);

#endif
