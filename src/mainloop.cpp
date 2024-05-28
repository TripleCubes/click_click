#include "mainloop.h"

#include "game_time.h"
#include "input.h"
#include "graphic_types/graphic_types.h"
#include "graphic/graphic.h"

#include "graphic_types/framebuffer.h"
#include "graphic_types/texture.h"

#include "graphic/basic_draw_op.h"
#include "graphic/draw_rect.h"
#include "graphic/draw_texture.h"
#include "graphic/draw_text.h"

#include "types/vec2.h"
#include "types/color.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

void update(const GraphicStuff &gs, const GameTime &game_time,
const Input &input) {
}

void draw(const GraphicStuff &gs, const GameTime &game_time) {
	bind_framebuffer(gs, FRAMEBUFFER_MAIN);
	clear(color_new(1, 1, 1, 1));

	draw_rect_sz(gs, fb_get_sz(gs, FRAMEBUFFER_MAIN),
		vec2_new(0, 0),
		vec2_new(10, 10),
		color_new(159/255.0f, 222/255.0f, 146/255.0f, 1));
	draw_rect_sz(gs, fb_get_sz(gs, FRAMEBUFFER_MAIN),
		vec2_new(10, 10),
		vec2_new(80, 80),
		color_new(168/255.0f, 237/255.0f, 227/255.0f, 1));
	draw_rect_sz(gs, fb_get_sz(gs, FRAMEBUFFER_MAIN),
		vec2_new(80, 90),
		vec2_new(30, 10),
		color_new(244/255.0f, 133/255.0f, 124/255.0f, 1));

	draw_text(
		gs,
		std::to_string(1/game_time.delta),
		fb_get_sz(gs, FRAMEBUFFER_MAIN),
		vec2_new(10, 10),
		vec2_new(80, 80),
		1,
		color_new(1, 1, 1, 1)
	);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gs.current_window_sz.x, gs.current_window_sz.y);
	clear(color_new(0, 0, 0, 1));

	Vec2i offset = vec2i_mul(fb_get_sz(gs, FRAMEBUFFER_MAIN), gs.px_scale);
	offset = vec2i_sub(gs.current_window_sz, offset);
	offset = vec2i_div_div(offset, 2);

	draw_texture(gs, gs.current_window_sz, fb_get_sz(gs, FRAMEBUFFER_MAIN),
		vec2_new(0, 0),
		to_vec2(fb_get_sz(gs, FRAMEBUFFER_MAIN)),
		to_vec2(offset),
		vec2_mul(to_vec2(fb_get_sz(gs, FRAMEBUFFER_MAIN)), gs.px_scale),
		fb_get_texture_id(gs, FRAMEBUFFER_MAIN),
		false
	);
}
