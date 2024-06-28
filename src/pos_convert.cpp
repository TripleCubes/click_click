#include "pos_convert.h"

#include "graphic_types/graphic_types.h"
#include "types/vec2.h"
#include "types/vec2i.h"
#include "tab/tab.h"

Vec2i get_main_fb_sz(const GraphicStuff &gs) {
	return vec2i_div_div(gs.current_window_sz, gs.px_scale);
}

Vec2 get_main_fb_offset(const GraphicStuff &gs) {
	Vec2i main_fb_sz = get_main_fb_sz(gs);
	Vec2i offset = vec2i_mul(main_fb_sz, gs.px_scale);
	offset = vec2i_sub(gs.current_window_sz, offset);
	offset = vec2i_div_div(offset, 2);
	return to_vec2(offset);
}

Vec2 get_main_fb_mouse_pos(const GraphicStuff &gs, Vec2 mouse_pos) {
	Vec2 result = vec2_div(mouse_pos, gs.px_scale);
	result = vec2_add(result, get_main_fb_offset(gs));
	return result;
}
