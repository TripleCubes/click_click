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
#include "ui/new_tab_menu/new_tab_menu.h"
#include "ui/top_left_menu/top_left_menu.h"
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

void draw_blurred_rects_1(GraphicStuff &gs, const States &states) {
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
//	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

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
	return states.file_picker_opening || states.new_tab_menu_opening
		|| states.resize_menu_opening || states.app_menu_opening;
}

void _tab_new(TabBar &tab_bar, GraphicStuff &gs, const OpenProjectData &data) {
	tab_bar_tab_new(tab_bar, gs, tab_bar.order_index + 1,
		data.pos, data.sz, data.px_scale);
	tab_bar.order_index++;

	int index = tab_bar.tab_order_list[tab_bar.order_index];
	Tab &tab = tab_bar.tab_list[index];

	tab.pallete_data.clear();
	tab.layer_list.clear();
	tab.layer_order_list.clear();

	for (int i = 0; i < 64; i++) {
		tab.color_pallete.color_list[i].r
			= (float)data.pallete_data[i*4    ] / 255;
		tab.color_pallete.color_list[i].g
			= (float)data.pallete_data[i*4 + 1] / 255;
		tab.color_pallete.color_list[i].b
			= (float)data.pallete_data[i*4 + 2] / 255;
		tab.color_pallete.color_list[i].a
			= (float)data.pallete_data[i*4 + 3] / 255;
	}
	tab.pallete_data.insert(
		tab.pallete_data.end(),
		data.pallete_data.begin(),
		data.pallete_data.end()
	);
	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);

	for (int i = 0; i < (int)data.layer_list.size(); i++) {
		const OpenProjectLayerData &layer_data = data.layer_list[i];

		tab_layer_new_data(
			tab,
			tab.layer_order_list.size(),
			layer_data.layer_name,
			gs,
			layer_data.data
		);
	}

	int pallete_index = tab.color_pallete.selected_index;
	Color color = tab.color_pallete.color_list[pallete_index];
	color_picker_set_rgb(tab.color_picker, color);
}

void file_picker_handling(States &states, FilePicker &file_picker,
TabBar &tab_bar, Tab &tab, GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states) && map_press(input, MAP_OPEN_FILE)) {
		states.file_picker_opening = true;
		file_picker.is_save_picker = false;
		tab.layer_name_editing = false;
		
		#ifdef __EMSCRIPTEN__
			file_picker_web_file_btn_list_update(file_picker);
		#endif
	}

	if (!menu_opening(states) && map_press(input, MAP_SAVE_FILE)) {
		states.file_picker_opening = true;
		
		file_picker.save_name_textarea.text
			= file_picker.save_name_textarea.defl_text;
		file_picker.is_save_picker = true;

		tab.layer_name_editing = false;

		#ifdef __EMSCRIPTEN__
			file_picker_web_file_btn_list_update(file_picker);
		#endif
	}

	if (states.file_picker_opening
	&& (file_picker.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.file_picker_opening = false;
	}

	if (states.file_picker_opening && file_picker.is_save_picker
	&& (file_picker.save_btn.clicked || map_press(input, MAP_ENTER))) {
		states.file_picker_opening = false;

		std::string save_link;
		file_picker_get_save_link(save_link, file_picker);
		
		save_project(save_link, tab);

		#ifdef __EMSCRIPTEN__
		EM_ASM(
			FS.syncfs(function(err) {
				if (err) {
					console.log(err);
				}
				else {
					console.log('file saved');
				}
			});
		);
		#endif
	}

	if (states.file_picker_opening) {
		std::string open_path;
		file_picker_open_file(open_path, file_picker);

		if (open_path.length() != 0) {
			states.file_picker_opening = false;

			OpenProjectData data;
			if (file_to_project_data(data, open_path)) {
				_tab_new(tab_bar, gs, data);
			}
			else {
				std::cout << "error opening file " << open_path << std::endl;
			}
		}
	}
}

void new_tab_menu_handling(States &states, NewTabMenu &new_tab_menu,
TabBar &tab_bar, GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states)
	&& (map_press(input, MAP_NEW_PROJECT)
		|| map_press(input, MAP_NEW_PROJECT_1))) {
		states.new_tab_menu_opening = true;
		new_tab_menu.ta_active = NEW_TAB_MENU_TA_ACTIVE_W;
	}
	
	if (states.new_tab_menu_opening
	&& (new_tab_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.new_tab_menu_opening = false;
	}

	if (states.new_tab_menu_opening
	&& (new_tab_menu.new_tab_btn.clicked || map_press(input, MAP_ENTER))
	&& new_tab_menu_all_field_valid(new_tab_menu)) {
		states.new_tab_menu_opening = false;

		Vec2i canvas_sz = new_tab_menu_get_canvas_sz(new_tab_menu);

		tab_bar_tab_new(tab_bar, gs, tab_bar.order_index + 1,
			vec2_new(0, 0), canvas_sz, 2);
		tab_bar.order_index++;

		int index = tab_bar.tab_order_list[tab_bar.order_index];
		tab_center_canvas(tab_bar.tab_list[index], gs);
	}
}

