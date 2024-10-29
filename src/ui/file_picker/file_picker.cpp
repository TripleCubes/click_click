#include "file_picker.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <filesystem>
#include <algorithm>
#include <cmath>

#include "../../tab/tab.h"
#include "../../graphic_types/graphic_types.h"
#include "../../input.h"
#include "../../game_time.h"
#include "../../graphic_types/framebuffer.h"
#include "../../graphic/graphic.h"
#include "../../graphic/draw_texture.h"
#include "../../graphic/draw_text.h"
#include "../../types/vec2.h"
#include "../../types/vec2i.h"
#include "../../consts.h"
#include "../../file/file.h"
#include "../../file/open_project.h"
#include "../../file/file_extension.h"
#include "../../file/save_png.h"

// TEST
#include <iostream>

namespace {

const std::vector<std::string> SYSTEM_PATH_LIST = {
	"$RECYCLE.BIN",
	"System Volume Information",

	"$Recycle.Bin",
	"$WinREAgent",
	"Documents and Settings",
	"Intel",
	"Program Files",
	"Program Files (x86)",
	"ProgramData",
	"Recovery",
//	"Users",
	"Windows",
};
#ifndef __EMSCRIPTEN__
const char SLASH = '\\';
#else
const char SLASH = '/';
#endif
const Vec2 SIDE_BTN_SZ = vec2_new(80, 12);
const Vec2 VIEW_BTN_SZ = vec2_new(28, 12);
const Vec2 SAVE_FORMAT_BTN_SZ = vec2_new(32, 12);
const Vec2 PNG_SCALE_BTN_SZ = vec2_new(16, 12);
const float PNG_SCALE_BTN_PADDING_X = 98;
const Vec2 SAVE_BTN_SZ = vec2_new(24, 12);
const Vec2 SIDE_PADDING = vec2_new(4, 4);
const float W = 320;
const float H = 220;

void rm_parent_path(std::string &path) {
	int last_slash_pos = 0;

	for (int i = 0; i < (int)path.length(); i++) {
		if (path[i] == SLASH) {
			last_slash_pos = i;
		}
	}

	path.erase(0, last_slash_pos + 1);
}

std::string trim_path(const std::string &path, float max_w,
bool trailing_after) {
	int max_sz = max_w / 4 - 6;

	if ((int)path.length() > max_sz) {
		std::string new_path = path;
		if (trailing_after) {
			new_path.erase(max_sz);
			return new_path + "...";
		}
		else {
			new_path.erase(0, path.length() - max_sz);
			return "..." + new_path;
		}
	}

	return path;
}

bool path_list_equals(const std::vector<std::string> &list_0,
const std::vector<std::string> &list_1) {
	if (list_0.size() != list_1.size()) {
		return false;
	}

	for (int i = 0; i < (int)list_0.size(); i++) {
		if (list_0[i] != list_1[i]) {
			return false;
		}
	}

	return true;
}

#ifndef __EMSCRIPTEN__
void side_item_add(std::vector<FilePickerSideItem> &item_list,
const std::vector<std::string> &path_list, const std::string &label,
bool pin_btn) {
	FilePickerSideItem new_item;
	item_list.push_back(new_item);

	FilePickerSideItem &item = item_list.back();

	item.path_list = path_list;
	if (!pin_btn) {
		item.btn_pair.btn
			= btn_new(vec2_new(0, 0), SIDE_BTN_SZ, BTN_TEXTAREA_COLOR, label);
	}
	else {
		item.btn_pair.btn = btn_new(
			vec2_new(0, 0),
			vec2_add(SIDE_BTN_SZ, vec2_new(-12, 0)),
			BTN_TEXTAREA_COLOR,
			trim_path(label, SIDE_BTN_SZ.x - 8, true)
		);
		item.btn_pair.sec_btn_used = true;
		item.btn_pair.btn_1 = btn_new(
			vec2_new(SIDE_BTN_SZ.x - 12, 0),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			"ICON_PIN"
		);
	}
}
#endif

void draw_path_bar(const FilePicker &file_picker, GraphicStuff &gs, Vec2 pos) {
	std::string path_str;

	for (int i = 0; i < (int)file_picker.current_path_list.size(); i++) {
		const std::string &str = file_picker.current_path_list[i];

		path_str += str;
		path_str += SLASH;
	}

	#ifndef __EMSCRIPTEN__
	if (path_str == "") {
		path_str = "recent";
	}
	#endif

	draw_text(
		gs,
		trim_path(path_str, W - SIDE_PADDING.x * 2 - 12 - 8, false),
		vec2_add(pos, vec2_new(SIDE_PADDING.x + 13 + 3, SIDE_PADDING.y + 3)),
		W - 18 - SIDE_PADDING.x * 2,
		1,
		BTN_TEXTAREA_COLOR,
		vec2_new(4, 3),
		false
	);
}

bool is_system_path(const std::string &path) {
	for (int i = 0; i < (int)SYSTEM_PATH_LIST.size(); i++) {
		if (path == SYSTEM_PATH_LIST[i]) {
			return true;
		}
	}

	return false;
}

void update_folder_file_list(
std::vector<FilePickerFolderFile> &folder_file_list,
const std::vector<std::string> &current_path_list) {
	folder_file_list.clear();

	if (current_path_list.size() == 0) {
		return;
	}

	std::string path_str;
	for (int i = 0; i < (int)current_path_list.size(); i++) {
		const std::string &str = current_path_list[i];
		
		path_str += str;
		path_str += SLASH;
	}

	namespace fs = std::filesystem;
	for (const auto &f: fs::directory_iterator(path_str)) {
		FilePickerFolderFile item;
		item.name = f.path().string();
		rm_parent_path(item.name);

		if (f.is_directory()) {
			item.is_folder = true;
		}
		else {
			item.is_folder = false;
		}

		if (!item.is_folder && !f.is_regular_file()) {
			continue;
		}

		if (is_system_path(item.name)) {
			continue;
		}

		folder_file_list.push_back(item);
	}

	std::sort(folder_file_list.begin(), folder_file_list.end(),
		[](const FilePickerFolderFile &a, const FilePickerFolderFile &b) {
			if (a.is_folder && !b.is_folder) {
				return true;
			}

			else if (!a.is_folder && b.is_folder) {
				return false;
			}

			return a.name < b.name;
		}
	);
}

void update_folder_file_btn_list(
std::vector<FilePickerBtnPair> &folder_file_btn_list,
const std::vector<FilePickerFolderFile> &folder_file_list) {
	folder_file_btn_list.clear();

	for (int i = 0; i < (int)folder_file_list.size(); i++) {
		const FilePickerFolderFile &folder_file = folder_file_list[i];

		FilePickerBtnPair btn_pair;

		const float BTN_0_W = W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x - 24;
		btn_pair.btn = btn_new(
			vec2_new(12, 0),
			vec2_new(BTN_0_W, 12),
			BTN_TEXTAREA_COLOR,
			trim_path(folder_file.name, BTN_0_W, true)
		);

		if (folder_file.is_folder) {
			btn_pair.sec_btn_used = true;
			btn_pair.btn_1 = btn_new(
				vec2_new(W - SIDE_PADDING.x - SIDE_BTN_SZ.x - 16, 0),
				vec2_new(12, 12),
				BTN_TEXTAREA_COLOR,
				"ICON_PIN"
			);
		}

		folder_file_btn_list.push_back(btn_pair);
	}
}

#ifdef __EMSCRIPTEN__
void update_folder_file_list_web(
std::vector<FilePickerFolderFile> &folder_file_list) {
	namespace fs = std::filesystem;

	folder_file_list.clear();

	for (const auto &f: fs::directory_iterator(WEB_DATA_DIR)) {
		FilePickerFolderFile folder_file;
		folder_file.name = f.path().string();
		rm_parent_path(folder_file.name);

		folder_file_list.push_back(folder_file);
	}
}

void update_folder_file_btn_list_web(
std::vector<FilePickerBtnPair> &folder_file_btn_list,
const std::vector<FilePickerFolderFile> &folder_file_list) {
	folder_file_btn_list.clear();

	for (int i = 0; i < (int)folder_file_list.size(); i++) {
		const FilePickerFolderFile &folder_file = folder_file_list[i];

		FilePickerBtnPair btn_pair;

		const float BTN_0_W = W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x
		                    - SAVE_FORMAT_BTN_SZ.x * 2 - 36;
		btn_pair.btn = btn_new(
			vec2_new(12, 0),
			vec2_new(BTN_0_W, 12),
			BTN_TEXTAREA_COLOR,
			trim_path(folder_file.name, BTN_0_W, true)
		);

		btn_pair.sec_btn_used = true;
		btn_pair.btn_1 = btn_new(
			vec2_new(W - SIDE_PADDING.x - SIDE_BTN_SZ.x - 16, 0),
			vec2_new(12, 12),
			BTN_TEXTAREA_COLOR,
			"ICON_X"
		);

		btn_pair.third_fourth_btn_used = true;
		btn_pair.btn_2 = btn_new(
			vec2_new(W - SIDE_PADDING.x - SIDE_BTN_SZ.x - 48, 0),
			SAVE_FORMAT_BTN_SZ,
			BTN_TEXTAREA_COLOR,
			DOT_PNG
		);
		btn_pair.btn_3 = btn_new(
			vec2_new(W - SIDE_PADDING.x - SIDE_BTN_SZ.x
				- 48 - SAVE_FORMAT_BTN_SZ.x, 0),
			SAVE_FORMAT_BTN_SZ,
			BTN_TEXTAREA_COLOR,
			DOT_CLICK
		);

		folder_file_btn_list.push_back(btn_pair);
	}
}
#endif

#ifndef __EMSCRIPTEN__
int pinned_i(const FilePicker &file_picker, const std::string &folder_name) {
	std::vector<std::string> path_list = file_picker.current_path_list;
	path_list.push_back(folder_name);

	for (int i = 0; i < (int)file_picker.pinned_folder_list.size(); i++) {
		const FilePickerSideItem &item = file_picker.pinned_folder_list[i];
		if (path_list_equals(item.path_list, path_list)) {
			return i;
		}
	}

	return -1;
}
#endif

}

