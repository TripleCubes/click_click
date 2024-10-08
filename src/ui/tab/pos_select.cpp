#include "pos_select.h"

#include "../../types/vec2.h"
#include "../../types/vec2i.h"
#include "../../types/color.h"
#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/framebuffer.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_rect.h"
#include "../../input.h"

#include "../../basic_math.h"
#include "../../pos_convert.h"

PosSelect pos_select_new(Vec2 pos, Vec2 sz) {
	PosSelect pos_select;
	pos_select.pos = pos;
	pos_select.sz = sz;
	return pos_select;
}

void pos_select_cond(PosSelect &pos_select,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, pos_select.pos);

	if (input.left_release) {
		pos_select.selected = false;
	}

	if (!show) {
		pos_select.selected = false;
		return;
	}

	if (!in_rect(mouse_pos, pos, pos_select.sz)) {
		return;
	}

	if (input.left_click) {
		pos_select.selected = true;
	}
}

void pos_select_select(PosSelect &pos_select, const GraphicStuff &gs,
const Input &input, Vec2 parent_pos) {
	Vec2 mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 pos = vec2_add(parent_pos, pos_select.pos);

	if (pos_select.selected) {
		pos_select.selected_pos = vec2_sub(mouse_pos, pos);
		pos_select.selected_pos.x = clampf(
			pos_select.selected_pos.x,
			0,
			pos_select.sz.x
		);
		pos_select.selected_pos.y = clampf(
			pos_select.selected_pos.y,
			0,
			pos_select.sz.y
		);
	}
}

void pos_select_update(PosSelect &pos_select,
const GraphicStuff &gs, const Input &input, Vec2 parent_pos, bool show) {
	pos_select_cond(pos_select, gs, input, parent_pos, show);
	pos_select_select(pos_select, gs, input, parent_pos);
}

void pos_select_draw(const PosSelect &pos_select,
GraphicStuff &gs, Vec2 parent_pos) {
	Vec2 pspos = vec2_add(pos_select.pos, parent_pos);
	pspos = vec2_add(pspos, pos_select.selected_pos);
	Color color = color_new(0, 0, 0, 1);

	draw_rect(
		gs,
		vec2_new(pspos.x, pspos.y - 3),
		vec2_new(1, 3),
		color
	);
	draw_rect(
		gs,
		vec2_new(pspos.x, pspos.y + 1),
		vec2_new(1, 3),
		color
	);
	draw_rect(
		gs,
		vec2_new(pspos.x - 3, pspos.y),
		vec2_new(3, 1),
		color
	);
	draw_rect(
		gs,
		vec2_new(pspos.x + 1, pspos.y),
		vec2_new(3, 1),
		color
	);
}

Vec2 pos_select_get_selected(const PosSelect &pos_select) {
	return vec2_new(
		pos_select.selected_pos.x / pos_select.sz.x,
		pos_select.selected_pos.y / pos_select.sz.y
	);
}
