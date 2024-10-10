#include "tab.h"

#include "../types/color.h"

#include "../graphic_types/texture.h"
#include "../graphic_types/framebuffer.h"
#include "../graphic_types/shader.h"
#include "../graphic_types/mesh.h"
#include "../graphic/graphic.h"
#include "../graphic/draw_rect.h"
#include "../graphic/draw_texture.h"
#include "../graphic/draw_text.h"

#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../input_map.h"
#include "../game_time.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../consts.h"
#include "../settings.h"

#include "../draw_tool/draw_tool_px.h"
#include "../draw_tool/draw_tool_line.h"

#include "brush.h"
#include "curve.h"
#include "fill.h"
#include "px_perfect_brush.h"

#include <string>
#include <cmath>

// TEST
#include <iostream>

namespace {

const int PALLETE_DRAW = 0;
const int PALLETE_TOOL_PREVIEW = 1;

const float LAYER_TEXTAREA_LIST_LINE_HEIGHT = 12;

const int ZOOM_MAX = 15;

int get_blank_index(const std::vector<Tab> &list) {
	for (int i = 0; i < (int)list.size(); i++) {
		if (!list[i].running) {
			return i;
		}
	}

	return -1;
}

void pallete_data_color(Tab &tab, int pallete_index, Color color) {
	int index = pallete_index * 4;
	tab.pallete_data[index    ] = color.r * 255;
	tab.pallete_data[index + 1] = color.g * 255;
	tab.pallete_data[index + 2] = color.b * 255;
	tab.pallete_data[index + 3] = color.a * 255;
}

void tool_preview_pallete_data_color(Tab &tab, int pallete_index, Color color){
	int index = pallete_index * 4;
	tab.tool_preview_pallete_data[index    ] = color.r * 255;
	tab.tool_preview_pallete_data[index + 1] = color.g * 255;
	tab.tool_preview_pallete_data[index + 2] = color.b * 255;
	tab.tool_preview_pallete_data[index + 3] = color.a * 255;
}

void layer_texture_draw(const Tab &tab, const GraphicStuff &gs,
int texture_index, int pallete, Vec2 pos) {
	Vec2 main_fb_sz_f = to_vec2(fb_get_sz(gs, FB_MAIN));
	Vec2 sz_f = to_vec2(tab.sz);
	Vec2 pos_normalized = vec2_new(
		pos.x / main_fb_sz_f.x,
		pos.y / main_fb_sz_f.y
	);
	pos_normalized = vec2_mul(pos_normalized, 2);
	pos_normalized = vec2_add(pos_normalized, vec2_new(-1, -1));
	Vec2 sz_normalized = vec2_new(
		sz_f.x / main_fb_sz_f.x * 2 * tab.px_scale,
		sz_f.y / main_fb_sz_f.y * 2 * tab.px_scale
	);
	pos_normalized.y = - pos_normalized.y - sz_normalized.y;

	use_shader(gs, SHADER_LAYER_DRAW);
	set_uniform_vec2(gs, SHADER_LAYER_DRAW, "u_pos", pos_normalized);
	set_uniform_vec2(gs, SHADER_LAYER_DRAW, "u_sz", sz_normalized);
	set_uniform_texture(gs, SHADER_LAYER_DRAW,
		"u_draw_texture", 0, texture_get_id(gs, texture_index));
	if (pallete == PALLETE_DRAW) {
		set_uniform_texture(gs, SHADER_LAYER_DRAW,
			"u_pallete_texture", 1,
			texture_get_id(gs, tab.pallete_texture_index));
	}
	else if (pallete == PALLETE_TOOL_PREVIEW) {
		set_uniform_texture(gs, SHADER_LAYER_DRAW,
			"u_pallete_texture", 1,
			texture_get_id(gs, tab.tool_preview_pallete_texture_index));
	}

	mesh_draw(gs, MESH_RECT);
}

void layer_list_draw(const Tab &tab, GraphicStuff &gs, Vec2 pos) {
	for (int i = (int)tab.layer_order_list.size() - 1; i >= 0; i--) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
		layer_texture_draw(tab, gs, layer.texture_index, PALLETE_DRAW,
			vec2_floor(pos));
	}

