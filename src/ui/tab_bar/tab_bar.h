#ifndef UI__TAB_BAR__TAB_BAR_H
#define UI__TAB_BAR__TAB_BAR_H

#include <vector>
#include "../btn.h"
#include "../../tab/tab.h"
#include "../../types/vec2.h"

struct GraphicStuff;
struct Input;

struct TabBar {
	Vec2 pos;
	std::vector<Tab> tab_list;
	std::vector<int> tab_order_list;
	int order_index = 0;
};

void tab_bar_init(TabBar &tab_bar, GraphicStuff &gs, Vec2 pos);

void tab_bar_update(TabBar &tab_bar, GraphicStuff &gs,
	const Input &input, bool show);

void tab_bar_draw(const TabBar &tab_bar, GraphicStuff &gs);

void tab_bar_tab_new(TabBar &tab_bar, GraphicStuff &gs, int at,
	Vec2 pos, Vec2i sz, int scale);

void tab_bar_release(TabBar &tab_bar, GraphicStuff &gs);

int tab_bar_get_tab_index(const TabBar& tab_bar);

#endif
