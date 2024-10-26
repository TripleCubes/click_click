#ifndef UI__RESIZE_MENU__RESIZE_MENU_HANDLING_H
#define UI__RESIZE_MENU__RESIZE_MENU_HANDLING_H

struct States;
struct ResizeMenu;
struct Tab;
struct GraphicStuff;
struct Input;

void resize_menu_open(States &states, ResizeMenu &resize_menu, Tab &tab);

void resize_menu_close(States &states);

void resize_menu_handling(States &states, ResizeMenu &resize_menu,
	Tab &tab, GraphicStuff &gs, const Input &input);

#endif
