#include "states.h"

namespace {

bool opening(const States &states) {
	return states.file_picker_opening || states.new_tab_menu_opening ||
		states.resize_menu_opening || states.app_menu_opening;
}

}

void states_update(States &states) {
	if (states.menu_open && !opening(states)) {
		states.menu_open = false;
		states.menu_just_close = true;
		return;
	}

	states.menu_just_close = false;

	if (opening(states)) {
		states.menu_open = true;
	}
}
