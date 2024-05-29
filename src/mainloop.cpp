#include "mainloop.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

#include <vector>

#include "game_time.h"
#include "input.h"
#include "graphic_types/graphic_types.h"
#include "tab/tab.h"
#include "graphic/graphic.h"

#include "graphic_types/framebuffer.h"
#include "graphic_types/texture.h"

#include "graphic/basic_draw_op.h"
#include "graphic/draw_rect.h"
#include "graphic/draw_texture.h"
#include "graphic/draw_text.h"

#include "types/vec2.h"
#include "types/vec2i.h"
#include "types/color.h"

#include "basic_math.h"

// TEST
#include <iostream>

namespace {

Vec2i get_main_fb_sz(const GraphicStuff &gs) {
	return vec2i_div_div(gs.current_window_sz, gs.px_scale);
}

Vec2 get_main_fb_offset(const GraphicStuff &gs) {
	Vec2i main_fb_sz = get_main_fb_sz(gs);
	Vec2i offset = vec2i_mul(main_fb_sz, gs.px_scale);
	offset = vec2i_sub(gs.current_window_sz, offset);
	offset = vec2i_div_div(offset, 2);
	return to_vec2(offset);
}

Vec2 get_main_fb_mouse_pos(const GraphicStuff &gs, Vec2 mouse_pos) {
	Vec2 result = vec2_div(mouse_pos, gs.px_scale);
	result = vec2_add(result, get_main_fb_offset(gs));
	return result;
}

Vec2i get_tex_draw_mouse_pos(const Tab &tab, Vec2 main_fb_mouse_pos) {
	Vec2 result = vec2_sub(main_fb_mouse_pos, tab.pos);
	result = vec2_div(result, tab.px_scale);
	return to_vec2i(result);
}

}

void update(GraphicStuff &gs, std::vector<Tab> &tab_list,
const GameTime &game_time, const Input &input) {
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2i tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab_list[0], main_fb_mouse_pos);

	if (tex_draw_mouse_pos.x >= 0 && tex_draw_mouse_pos.y >= 0
	&& tex_draw_mouse_pos.x < tab_list[0].sz.x
	&& tex_draw_mouse_pos.y < tab_list[0].sz.y) {
		if ((input.left_down && input.mouse_move) || input.left_click) {
			px(tab_list[0], tex_draw_mouse_pos, color_new(1, 0, 0, 1));
			texture_data(gs, TEXTURE_DRAW, tab_list[0].sz, tab_list[0].data);
		}
	}
}

void draw(const GraphicStuff &gs, const std::vector<Tab> &tab_list,
const GameTime &game_time, const Input &input) {
	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);
	Vec2i tex_draw_sz = texture_get_sz(gs, TEXTURE_DRAW);

	bind_framebuffer(gs, FRAMEBUFFER_MAIN);
	clear(color_new(1, 1, 1, 1));

	draw_texture(
		gs,
		main_fb_sz,
		tex_draw_sz,
		
		vec2_new(0, 0),
		to_vec2(tex_draw_sz),
		tab_list[0].pos,
		vec2_mul(
			to_vec2(tex_draw_sz), tab_list[0].px_scale
		),
		
		texture_get_id(gs, TEXTURE_DRAW),
		true
	);

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2i tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab_list[0], main_fb_mouse_pos);

	if (tex_draw_mouse_pos.x >= 0 && tex_draw_mouse_pos.y >= 0
	&& tex_draw_mouse_pos.x < tab_list[0].sz.x
	&& tex_draw_mouse_pos.y < tab_list[0].sz.y) {
		draw_rect_sz(
			gs,
			main_fb_sz,
			vec2_new(
				floor2(main_fb_mouse_pos.x, tab_list[0].px_scale),
				floor2(main_fb_mouse_pos.y, tab_list[0].px_scale)
			),
			vec2_new(tab_list[0].px_scale, tab_list[0].px_scale),
			color_new(0, 0, 0, 1)
		);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gs.current_window_sz.x, gs.current_window_sz.y);
	clear(color_new(1, 1, 1, 1));

	Vec2 offset = get_main_fb_offset(gs);
	
	draw_texture(gs, gs.current_window_sz, main_fb_sz,
		vec2_new(0, 0),
		to_vec2(main_fb_sz),
		offset,
		vec2_mul(to_vec2(main_fb_sz), gs.px_scale),
		fb_get_texture_id(gs, FRAMEBUFFER_MAIN),
		false
	);
}
