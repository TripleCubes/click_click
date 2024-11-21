#include "tab.h"

#include <cmath>
#include <stdlib.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

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
#include "../states.h"

#include "../draw_tool/draw_tool_px.h"
#include "../draw_tool/draw_tool_line.h"

#include "brush.h"
#include "curve.h"
#include "fill.h"
#include "px_perfect_brush.h"

#include "copypaste.h"
#include "color_pallete_copypaste.h"

// TEST
#include <iostream>

extern std::string clipboard;

namespace {

const int PALLETE_DRAW = 0;
const int PALLETE_TOOL_PREVIEW = 1;

const float LAYER_TEXTAREA_LIST_LINE_HEIGHT = 12;

const int ZOOM_MAX = 15;

std::vector<unsigned char> selected_data;
Vec2i top_left;
Vec2i bottom_right;

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

int get_layer_index(const Tab &tab) {
	return tab.layer_order_list[tab.layer_order_list_index];
}

void tab_set_layer_order_list_index_with_history(Tab &tab, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings,
int index) {
	tab.layer_order_list_index = index;
}

void tab_layer_new_with_history(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, const Settings &settings) {
	history_commit_prepare(tab.history, tab.tab_commands);

	Command command = command_new(
		tab.history.time_pos_current,
		COMMAND_LAYER_NEW,
		
		tab.history.layer_list.size(),
		tab.layer_order_list_index,
		0,
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),

		"layer " + std::to_string(tab.num_layer_created),
		""
	);
	int layer_index = tab_commands_do_and_add(
		tab.tab_commands,
		command,
		tab.history,
		gs,
		input,
		game_time,
		settings,
		tab
	);

	history_layer_add(tab.history, tab.layer_list[layer_index]);
}

void tab_layer_delete_with_history(Tab &tab, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings) {
	history_commit_prepare(tab.history, tab.tab_commands);

	const Layer &layer = tab.layer_list[get_layer_index(tab)];

	Command command = command_new(
		tab.history.time_pos_current,
		COMMAND_LAYER_DELETE,
		
		layer.history_layer_index,
		tab.layer_order_list_index,
		0,
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),

		layer.textarea.text,
		""
	);
	tab_commands_do_and_add(
		tab.tab_commands,
		command,
		tab.history,
		gs,
		input,
		game_time,
		settings,
		tab
	);
}

void layer_name_set_with_history(Tab &tab, GraphicStuff &gs,
const Input &input, const GameTime &game_time, const Settings &settings) {
	history_commit_prepare(tab.history, tab.tab_commands);

	const Layer &layer = tab.layer_list[get_layer_index(tab)];

	Command command = command_new(
		tab.history.time_pos_current,
		COMMAND_LAYER_RENAME,
		
		0,
		tab.layer_order_list_index,
		0,
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),
		vec2i_new(0, 0),

		layer.textarea.text,
		tab.prev_layer_name
	);
	tab_commands_do_and_add(
		tab.tab_commands,
		command,
		tab.history,
		gs,
		input,
		game_time,
		settings,
		tab
	);
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
		if (layer.hidden) {
			continue;
		}

		layer_texture_draw(tab, gs, layer.texture_index, PALLETE_DRAW,
			vec2_floor(pos));

		if (i == tab.layer_order_list_index) {
			layer_texture_draw(tab, gs, tab.move_preview_texture_index,
				PALLETE_DRAW, vec2_floor(pos));
		}
	}

	layer_texture_draw(tab, gs, tab.tool_preview_texture_index,
		PALLETE_TOOL_PREVIEW, vec2_floor(pos));

	layer_texture_draw(tab, gs, tab.selection_preview_texture_index,
		PALLETE_TOOL_PREVIEW, vec2_floor(pos));
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
		vec2_new(SIDE_BAR_W + 3, 3),
		vec2_new(main_fb_sz.x - SIDE_BAR_W - 3 * 2, TOP_BAR_H - 4)
	);
	if (on_ui) { return true; }

	on_ui = check(
		mouse_pos,
		vec2_sub(tab.color_pallete.pos, vec2_new(24 + 3, 3)),
		vec2_add(COLOR_PALLETE_SZ, vec2_new(24 + 6, 6))
	);
	if (on_ui) { return true; }

	if (tab.color_pallete.at_page != 0
	|| tab.color_pallete.selected_index != 0) {
		Vec2 color_picker_pos
			= vec2_add(tab.color_picker.pos, vec2_new(0, main_fb_sz.y));
		on_ui = check(
			mouse_pos,
			vec2_sub(color_picker_pos, vec2_new(4, 4)),
			vec2_add(color_picker_get_sz(tab.color_picker), vec2_new(8, 8))
		);
		if (on_ui) { return true; }
	}

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

