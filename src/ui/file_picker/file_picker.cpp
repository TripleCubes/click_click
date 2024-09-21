#include "file_picker.h"

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

#include <filesystem>
#include <algorithm>

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
const char SLASH = '\\';
const Vec2 SIDE_BTN_SZ = vec2_new(80, 12);
const Vec2 VIEW_BTN_SZ = vec2_new(28, 12);
const Vec2 SAVE_FORMAT_BTN_SZ = vec2_new(32, 12);
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

void draw_path_bar(const FilePicker &file_picker, GraphicStuff &gs, Vec2 pos) {
	std::string path_str;

	for (int i = 0; i < (int)file_picker.current_path_list.size(); i++) {
		const std::string &str = file_picker.current_path_list[i];

		path_str += str;
		path_str += SLASH;
	}

	if (path_str == "") {
		path_str = "recent";
	}

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

		btn_pair.btn = btn_new(
			vec2_new(12, 0),
			vec2_new(W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x - 12 - 12, 12),
			BTN_TEXTAREA_COLOR,
			trim_path(
				folder_file.name,
				W - SIDE_PADDING.x * 2 - SIDE_BTN_SZ.x - 24,
				true)
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
		"recent"
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

	file_picker.png_save_btn = btn_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + 1,
		         H - SIDE_PADDING.y - 10 - 14),
		SAVE_FORMAT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		".png"
	);

	file_picker.project_save_btn = btn_new(
		vec2_new(SIDE_PADDING.x + SIDE_BTN_SZ.x + SAVE_FORMAT_BTN_SZ.x + 1,
		         H - SIDE_PADDING.y - 10 - 14),
		SAVE_FORMAT_BTN_SZ,
		BTN_TEXTAREA_COLOR,
		".click"
	);

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
}

void file_picker_update(FilePicker &file_picker, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos, bool show) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_update(file_picker.close_btn, gs, input, pos, show);
	btn_update(file_picker.recent_btn, gs, input, pos, show);
	btn_update(file_picker.up_btn, gs, input, pos, show);

	btn_update(file_picker.list_view_btn, gs, input,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)), show);
	btn_update(file_picker.image_view_btn, gs, input,
		vec2_new(pos.x, pos.y + (file_picker.is_save_picker? 0 : 12)), show);

	bool show_1 = show && file_picker.is_save_picker;
	textarea_update(file_picker.save_name_textarea, gs, game_time, input, pos,
		file_picker.file_name_editing, show_1);
	btn_update(file_picker.save_btn, gs, input, pos, show_1);
	btn_update(file_picker.png_save_btn, gs, input, pos, show_1);
	btn_update(file_picker.project_save_btn, gs, input, pos, show_1);

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
		pos_2.y += 12;
	}


	if (!show) {
		return;
	}


	auto update = [&file_picker]() {
		update_folder_file_list(file_picker.folder_file_list,
		                        file_picker.current_path_list);
		update_folder_file_btn_list(file_picker.folder_file_btn_list,
		                            file_picker.folder_file_list);
	};

	if (file_picker.up_btn.clicked
	&& file_picker.current_path_list.size() > 1) {
		file_picker.current_path_list.pop_back();
		update();
	}

	if (file_picker.recent_btn.clicked) {
		file_picker.current_path_list.clear();
		update();
	}

	if (file_picker.list_view_btn.clicked) {
		file_picker.is_image_view = false;
	}
	if (file_picker.image_view_btn.clicked) {
		file_picker.is_image_view = true;
	}

	if (file_picker.png_save_btn.clicked) {
		file_picker.is_project_save = false;
	}
	if (file_picker.project_save_btn.clicked) {
		file_picker.is_project_save = true;
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
	}

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

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	draw(vec2_new(X, Y), vec2_new(W, H));
}

void file_picker_ui_draw(const FilePicker &file_picker, GraphicStuff &gs,
const Input &input, const GameTime &game_time, Vec2 parent_pos) {
	Vec2i main_fb_sz = fb_get_sz(gs, FB_MAIN);

	const float X = (main_fb_sz.x - W) / 2;
	const float Y = (main_fb_sz.y - H) / 2;
	Vec2 pos = vec2_add(parent_pos, vec2_new(X, Y));

	btn_draw(file_picker.close_btn, gs, pos, false);
	btn_draw(file_picker.up_btn, gs, pos, false);
	draw_path_bar(file_picker, gs, pos);
	btn_draw(file_picker.recent_btn, gs, pos,
		file_picker.current_path_list.size() == 0);

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
		btn_draw(file_picker.png_save_btn, gs, pos,
			!file_picker.is_project_save);
		btn_draw(file_picker.project_save_btn, gs, pos,
			file_picker.is_project_save);
	}

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
				pinned_i(file_picker, folder_file.name) != -1
			);
		}

		int icon = folder_file.is_folder? ICON_FOLDER : ICON_FILE;
		draw_icon(
			gs,
			icon,
			vec2_add(pos_2, vec2_new(3, 3)),
			1,
			BTN_TEXTAREA_COLOR,
			vec2_new(3, 3),
			false
		);

		pos_2.y += 12;
	}
}