void resize_menu_handling(States &states, ResizeMenu &resize_menu,
Tab &tab, GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states) && map_press(input, MAP_RESIZE_CANVAS)) {
		states.resize_menu_opening = true;
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_W;
		resize_menu.w_ta.text = std::to_string(tab.sz.x);
		resize_menu.h_ta.text = std::to_string(tab.sz.y);
	}
	
	if (states.resize_menu_opening
	&& (resize_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.resize_menu_opening = false;
	}

	if (states.resize_menu_opening
	&& (resize_menu.resize_btn.clicked || map_press(input, MAP_ENTER))
	&& resize_menu_all_field_valid(resize_menu)) {
		states.resize_menu_opening = false;

		Vec2i prev_sz = tab.sz;
		Vec2i new_sz = resize_menu_get_canvas_sz(resize_menu);
		Vec2i new_pos = vec2i_new(0, 0);

		switch (resize_menu.align_btn_selected) {
		case 0: 
			new_pos.x = 0;
			new_pos.y = 0;
			break;
		case 1:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = 0;
			break;
		case 2:
			new_pos.x = prev_sz.x - new_sz.x;
			new_pos.y = 0;
			break;
		case 3:
			new_pos.x = 0;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 4:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 5:
			new_pos.x = prev_sz.x - new_sz.x;
			new_pos.y = prev_sz.y / 2 - new_sz.y / 2;
			break;
		case 6:
			new_pos.x = 0;
			new_pos.y = prev_sz.y - new_sz.y;
			break;
		case 7:
			new_pos.x = prev_sz.x / 2 - new_sz.x / 2;
			new_pos.y = prev_sz.y - new_sz.y;
			break;
		case 8:
			new_pos.y = prev_sz.y - new_sz.y;
			new_pos.x = prev_sz.x - new_sz.x;
			break;
		}

		selection_clear(tab.selection, tab.sz);
		tab_resize(tab, gs, new_pos, new_sz);
	}
}

void top_left_menu_handling(States &states, const TopLeftMenu &top_left_menu,
const Input &input) {
	if (!menu_opening(states) && top_left_menu.hamburger_btn.clicked) {
		states.app_menu_opening = true;
	}
}

void app_menu_handling(States &states, AppUI &app_ui, Tab &tab,
GraphicStuff &gs, const Input &input) {
	AppMenu &app_menu = app_ui.app_menu;
	FilePicker &file_picker = app_ui.file_picker;
	NewTabMenu &new_tab_menu = app_ui.new_tab_menu;
	ResizeMenu &resize_menu = app_ui.resize_menu;

	if (!menu_opening(states) && map_press(input, MAP_APP_MENU)) {
		states.app_menu_opening = true;
	}
	
	if (states.app_menu_opening
	&& (app_menu.close_btn.clicked || map_press(input, MAP_ESC))) {
		states.app_menu_opening = false;
	}

	if (app_menu.new_tab_btn.clicked) {
		states.app_menu_opening = false;
		states.new_tab_menu_opening = true;

		new_tab_menu.ta_active = NEW_TAB_MENU_TA_ACTIVE_W;
	}

	if (app_menu.open_file_btn.clicked) {
		states.app_menu_opening = false;

		states.file_picker_opening = true;
		file_picker.is_save_picker = false;
		tab.layer_name_editing = false;
	}

	if (app_menu.save_btn.clicked) {
		states.app_menu_opening = false;

		states.file_picker_opening = true;
		
		file_picker.save_name_textarea.text
			= file_picker.save_name_textarea.defl_text;
		file_picker.is_save_picker = true;

		tab.layer_name_editing = false;
	}

	if (app_menu.save_as_btn.clicked) {
		states.app_menu_opening = false;
	}

	if (app_menu.resize_btn.clicked) {
		states.app_menu_opening = false;

		states.resize_menu_opening = true;
		resize_menu.ta_active = RESIZE_MENU_TA_ACTIVE_W;
		resize_menu.w_ta.text = std::to_string(tab.sz.x);
		resize_menu.h_ta.text = std::to_string(tab.sz.y);
	}
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
		!menu_opening(states));


	file_picker_update(app_ui.file_picker, gs, input, game_time,
		FILE_PICKER_OFFSET, states.file_picker_opening);
	file_picker_handling(states, app_ui.file_picker, app_ui.tab_bar,
		tab, gs, input);

	new_tab_menu_update(app_ui.new_tab_menu, gs, input, game_time,
		NEW_TAB_MENU_OFFSET, states.new_tab_menu_opening);
	new_tab_menu_handling(states, app_ui.new_tab_menu, app_ui.tab_bar,
		gs, input);

	resize_menu_update(app_ui.resize_menu, gs, input, game_time,
		RESIZE_MENU_OFFSET, states.resize_menu_opening);
	resize_menu_handling(states, app_ui.resize_menu, tab, gs, input);

	top_left_menu_update(app_ui.top_left_menu, gs, input, game_time,
		TOP_LEFT_MENU_OFFSET, !menu_opening(states));
	top_left_menu_handling(states, app_ui.top_left_menu, input);

	#ifndef __EMSCRIPTEN__
	app_menu_update(app_ui.app_menu, settings, gs, input, game_time,
		APP_MENU_OFFSET, states.app_menu_opening, glfw_window);
	#else
	app_menu_update(app_ui.app_menu, settings, gs, input, game_time,
		APP_MENU_OFFSET, states.app_menu_opening);
	#endif
	app_menu_handling(states, app_ui, tab, gs, input);
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

		if (gs.draw_secondlayer_ui) {
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_MAIN),
				BLUR_COLOR, false);
			draw_blurred_texture(gs, fb_get_texture_id(gs, FB_BLUR_1),
				BLUR_COLOR, true);
		
			draw_blurred_rects_1(gs, states);
			draw_ui_1(states, settings, gs, app_ui, tab, input, game_time);

			// TEST
			//std::cout << "drawing secondlayer ui" << std::endl;
		}

		if (!settings.use_hardware_cursor) {
			draw_cursor(gs, input);
		}
	}

	draw_fb_main(gs);
}
 
