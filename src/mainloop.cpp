#include "mainloop.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

#include <vector>

#include "game_time.h"
#include "input.h"
#include "tab/tab.h"

#include "graphic_types/graphic_types.h"
#include "graphic_types/framebuffer.h"
#include "graphic_types/shader.h"
#include "graphic_types/mesh.h"
#include "graphic_types/texture.h"

#include "graphic/graphic.h"
#include "graphic/basic_draw_op.h"

#include "types/vec2.h"
#include "types/vec2i.h"

#include "basic_math.h"
#include "pos_convert.h"

// TEST
#include <iostream>
#include "graphic/draw_text.h"

namespace {

void draw_fb_main(const GraphicStuff &gs) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 offset = get_main_fb_offset(gs);

	Vec2 pos_normalized = vec2_new(offset.x / gs.current_window_sz.x,
	                               offset.y / gs.current_window_sz.y);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));

	Vec2 sz_normalized = vec2_mul(to_vec2(main_fb_sz), gs.px_scale);
	sz_normalized.x /= gs.current_window_sz.x;
	sz_normalized.y /= gs.current_window_sz.y;
	sz_normalized = vec2_mul(sz_normalized, 2);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gs.current_window_sz.x, gs.current_window_sz.y);
	clear(color_new(1, 1, 1, 1));

	use_shader(gs, SHADER_FB_MAIN);
	set_uniform_vec2(gs, SHADER_FB_MAIN, "u_pos", pos_normalized);
	set_uniform_vec2(gs, SHADER_FB_MAIN, "u_sz", sz_normalized);
	set_uniform_texture(gs, SHADER_FB_MAIN, "u_texture",
		0, fb_get_texture_id(gs, FB_MAIN));

	mesh_draw(gs, MESH_RECT);
}

}

void update(GraphicStuff &gs,
std::vector<Tab> &tab_list,
const GameTime &game_time,
const Input &input) {
	tab_update(tab_list[0], gs, input, vec2_new(0, 0), true);
}

void draw(GraphicStuff &gs,
const std::vector<Tab> &tab_list,
const GameTime &game_time,
const Input &input) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	clear(color_new(1, 1, 1, 1));

	tab_draw(tab_list[0], gs, input, vec2_new(0, 0));

	draw_text(
		gs,
		std::to_string((int)(1/game_time.delta)),
		vec2_new(main_fb_sz.x - 30, 10),
		20,
		1,
		color_new(0, 0, 0, 1),
		vec2_new(4, 3),
		true
	);

	draw_text(
		gs,
		std::to_string((int)(1/game_time.frame_time)),
		vec2_new(main_fb_sz.x - 30, 22),
		20,
		1,
		color_new(0, 0, 0, 1),
		vec2_new(4, 3),
		true
	);

	draw_text(
		gs,
		"Hello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a testHello this is a test, Hello this is a test",
		vec2_new(10, 10),
		300,
		1,
		color_new(0, 0, 0, 1),
		vec2_new(4, 3),
		false
	);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);

	draw_fb_main(gs);
}
