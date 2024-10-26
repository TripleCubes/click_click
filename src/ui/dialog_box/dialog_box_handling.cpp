#include "dialog_box_handling.h"

#include "dialog_box.h"
#include "../app_ui.h"
#include "../../states.h"
#ifdef __EMSCRIPTEN__
#include "../file_picker/file_picker.h"

#include <emscripten.h>
#endif

void dialog_box_handling(AppUI &app_ui, States &states) {
	DialogBox &dialog_box = app_ui.dialog_box;
	#ifdef __EMSCRIPTEN__
	FilePicker &file_picker = app_ui.file_picker;
	#endif

	if (dialog_box.ok_btn.clicked) {
		#ifdef __EMSCRIPTEN__
		if (dialog_box.dialog_type == DIALOG_BOX_WEB_DELETE_FILE) {
			EM_ASM({
				FS.unlink("./data/" + UTF8ToString($0));
				FS.syncfs(function(err) {
					if (err) {
						console.log(err);
					};
				});
			}, dialog_box.web_delete_file_name.c_str());
			
			file_picker_web_file_btn_list_update(file_picker);
		}
		#endif
	}

	if (dialog_box.ok_btn.clicked || dialog_box.cancel_btn.clicked) {
		states.dialog_box_opening = false;
	}
}
