#include "dialog_box_handling.h"

#include "dialog_box.h"
#include "../app_ui.h"
#include "../../states.h"
#include "../../tab/tab.h"
#include "../file_picker/file_picker_handling.h"
#include "../file_picker/file_picker.h"
#include "../../consts.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

void dialog_box_handling(AppUI &app_ui, States &states, Tab &tab) {
	DialogBox &dialog_box = app_ui.dialog_box;
	FilePicker &file_picker = app_ui.file_picker;

	if (dialog_box.ok_btn.clicked) {
		#ifdef __EMSCRIPTEN__
		if (dialog_box.dialog_type == DIALOG_BOX_WEB_DELETE_FILE) {
			EM_ASM({
				FS.unlink(UTF8ToString($0) + UTF8ToString($1));
				FS.syncfs(function(err) {
					if (err) {
						console.log(err);
					};
				});
			}, WEB_DATA_DIR.c_str(), dialog_box.web_delete_file_name.c_str());
			
			file_picker_web_file_btn_list_update(file_picker);
		}

		if (dialog_box.dialog_type == DIALOG_BOX_WEB_FIRST_RAN_NOTICE) {
			EM_ASM(
				set_first_ran_notice_okay();
			);
		}
		#endif

		if (dialog_box.dialog_type == DIALOG_BOX_OVERRIDE_FILE) {
			file_picker_save_tab_path_assign(
				dialog_box.override_file_name,
				dialog_box.override_file_path,
				tab,
				file_picker.png_save_scale_selected_index
			);

			file_picker_close(states);
		}
	}

	if (dialog_box.ok_btn.clicked || dialog_box.cancel_btn.clicked) {
		states.dialog_box_opening = false;
	}
}
