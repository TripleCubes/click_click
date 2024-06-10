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
#include "ui/color_picker.h"
#include "ui/color_pallete.h"
#include "graphic/graphic.h"

#include "graphic_types/framebuffer.h"
#include "graphic_types/texture.h"

#include "graphic/basic_draw_op.h"
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
const GameTime &game_time,
const Input &input) {
	tab_update(tab_list[0], gs, input, vec2_new(0, 0), true);
}

void draw(const GraphicStuff &gs,
const std::vector<Tab> &tab_list,
const GameTime &game_time,
const Input &input) {
	Vec2i main_fb_sz = fb_get_sz(gs, FRAMEBUFFER_MAIN);

	bind_framebuffer(gs, FRAMEBUFFER_MAIN);
	clear(color_new(1, 1, 1, 1));

	tab_draw(tab_list[0], gs, input, vec2_new(0, 0));

	draw_text(
		gs,
		main_fb_sz,
		"This is a test thsi is a test this is a tes",
		vec2_new(10, 10),
		60,
		1,
		color_new(0, 0, 0, 1),
		vec2_new(4, 3),
		true
	);


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, gs.current_window_sz.x, gs.current_window_sz.y);
	clear(color_new(1, 1, 1, 1));

	Vec2 offset = get_main_fb_offset(gs);
	
	draw_texture(
		gs,
		fb_get_texture_id(gs, FRAMEBUFFER_MAIN),

		main_fb_sz,
		gs.current_window_sz,

		vec2_new(0, 0),
		to_vec2(main_fb_sz),
		offset,
		vec2_mul(to_vec2(main_fb_sz), gs.px_scale),
		
		false
	);
}
