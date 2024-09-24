#ifndef UI__APP_UI_H
#define UI__APP_UI_H

#include "tab_bar.h"
#include "file_picker/file_picker.h"

struct GraphicStuff;

struct AppUI {
	TabBar tab_bar;
	FilePicker file_picker;
};

void app_ui_init(AppUI &app_ui, GraphicStuff &gs);

void app_ui_release(AppUI &app_ui, GraphicStuff &gs);

#endif
