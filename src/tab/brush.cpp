#include "brush.h"

#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "tab.h"
#include "../types/vec2.h"
#include "../pos_convert.h"
#include "../basic_math.h"
#include "../draw_tool/draw_tool_px.h"
#include "../draw_tool/draw_tool_line.h"

void brush_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	int pallete_index = tab.color_pallete.selected_index;
	
	Layer &layer = tab.layer_list[layer_index];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	bool drawn = false;

	if (input.left_click) {
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;
		draw_tool_px(layer.data, tab.sz,
			to_vec2i(tex_draw_mouse_pos), pallete_index);

		drawn = true;
	}

	if (input.left_down && input.mouse_move) {
		draw_tool_line(layer.data, tab.sz, tab.tex_draw_tag_pos,
			tex_draw_mouse_pos, pallete_index);
		tab.tex_draw_tag_pos = tex_draw_mouse_pos;

		drawn = true;
	}

	if (drawn) {
		layer_set_texture_data(layer, gs);
	}
}
