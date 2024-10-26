#include "file_picker_handling.h"

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
#include "../../types/vec2.h"
#include "../../types/vec2i.h"

namespace {

void _tab_new(TabBar &tab_bar, GraphicStuff &gs, const OpenProjectData &data) {
	tab_bar_tab_new(tab_bar, gs, tab_bar.order_index + 1,
		data.pos, data.sz, data.px_scale);
	tab_bar.order_index++;

	int index = tab_bar.tab_order_list[tab_bar.order_index];
	Tab &tab = tab_bar.tab_list[index];

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
	
	#ifdef __EMSCRIPTEN__
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

	#ifdef __EMSCRIPTEN__
	file_picker_web_file_btn_list_update(file_picker);
	#endif
}

void file_picker_close(States &states) {
	states.file_picker_opening = false;
}

void file_picker_handling(States &states, FilePicker &file_picker,
TabBar &tab_bar, Tab &tab, DialogBox &dialog_box,
GraphicStuff &gs, const Input &input) {
	if (!menu_opening(states) && map_press(input, MAP_OPEN_FILE)) {
		file_picker_type_open_file_open(states, file_picker, tab);
	}

	if (!menu_opening(states) && map_press(input, MAP_SAVE_FILE)) {
		file_picker_type_save_file_open(states, file_picker, tab);
	}

	if (states.file_picker_opening
	&& (file_picker.close_btn.clicked || map_press(input, MAP_ESC))) {
		file_picker_close(states);
	}

	if (states.file_picker_opening && file_picker.is_save_picker
	&& (file_picker.save_btn.clicked || map_press(input, MAP_ENTER))) {
		states.file_picker_opening = false;

		std::string save_link;
		file_picker_get_save_link(save_link, file_picker);
		
		save_project(save_link, tab);

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

	if (states.file_picker_opening) {
		std::string open_path;
		file_picker_open_file(open_path, file_picker);

		if (open_path.length() != 0) {
			states.file_picker_opening = false;

			OpenProjectData data;
			if (file_to_project_data(data, open_path)) {
				_tab_new(tab_bar, gs, data);
			}
			else {
				std::cout << "error opening file " << open_path << std::endl;
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
