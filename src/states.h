#ifndef STATES_H
#define STATES_H

struct States {
	bool file_picker_opening = false;
	bool new_tab_menu_opening = false;
	bool resize_menu_opening = false;
	bool app_menu_opening = false;
	bool dialog_box_opening = false;

	bool menu_open = false;
	bool menu_just_close = false;
};

void states_update(States &states);

#endif
