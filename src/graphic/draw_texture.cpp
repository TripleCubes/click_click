#include "draw_texture.h"

#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic/graphic.h"
#include "../graphic_types/mesh.h"
#include "../graphic_types/shader.h"

void draw_texture(const GraphicStuff &gs, Vec2i fb_sz, Vec2i texture_sz,
Vec2 from_pos, Vec2 from_sz, Vec2 pos, Vec2 sz, unsigned int texture_id,
bool flip) {
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
	Vec2 texture_sz_f = to_vec2(texture_sz);

	Vec2 from_pos_normalized = vec2_new(
		from_pos.x / texture_sz_f.x,
		from_pos.y / texture_sz_f.y
	);

	Vec2 from_sz_normalized = vec2_new(
		from_sz.x / texture_sz_f.x,
		from_sz.y / texture_sz_f.y
	);

	from_pos_normalized.y = 1 - from_pos_normalized.y - from_sz_normalized.y;
	
	Vec2 pos_normalized = vec2_new(pos.x / fb_sz_f.x, pos.y / fb_sz_f.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_new(sz.x / fb_sz_f.x, sz.y / fb_sz_f.y);
	sz_normalized = vec2_mul(sz_normalized, 2);

	pos_normalized.y = -pos_normalized.y - sz_normalized.y;

	use_shader(gs, SHADER_TEXTURE_RECT);
	set_uniform_bool(gs, SHADER_TEXTURE_RECT, "u_flip", flip);
	set_uniform_vec2(gs, SHADER_TEXTURE_RECT,"u_from_pos",from_pos_normalized);
	set_uniform_vec2(gs, SHADER_TEXTURE_RECT, "u_from_sz", from_sz_normalized);
	set_uniform_vec2(gs, SHADER_TEXTURE_RECT, "u_pos", pos_normalized);
	set_uniform_vec2(gs, SHADER_TEXTURE_RECT, "u_sz", sz_normalized);
	set_uniform_texture(gs, SHADER_TEXTURE_RECT, "u_texture", 0, texture_id);
	draw_mesh(gs, MESH_TEXTURE_RECT);
}
