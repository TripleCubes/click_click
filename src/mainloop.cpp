#include "mainloop.h"

#ifndef __EMSCRIPTEN__
#include <glad/glad.h>
#else
#include <GLES2/gl2.h>
#endif

#include <vector>

#include "game_time.h"
#include "input.h"
#include "input_map.h"
#include "tab/tab.h"
#include "ui/tab_bar.h"
#include "ui/file_picker/file_picker.h"

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
#include "consts.h"
#include "states.h"

// TEST
#include <iostream>

namespace {

const Color BLUR_COLOR = color_new(1, 1, 1, 1);
const Vec2 TAB_OFFSET = vec2_new(0, 0);
const Vec2 FILE_PICKER_OFFSET = vec2_new(0, 0);

void draw_canvas_bkg(GraphicStuff &gs, const Tab &tab) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	clear(color_new(0.69, 0.94, 0.62, 1));

	tab_bkg_draw(tab, gs, TAB_OFFSET);
	
	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_canvas(GraphicStuff &gs, const Tab &tab, const Input &input) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	tab_canvas_draw(tab, gs, input, TAB_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_blurred_texture(GraphicStuff &gs, unsigned int from_texture_id,
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

	draw(vec2_new(0, 0), vec2_new(SIDE_BAR_W, main_fb_sz.y));
	draw(
		vec2_new(SIDE_BAR_W + 4, 4),
		vec2_new(main_fb_sz.x - SIDE_BAR_W - 4 * 2, TOP_BAR_H - 4)
	);
	tab_blur_rects_draw(tab, gs, TAB_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_blurred_rects_1(GraphicStuff &gs, const Tab &tab) {
//	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

//	auto draw = [&gs, main_fb_sz](Vec2 pos, Vec2 sz) -> void {
//		draw_texture(
//			gs,
//			main_fb_sz,
//			pos,
//			sz,
//			pos,
//			sz,
//			color_new(0, 0, 0, 0),
//			false
//		);
//	};

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	
	file_picker_bkg_draw(gs, FILE_PICKER_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui(
GraphicStuff &gs,
const TabBar &tab_bar,
const Tab &tab,
const Input &input,
const GameTime &game_time
) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	tab_bar_draw(tab_bar, gs);
	tab_ui_draw(tab, gs, input, game_time, TAB_OFFSET);

	draw_text(
		gs,
		std::to_string((int)(1/game_time.delta)),
		vec2_new(main_fb_sz.x - 30, TOP_BAR_H + 8),
		20,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		true
	);

	draw_text(
		gs,
		std::to_string((int)(1/game_time.frame_time)),
		vec2_new(main_fb_sz.x - 30, TOP_BAR_H + 20),
		20,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		true
	);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui_1(
const States &states,
GraphicStuff &gs,
const TabBar &tab_bar,
const FilePicker &file_picker,
const Tab &tab,
const Input &input,
const GameTime &game_time
) {
//	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	if (states.file_picker_opening) {
		file_picker_ui_draw(file_picker, gs, input, game_time,
			FILE_PICKER_OFFSET);
	}

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
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

void draw_cursor(GraphicStuff &gs, const Input &input) {
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	Vec2 draw_pos_add;
	if (gs.cursor_icon == CURSOR_TEXT) {
		draw_pos_add = vec2_new(-1, -5);
	}

//	draw_texture(
//		gs,
//		texture_get_sz(gs, TEXTURE_ICONS),
//		vec2_new(10 * gs.cursor_icon, 0),
//		vec2_new(10, 10),
//		vec2_add(vec2_add(vec2_floor(mouse_pos), draw_pos_add), vec2_new(1,1)),
//		vec2_new(10, 10),
//		color_new(0, 0, 0, 1),
//		false
//	);
	draw_texture(
		gs,
		texture_get_sz(gs, TEXTURE_ICONS),
		vec2_new(10 * gs.cursor_icon, 0),
		vec2_new(10, 10),
		vec2_add(vec2_floor(mouse_pos), draw_pos_add),
		vec2_new(10, 10),
		color_new(0.5, 0.5, 0.5, 1),
		false
	);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_ICONS));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

bool menu_opening(const States &states) {
	return states.file_picker_opening;
}

}

void update(
States &states,
GraphicStuff &gs,
TabBar &tab_bar,
FilePicker &file_picker,
const GameTime &game_time,
const Input &input
) {
	Tab &tab = tab_bar.tab_list[tab_bar_get_tab_index(tab_bar)];

	if (!states.file_picker_opening && map_press(input, MAP_OPEN_FILE)) {
		states.file_picker_opening = true;
		
		file_picker.is_save_picker = false;
		
		tab.layer_name_editing = false;
	}
	if (!states.file_picker_opening && map_press(input, MAP_SAVE_FILE)) {
		states.file_picker_opening = true;
		
		file_picker.save_name_textarea.text
			= file_picker.save_name_textarea.defl_text;
		file_picker.is_save_picker = true;

		tab.layer_name_editing = false;
	}
	if (states.file_picker_opening
	&& (file_picker.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.file_picker_opening = false;
	}

	tab_bar_update(tab_bar, gs, input, !menu_opening(states));

	tab_update(tab, gs, input, game_time, TAB_OFFSET, !menu_opening(states));

	file_picker_update(file_picker, gs, input, game_time,
		FILE_PICKER_OFFSET, states.file_picker_opening);
}

void draw(
const States &states,
GraphicStuff &gs,
const TabBar &tab_bar,
const FilePicker &file_picker,
const GameTime &game_time,
const Input &input
) {
	bool mouse_in_window = in_rect(
		input.mouse_pos, vec2_new(0, 0), to_vec2(gs.current_window_sz)
	);
	if ((input.mouse_event && mouse_in_window) || input.left_down
	|| input.left_release || input.key_event || game_time.frame_passed == 0
	|| gs.just_resized || gs.redraw_requested) {
		const Tab &tab = tab_bar.tab_list[tab_bar_get_tab_index(tab_bar)];

		draw_canvas_bkg(gs, tab);
		draw_canvas(gs, tab, input);

		draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
			BLUR_COLOR, false);
		draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
			BLUR_COLOR, true);
	
		draw_blurred_rects(gs, tab);
		draw_ui(gs, tab_bar, tab, input, game_time);

		if (gs.draw_secondlayer_ui) {
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
				BLUR_COLOR, false);
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
				BLUR_COLOR, true);
		
			draw_blurred_rects_1(gs, tab);
			draw_ui_1(states, gs, tab_bar, file_picker, tab, input, game_time);
		}

		draw_cursor(gs, input);
	}

	draw_fb_main(gs);
}
