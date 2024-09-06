#include "fill.h"

#include "tab.h"
#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../types/vec2.h"
#include "../types/vec2i.h"
#include "../draw_tool/draw_tool_fill.h"
#include "../pos_convert.h"

void fill_tool_update(Tab &tab, int layer_index, GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 pos = vec2_add(parent_pos, tab.pos);
	
	int pallete_index = tab.color_pallete.selected_index;
	Layer &layer = tab.layer_list[layer_index];

	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	if (input.left_click) {
		draw_tool_fill(
			layer.data,
			tab.selection,
			tab.sz,
			to_vec2i(tex_draw_mouse_pos),
			pallete_index
		);
		layer_set_texture_data(layer, gs);
	}
}