	layer_texture_draw(tab, gs, tab.tool_preview_texture_index,
		PALLETE_TOOL_PREVIEW, vec2_floor(pos));

	layer_texture_draw(tab, gs, tab.selection_preview_texture_index,
		PALLETE_TOOL_PREVIEW, vec2_floor(pos));
}

int get_layer_index(const Tab &tab) {
	return tab.layer_order_list[tab.layer_order_list_index];
}

bool cursor_on_ui(const Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);

	auto check = [parent_pos](Vec2 pos, Vec2 rect_pos, Vec2 rect_sz) -> bool {
		return in_rect(pos, vec2_add(parent_pos, rect_pos), rect_sz);
	};

	bool on_ui = false;
	
	on_ui = in_rect(
		mouse_pos,
		vec2_new(0, 0),
		vec2_new(SIDE_BAR_W, main_fb_sz.y)
	);
	if (on_ui) { return true; }

	on_ui = in_rect(
		mouse_pos,
		vec2_new(SIDE_BAR_W + 4, 4),
		vec2_new(main_fb_sz.x - SIDE_BAR_W - 4 * 2, TOP_BAR_H - 4)
	);
	if (on_ui) { return true; }

	on_ui = check(
		mouse_pos,
		vec2_sub(tab.color_pallete.pos, vec2_new(24 + 3, 3)),
		vec2_add(COLOR_PALLETE_SZ, vec2_new(24 + 6, 6))
	);
	if (on_ui) { return true; }

	Vec2 color_picker_pos
		= vec2_add(tab.color_picker.pos, vec2_new(0, main_fb_sz.y));
	on_ui = check(
		mouse_pos,
		vec2_sub(color_picker_pos, vec2_new(4, 4)),
		vec2_add(color_picker_get_sz(tab.color_picker), vec2_new(8, 8))
	);
	if (on_ui) { return true; }

	on_ui = check(
		mouse_pos,
		vec2_sub(tab.tool_picker.pos, vec2_new(3, 3)),
		vec2_add(TOOL_PICKER_SZ, vec2_new(5, 6))
	);
	if (on_ui) { return true; }

	on_ui = check(
		mouse_pos,
		vec2_sub(tab.btn_panel.pos, vec2_new(1, 1)),
		vec2_add(BTN_PANEL_SZ, vec2_new(2, 2))
	);
	if (on_ui) { return true; }

	return false;
}

void px_tool_preview_update(Tab &tab, int sz, GraphicStuff &gs,
const Input &input, bool use_selection, Vec2 parent_pos) {
	auto px = [&tab, sz, use_selection]
	(Vec2i pos, unsigned char pallete_index) {
		if (use_selection) {
			draw_tool_px(
				tab.tool_preview_data,
				tab.selection,
				tab.sz,
				pos,
				pallete_index,
				sz
			);
		}
		else {
			draw_tool_px_no_selection(
				tab.tool_preview_data,
				tab.selection,
				tab.sz,
				pos,
				pallete_index,
				sz
			);
		}
	};

	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.mouse_move || input.key_event) {
		px(to_vec2i(tex_draw_mouse_pos), 1);
		texture_data_red(gs, tab.tool_preview_texture_index,
			tab.sz, tab.tool_preview_data);
		px(to_vec2i(tex_draw_mouse_pos), 0);
	}
}

