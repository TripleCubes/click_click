#ifndef GRAPHIC__DRAW_TEXTURE_H
#define GRAPHIC__DRAW_TEXTURE_H

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Color;

void draw_texture(const GraphicStuff &gs, unsigned int texture_id,
	Vec2i texture_sz, Vec2i fb_sz,
	Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz,
	bool flip);
void draw_texture_one_color(
	const GraphicStuff &gs, unsigned int texture_id,
	Vec2i texture_sz, Vec2i fb_sz,
	Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz,
	bool flip, Color color, bool flip_color);

#endif
