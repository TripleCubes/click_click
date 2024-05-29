#ifndef GRAPHIC_TYPES__FRAMEBUFFER_H
#define GRAPHIC_TYPES__FRAMEBUFFER_H

struct GraphicStuff;
struct Vec2i;

int framebuffer_new(GraphicStuff &gs, int w, int h);
void bind_framebuffer(const GraphicStuff &gs, int index);
unsigned int fb_get_texture_id(const GraphicStuff &gs, int index);
Vec2i fb_get_sz(const GraphicStuff &gs, int index);
void fb_resize(GraphicStuff &gs, int index, Vec2i sz);

#endif
