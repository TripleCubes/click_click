#include "mainloop.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <vector>

#include "game_time.h"
#include "input.h"
#include "input_map.h"
#include "tab/tab.h"
#include "ui/app_ui.h"

#include "ui/file_picker/file_picker.h"
#include "ui/file_picker/file_picker_handling.h"
#include "ui/new_tab_menu/new_tab_menu.h"
#include "ui/new_tab_menu/new_tab_menu_handling.h"
#include "ui/resize_menu/resize_menu.h"
#include "ui/resize_menu/resize_menu_handling.h"
#include "ui/top_left_menu/top_left_menu.h"
#include "ui/top_left_menu/top_left_menu_handling.h"
#include "ui/app_menu/app_menu.h"
#include "ui/app_menu/app_menu_handling.h"
#include "ui/dialog_box/dialog_box.h"
#include "ui/dialog_box/dialog_box_handling.h"

#include "ui/tab/color_picker.h"

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
#include "settings.h"

#include "file/save_project.h"
#include "file/open_project.h"

// TEST
#include <iostream>

namespace {

const Color BLUR_COLOR = color_new(1, 1, 1, 1);
const Vec2 TAB_OFFSET = vec2_new(0, 0);
const Vec2 FILE_PICKER_OFFSET = vec2_new(0, 0);
const Vec2 NEW_TAB_MENU_OFFSET = vec2_new(0, 0);
const Vec2 RESIZE_MENU_OFFSET = vec2_new(0, 0);
const Vec2 TOP_LEFT_MENU_OFFSET = vec2_new(0, 0);
const Vec2 APP_MENU_OFFSET = vec2_new(0, 0);
const Vec2 DIALOG_BOX_OFFSET = vec2_new(0, 0);

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
		vec2_new(SIDE_BAR_W + 3, 3),
		vec2_new(main_fb_sz.x - SIDE_BAR_W - 3 * 2, TOP_BAR_H - 4)
	);
	tab_blur_rects_draw(tab, gs, TAB_OFFSET);

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_blurred_rects_1(GraphicStuff &gs, const AppUI &app_ui,
const States &states) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	
	if (states.file_picker_opening) {
		file_picker_bkg_draw(gs, FILE_PICKER_OFFSET);
	}
	if (states.new_tab_menu_opening) {
		new_tab_menu_bkg_draw(gs, NEW_TAB_MENU_OFFSET);
	}
	if (states.resize_menu_opening) {
		resize_menu_bkg_draw(gs, RESIZE_MENU_OFFSET);
	}
	if (states.app_menu_opening) {
		app_menu_bkg_draw(gs, APP_MENU_OFFSET);
	}
	if (states.dialog_box_opening) {
		dialog_box_bkg_draw(app_ui.dialog_box, gs, DIALOG_BOX_OFFSET);
	}

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_blurred_rects_2(GraphicStuff &gs, const AppUI &app_ui,
const States &states) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);
	
	if (states.dialog_box_opening) {
		dialog_box_bkg_draw(app_ui.dialog_box, gs, DIALOG_BOX_OFFSET);
	}

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		fb_get_texture_id(gs, FB_BLUR_1));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui(
GraphicStuff &gs,
const AppUI &app_ui,
const Tab &tab,
const Input &input,
const GameTime &game_time
) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	tab_bar_draw(app_ui.tab_bar, gs);
	tab_ui_draw(tab, gs, input, game_time, TAB_OFFSET);

	top_left_menu_draw(app_ui.top_left_menu, gs, input, game_time,
		TOP_LEFT_MENU_OFFSET);

	draw_text(
		gs,
		std::to_string((int)(1/game_time.delta)),
		vec2_new(main_fb_sz.x - 26, TOP_BAR_H + 5),
		20,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		true
	);

	draw_text(
		gs,
		std::to_string((int)(1/game_time.frame_time)),
		vec2_new(main_fb_sz.x - 26, TOP_BAR_H + 17),
		20,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		true
	);

	float x_add = 0;
	if (tab.color_pallete.selected_index == 0
	&& tab.color_pallete.at_page != 0) {
		x_add = -76;
	}
	draw_text(
		gs,
		std::to_string(tab.sz.x)
			+ "x" + std::to_string(tab.sz.y),
		vec2_new(SIDE_BAR_W + 80 + x_add, main_fb_sz.y - 10),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);
	Vec2 pos = vec2_add(TAB_OFFSET, tab.pos);
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
	Vec2i mouse_pos_i = to_vec2i(tex_draw_mouse_pos);
	draw_text(
		gs,
		std::to_string(mouse_pos_i.x)
			+ " " + std::to_string(mouse_pos_i.y),
		vec2_new(SIDE_BAR_W + 120 + x_add, main_fb_sz.y - 10),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);

	if (tab.selection.rect_point_1.x != -1) {
		std::string selection_wh_str;
		Vec2i selection_wh
			= vec2i_sub(tab.selection.rect_point_2,tab.selection.rect_point_1);
		selection_wh_str += std::to_string(std::abs(selection_wh.x) + 1);
		selection_wh_str += "x";
		selection_wh_str += std::to_string(std::abs(selection_wh.y) + 1);
		draw_text(
			gs,
			selection_wh_str,
			vec2_new(SIDE_BAR_W + 160 + x_add, main_fb_sz.y - 10),
			100,
			1,
			BTN_TEXTAREA_COLOR,
			vec2_new(4, 3),
			false
		);
	}

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui_1(
const States &states,
const Settings &settings,
GraphicStuff &gs,
const AppUI &app_ui,
const Tab &tab,
const Input &input,
const GameTime &game_time
) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	if (states.file_picker_opening) {
		file_picker_ui_draw(app_ui.file_picker, gs, input, game_time,
			FILE_PICKER_OFFSET);
	}
	if (states.new_tab_menu_opening) {
		new_tab_menu_ui_draw(app_ui.new_tab_menu, gs, input, game_time,
			NEW_TAB_MENU_OFFSET);
	}
	if (states.resize_menu_opening) {
		resize_menu_ui_draw(app_ui.resize_menu, gs, input, game_time,
			RESIZE_MENU_OFFSET);
	}
	if (states.app_menu_opening) {
		app_menu_ui_draw(app_ui.app_menu, settings, gs, input, game_time,
			APP_MENU_OFFSET);
	}

	use_shader(gs, SHADER_BASIC_DRAW);
	set_uniform_texture(gs, SHADER_BASIC_DRAW, "u_texture", 0,
		texture_get_id(gs, TEXTURE_FONT));
	mesh_set(gs, MESH_BASIC_DRAW);
	mesh_draw(gs, MESH_BASIC_DRAW);
}

