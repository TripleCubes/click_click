#ifndef UI__FILE_PICKER__FILE_PICKER_H
#define UI__FILE_PICKER__FILE_PICKER_H

#include "../btn.h"
#include "../textarea.h"
#include <vector>
#include <string>

struct Tab;
struct GraphicStuff;
struct Input;
struct GameTime;
struct Vec2;

struct FilePickerBtnPair {
	Btn btn;
	bool sec_btn_used = false;
	Btn btn_1;

	#ifdef __EMSCRIPTEN__
	bool third_btn_used = false;
	Btn btn_2;
	#endif
};

struct FilePickerSideItem {
	FilePickerBtnPair btn_pair;
	std::vector<std::string> path_list;
};

struct FilePickerFolderFile {
	std::string name;
	bool is_folder = false;
};

struct FilePicker {
	bool is_image_view = false;
	bool is_dot_click_save = true;
	bool is_save_picker = false;
	bool file_name_editing = false;
	
	Btn close_btn;

	Btn up_btn;
	Btn recent_btn;
	Btn list_view_btn;
	Btn image_view_btn;

	TextArea save_name_textarea;
	Btn save_btn;
	Btn project_save_btn;
	Btn png_save_btn;
	std::vector<Btn> png_save_scale_btn_list;
	int png_save_scale_selected_index = 0;
	#ifdef __EMSCRIPTEN__
	Btn upload_btn;
	#endif

	std::vector<FilePickerSideItem> pinned_folder_list;
	std::vector<FilePickerSideItem> quick_access_folder_list;
	std::vector<std::string> current_path_list;
	std::vector<FilePickerFolderFile> folder_file_list;
	std::vector<FilePickerBtnPair> folder_file_btn_list;
};

void file_picker_init(FilePicker &file_picker);

void file_picker_update(FilePicker &file_picker, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show);

void file_picker_bkg_draw(GraphicStuff &gs, Vec2 parent_pos);

void file_picker_ui_draw(const FilePicker &file_picker, GraphicStuff &gs,
	const Input &input, const GameTime &game_time, Vec2 parent_pos);

void file_picker_get_save_path(std::string &save_name, std::string &save_path,
	const FilePicker &file_picker);

void file_picker_open_file(std::string &file_name, std::string &file_path,
	const FilePicker &file_picker);

void file_picker_file_btn_list_update(FilePicker &file_picker);

int i_to_px_scale(int i);

#ifdef __EMSCRIPTEN__
void file_picker_web_file_btn_list_update(FilePicker &file_picker);
#endif

#endif