void tool_update(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, const Settings &settings, Vec2 parent_pos) {
	if (tab.panning || tab.after_panning_1_frame) { return; }

	bool b_cursor_on_ui
		= cursor_on_ui(tab, gs, input, parent_pos);

	if (b_cursor_on_ui && input.left_click) {
		tab.clicked_and_hold_on_ui = true;
	}

	if (!tab.clicked_and_hold_on_ui && tab.layer_order_list.size() != 0) {
		if (tab.tool_picker.selected_index == TOOL_BRUSH) {
			if (settings.use_px_perfect_brush
			&& tab.tool_picker.brush_selected_index == 0) {
				px_perfect_brush_tool_preview_update(tab, gs, input,parent_pos);
				px_perfect_brush_tool_update(tab, get_layer_index(tab), gs,
					input, parent_pos);
			}
			else {
				px_tool_preview_update(tab,
					tab.tool_picker.brush_selected_index,
					gs, input, true, parent_pos);
				brush_tool_update(tab, get_layer_index(tab),
					tab.tool_picker.brush_selected_index, gs, input,
					parent_pos);
			}
		}

		else if (tab.tool_picker.selected_index == TOOL_CURVE) {
			if (!input.left_down && !input.left_release) {
				px_tool_preview_update(tab,
					tab.tool_picker.curve_selected_index,
					gs, input, true, parent_pos);
			}
			curve_tool_preview_update(tab,tab.tool_picker.curve_selected_index,
				gs, input, parent_pos);
			curve_tool_update(tab, get_layer_index(tab),
				tab.tool_picker.curve_selected_index, gs, input, parent_pos);
		}

		else if (tab.tool_picker.selected_index == TOOL_FILL) {
			if (!b_cursor_on_ui) {
				gs.cursor_icon = CURSOR_FILL;
			}
			px_tool_preview_update(tab, 0, gs, input, true, parent_pos);
			fill_tool_update(tab, get_layer_index(tab), gs, input, parent_pos);
		}

		else if (tab.tool_picker.selected_index == TOOL_SELECT) {
			if (!b_cursor_on_ui
			&& tab.tool_picker.select_selected_index == 2) {
				gs.cursor_icon = CURSOR_MAGIC_WAND;
			}
			px_tool_preview_update(tab, 0, gs, input, false, parent_pos);
			select_tool_update(tab, get_layer_index(tab), gs, input,
				game_time, tab.tool_picker.select_second_selected_index == 1,
				tab.tool_picker.select_selected_index, parent_pos);
		}
	}

	if (map_press(input, MAP_SELECT_ALL)) {
		selection_all(tab.selection, tab.sz);
	}

	if (map_press(input, MAP_DESELECT_ALL) || map_press(input, MAP_ESC)) {
		selection_clear(tab.selection, tab.sz);
	}

	if (input.left_release) {
		tab.clicked_and_hold_on_ui = false;
	}

	if (tab.selection.draw_preview_list.size() != 0
	|| tab.selection.full_preview_list.size() != 0) {
		gs.redraw_requested = true;
	}
}

void color_picker_color_pallete_data_update(Tab &tab, GraphicStuff &gs) {
	int pallete_index = tab.color_pallete.selected_index;

	if (tab.color_picker.color_changed) {
		Color color = color_picker_get_rgb(tab.color_picker);
		tab.color_pallete.color_list[pallete_index] = color;

		pallete_data_color(tab, pallete_index, color);
		texture_data(gs, tab.pallete_texture_index,
			vec2i_new(16, 16), tab.pallete_data);
	}

	if (tab.color_pallete.selection_changed) {
		Color color = tab.color_pallete.color_list[pallete_index];
		color_picker_set_rgb(tab.color_picker, color);
	}
}

void layer_textarea_list_update(Tab &tab, GraphicStuff &gs,
const GameTime &game_time, const Input &input, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 bottom_pos = vec2_add(parent_pos, vec2_new(0, main_fb_sz.y));

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(tab.layer_bar.pos.x,
			tab.layer_bar.pos.y + LAYER_TEXTAREA_LIST_LINE_HEIGHT * i);
		layer_textarea_update(
			layer,
			gs,
			game_time,
			input,
			vec2_add(bottom_pos, add),
			i == tab.layer_order_list_index && tab.layer_name_editing,
			show
		);
	}

	if (input.left_click || input.key_list[KEY_ENTER].press
	|| map_press(input, MAP_ESC)) {
		tab.layer_name_editing = false;
	}

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
	
		if (layer.textarea.clicked) {
			if (tab.layer_order_list_index == i) {
				tab.layer_name_editing = true;
				gs.cursor_icon = CURSOR_TEXT;
				return;
			}

			tab.layer_order_list_index = i;
			return;
		}

		if (layer.delete_btn.clicked && tab.layer_order_list.size() > 1) {
			layer_release(tab.layer_list, gs, index);
			tab.layer_order_list.erase(tab.layer_order_list.begin() + i);

			if (i < tab.layer_order_list_index) {
				tab.layer_order_list_index--;
			}

			tab.layer_order_list_index = clampi(
				tab.layer_order_list_index,
				0,
				(int)tab.layer_order_list.size() - 1
			);

			return;
		}

		if (layer.textarea.hovered && tab.layer_order_list_index == i) {
			gs.cursor_icon = CURSOR_TEXT;
			return;
		}
	}
}

