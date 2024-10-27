#include "file_picker_handling.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <iostream>

#include "file_picker.h"
#include "../../states.h"
#include "../../input.h"
#include "../../tab/tab.h"
#include "../tab_bar/tab_bar.h"
#include "../dialog_box/dialog_box.h"
#include "../../graphic_types/graphic_types.h"
#include "../../graphic_types/texture.h"
#include "../../file/open_project.h"
#include "../../file/save_project.h"
#include "../../file/save_png.h"
#include "../../types/vec2.h"
#include "../../types/vec2i.h"

namespace {

void rm_extension(std::string &str) {
	for (int i = (int)str.length() - 1; i >= 0; i--) {
		if (str[i] == '.') {
			str.pop_back();
			break;
		}

		str.pop_back();
	}
}

void _tab_new(TabBar &tab_bar, GraphicStuff &gs, const OpenProjectData &data,
const std::string &file_name, const std::string &file_path) {
	tab_bar_tab_new(tab_bar, gs, tab_bar.order_index + 1,
		data.pos, data.sz, data.px_scale);
	tab_bar.order_index++;

	int index = tab_bar.tab_order_list[tab_bar.order_index];
	Tab &tab = tab_bar.tab_list[index];

	tab.path = file_path;
	tab.name = file_name;
	rm_extension(tab.name);
	tab.btn.text = tab.name;
	tab.btn.sz.x = tab.name.length() * 4 + TAB_BTN_W_ADD;
	tab.close_btn.pos.x = tab.btn.sz.x;

	tab.pallete_data.clear();
	tab.layer_list.clear();
	tab.layer_order_list.clear();

	for (int i = 0; i < 64; i++) {
		tab.color_pallete.color_list[i].r
			= (float)data.pallete_data[i*4    ] / 255;
		tab.color_pallete.color_list[i].g
			= (float)data.pallete_data[i*4 + 1] / 255;
		tab.color_pallete.color_list[i].b
			= (float)data.pallete_data[i*4 + 2] / 255;
		tab.color_pallete.color_list[i].a
			= (float)data.pallete_data[i*4 + 3] / 255;
	}
	tab.pallete_data.insert(
		tab.pallete_data.end(),
		data.pallete_data.begin(),
		data.pallete_data.end()
	);
	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);

	for (int i = 0; i < (int)data.layer_list.size(); i++) {
		const OpenProjectLayerData &layer_data = data.layer_list[i];

		tab_layer_new_data(
			tab,
			tab.layer_order_list.size(),
			layer_data.layer_name,
			gs,
			layer_data.data
		);
	}

	int pallete_index = tab.color_pallete.selected_index;
	Color color = tab.color_pallete.color_list[pallete_index];
	color_picker_set_rgb(tab.color_picker, color);
}

}

void file_picker_type_open_file_open(States &states, FilePicker &file_picker,
Tab &tab) {
	states.file_picker_opening = true;
	file_picker.is_save_picker = false;
	tab.layer_name_editing = false;
	
	#ifndef __EMSCRIPTEN__
	file_picker_file_btn_list_update(file_picker);
	#else
	file_picker_web_file_btn_list_update(file_picker);
	#endif
}

void file_picker_type_save_file_open(States &states, FilePicker &file_picker,
Tab &tab) {
	states.file_picker_opening = true;
	
	file_picker.save_name_textarea.text
		= file_picker.save_name_textarea.defl_text;
	file_picker.is_save_picker = true;

	tab.layer_name_editing = false;

	#ifndef __EMSCRIPTEN__
	if (file_picker.current_path_list.size() == 0
	&& file_picker.quick_access_folder_list.size() > 0) {
		file_picker.current_path_list
			= file_picker.quick_access_folder_list[0].path_list;
	}
	file_picker_file_btn_list_update(file_picker);
	#else
	file_picker_web_file_btn_list_update(file_picker);
	#endif
}

void file_picker_close(States &states) {
	states.file_picker_opening = false;
}

void file_picker_save_project(const FilePicker &file_picker,
const std::string &save_path, Tab &tab) {
	save_project(save_path, tab);
	save_png(save_path + ".png", tab);

	tab.path = save_path;
	tab.name = file_picker.save_name_textarea.text;
	tab.btn.text = tab.name;
	tab.btn.sz.x = tab.name.length() * 4 + TAB_BTN_W_ADD;
	tab.close_btn.pos.x = tab.btn.sz.x;

	#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.syncfs(function(err) {
			if (err) {
				console.log(err);
			}
		});
	);
	#endif
}

void file_picker_handling(States &states, FilePicker &file_picker,
TabBar &tab_bar, Tab &tab, DialogBox &dialog_box,
GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states) && map_press(input, MAP_OPEN_FILE)) {
		file_picker_type_open_file_open(states, file_picker, tab);
	}

	if (!menu_opening(states) && map_press(input, MAP_SAVE_FILE_AS)) {
		file_picker_type_save_file_open(states, file_picker, tab);
	}

	if (!menu_opening(states) && map_press(input, MAP_SAVE_FILE)) {
		if (tab.path.length() == 0) {
			file_picker_type_save_file_open(states, file_picker, tab);
		}
		else {
			save_project(tab.path, tab);
		}
	}

	if (states.file_picker_opening
	&& (file_picker.close_btn.clicked || map_press(input, MAP_ESC))) {
		file_picker_close(states);
	}

	if (states.file_picker_opening && file_picker.is_save_picker
	&& (file_picker.save_btn.clicked || map_press(input, MAP_ENTER))) {
		if (file_picker.save_name_textarea.text.length() == 0) {
			return;
		}

		std::string save_path;
		std::string save_name;
		file_picker_get_save_path(save_name, save_path, file_picker);
		
		bool call_override_dialog = false;
		for (int i = 0; i < (int)file_picker.folder_file_list.size(); i++) {
			FilePickerFolderFile &folder_file =file_picker.folder_file_list[i];
			if (folder_file.name == save_name + ".click") {
				call_override_dialog = true;
			}
		}
		
		if (call_override_dialog) {
			states.dialog_box_opening = true;
			dialog_box.override_file_name = save_name;
			dialog_box.override_file_path = save_path;
			dialog_box_set(dialog_box, DIALOG_BOX_OVERRIDE_FILE);
		}
		else {
			file_picker_save_project(file_picker, save_path, tab);

			file_picker_close(states);
		}
	}

	if (states.file_picker_opening) {
		std::string file_name;
		std::string file_path;
		file_picker_open_file(file_name, file_path, file_picker);

		if (file_path.length() != 0) {
			file_picker_close(states);

			OpenProjectData data;
			if (file_to_project_data(data, file_path)) {
				_tab_new(tab_bar, gs, data, file_name, file_path);
			}
			else {
				std::cout << "error opening file " << file_path << std::endl;
			}
		}
	}

	#ifdef __EMSCRIPTEN__
	for (int i = 0; i < (int)file_picker.folder_file_btn_list.size(); i++) {
		FilePickerBtnPair &btn_pair = file_picker.folder_file_btn_list[i];
		FilePickerFolderFile &folder_file = file_picker.folder_file_list[i];

		if (btn_pair.btn_1.clicked) {
			states.dialog_box_opening = true;
			dialog_box.web_delete_file_name = folder_file.name;
			dialog_box_set(dialog_box, DIALOG_BOX_WEB_DELETE_FILE);
		}
	}
	#endif
}