void file_picker_init(FilePicker &file_picker) {
	file_picker.close_btn = btn_new(
		vec2_new(W - 11 - SIDE_PADDING.x, SIDE_PADDING.y),
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_X"
	);

	file_picker.up_btn = btn_new(
		SIDE_PADDING,
		vec2_new(12, 12),
		BTN_TEXTAREA_COLOR,
		"ICON_ARROW_UP"
	);

	file_picker.recent_btn = btn_new(
		vec2_new(SIDE_PADDING.x, SIDE_PADDING.y + 13),
		SIDE_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		#ifndef __EMSCRIPTEN__
		"recent"
		#else
		"files"
		#endif
	);

	file_picker.list_view_btn = btn_new(
		vec2_new(W - SIDE_PADDING.x - VIEW_BTN_SZ.x * 2 + 1,
		         H - SIDE_PADDING.y - VIEW_BTN_SZ.y - 12),
		VIEW_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"list"
	);

	file_picker.image_view_btn = btn_new(
		vec2_new(W - SIDE_PADDING.x - VIEW_BTN_SZ.x + 1,
		          H - SIDE_PADDING.y - VIEW_BTN_SZ.y - 12),
		VIEW_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"image"
	);

	file_picker.save_name_textarea = textarea_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1,
		         H - SIDE_PADDING.y - 11),
		vec2_new(W - SAVE_BTN_SZ.x - SIDE_PADDING.x * 2
		         - SIDE_BTN_SZ.x, 12),
		BTN_TEXTAREA_COLOR,
		"file_name"
	);

	file_picker.save_btn = btn_new(
		vec2_new(W - SAVE_BTN_SZ.x - SIDE_PADDING.x + 1,
		         H - SIDE_PADDING.y - 11),
		SAVE_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		"save"
	);

	file_picker.project_save_btn = btn_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1,
		         H - SIDE_PADDING.y - 10 - 14),
		SAVE_FORMAT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		DOT_CLICK
	);

	#ifndef __EMSCRIPTEN__
	file_picker.png_save_btn = btn_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + SAVE_FORMAT_BTN_SZ.x + 1,
		         H - SIDE_PADDING.y - 10 - 14),
		SAVE_FORMAT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		DOT_PNG
	);
	#endif

	const int PNG_SCALE_COUNT = 4;
	for (int i = 0; i < PNG_SCALE_COUNT; i++) {
		Btn btn;
		file_picker.png_save_scale_btn_list.push_back(btn);
		file_picker.png_save_scale_btn_list[i] = btn_new(
			#ifndef __EMSCRIPTEN__
			vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x
			                        + SAVE_FORMAT_BTN_SZ.x * 2 + 1
			                        + PNG_SCALE_BTN_SZ.x * i,
			         H - SIDE_PADDING.y - 10 - 14),
			#else
			vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1
			         + PNG_SCALE_BTN_SZ.x * i + 70 + PNG_SCALE_BTN_PADDING_X,
				H - SIDE_PADDING.y - 25),
			#endif
			PNG_SCALE_BTN_SZ,
			BTN_TEXTAREA_COLOR,
			"x" + std::to_string(i_to_px_scale(i))
		);
	}

	#ifdef __EMSCRIPTEN__
	file_picker.upload_btn = btn_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1, H - SIDE_PADDING.y - 11),
		vec2_new(33, 12),
		BTN_TEXTAREA_COLOR,
		"upload"
	);
	#endif

	#ifndef __EMSCRIPTEN__
	if (std::filesystem::exists("C:\\")) {
		std::vector<std::string> path_list = {
			"C:"
		};
		side_item_add(
			file_picker.quick_access_folder_list,
			path_list,
			"C:",
			false
		);
	}
	if (std::filesystem::exists("D:\\")) {
		std::vector<std::string> path_list = {
			"D:"
		};
		side_item_add(
			file_picker.quick_access_folder_list,
			path_list,
			"D:",
			false
		);
	}
	if (std::filesystem::exists("E:\\")) {
		std::vector<std::string> path_list = {
			"E:"
		};
		side_item_add(
			file_picker.quick_access_folder_list,
			path_list,
			"E:",
			false
		);
	}
	#endif
}

