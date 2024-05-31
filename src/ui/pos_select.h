#ifndef UI__POS_SELECT_H
#define UI__POS_SELECT_H

#include "../types/vec2.h"

struct GraphicStuff;
struct Input;

struct PosSelect {
	bool selected = false;
	Vec2 pos;
	Vec2 sz;
	Vec2 selected_pos;
};

PosSelect pos_select_new(Vec2 pos, Vec2 sz);

void pos_select_update(PosSelect &pos_select,
	const GraphicStuff &gs, const Input &input);

void pos_select_draw(const PosSelect &pos_select,
	const GraphicStuff &gs);

Vec2 pos_select_get_selected(const PosSelect &pos_select);

#endif
