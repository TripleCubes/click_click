#include "draw_rect.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../types/color.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic/graphic.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/shader.h"

void draw_rect_sz(const GraphicStuff &gs, Vec2i fb_sz, Vec2 pos, Vec2 sz,
Color color) {
	if (sz.x == 0 || sz.y == 0) {
		return;
	}

	if (sz.x < 0) {
		sz.x = -sz.x;
		pos.x = pos.x - sz.x + 1;
	}

	if (sz.y < 0) {
		sz.y = -sz.y;
		pos.y = pos.y - sz.y + 1;
	}

	Vec2 fb_sz_f = to_vec2(fb_sz);
	
	Vec2 pos_normalized = vec2_new(pos.x / fb_sz_f.x, pos.y / fb_sz_f.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_new(sz.x / fb_sz_f.x, sz.y / fb_sz_f.y);
	sz_normalized = vec2_mul(sz_normalized, 2);

	pos_normalized.y = -pos_normalized.y - sz_normalized.y;

	use_shader(gs, SHADER_COLOR_RECT);
	set_uniform_vec2(gs, SHADER_COLOR_RECT, "u_pos", pos_normalized);
	set_uniform_vec2(gs, SHADER_COLOR_RECT, "u_sz", sz_normalized);
	set_uniform_color(gs, SHADER_COLOR_RECT, "u_color", color);
	draw_mesh(gs, MESH_RECT);
}

void draw_rect_pos(const GraphicStuff &gs, Vec2i fb_sz, Vec2 pos_a, Vec2 pos_b,
Color color){
	if (pos_a.x == pos_b.x || pos_a.y == pos_b.y) {
		return;
	}

	if (pos_b.x < pos_a.x) {
		float swap = pos_a.x;
		pos_a.x = pos_b.x;
		pos_b.x = swap;
	}

	if (pos_b.y < pos_a.y) {
		float swap = pos_a.y;
		pos_a.y = pos_b.y;
		pos_b.y = swap;
	}

	draw_rect_sz(gs, fb_sz, pos_a, vec2_sub(pos_b, pos_a), color);
}

void draw_rect_border_sz(const GraphicStuff &gs, Vec2i fb_sz, Vec2 pos,
Vec2 sz, float border_w, Color color) {
	if (sz.x == 0 || sz.y == 0) {
		return;
	}

	if (sz.x < 0) {
		sz.x = -sz.x;
		pos.x = pos.x - sz.x + 1;
	}

	if (sz.y < 0) {
		sz.y = -sz.y;
		pos.y = pos.y - sz.y + 1;
	}

	Vec2 top_left = vec2_new(pos.x, pos.y);
	Vec2 top_right = vec2_new(pos.x + sz.x - 1, pos.y);
	Vec2 bottom_right = vec2_new(pos.x + sz.x - 1, pos.y + sz.y - 1);
	Vec2 bottom_left = vec2_new(pos.x, pos.y + sz.y - 1);

	draw_rect_sz(
		gs,
		fb_sz,
		top_left,
		vec2_new(sz.x, border_w),
		color
	);
	draw_rect_sz(
		gs,
		fb_sz,
		top_right,
		vec2_new(- border_w, sz.y),
		color
	);
	draw_rect_sz(
		gs,
		fb_sz,
		bottom_right,
		vec2_new(- sz.x, - border_w),
		color);
	draw_rect_sz(
		gs,
		fb_sz,
		bottom_left,
		vec2_new(border_w, - sz.y),
		color);
}
