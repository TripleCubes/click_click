#include "draw_rect.h"

#include <array>
#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/mesh.h"

#include "graphic.h"

void draw_rect(GraphicStuff &gs, Vec2 pos, Vec2 sz, Color color) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	
	pos.y = main_fb_sz.y - pos.y - sz.y;

	Vec2 a = vec2_new(pos.x / main_fb_sz.x, pos.y / main_fb_sz.y);
	Vec2 b = vec2_add(a, vec2_new(sz.x / main_fb_sz.x, 0));
	Vec2 c = vec2_add(a, vec2_new(sz.x / main_fb_sz.x, sz.y / main_fb_sz.y));
	Vec2 d = vec2_add(a, vec2_new(0, sz.y / main_fb_sz.y));

	a = vec2_mul(a, 2);
	a = vec2_add(a, vec2_new(-1, -1));
	b = vec2_mul(b, 2);
	b = vec2_add(b, vec2_new(-1, -1));
	c = vec2_mul(c, 2);
	c = vec2_add(c, vec2_new(-1, -1));
	d = vec2_mul(d, 2);
	d = vec2_add(d, vec2_new(-1, -1));

	std::array<float, VERT_SZ * 6> verts = {
		a.x, a.y, -1, -1, color.r, color.g, color.b, color.a, 0,
		d.x, d.y, -1, -1, color.r, color.g, color.b, color.a, 0,
		b.x, b.y, -1, -1, color.r, color.g, color.b, color.a, 0,

		b.x, b.y, -1, -1, color.r, color.g, color.b, color.a, 0,
		d.x, d.y, -1, -1, color.r, color.g, color.b, color.a, 0,
		c.x, c.y, -1, -1, color.r, color.g, color.b, color.a, 0,
	};

	mesh_add_arr(gs, MESH_BASIC_DRAW, verts);
}

void draw_rect_border(GraphicStuff &gs,
Vec2 pos, Vec2 sz, Color color, float border_w) {
	Vec2 top_left = vec2_new(pos.x, pos.y);
	Vec2 top_right = vec2_new(pos.x + sz.x, pos.y);
	Vec2 bottom_right = vec2_new(pos.x + sz.x, pos.y + sz.y);
	Vec2 bottom_left = vec2_new(pos.x, pos.y + sz.y);

	draw_rect(
		gs,
		top_left,
		vec2_new(sz.x, border_w),
		color
	);
	draw_rect(
		gs,
		vec2_add(top_right, vec2_new(-border_w, 0)),
		vec2_new(border_w, sz.y),
		color
	);
	draw_rect(
		gs,
		vec2_add(bottom_right, vec2_new(-sz.x, -border_w)),
		vec2_new(sz.x, border_w),
		color
	);
	draw_rect(
		gs,
		vec2_add(bottom_left, vec2_new(0, -sz.y)),
		vec2_new(border_w, sz.y),
		color
	);
}