void file_picker_update(FilePicker &file_picker, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_update(file_picker.close_btn, gs, input, pos, show);
	#ifndef __EMSCRIPTEN__
	btn_update(file_picker.recent_btn, gs, input, pos,
		!file_picker.is_save_picker);
	btn_update(file_picker.up_btn, gs, input, pos, show);
	#else
	btn_update(file_picker.up_btn, gs, input, pos, false);
	#endif

	btn_update(file_picker.list_view_btn, gs, input,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)), show);
	btn_update(file_picker.image_view_btn, gs, input,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)), show);

	bool show_1 = show && file_picker.is_save_picker;
	textarea_update(file_picker.save_name_textarea, gs, game_time, input, pos,
		file_picker.file_name_editing, show_1);
	btn_update(file_picker.save_btn, gs, input, pos, show_1);
	btn_update(file_picker.project_save_btn, gs, input, pos, show_1);
	#ifndef __EMSCRIPTEN__
	btn_update(file_picker.png_save_btn, gs, input, pos, show_1);
	#endif

	#ifndef __EMSCRIPTEN__
	for (int i = 0; i < (int)file_picker.png_save_scale_btn_list.size(); i++) {
		Btn &btn = file_picker.png_save_scale_btn_list[i];
		btn_update(btn, gs, input, pos, show_1
			&& !file_picker.is_dot_click_save);
	}
	#else
	for (int i = 0; i < (int)file_picker.png_save_scale_btn_list.size(); i++) {
		Btn &btn = file_picker.png_save_scale_btn_list[i];
		Vec2 pos_1 = vec2_new(pos.x,
		                      pos.y - (file_picker.is_save_picker? 12 : 0));
		btn_update(btn, gs, input, pos_1, show);
	}
	#endif

	#ifdef __EMSCRIPTEN__
	bool show_2 = show && !file_picker.is_save_picker;
	btn_update(file_picker.upload_btn, gs, input, pos, show_2);
	#endif

	Vec2 pos_1 = vec2_add(SIDE_PADDING, pos);
	pos_1.y += 25;
	for (int i = 0; i < (int)file_picker.pinned_folder_list.size(); i++) {
		FilePickerSideItem &item = file_picker.pinned_folder_list[i];
		btn_update(item.btn_pair.btn, gs, input, pos_1, show);
		if (item.btn_pair.sec_btn_used) {
			btn_update(item.btn_pair.btn_1, gs, input, pos_1, show);
		}
		pos_1.y += 12;
	}
	for (int i = 0; i < (int)file_picker.quick_access_folder_list.size(); i++) {
		FilePickerSideItem &item = file_picker.quick_access_folder_list[i];
		btn_update(item.btn_pair.btn, gs, input, pos_1, show);
		if (item.btn_pair.sec_btn_used) {
			btn_update(item.btn_pair.btn_1, gs, input, pos_1, show);
		}
		pos_1.y += 12;
	}

	Vec2 pos_2 = vec2_add(pos, vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1,
	                                    SIDE_PADDING.y + 13));
	for (int i = 0; i < (int)file_picker.folder_file_btn_list.size(); i++) {
		FilePickerBtnPair &btn_pair = file_picker.folder_file_btn_list[i];
		btn_update(btn_pair.btn, gs, input, pos_2, show);
		
		if (btn_pair.sec_btn_used) {
			btn_update(btn_pair.btn_1, gs, input, pos_2, show);
		}

		#ifdef __EMSCRIPTEN__
		if (btn_pair.third_fourth_btn_used) {
			btn_update(btn_pair.btn_2, gs, input, pos_2, show);
			btn_update(btn_pair.btn_3, gs, input, pos_2, show);
		}
		#endif

		pos_2.y += 12;
	}


	if (!show) {
		return;
	}


	auto update = [&file_picker]() {
		file_picker_file_btn_list_update(file_picker);
	};

	if (file_picker.up_btn.clicked
	&& file_picker.current_path_list.size() > 1) {
		file_picker.current_path_list.pop_back();
		update();
	}

	#ifndef __EMSCRIPTEN__
	if (file_picker.recent_btn.clicked) {
		file_picker.current_path_list.clear();
		update();
	}
	#endif

	if (file_picker.list_view_btn.clicked) {
		file_picker.is_image_view = false;
	}
	if (file_picker.image_view_btn.clicked) {
		file_picker.is_image_view = true;
	}

	if (file_picker.png_save_btn.clicked) {
		file_picker.is_dot_click_save = false;
	}
	if (file_picker.project_save_btn.clicked) {
		file_picker.is_dot_click_save = true;
	}
	for (int i = 0; i < (int)file_picker.png_save_scale_btn_list.size(); i++) {
		Btn &btn = file_picker.png_save_scale_btn_list[i];
		if (btn.clicked) {
			file_picker.png_save_scale_selected_index = i;
		}
	}

	if (file_picker.is_save_picker) {
		if (input.left_click || input.key_list[KEY_ENTER].press
		|| map_press(input, MAP_ESC)) {
			file_picker.file_name_editing = false;
		}
		if (file_picker.save_name_textarea.clicked) {
			file_picker.file_name_editing = true;
		}
		if (file_picker.save_name_textarea.hovered) {
			gs.cursor_icon = CURSOR_TEXT;
		}
	}


	for (int i = (int)file_picker.pinned_folder_list.size() - 1; i >= 0; i--) {
		FilePickerSideItem &item = file_picker.pinned_folder_list[i];
		if (item.btn_pair.btn.clicked) {
			file_picker.current_path_list = item.path_list;
			update();
		}
		if (item.btn_pair.btn_1.clicked) {
			file_picker.pinned_folder_list.erase(
				file_picker.pinned_folder_list.begin() + i
			);
		}
	}
	for (int i = 0; i < (int)file_picker.quick_access_folder_list.size(); i++) {
		FilePickerSideItem &item = file_picker.quick_access_folder_list[i];
		if (item.btn_pair.btn.clicked) {
			file_picker.current_path_list = item.path_list;
			update();
		}
	}

	for (int i = 0; i < (int)file_picker.folder_file_btn_list.size(); i++) {
		const FilePickerBtnPair &btn_pair = file_picker.folder_file_btn_list[i];
		const FilePickerFolderFile &folder_file
			= file_picker.folder_file_list[i];

		if (btn_pair.btn.clicked && folder_file.is_folder) {
			file_picker.current_path_list.push_back(folder_file.name);
			update();
		}

		if (btn_pair.btn.clicked && !folder_file.is_folder
		&& file_picker.is_save_picker && file_picker.is_dot_click_save
		&& is_dot_click(folder_file.name)) {
			file_picker.save_name_textarea.text = folder_file.name;
			rm_extension(file_picker.save_name_textarea.text);
		}

		if (btn_pair.btn.clicked && !folder_file.is_folder
		&& file_picker.is_save_picker && !file_picker.is_dot_click_save
		&& is_dot_png(folder_file.name)) {
			file_picker.save_name_textarea.text = folder_file.name;
			rm_extension(file_picker.save_name_textarea.text);
		}

		#ifndef __EMSCRIPTEN__
		if (btn_pair.btn_1.clicked) {
			int index = pinned_i(file_picker, folder_file.name);
			if (index == -1) {
				std::vector<std::string> path_list
					= file_picker.current_path_list;
				path_list.push_back(folder_file.name);
				side_item_add(
					file_picker.pinned_folder_list,
					path_list,
					folder_file.name,
					true
				);
			}
			else {
				file_picker.pinned_folder_list.erase(
					file_picker.pinned_folder_list.begin() + index
				);
			}
		}
		#else
		if (btn_pair.btn_2.clicked) {
			std::string file_name = folder_file.name;
			rm_extension(file_name);
			std::string file_name_dot_click = file_name + DOT_CLICK;
			std::string file_name_dot_png = file_name + DOT_PNG;

			OpenProjectData open_project_data;
			file_to_project_data(open_project_data, WEB_DATA_DIR
			                                      + file_name_dot_click);
			web_save_png_from_open_project_data_no_syncfs(
				WEB_DATA_DIR + file_name_dot_png,
				open_project_data,
				i_to_px_scale(file_picker.png_save_scale_selected_index)
			);
			web_download_bin_file(WEB_DATA_DIR + file_name_dot_png,
				file_name_dot_png);
			EM_ASM({
				FS.unlink(UTF8ToString($0) + UTF8ToString($1));
				FS.syncfs(function(err) {
					if (err) {
						console.log(err);
					};
				});
			}, WEB_DATA_DIR.c_str(), file_name_dot_png.c_str());
		}
		if (btn_pair.btn_3.clicked) {
			web_download_file(WEB_DATA_DIR + folder_file.name,
				folder_file.name);
		}
		#endif
	}

	#ifdef __EMSCRIPTEN__
	if (file_picker.upload_btn.clicked) {
		EM_ASM(
			file_input.click();
		);
	}

	bool file_picked = EM_ASM_INT(
		return file_picked;
	);

	if (file_picked == 1) {
		EM_ASM(
			file_picked = 0;
		);
		file_picker_web_file_btn_list_update(file_picker);
	}
	#endif

	gs.draw_secondlayer_ui = true;
}

