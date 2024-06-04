#ifndef POS_CONVERT_H
#define POS_CONVERT_H

struct GraphicStuff;
struct Vec2;
struct Vec2i;
struct Tab;

Vec2i get_main_fb_sz(const GraphicStuff &gs);
Vec2 get_main_fb_offset(const GraphicStuff &gs);
Vec2 get_main_fb_mouse_pos(const GraphicStuff &gs, Vec2 mouse_pos);
Vec2i get_tex_draw_mouse_pos(const Tab &tab, Vec2 main_fb_tab_pos,
	Vec2 main_fb_mouse_pos);

#endif