void layer_textarea_list_draw(const Tab &tab, GraphicStuff &gs,
const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 bottom_pos = vec2_add(parent_pos, vec2_new(0, main_fb_sz.y));

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];

		Vec2 add = vec2_new(tab.layer_bar.pos.x,
			tab.layer_bar.pos.y + LAYER_TEXTAREA_LIST_LINE_HEIGHT * i);
		layer_textarea_draw(layer, gs, game_time,
			vec2_add(bottom_pos, add),
			i == tab.layer_order_list_index && tab.layer_name_editing,
			i == tab.layer_order_list_index);
	}
}

void layer_bar_event_handle(Tab &tab) {
	if (tab.layer_order_list_index < (int)tab.layer_order_list.size() - 1
	&& tab.layer_bar.down_btn.clicked) {
		int swap = tab.layer_order_list[tab.layer_order_list_index + 1];
		tab.layer_order_list[tab.layer_order_list_index + 1]
			= tab.layer_order_list[tab.layer_order_list_index];
		tab.layer_order_list[tab.layer_order_list_index] = swap;

		tab.layer_order_list_index++;
	}
	else if (tab.layer_order_list_index > 0
	&& tab.layer_bar.up_btn.clicked) {
		int swap = tab.layer_order_list[tab.layer_order_list_index - 1];
		tab.layer_order_list[tab.layer_order_list_index - 1]
			= tab.layer_order_list[tab.layer_order_list_index];
		tab.layer_order_list[tab.layer_order_list_index] = swap;

		tab.layer_order_list_index--;
	}
}

void canvas_move_update(Tab &tab, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	if (map_down(input, MAP_PAN) && input.left_down) {
		tab.panning = true;
		tab.after_panning_1_frame = true;
	}

	if (tab.panning == false) {
		tab.after_panning_1_frame = false;
	}

	if (input.left_release) {
		tab.panning = false;
	}

	if (tab.panning && input.left_down && input.mouse_move){
		Vec2 pos = vec2_add(parent_pos, tab.pos);

		Vec2 main_fb_mouse_pos
			= get_main_fb_mouse_pos(gs, input.mouse_pos);
		Vec2 prev_main_fb_mouse_pos
			= get_main_fb_mouse_pos(gs, input.prev_mouse_pos);
		Vec2 tex_draw_mouse_pos
			= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);
		Vec2 prev_tex_draw_mouse_pos
			= get_tex_draw_mouse_pos(tab, pos, prev_main_fb_mouse_pos);

		Vec2 mouse_diff = vec2_sub(
			tex_draw_mouse_pos,
			prev_tex_draw_mouse_pos
		);
		tab.pos = vec2_add(tab.pos, vec2_mul(mouse_diff, tab.px_scale));
	}
}

bool kb_zoom_out(const Input &input) {
	return map_press(input, MAP_ZOOM_OUT);
}

bool kb_zoom_in(const Input &input) {
	return map_press(input, MAP_ZOOM_IN);
}

bool kb_zoom_0(const Input &input) {
	return map_press(input, MAP_ZOOM_0);
}

void canvas_zoom_move(Tab &tab, Vec2 sz_diff) {
	sz_diff = vec2_div(sz_diff, 2);
	tab.pos = vec2_sub(tab.pos, sz_diff);
}

void canvas_zoom_out(Tab &tab) {
	Vec2 before_sz = vec2_mul(to_vec2(tab.sz), tab.px_scale);
	tab.px_scale = clampi(tab.px_scale - 1, 1, ZOOM_MAX);
	Vec2 after_sz = vec2_mul(to_vec2(tab.sz), tab.px_scale);

	canvas_zoom_move(tab, vec2_sub(after_sz, before_sz));
}

void canvas_zoom_in(Tab &tab) {
	Vec2 before_sz = vec2_mul(to_vec2(tab.sz), tab.px_scale);
	tab.px_scale = clampi(tab.px_scale + 1, 1, ZOOM_MAX);
	Vec2 after_sz = vec2_mul(to_vec2(tab.sz), tab.px_scale);

	canvas_zoom_move(tab, vec2_sub(after_sz, before_sz));
}