void file_picker_bkg_draw(GraphicStuff &gs, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	auto draw = [&gs, main_fb_sz, parent_pos](Vec2 pos, Vec2 sz) -> void {
		draw_texture(
			gs,
			main_fb_sz,
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			vec2_add(parent_pos, pos),
			vec2_add(parent_pos, sz),
			color_new(0, 0, 0, 0),
			false
		);
	};

	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
	draw(vec2_new(X, Y), vec2_new(W, H));
}

void file_picker_ui_draw(const FilePicker &file_picker, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = std::floor((main_fb_sz.x - W) / 2);
	const float Y = std::floor((main_fb_sz.y - H) / 2);
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_draw(file_picker.close_btn, gs, pos, false);
	#ifndef __EMSCRIPTEN__
	btn_draw(file_picker.up_btn, gs, pos, false);
	#endif
	draw_path_bar(file_picker, gs, pos);
	
	#ifndef __EMSCRIPTEN__
	if (!file_picker.is_save_picker) {
		btn_draw(file_picker.recent_btn, gs, pos,
			file_picker.current_path_list.size() == 0);
	}
	#endif

	btn_draw(file_picker.list_view_btn, gs,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)),
		!file_picker.is_image_view);
	btn_draw(file_picker.image_view_btn, gs,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)),
		file_picker.is_image_view);

	if (file_picker.is_save_picker) {
		textarea_draw(file_picker.save_name_textarea, gs, game_time, pos,
			file_picker.file_name_editing, true);
		btn_draw(file_picker.save_btn, gs, pos, false);
		btn_draw(file_picker.project_save_btn, gs, pos,
			file_picker.is_dot_click_save);
		#ifndef __EMSCRIPTEN__
		btn_draw(file_picker.png_save_btn, gs, pos,
			!file_picker.is_dot_click_save);
		#endif

		if (!file_picker.is_dot_click_save) {
			for (int i = 0;i < (int)file_picker.png_save_scale_btn_list.size();
			i++) {
				const Btn &btn = file_picker.png_save_scale_btn_list[i];
				btn_draw(btn, gs, pos,
					file_picker.png_save_scale_selected_index == i);
			}
		}
	}
	#ifdef __EMSCRIPTEN__
	else {
		btn_draw(file_picker.upload_btn, gs, pos, false);
	}

	for (int i = 0;i < (int)file_picker.png_save_scale_btn_list.size(); i++) {
		const Btn &btn = file_picker.png_save_scale_btn_list[i];
		Vec2 pos_1 = vec2_new(pos.x,
		                      pos.y - (file_picker.is_save_picker? 12 : 0));
		btn_draw(btn, gs, pos_1,
			file_picker.png_save_scale_selected_index == i);
	}
	#endif

	Vec2 pos_1 = vec2_add(SIDE_PADDING, pos);
	pos_1.y += 25;
	for (int i = 0; i < (int)file_picker.pinned_folder_list.size(); i++) {
		const FilePickerSideItem &item = file_picker.pinned_folder_list[i];
		btn_draw(
			item.btn_pair.btn, gs, pos_1,
			path_list_equals(item.path_list, file_picker.current_path_list)
		);
		if (item.btn_pair.sec_btn_used) {
			btn_draw(item.btn_pair.btn_1, gs, pos_1, true);
		}
		pos_1.y += 12;
	}
	for (int i = 0; i < (int)file_picker.quick_access_folder_list.size(); i++) {
		const FilePickerSideItem &item
			= file_picker.quick_access_folder_list[i];
		btn_draw(
			item.btn_pair.btn, gs, pos_1,
			path_list_equals(item.path_list, file_picker.current_path_list)
		);
		if (item.btn_pair.sec_btn_used) {
			btn_draw(item.btn_pair.btn_1, gs, pos_1, true);
		}
		pos_1.y += 12;
	}

	Vec2 pos_2 = vec2_add(pos, vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1,
	                                    SIDE_PADDING.y + 13));
	for (int i = 0; i < (int)file_picker.folder_file_btn_list.size(); i++) {
		const FilePickerFolderFile &folder_file
			= file_picker.folder_file_list[i];
		const FilePickerBtnPair &btn_pair= file_picker.folder_file_btn_list[i];
		
		btn_draw(btn_pair.btn, gs, pos_2, false);

		if (btn_pair.sec_btn_used) {
			btn_draw(
				btn_pair.btn_1,
				gs,
				pos_2,
				#ifndef __EMSCRIPTEN__
				pinned_i(file_picker, folder_file.name) != -1
				#else
				false
				#endif
			);
		}

		#ifdef __EMSCRIPTEN__
		if (btn_pair.third_fourth_btn_used) {
			btn_draw(
				btn_pair.btn_2,
				gs,
				pos_2,
				false
			);
			btn_draw(
				btn_pair.btn_3,
				gs,
				pos_2,
				false
			);
		}
	
		draw_icon(
			gs,
			ICON_DOWNLOAD,
			vec2_add(pos_2, vec2_new(147, 3)),
			1,
			KEY_HINT_COLOR,
			vec2_new(3, 3),
			false
		);
		#endif

		int icon = folder_file.is_folder? ICON_FOLDER : ICON_FILE;
		draw_icon(
			gs,
			icon,
			vec2_add(pos_2, vec2_new(3, 3)),
			1,
			#ifndef __EMSCRIPTEN__
			BTN_TEXTAREA_COLOR,
			#else
			KEY_HINT_COLOR,
			#endif
			vec2_new(3, 3),
			false
		);

		pos_2.y += 12;
	}

	#ifdef __EMSCRIPTEN__
	if (file_picker.folder_file_btn_list.size() == 0) {
		draw_text(
			gs,
			"um... empty...",
			vec2_new(X + SIDE_PADDING.x + SIDE_BTN_SZ.x + 5,
			         Y + SIDE_PADDING.y + 13 + 3),
			W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x - 8,
			1,
			KEY_HINT_COLOR,
			vec2_new(4, 3),
			false
		);
	}

	draw_text(
		gs,
		"png download sz:",
		vec2_new(X + SIDE_PADDING.x + SIDE_BTN_SZ.x + 6
		           + PNG_SCALE_BTN_PADDING_X,
		         Y + H - SIDE_PADDING.y - 25 + 3
		           - (file_picker.is_save_picker? 12 : 0)),
		W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x - 8 + 4,
		1,
		KEY_HINT_COLOR,
		vec2_new(4, 3),
		false
	);
	#endif
}

