#include "curve.h"

#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "../input.h"
#include "tab.h"
#include "../types/vec2.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../draw_tool/draw_tool_line.h"

void curve_tool_preview_update(Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (!in_rect(main_fb_mouse_pos, vec2_new(0, 0), main_fb_sz)) {
		return;
	}

	if (input.left_down && input.mouse_move) {
		draw_tool_line(
			tab.tool_preview_data,
			tab.sz,
			tab.tex_draw_tag_pos,
			tex_draw_mouse_pos,
			1
		);
		texture_data_red(gs, tab.tool_preview_texture_index,
			tab.sz, tab.tool_preview_data);
		draw_tool_line(
			tab.tool_preview_data,
			tab.sz,
			tab.tex_draw_tag_pos,
			tex_draw_mouse_pos,
			0
		);
	}

	if (input.left_release) {
		texture_data_red(gs, tab.tool_preview_texture_index,
			tab.sz, tab.tool_preview_data);
	}
}

void curve_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	int pallete_index = tab.color_pallete.selected_index;
	Layer &layer = tab.layer_list[layer_index];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 main_fb_sz = to_vec2(get_main_fb_sz(gs));
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (!in_rect(main_fb_mouse_pos, vec2_new(0, 0), main_fb_sz)) {
		return;
	}

	if (input.left_click) {
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;
	}

	if (input.left_release) {
		draw_tool_line(layer.data, tab.sz, tab.tex_draw_tag_pos,
			tex_draw_mouse_pos, pallete_index);
		layer_set_texture_data(layer, gs);
	}
}