void layer_data_resize(std::vector<unsigned char> &data,
Vec2i prev_sz, Vec2i new_pos, Vec2i new_sz) {
	auto get_px = [&data, prev_sz](int x, int y) -> unsigned char {
		if (x < 0 || y < 0 || x >= prev_sz.x || y >= prev_sz.y) {
			return 0;
		}

		int index = prev_sz.x * y + x;
		return data[index];
	};

	std::vector<unsigned char> data_1;

	for (int y = new_pos.y; y < new_pos.y + new_sz.y; y++) {
	for (int x = new_pos.x; x < new_pos.x + new_sz.x; x++) {
		data_1.push_back(get_px(x, y));
	}
	}

	data = data_1;
}

}

int tab_new(std::vector<Tab> &tab_list, GraphicStuff &gs,
Vec2 pos, Vec2i sz, int px_scale) {
	int index = get_blank_index(tab_list);
	if (index == -1) {
		Tab new_tab;
		tab_list.push_back(new_tab);

		index = (int)tab_list.size() - 1;
	}
	Tab &tab = tab_list[index];

	tab.btn = btn_new(
		vec2_new(0, 0), vec2_new(4 * 5 + 1, 12), BTN_TEXTAREA_COLOR, "tab"
	);
	tab.close_btn = btn_new(
		vec2_new(4 * 5 + 1, 0), vec2_new(12, 12),
		BTN_TEXTAREA_COLOR, "ICON_X"
	);

	tab.pos = pos;
	tab.sz = sz;
	tab.px_scale = px_scale;

	tab.color_picker = color_picker_new(
		vec2_new(SIDE_BAR_W + 4 + 4, -88 - 4)
	);
	tab.color_pallete = color_pallete_new(
		vec2_new(SIDE_BAR_W + 27 + 4, TOP_BAR_H + 3 + 4)
	);
	tab.layer_bar = layer_bar_new(
		vec2_new(4, -100 - 3), vec2_new(100, 100)
	);
	tab.tool_picker = tool_picker_new(
		vec2_new(SIDE_BAR_W + 57 + 4, TOP_BAR_H + 3 + 4)
	);
	tab.btn_panel = btn_panel_new(
		vec2_new(SIDE_BAR_W + 154 + 4, TOP_BAR_H + 1 + 4)
	);

	tab.pallete_data.resize(16 * 16, 255);
	pallete_data_color(tab, 0, color_new(0, 0, 0, 0));
	tab.pallete_texture_index = texture_blank_new(gs, 16, 16);
	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);

	tab.tool_preview_pallete_data.resize(16 * 16, 1);
	tool_preview_pallete_data_color(tab, 0, color_new(0, 0, 0, 0));
	tool_preview_pallete_data_color(tab, 1, color_new(0, 0, 0, 1));
	tab.tool_preview_pallete_texture_index = texture_blank_new(gs, 16, 16);
	texture_data(gs, tab.tool_preview_pallete_texture_index,
		vec2i_new(16, 16), tab.tool_preview_pallete_data);

	tab.tool_preview_data.resize(sz.x * sz.y, 0);
	tab.tool_preview_texture_index = texture_blank_new_red(gs, sz.x, sz.y);
	texture_data_red(gs, tab.tool_preview_texture_index, sz,
		tab.tool_preview_data);

	selection_init(tab.selection, sz);
	tab.selection_preview_data.resize(sz.x * sz.y, 0);
	tab.selection_preview_texture_index= texture_blank_new_red(gs, sz.x, sz.y);

	tab_layer_new(tab, 0, "bkg", gs);

	tab.running = true;

	return index;
}

