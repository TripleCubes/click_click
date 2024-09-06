#ifndef DRAW_TOOL__DRAW_TOOL_PX_H
#define DRAW_TOOL__DRAW_TOOL_PX_H

#include <vector>

struct Vec2i;
struct Selection;

void draw_tool_px(std::vector<unsigned char> &data, const Selection &selection,
	Vec2i data_sz, Vec2i pos, unsigned char pallete_index, int sz);
void draw_tool_px_no_selection(std::vector<unsigned char> &data,
	const Selection &selection,
	Vec2i data_sz, Vec2i pos, unsigned char pallete_index, int sz);

#endif
