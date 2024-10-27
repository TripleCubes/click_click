#ifndef UI__FILE_PICKER__FILE_PICKER_HANDLING_H
#define UI__FILE_PICKER__FILE_PICKER_HANDLING_H

#include <string>

struct States;
struct FilePicker;
struct Tab;
struct TabBar;
struct DialogBox;
struct GraphicStuff;
struct Input;

void file_picker_type_open_file_open(States &states, FilePicker &file_picker,
	Tab &tab);

void file_picker_type_save_file_open(States &states, FilePicker &file_picker,
	Tab &tab);

void file_picker_close(States &states);

void file_picker_handling(States &states, FilePicker &file_picker,
	TabBar &tab_bar, Tab &tab, DialogBox &dialog_box,
	GraphicStuff &gs, const Input &input);

void file_picker_save_project(const FilePicker &file_picker,
	const std::string &save_path, Tab &tab);

#endif