void tab_update(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, const Settings &settings, Vec2 parent_pos,bool show){
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 bottom_pos
		= vec2_add(parent_pos, vec2_new(0, main_fb_sz.y));

	color_picker_update(tab.color_picker, gs, input, bottom_pos, show);
	color_pallete_update(tab.color_pallete, gs, input, parent_pos, 
		!tab.layer_name_editing, show);
	layer_bar_update(tab.layer_bar, gs, input, bottom_pos, show);
	tool_picker_update(tab.tool_picker, gs, input, parent_pos,
		!tab.layer_name_editing && !input.left_down && !input.left_release,
		show);
	btn_panel_update(tab.btn_panel, gs, input, parent_pos, show);

	layer_textarea_list_update(tab, gs, game_time, input, parent_pos, show);

	if (!show) {
		return;
	}

	if (tab.layer_bar.add_btn.clicked) {
		tab_layer_new(
			tab,
			tab.layer_order_list_index + 1,
			"layer " + std::to_string(tab.num_layer_created),
			gs
		);
		tab.layer_order_list_index++;
	}

	if (tab.btn_panel.zoom_out_btn.clicked || kb_zoom_out(input)) {
		canvas_zoom_out(tab);
	}
	else if (tab.btn_panel.zoom_in_btn.clicked || kb_zoom_in(input)) {
		canvas_zoom_in(tab);
	}
	else if (tab.btn_panel.zoom_0_btn.clicked || kb_zoom_0(input)) {
		tab_center_canvas(tab, gs);
	}

	layer_bar_event_handle(tab);
	canvas_move_update(tab, gs, input, parent_pos);
	color_picker_color_pallete_data_update(tab, gs);
	tool_update(tab, gs, input, game_time, settings, parent_pos);
	select_tool_preview_update(tab, gs, game_time);
}

void tab_blur_rects_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	auto draw = [&gs, main_fb_sz, parent_pos](Vec2 pos, Vec2 sz) -> void {
		draw_texture(
			gs,
			main_fb_sz,
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			color_new(0, 0, 0, 0),
			false
		);
	};

	draw(
		vec2_sub(tab.color_pallete.pos, vec2_new(24 + 3, 3)),
		vec2_add(COLOR_PALLETE_SZ, vec2_new(24 + 6, 6))
	);
	
	Vec2 color_picker_pos
		= vec2_add(tab.color_picker.pos, vec2_new(0, main_fb_sz.y));
	draw(
		vec2_sub(color_picker_pos, vec2_new(4, 4)),
		vec2_add(color_picker_get_sz(tab.color_picker), vec2_new(8, 8))
	);

	draw(
		vec2_sub(tab.tool_picker.pos, vec2_new(3, 3)),
		vec2_add(TOOL_PICKER_SZ, vec2_new(5, 6))
	);

	draw(
		vec2_sub(tab.btn_panel.pos, vec2_new(1, 1)),
		vec2_add(BTN_PANEL_SZ, vec2_new(2, 2))
	);
}

void tab_bkg_draw(const Tab &tab, GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 pos = vec2_floor(vec2_add(parent_pos, tab.pos));
	Vec2 sz = vec2_mul(to_vec2(tab.sz), tab.px_scale);

	const float TILE_SZ = 50;
	
	for (float x = pos.x; floor2(x, TILE_SZ) <= pos.x + sz.x; x += TILE_SZ){
		float start_x = floor2(x, TILE_SZ);
		float end_x = start_x + TILE_SZ;
		if (start_x < pos.x) {
			start_x = std::floor(pos.x);
		}
		if (end_x > pos.x + sz.x) {
			end_x = std::floor(pos.x) + sz.x;
		}

		for (float y = pos.y; floor2(y, TILE_SZ) <= pos.y + sz.y;
		y += TILE_SZ) {
			float start_y = floor2(y, TILE_SZ);
			float end_y = start_y + TILE_SZ;
			if (start_y < pos.y) {
				start_y = std::floor(pos.y);
			}
			if (end_y > pos.y + sz.y) {
				end_y = std::floor(pos.y) + sz.y;
			}

			Color color_0 = color_new(0.9, 0.9, 0.9, 1);
			Color color_1 = color_new(0.75, 0.75, 0.75, 1);
			Color color;
			int x_even = (int)start_x / (int)TILE_SZ % 2 == 0;
			int y_even = (int)start_y / (int)TILE_SZ % 2 == 0;
			if (x_even == y_even) {
				color = color_0;
			}
			else {
				color = color_1;
			}

			draw_rect(
				gs,
				vec2_new(start_x, start_y),
				vec2_new(end_x - start_x, end_y - start_y),
				color
			);
		}
	}
}

