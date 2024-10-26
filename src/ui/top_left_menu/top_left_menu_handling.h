#ifndef UI__TOP_LEFT_MENU__TOP_LEFT_MENU_HANDLING_H
#define UI__TOP_LEFT_MENU__TOP_LEFT_MENU_HANDLING_H

struct States;
struct TopLeftMenu;
struct Input;

void top_left_menu_handling(States &states, const TopLeftMenu &top_left_menu,
	const Input &input);

#endif
