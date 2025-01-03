#include "curve.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "tab.h"
#include "../types/vec2.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../draw_tool/draw_tool_line.h"

void curve_tool_preview_update(Tab &tab, int sz, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.left_down && input.mouse_move) {
		draw_tool_line(
			tab.tool_preview_data,
			tab.selection,
			tab.sz,
			tab.tex_draw_tag_pos,
			tex_draw_mouse_pos,
			1,
			sz
		);
		texture_data_red(gs, tab.tool_preview_texture_index,
			tab.sz, tab.tool_preview_data);
		draw_tool_line(
			tab.tool_preview_data,
			tab.selection,
			tab.sz,
			tab.tex_draw_tag_pos,
			tex_draw_mouse_pos,
			0,
			sz
		);
	}

	if (input.left_release) {
		texture_data_red(gs, tab.tool_preview_texture_index,
			tab.sz, tab.tool_preview_data);
	}
}

bool curve_tool_update(Tab &tab, int layer_index, int sz, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	int pallete_index = tab.color_pallete.selected_index;
	Layer &layer = tab.layer_list[layer_index];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	bool px_ed = false;

	if (input.left_click) {
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;
	}

	if (input.left_release) {
		if (draw_tool_line(layer.data, tab.selection, tab.sz,
				tab.tex_draw_tag_pos,
				tex_draw_mouse_pos, pallete_index, sz)) {
			px_ed = true;
		}
		layer_set_texture_data(layer, gs);
	}

	return px_ed;
}