void file_picker_get_save_path(std::string &save_name, std::string &save_path,
const FilePicker &file_picker) {
	#ifndef __EMSCRIPTEN__
	
	save_name = file_picker.save_name_textarea.text;

	for (int i = 0; i < (int)file_picker.current_path_list.size(); i++) {
		save_path += file_picker.current_path_list[i] + SLASH;
	}

	save_path += save_name;

	if (file_picker.is_dot_click_save) {
		save_path += DOT_CLICK;
	}
	else {
		save_path += DOT_PNG;
	}

	#else
	save_name = file_picker.save_name_textarea.text;
	save_path = WEB_DATA_DIR + save_name + DOT_CLICK;
	#endif
}

void file_picker_open_file(std::string &file_name, std::string &file_path,
const FilePicker &file_picker) {
	if (file_picker.is_save_picker) {
		return;
	}

	#ifndef __EMSCRIPTEN__
	auto get_file_path = [&file_picker](std::string &file_path, int index) {
		const FilePickerFolderFile &folder_file
			= file_picker.folder_file_list[index];

		for (int i = 0; i < (int)file_picker.current_path_list.size(); i++) {
			file_path += file_picker.current_path_list[i] + SLASH;
		}

		file_path += folder_file.name;
	};
	#endif

	for (int i = 0; i < (int)file_picker.folder_file_btn_list.size(); i++) {
		const FilePickerBtnPair &btn_pair
			= file_picker.folder_file_btn_list[i];
		const FilePickerFolderFile &folder_file
			= file_picker.folder_file_list[i];

		if (!folder_file.is_folder && btn_pair.btn.clicked
		&& is_dot_click(folder_file.name)) {
			#ifndef __EMSCRIPTEN__
			get_file_path(file_path, i);
			#else
			file_path = WEB_DATA_DIR;
			file_path += folder_file.name;
			#endif

			file_name = folder_file.name;
			
			break;
		}
	}
}

void file_picker_file_btn_list_update(FilePicker &file_picker) {
	update_folder_file_list(file_picker.folder_file_list,
	                        file_picker.current_path_list);
	update_folder_file_btn_list(file_picker.folder_file_btn_list,
	                            file_picker.folder_file_list);
}

int i_to_px_scale(int i) {
	switch (i) {
		case 0: return 1;
		case 1: return 2;
		case 2: return 4;
		case 3: return 6;
		case 4: return 8;
		default: return 1;
	}
}

#ifdef __EMSCRIPTEN__
void file_picker_web_file_btn_list_update(FilePicker &file_picker) {
	update_folder_file_list_web(file_picker.folder_file_list);
	update_folder_file_btn_list_web(file_picker.folder_file_btn_list,
	                                file_picker.folder_file_list);
}
#endif
