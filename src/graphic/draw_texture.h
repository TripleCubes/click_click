#ifndef GRAPHIC__DRAW_TEXTURE_H
#define GRAPHIC__DRAW_TEXTURE_H

struct GraphicStuff;
struct Vec2;
struct Vec2i;

void draw_texture(const GraphicStuff &gs, Vec2i texture_sz, Vec2i fb_sz,
	Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz, unsigned int texture_id,
	bool flip);

#endif
