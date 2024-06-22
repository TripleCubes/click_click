#include "draw_texture.h"

#include <array>

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/mesh.h"
#include "graphic.h"
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../types/color.h"

void draw_texture(GraphicStuff &gs, Vec2i tex_sz,
Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz, Color color, bool flip_color) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	pos.y = main_fb_sz.y - pos.y - sz.y;
	from_pos.y = tex_sz.y - from_pos.y - from_sz.y;

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

	Vec2 uv_a = vec2_new(from_pos.x / tex_sz.x, from_pos.y / tex_sz.y);
	Vec2 uv_b = vec2_add(uv_a, vec2_new(from_sz.x / tex_sz.x, 0));
	Vec2 uv_c = vec2_add(uv_a, vec2_new(from_sz.x / tex_sz.x,
	                                    from_sz.y / tex_sz.y));
	Vec2 uv_d = vec2_add(uv_a, vec2_new(0, from_sz.y / tex_sz.y));

	float flip_color_f = 0;
	if (flip_color) {
		flip_color_f = 1;
	}

	std::array<float, VERT_SZ * 6> verts = {
		a.x, a.y, uv_a.x, uv_a.y, color.r,color.g,color.b,color.a,flip_color_f,
		d.x, d.y, uv_d.x, uv_d.y, color.r,color.g,color.b,color.a,flip_color_f,
		b.x, b.y, uv_b.x, uv_b.y, color.r,color.g,color.b,color.a,flip_color_f,

		b.x, b.y, uv_b.x, uv_b.y, color.r,color.g,color.b,color.a,flip_color_f,
		d.x, d.y, uv_d.x, uv_d.y, color.r,color.g,color.b,color.a,flip_color_f,
		c.x, c.y, uv_c.x, uv_c.y, color.r,color.g,color.b,color.a,flip_color_f,
	};

	mesh_add_arr(gs, MESH_BASIC_DRAW, verts);
}
