#include "top_left_menu_handling.h"

#include "top_left_menu.h"
#include "../../states.h"
#include "../../input.h"

void top_left_menu_handling(States &states, const TopLeftMenu &top_left_menu,
const Input &input) {
	if (!menu_opening(states) && top_left_menu.hamburger_btn.clicked) {
		states.app_menu_opening = true;
	}
}