void tool_update(Tab &tab, GraphicStuff &gs, const States &states,
const Input &input, const GameTime &game_time, const Settings &settings,
Vec2 parent_pos) {
	if (tab.panning || tab.after_panning_1_frame) { return; }
	const Layer &layer = tab.layer_list[get_layer_index(tab)];
	if (layer.locked || layer.hidden) { return; }

	bool b_cursor_on_ui
		= cursor_on_ui(tab, gs, input, parent_pos);

	if ((b_cursor_on_ui && input.left_click)
	|| (states.menu_just_close && input.left_down)) {
		tab.clicked_and_hold_on_ui = true;
	}

	bool px_ed = false;

	if (!tab.clicked_and_hold_on_ui && tab.layer_order_list.size() != 0) {
		if (tab.tool_picker.selected_index == TOOL_BRUSH) {
			if (settings.use_px_perfect_brush
			&& tab.tool_picker.brush_selected_index == 0) {
				px_perfect_brush_tool_preview_update(tab, gs,input,parent_pos);
				px_ed = px_perfect_brush_tool_update(tab, get_layer_index(tab),
					gs, input, parent_pos);
			}
			else {
				px_tool_preview_update(tab,
					tab.tool_picker.brush_selected_index,
					gs, input, true, parent_pos);
				px_ed = brush_tool_update(tab, get_layer_index(tab),
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
			px_ed = curve_tool_update(tab, get_layer_index(tab),
				tab.tool_picker.curve_selected_index, gs, input, parent_pos);
		}

		else if (tab.tool_picker.selected_index == TOOL_FILL) {
			if (!b_cursor_on_ui) {
				gs.cursor_icon = CURSOR_FILL;
			}
			px_tool_preview_update(tab, 0, gs, input, true, parent_pos);
			px_ed = fill_tool_update(tab, get_layer_index(tab), gs,
				input, parent_pos);
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

		else if (tab.tool_picker.selected_index == TOOL_MOVE) {
			move_tool_preview_update(tab.move, tab, gs, input, parent_pos);
			move_tool_update(tab.move, tab, gs, input, parent_pos);
		}
	}

	if (px_ed) {
		tab.px_change_pending = true;
	}

	if (map_press(input, MAP_SELECT_ALL)) {
		selection_all(tab.selection, tab.sz);
	}

	if (map_press(input, MAP_DESELECT_ALL)) {
		selection_clear(tab.selection, tab.sz);
	}

	if (!tab.clicked_and_hold_on_ui && input.left_release
	&& tab.px_change_pending) {
		tab.px_change_pending = false;
		history_commit_prepare(tab.history, tab.tab_commands);
		history_commit_layer(
			tab.history, tab.history.layer_list[layer.history_layer_index],
			layer
		);
		tab_set_name(tab);
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
const GameTime &game_time, const Input &input, const Settings &settings,
Vec2 parent_pos, bool show) {
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
		if (tab.layer_name_editing == true) {
			layer_name_set_with_history(tab, gs, input, game_time, settings);
		}
		tab.layer_name_editing = false;
	}

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[index];
	
		if (layer.textarea.clicked) {
			if (tab.layer_order_list_index == i) {
				tab.layer_name_editing = true;
				tab.prev_layer_name = layer.textarea.text;
				gs.cursor_icon = CURSOR_TEXT;
				return;
			}

			tab_set_layer_order_list_index_with_history(
				tab, gs, input, game_time, settings, i
			);
			return;
		}

		if (layer.textarea.hovered && tab.layer_order_list_index == i) {
			gs.cursor_icon = CURSOR_TEXT;
			return;
		}
	}

	if (tab.layer_bar.delete_layer_btn.clicked
	&& tab.layer_order_list.size() > 1) {
		tab_layer_delete_with_history(tab, gs, input, game_time, settings);
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

void layer_bar_event_handle(Tab &tab, GraphicStuff &gs, const Input &input,
const GameTime &game_time, const Settings &settings) {
	if (tab.layer_order_list_index > 0
	&& tab.layer_bar.up_btn.clicked) {
		history_commit_prepare(tab.history, tab.tab_commands);

		Command command = command_new(
			tab.history.time_pos_current,
			COMMAND_LAYER_MOVE_UP,
			
			0,
			tab.layer_order_list_index - 1,
			0,
			vec2i_new(0, 0),
			vec2i_new(0, 0),
			vec2i_new(0, 0),
			vec2i_new(0, 0),

			"",
			""
		);

		tab_commands_do_and_add(
			tab.tab_commands,
			command,
			tab.history,
			gs,
			input,
			game_time,
			settings,
			tab
		);
	}

	else if (tab.layer_order_list_index < (int)tab.layer_order_list.size() - 1
	&& tab.layer_bar.down_btn.clicked) {
		history_commit_prepare(tab.history, tab.tab_commands);

		Command command = command_new(
			tab.history.time_pos_current,
			COMMAND_LAYER_MOVE_DOWN,
			
			0,
			tab.layer_order_list_index + 1,
			0,
			vec2i_new(0, 0),
			vec2i_new(0, 0),
			vec2i_new(0, 0),
			vec2i_new(0, 0),

			"",
			""
		);

		tab_commands_do_and_add(
			tab.tab_commands,
			command,
			tab.history,
			gs,
			input,
			game_time,
			settings,
			tab
		);
	}
}

void canvas_move_update(Tab &tab, const GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	if (map_press(input, MAP_PAN)) {
		tab.panning_pan_key_down_at = game_time.time_since_start;
	}
	if (input.left_click) {
		tab.panning_left_down_at = game_time.time_since_start;
	}

	if (map_down(input, MAP_PAN) && input.left_down
	&& tab.panning_left_down_at > tab.panning_pan_key_down_at) {
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

void delete_selected_data(Tab &tab, GraphicStuff &gs) {
	Layer &layer = tab.layer_list[get_layer_index(tab)];

	for (int y = 0; y < tab.sz.y; y++) {
	for (int x = 0; x < tab.sz.x; x++) {
		int i = y * tab.sz.x + x;

		if (tab.selection.map[i] == 0) {
			continue;
		}

		layer.data[i] = 0;
	}
	}

	layer_set_texture_data(layer, gs);

	history_commit_prepare(tab.history, tab.tab_commands);
	history_commit_layer(
		tab.history, tab.history.layer_list[layer.history_layer_index],
		layer
	);
}

void delete_whole_layer_data(Tab &tab, GraphicStuff &gs) {
	Layer &layer = tab.layer_list[get_layer_index(tab)];

	for (int y = 0; y < tab.sz.y; y++) {
	for (int x = 0; x < tab.sz.x; x++) {
		layer.data[y * tab.sz.x + x] = 0;
	}
	}

	layer_set_texture_data(layer, gs);

	history_commit_prepare(tab.history, tab.tab_commands);
	history_commit_layer(
		tab.history, tab.history.layer_list[layer.history_layer_index],
		layer
	);
}

void delete_selected_or_whole_layer_data(Tab &tab, GraphicStuff &gs) {
	if (tab.selection.full_preview_list.size() != 0) {
		delete_selected_data(tab, gs);
	}
	else {
		delete_whole_layer_data(tab, gs);
	}
}

void history_and_move_tool_handling(Tab &tab, const Layer &layer,
GraphicStuff &gs, const Input &input, const GameTime &game_time,
const Settings &settings, Vec2 parent_pos
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	auto _move_tool_start = [&tab, &gs, &input, parent_pos]() {
		move_tool_prepare(tab.move);
		Vec2i pos;
		tab_get_selected_data(tab, tab.move.data, pos, tab.move.sz);
		tab.move.pos = to_vec2(pos);
		move_tool_start(tab.move, tab, gs, input, parent_pos);

		delete_selected_or_whole_layer_data(tab, gs);
		selection_clear(tab.selection, tab.sz);
	};

	auto _move_tool_end = [&tab, &gs]() {
		move_tool_commit(tab.move, tab, gs);
	};

	if (tab.tool_picker.selection_changed && !tab.move.moving
	&& tab.tool_picker.selected_index == TOOL_MOVE
	&& tab.selection.full_preview_list.size() != 0) {
		_move_tool_start();
	}

	if (tab.tool_picker.selected_index == TOOL_MOVE && !tab.move.moving
	&& input.left_click && tab.selection.full_preview_list.size() != 0) {
		_move_tool_start();
	}

	if (tab.tool_picker.selection_changed
	&& tab.tool_picker.selected_index != TOOL_MOVE
	&& tab.move.moving) {
		_move_tool_end();
	}

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		const Layer &layer = tab.layer_list[i];
		if (!layer.running) {
			continue;
		}
		if (layer.textarea.clicked) {
			_move_tool_end();
		}
	}

	if ((layer.show_hide_btn.clicked || layer.lock_btn.clicked
	|| layer.textarea.clicked)
	&& tab.move.moving) {
		_move_tool_end();
	}

	if (tab.layer_bar.add_btn.clicked || tab.layer_bar.up_btn.clicked
	|| tab.layer_bar.down_btn.clicked
	|| tab.layer_bar.delete_layer_btn.clicked) {
		_move_tool_end();
	}

	if (tab.move.moving && map_press(input, MAP_ENTER)) {
		_move_tool_end();
	}


	if (map_press(input, MAP_UNDO)
	|| tab.btn_panel.undo_btn.clicked) {
		move_tool_discard(tab.move, tab, gs);
		history_undo(tab.history, tab, gs, input, game_time, settings);
		tab_set_name(tab);
	}
	if (map_press(input, MAP_REDO) || map_press(input, MAP_REDO_1)
	|| tab.btn_panel.redo_btn.clicked) {
		history_redo(tab.history, tab, gs, input, game_time, settings);
		tab_set_name(tab);
	}

	if (map_press(input, MAP_DELETE)) {
		delete_selected_or_whole_layer_data(tab, gs);
	}

	#ifdef __EMSCRIPTEN__
	if (map_press(input, MAP_LOCAL_CLIPBOARD)) {
		const char *c_ptr = (const char *)EM_ASM_PTR({
		{
			let str = prompt('local clipboard:', UTF8ToString($0));

			if (str == null) {
				return stringToNewUTF8(UTF8ToString($0));
			}
			else {
				return stringToNewUTF8(str);
			}
		}
		}, clipboard.c_str());

		clipboard = c_ptr;
		free((void*)c_ptr);
	}
	#endif

	if (map_press(input, MAP_COPY)) {
		to_clipboard(tab, false
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif
		);
	}
	if (map_press(input, MAP_COPY_DATA_ONLY)) {
		to_clipboard(tab, true
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif
		);
	}
	if (map_press(input, MAP_CUT)) {
		to_clipboard(tab, false
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif
		);
		delete_selected_or_whole_layer_data(tab, gs);
	}
	if (map_press(input, MAP_PASTE) && !layer.locked && !layer.hidden) {
		if (tab.move.moving) {
			_move_tool_end();
		}
		move_tool_prepare(tab.move);
		Vec2i pos;
		bool valid = get_paste_data(
			tab.move.data,
			pos,
			tab.move.sz
			#ifndef __EMSCRIPTEN__
			,glfw_window
			#endif
		);
		tab.move.pos = to_vec2(pos);
		if (valid) {
			move_tool_start(tab.move, tab, gs, input, parent_pos);

			selection_clear(tab.selection, tab.sz);
			tab.tool_picker.selected_index = TOOL_MOVE;
		}
	}


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

	tab.name = "tab";
	tab.btn = btn_new(
		vec2_new(0, 0), vec2_new(4 * 3 + TAB_BTN_W_ADD, 12),
		BTN_TEXTAREA_COLOR, tab.name
	);
	tab.close_btn = btn_new(
		vec2_new(4 * 3 + TAB_BTN_W_ADD, 0), vec2_new(12, 12),
		BTN_TEXTAREA_COLOR, "ICON_X"
	);

	tab.pos = pos;
	tab.sz = sz;
	tab.px_scale = px_scale;

	tab.color_picker = color_picker_new(
		vec2_new(SIDE_BAR_W + 7, -91)
	);
	tab.color_pallete = color_pallete_new(
		vec2_new(SIDE_BAR_W + 30, TOP_BAR_H + 5)
	);
	tab.layer_bar = layer_bar_new(
		vec2_new(3, -122), vec2_new(0, 120)
	);
	tab.tool_picker = tool_picker_new(
		vec2_new(SIDE_BAR_W + 59, TOP_BAR_H + 5)
	);
	tab.btn_panel = btn_panel_new(
		vec2_new(SIDE_BAR_W + 149, TOP_BAR_H + 3)
	);
	tab.color_pallete_btn_panel = color_pallete_btn_panel_new(
		vec2_new(SIDE_BAR_W + 6, TOP_BAR_H + 62)
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

	tab.move_preview_data.resize(sz.x * sz.y, 0);
	tab.move_preview_texture_index = texture_blank_new_red(gs, sz.x, sz.y);

	history_init(tab.history);
	tab_commands_init(tab.tab_commands);

	tab_layer_new(tab, 0, "bkg", gs, 0);
	history_layer_add(tab.history, tab.layer_list[0]);

	tab.running = true;

	return index;
}

void tab_update(Tab &tab, GraphicStuff &gs, const States &states,
const Input &input, const GameTime &game_time, const Settings &settings,
Vec2 parent_pos, bool show
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
){
	const Layer &layer = tab.layer_list[get_layer_index(tab)];

	bool tool_key_allowed = !tab.layer_name_editing && !input.left_down
		&& !input.left_release;

	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);
	Vec2 bottom_pos
		= vec2_add(parent_pos, vec2_new(0, main_fb_sz.y));

	color_picker_update(tab.color_picker, gs, input, bottom_pos,
		show && (tab.color_pallete.at_page != 0
		         || tab.color_pallete.selected_index != 0));
	color_pallete_update(tab.color_pallete, gs, input, parent_pos,
		!tab.layer_name_editing, show);
	layer_bar_update(tab.layer_bar, gs, input, bottom_pos, show);
	tool_picker_update(tab.tool_picker, gs, input, parent_pos,
		tool_key_allowed, show);
	btn_panel_update(tab.btn_panel, gs, input, parent_pos, show);
	color_pallete_btn_panel_update(
		tab.color_pallete_btn_panel, gs, input, parent_pos, show
	);

	layer_textarea_list_update(tab, gs, game_time, input, settings,
		parent_pos, show);

	if (!show) {
		return;
	}

	layer_bar_event_handle(tab, gs, input, game_time, settings);
	canvas_move_update(tab, gs, input, game_time, parent_pos);
	color_picker_color_pallete_data_update(tab, gs);
	tool_update(tab, gs, states, input, game_time, settings, parent_pos);
	select_tool_preview_update(tab, gs, game_time);

	history_and_move_tool_handling(tab, layer, gs, input, game_time, settings,
		parent_pos
		#ifndef __EMSCRIPTEN__
		,glfw_window
		#endif
	);

	if (tab.layer_bar.add_btn.clicked) {
		tab_layer_new_with_history(tab, gs, input, game_time, settings);
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

	if (tab.tool_picker.btn_list[TOOL_ERASER].clicked
	|| (tool_key_allowed && map_press(input, MAP_TOOL_ERASER))) {
		color_pallete_toggle_eraser(tab.color_pallete);
	}

	if (layer.show_hide_btn.clicked || layer.lock_btn.clicked) {
		selection_clear(tab.selection, tab.sz);
	}

	if (tab.color_pallete_btn_panel.copy_btn.clicked) {
		color_pallete_to_clipboard(tab.color_pallete
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif
		);
	}
	if (tab.color_pallete_btn_panel.paste_btn.clicked) {
		color_pallete_paste(tab.color_pallete, tab, gs
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif
		);
	}
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

	if (tab.color_pallete.at_page != 0
	|| tab.color_pallete.selected_index != 0) {
		Vec2 color_picker_pos
			= vec2_add(tab.color_picker.pos, vec2_new(0, main_fb_sz.y));
		draw(
			vec2_sub(color_picker_pos, vec2_new(4, 4)),
			vec2_add(color_picker_get_sz(tab.color_picker), vec2_new(8, 8))
		);
	}

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
	
	if (tab.color_pallete.at_page != 0
	|| tab.color_pallete.selected_index != 0) {
		color_picker_draw(tab.color_picker, gs, bottom_pos);
	}
	color_pallete_draw(tab.color_pallete, gs, input, parent_pos);
	layer_bar_draw(tab.layer_bar, gs, bottom_pos);
	tool_picker_draw(tab.tool_picker, gs, input, parent_pos);
	btn_panel_draw(tab.btn_panel, gs, input, parent_pos);
	color_pallete_btn_panel_draw(
		tab.color_pallete_btn_panel, gs, parent_pos
	);
	
	draw_text(
		gs,
		"x" + std::to_string(tab.px_scale),
		vec2_add(tab.btn_panel.pos, vec2_new(72, 3)),
		100,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(0, 0),
		false
	);
}

int tab_layer_new(Tab &tab, int at,
const std::string &layer_name, GraphicStuff &gs, int history_layer_index) {
	std::vector<unsigned char> data;
	data.resize(tab.sz.x * tab.sz.y, 0);
	int index = layer_new(tab.layer_list, gs, layer_name,
		tab.sz, data, false, false, history_layer_index);
	
	tab.layer_order_list.insert(tab.layer_order_list.begin() + at, index);

	return index;
}

void tab_layer_new_data(Tab &tab, int at, const std::string &layer_name,
bool hidden, bool locked,
GraphicStuff &gs, const std::vector<unsigned char> &data) {
	int index = layer_new(tab.layer_list, gs, layer_name, tab.sz,
		data, hidden, locked, tab.history.layer_list.size() - 1);
	history_layer_add(tab.history, tab.layer_list[index]);
	history_commit_layer(
		tab.history,
		tab.history.layer_list[tab.layer_list[index].history_layer_index],
		tab.layer_list[index]
	);

	tab.layer_order_list.insert(tab.layer_order_list.begin() + at, index);
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
	tab.pos.y += TOP_BAR_H;
}

void tab_resize(Tab &tab, GraphicStuff &gs, Vec2i new_pos, Vec2i new_sz) {
	if (tab.move.moving) {
		move_tool_commit(tab.move, tab, gs);
	}

	layer_data_resize(tab.tool_preview_data, tab.sz, new_pos, new_sz);
	texture_data_red(gs, tab.tool_preview_texture_index, new_sz,
		tab.tool_preview_data);
	
	layer_data_resize(tab.selection_preview_data, tab.sz, new_pos, new_sz);
	texture_data_red(gs, tab.selection_preview_texture_index, new_sz,
		tab.selection_preview_data);

	layer_data_resize(tab.move_preview_data, tab.sz, new_pos, new_sz);
	texture_data_red(gs, tab.move_preview_texture_index, new_sz,
		tab.move_preview_data);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		Layer &layer = tab.layer_list[i];

		if (!layer.running) {
			continue;
		}

		layer_data_resize(layer.data, tab.sz, new_pos, new_sz);
		texture_data_red(gs, layer.texture_index, new_sz, layer.data);
		layer.sz = new_sz;
	}

	selection_resize(tab.selection, tab.sz, new_sz);

	tab.sz = new_sz;
	tab.pos = vec2_add(tab.pos, to_vec2(new_pos));
}

void tab_get_selected_data(Tab &tab, std::vector<unsigned char> &data,
Vec2i &pos, Vec2i &sz) {
	selected_data.clear();
	top_left = vec2i_new(tab.sz.x - 1, tab.sz.y - 1);
	bottom_right = vec2i_new(0, 0);

	selected_data.resize(tab.sz.x * tab.sz.y, 0);

	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	const Layer &layer = tab.layer_list[layer_index];
	for (int y = 0; y < (int)layer.sz.y; y++) {
	for (int x = 0; x < (int)layer.sz.x; x++) {
		int i = y * tab.sz.x + x;

		if (tab.selection.map[i] == 0) {
			continue;
		}

		if (x < top_left.x) {
			top_left.x = x;
		}
		if (y < top_left.y) {
			top_left.y = y;
		}
		if (x > bottom_right.x) {
			bottom_right.x = x;
		}
		if (y > bottom_right.y) {
			bottom_right.y = y;
		}

		selected_data[i] = layer.data[i];
	}
	}

	data.clear();
	int data_w = bottom_right.x - top_left.x + 1;
	int data_h = bottom_right.y - top_left.y + 1;
	data.resize(data_w * data_h, 0);
	for (int y = top_left.y; y <= bottom_right.y; y++) {
	for (int x = top_left.x; x <= bottom_right.x; x++) {
		int selected_data_i = y * tab.sz.x + x;
		int data_i = (y - top_left.y) * data_w + (x - top_left.x);

		data[data_i] = selected_data[selected_data_i];
	}
	}

	pos = top_left;
	sz = vec2i_new(data_w, data_h);
}

void tab_set_name(Tab &tab) {
	if (tab.history.time_pos_current != tab.time_pos_last_save) {
		tab.btn.text = tab.name + " *";
	}
	else {
		tab.btn.text = tab.name;
	}
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

	tab.move_preview_data.clear();
	texture_release(gs, tab.move_preview_texture_index);

	selection_release(tab.selection);
	history_release(tab.history);
	tab_commands_release(tab.tab_commands);

	for (int i = 0; i < (int)tab.layer_list.size(); i++) {
		if (tab.layer_list[i].running) {
			layer_release(tab.layer_list, gs, i);
		}
	}

	tab.layer_list.clear();
	tab.layer_order_list.clear();

	tab.running = false;
}