void draw_ui_2(
const States &states,
const Settings &settings,
GraphicStuff &gs,
const AppUI &app_ui,
const Tab &tab,
const Input &input,
const GameTime &game_time
) {
	mesh_clear(gs, MESH_BASIC_DRAW);
	bind_framebuffer(gs, FB_MAIN);

	if (states.dialog_box_opening) {
		dialog_box_draw(app_ui.dialog_box, gs, input, game_time,
			DIALOG_BOX_OFFSET);
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

}

void update(
States &states,
Settings &settings,
GraphicStuff &gs,
const GameTime &game_time,
const Input &input,
AppUI &app_ui
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	Tab &tab = app_ui.tab_bar.tab_list[tab_bar_get_tab_index(app_ui.tab_bar)];

	tab_bar_update(app_ui.tab_bar, gs, input, !menu_opening(states));

	tab_update(tab, gs, states, input, game_time, settings, TAB_OFFSET,
		!menu_opening(states)
		#ifndef __EMSCRIPTEN__	
		,glfw_window
		#endif
	);


	dialog_box_update(app_ui.dialog_box, gs, input, game_time,
		DIALOG_BOX_OFFSET, states.dialog_box_opening);
	dialog_box_handling(app_ui, states, tab);


	bool dialog_opening = states.dialog_box_opening;

	#ifndef __EMSCRIPTEN__
	app_menu_update(app_ui.app_menu, settings, gs, input, game_time,
		APP_MENU_OFFSET, states.app_menu_opening && !dialog_opening,
		glfw_window);
	#else
	app_menu_update(app_ui.app_menu, settings, gs, input, game_time,
		APP_MENU_OFFSET, states.app_menu_opening && !dialog_opening);
	#endif
	app_menu_handling(states, app_ui, tab, gs, input);
	

	file_picker_update(app_ui.file_picker, gs, input, game_time,
		FILE_PICKER_OFFSET, states.file_picker_opening && !dialog_opening);
	file_picker_handling(states, app_ui.file_picker, app_ui.tab_bar,
		tab, app_ui.dialog_box, gs, input);

	new_tab_menu_update(app_ui.new_tab_menu, gs, input, game_time,
		NEW_TAB_MENU_OFFSET, states.new_tab_menu_opening && !dialog_opening);
	new_tab_menu_handling(states, app_ui.new_tab_menu, app_ui.tab_bar,
		gs, input);

	resize_menu_update(app_ui.resize_menu, gs, input, game_time,
		RESIZE_MENU_OFFSET, states.resize_menu_opening && !dialog_opening);
	resize_menu_handling(states, app_ui.resize_menu, tab, gs, input,
		game_time, settings);

	top_left_menu_update(app_ui.top_left_menu, gs, input, game_time,
		TOP_LEFT_MENU_OFFSET, !menu_opening(states));
	top_left_menu_handling(states, app_ui.top_left_menu, input);
}

void draw(
const States &states,
const Settings &settings,
GraphicStuff &gs,
const GameTime &game_time,
const Input &input,
const AppUI &app_ui
) {
	bool mouse_in_window = in_rect(
		input.mouse_pos, vec2_new(0, 0), to_vec2(gs.current_window_sz)
	);
	if ((input.mouse_event && mouse_in_window) || input.left_down
	|| input.left_release || input.key_event || game_time.frame_passed == 0
	|| gs.just_resized || gs.redraw_requested) {
		const Tab &tab
			= app_ui.tab_bar.tab_list[tab_bar_get_tab_index(app_ui.tab_bar)];

		draw_canvas_bkg(gs, tab);
		draw_canvas(gs, tab, input);

		draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
			BLUR_COLOR, false);
		draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
			BLUR_COLOR, true);
	
		draw_blurred_rects(gs, tab);
		draw_ui(gs, app_ui, tab, input, game_time);

		if (gs.draw_secondlayer_ui || states.dialog_box_opening) {
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
				BLUR_COLOR, false);
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
				BLUR_COLOR, true);
		
			draw_blurred_rects_1(gs, app_ui, states);
			draw_ui_1(states, settings, gs, app_ui, tab, input, game_time);

			// TEST
			// std::cout << "drawing secondlayer ui" << std::endl;
		}

		if (gs.draw_thirdlayer_ui) {
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
				BLUR_COLOR, false);
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
				BLUR_COLOR, true);
		
			draw_blurred_rects_2(gs, app_ui, states);
			draw_ui_2(states, settings, gs, app_ui, tab, input, game_time);

			// TEST
			// std::cout << "drawing thirdlayer ui" << std::endl;
		}

		if (!settings.use_hardware_cursor) {
			draw_cursor(gs, input);
		}
	}

	draw_fb_main(gs);
}