void tab_canvas_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);

	layer_list_draw(tab, gs, pos);
}

void tab_ui_draw(const Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 bottom_pos
		= vec2_add(parent_pos, vec2_new(0, main_fb_sz.y));

	layer_textarea_list_draw(tab, gs, game_time, parent_pos);

	color_picker_draw(tab.color_picker, gs, bottom_pos);
	color_pallete_draw(tab.color_pallete, gs, input, parent_pos);
	layer_bar_draw(tab.layer_bar, gs, bottom_pos);
	tool_picker_draw(tab.tool_picker, gs, input, parent_pos);
	btn_panel_draw(tab.btn_panel, gs, parent_pos);
	
	draw_text(
		gs,
		"x" + std::to_string(tab.px_scale),
		vec2_add(tab.btn_panel.pos, vec2_new(45, 3)),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(0, 0),
		false
	);
}

void tab_layer_new(Tab &tab, int at, const std::string &layer_name,
GraphicStuff &gs) {
	std::vector<unsigned char> data;
	data.resize(tab.sz.x * tab.sz.y, 0);
	int index = layer_new(tab.layer_list, gs, layer_name, tab.sz, data);
	
	tab.layer_order_list.insert(tab.layer_order_list.begin() + at, index);
	tab.num_layer_created++;
}

void tab_layer_new_data(Tab &tab, int at, const std::string &layer_name,
GraphicStuff &gs, const std::vector<unsigned char> &data) {
	int index = layer_new(tab.layer_list, gs, layer_name, tab.sz, data);
	
	tab.layer_order_list.insert(tab.layer_order_list.begin() + at, index);
	tab.num_layer_created++;
}

void tab_center_canvas(Tab &tab, const GraphicStuff &gs) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 view_sz = vec2_new(main_fb_sz.x - SIDE_BAR_W, main_fb_sz.y);

	const float MARGIN = 30;
	int px_scale_max_x = (view_sz.x - MARGIN*2) / tab.sz.x;
	int px_scale_max_y = (view_sz.y - MARGIN*2) / tab.sz.y;

	tab.px_scale = std::min(px_scale_max_x, px_scale_max_y);
	tab.px_scale = clampi(tab.px_scale, 1, ZOOM_MAX);
	tab.pos.x = view_sz.x / 2 - tab.sz.x * tab.px_scale / 2 + SIDE_BAR_W;
	tab.pos.y = view_sz.y / 2 - tab.sz.y * tab.px_scale / 2;
}

void tab_resize(Tab &tab, GraphicStuff &gs, Vec2i new_pos, Vec2i new_sz) {
	layer_data_resize(tab.tool_preview_data, tab.sz, new_pos, new_sz);
	texture_data_red(gs, tab.tool_preview_texture_index, new_sz,
		tab.tool_preview_data);
	
	layer_data_resize(tab.selection_preview_data, tab.sz, new_pos, new_sz);
	texture_data_red(gs, tab.selection_preview_texture_index, new_sz,
		tab.selection_preview_data);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		Layer &layer = tab.layer_list[i];

		if (!layer.running) {
			continue;
		}

		layer_data_resize(layer.data, tab.sz, new_pos, new_sz);
		texture_data_red(gs, layer.texture_index, new_sz, layer.data);
		layer.sz = new_sz;
	}

	tab.sz = new_sz;
	tab.pos = vec2_add(tab.pos, to_vec2(new_pos));
}

void tab_close(std::vector<Tab> &tab_list, GraphicStuff &gs, int index) {
	Tab &tab = tab_list[index];
	
	tab.pallete_data.clear();
	texture_release(gs, tab.pallete_texture_index);
	tab.tool_preview_pallete_data.clear();
	texture_release(gs, tab.tool_preview_pallete_texture_index);

	tab.tool_preview_data.clear();
	texture_release(gs, tab.tool_preview_texture_index);
	
	tab.selection_preview_data.clear();
	texture_release(gs, tab.selection_preview_texture_index);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		if (tab.layer_list[i].running) {
			layer_release(tab.layer_list, gs, i);
		}
	}

	tab.layer_list.clear();
	tab.layer_order_list.clear();

	tab.running = false;
}
