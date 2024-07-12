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
#include "graphic/draw_texture.h"
#include "graphic/draw_text.h"

#include "types/vec2.h"
#include "types/vec2i.h"

#include "basic_math.h"
#include "pos_convert.h"

// TEST
#include <iostream>

namespace {

const Color BLUR_COLOR = color_new(129/255.0f, 255/255.0f, 83/255.0f, 1);
const Vec2 TAB_OFFSET = vec2_new(0, 0);

void draw_canvas(GraphicStuff &gs, const Tab &tab, const Input &input) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	clear(color_new(1, 1, 1, 1));

	tab_canvas_draw(tab, gs, input, TAB_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui(
GraphicStuff &gs,
const Tab &tab,
const GameTime &game_time
) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	tab_ui_draw(tab, gs, TAB_OFFSET);

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

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void create_blurred_texture(GraphicStuff &gs, unsigned int from_texture_id,
Color color, bool flip) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	bind_framebuffer(gs, FB_BLUR_0);
	clear(color_new(0, 0, 0, 0));
	use_shader(gs, SHADER_BLUR);
	set_uniform_vec2(gs, SHADER_BLUR, "u_main_fb_sz", to_vec2(main_fb_sz));
	set_uniform_color(gs, SHADER_BLUR, "u_color", color);
	set_uniform_bool(gs, SHADER_BLUR, "u_flip", false);
	set_uniform_bool(gs, SHADER_BLUR, "u_horizontal", true);
	set_uniform_texture(gs, SHADER_BLUR, "u_texture", 0,
		from_texture_id);

	mesh_draw(gs, MESH_RECT);

	bind_framebuffer(gs, FB_BLUR_1);
	clear(color_new(0, 0, 0, 0));
	set_uniform_bool(gs, SHADER_BLUR, "u_flip", flip);
	set_uniform_bool(gs, SHADER_BLUR, "u_horizontal", false);
	set_uniform_texture(gs, SHADER_BLUR, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_0));

	mesh_draw(gs, MESH_RECT);
}

void draw_blurred_rects(GraphicStuff &gs, const Tab &tab) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	auto draw = [&gs, main_fb_sz](Vec2 pos, Vec2 sz) -> void {
		draw_texture(
			gs,
			main_fb_sz,
			pos,
			sz,
			pos,
			sz,
			color_new(0, 0, 0, 0),
			false
		);
	};

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	draw(vec2_new(0, 0), vec2_new(100, main_fb_sz.y));
	tab_blur_rects_draw(tab, gs, TAB_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

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
	tab_update(tab_list[0], gs, input, game_time, TAB_OFFSET, true);
}

void draw(GraphicStuff &gs,
const std::vector<Tab> &tab_list,
const GameTime &game_time,
const Input &input) {
	bool mouse_in_window = in_rect(
		input.mouse_pos, vec2_new(0, 0), to_vec2(gs.current_window_sz)
	);
	if ((input.mouse_event && mouse_in_window) || input.key_event
	|| game_time.frame_passed == 0) {
		draw_canvas(gs, tab_list[0], input);
		create_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
			BLUR_COLOR, false);
		create_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
			BLUR_COLOR, true);
		draw_blurred_rects(gs, tab_list[0]);
		draw_ui(gs, tab_list[0], game_time);
	}

	draw_fb_main(gs);
}
