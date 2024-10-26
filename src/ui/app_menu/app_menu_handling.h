#ifndef UI__APP_MENU__APP_MENU_HANDLING_H
#define UI__APP_MENU__APP_MENU_HANDLING_H

struct States;
struct AppUI;
struct Tab;
struct GraphicStuff;
struct Input;

void app_menu_handling(States &states, AppUI &app_ui, Tab &tab,
	GraphicStuff &gs, const Input &input);

#endif
