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
#include "ui/btn.h"
#include "ui/ui_init.h"
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
#include "pos_convert.h"

// TEST
#include <iostream>

void update(GraphicStuff &gs,
std::vector<Tab> &tab_list,
std::vector<Btn> &btn_list,
const GameTime &game_time,
const Input &input) {
	btn_list_update(btn_list, gs, input);

	if (btn_clicked(btn_list, BTN_TEST)) {
		std::cout << "btn clicked" << std::endl;
	}

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

void draw(const GraphicStuff &gs,
const std::vector<Tab> &tab_list,
const std::vector<Btn> &btn_list,
const GameTime &game_time,
const Input &input) {
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

	btn_list_draw(btn_list, gs, input);


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
