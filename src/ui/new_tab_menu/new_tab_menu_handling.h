#ifndef UI__NEW_TAB_MENU__NEW_TAB_MENU_HANDLING_H
#define UI__NEW_TAB_MENU__NEW_TAB_MENU_HANDLING_H

struct States;
struct NewTabMenu;
struct TabBar;
struct GraphicStuff;
struct Input;

void new_tab_menu_open(States &states, NewTabMenu &new_tab_menu);

void new_tab_menu_close(States &states);

void new_tab_menu_handling(States &states, NewTabMenu &new_tab_menu,
	TabBar &tab_bar, GraphicStuff &gs, const Input &input);

#endif
