#include "input_map.h"

#include "input.h"

namespace {

bool modifier_down(const Input &input, int modifier) {
	if (modifier == -1) {
		return true;
	}

	if (modifier == MODIFIER_LEFT_CTRL) {
		if (input.key_list[KEY_LEFT_CTRL].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_RIGHT_CTRL) {
		if (input.key_list[KEY_RIGHT_CTRL].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_BOTH_CTRL) {
		if (input.key_list[KEY_LEFT_CTRL].down
		|| input.key_list[KEY_RIGHT_CTRL].down) {
			return true;
		}
	}
	
	else if (modifier == MODIFIER_LEFT_SHIFT) {
		if (input.key_list[KEY_LEFT_SHIFT].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_RIGHT_SHIFT) {
		if (input.key_list[KEY_RIGHT_SHIFT].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_BOTH_SHIFT) {
		if (input.key_list[KEY_LEFT_SHIFT].down
		|| input.key_list[KEY_RIGHT_SHIFT].down){
			return true;
		}
	}

	else if (modifier == MODIFIER_LEFT_ALT) {
		if (input.key_list[KEY_LEFT_ALT].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_RIGHT_ALT) {
		if (input.key_list[KEY_RIGHT_ALT].down) {
			return true;
		}
	}
	else if (modifier == MODIFIER_BOTH_ALT) {
		if (input.key_list[KEY_LEFT_ALT].down
		|| input.key_list[KEY_RIGHT_ALT].down) {
			return true;
		}
	}

	return false;
}

bool modifier_match(int modifier_0, int modifier_1) {
	if (modifier_0 == modifier_1) {
		return true;
	}

	if (modifier_1 == MODIFIER_BOTH_CTRL
	|| modifier_1 == MODIFIER_BOTH_SHIFT
	|| modifier_1 == MODIFIER_BOTH_ALT) {
		if (modifier_0 <= modifier_1 && modifier_0 >= modifier_1 - 2) {
			return true;
		}
	}

	return false;
}

bool unneeded_modifier_down(const Input &input, int modifier, int sec_modifier){
	for (int i = MODIFIER_LEFT_CTRL; i <= MODIFIER_BOTH_ALT; i++) {
		if (i == MODIFIER_BOTH_CTRL
		|| i == MODIFIER_BOTH_SHIFT
		|| i == MODIFIER_BOTH_ALT) {
			continue;
		}

		if (!modifier_down(input, i)) {
			continue;
		}

		if (!modifier_match(i, modifier) && !modifier_match(i, sec_modifier)) {
			return true;
		}
	}

	return false;
}

bool modifier_cond(const Input &input, int modifier, int sec_modifier) {
	if (unneeded_modifier_down(input, modifier, sec_modifier)) {
		return false;
	}
	return modifier_down(input, modifier) && modifier_down(input, sec_modifier);
}

}

void input_map_init(InputMap &input_map) {
	auto set = [&input_map](MappedKeyIndex index, int key = -1,
	int modifier = -1, int sec_modifier = -1) {
		input_map.key_list[index].key = key;
		input_map.key_list[index].modifier = modifier;
		input_map.key_list[index].sec_modifier = sec_modifier;
	};

	set(MAP_ESC             , KEY_ESC                       );
	set(MAP_ENTER           , KEY_ENTER                     );
	set(MAP_TAB             , KEY_TAB                       );
	set(MAP_SHIFT_TAB       , KEY_TAB  , MODIFIER_BOTH_SHIFT);

	set(MAP_PAN             , KEY_SPACE                     );
	
	set(MAP_ZOOM_IN         , KEY_EQUAL, MODIFIER_BOTH_CTRL );
	set(MAP_ZOOM_OUT        , KEY_MINUS, MODIFIER_BOTH_CTRL );
	set(MAP_ZOOM_0          , KEY_0    , MODIFIER_BOTH_CTRL );

	set(MAP_TOOL_BRUSH      , KEY_B                         );
	set(MAP_TOOL_CURVE      , KEY_C                         );
	set(MAP_TOOL_FILL       , KEY_F                         );
	set(MAP_TOOL_SELECT     , KEY_S                         );
	set(MAP_TOOL_SELECT_MODE, KEY_D                         );
	set(MAP_TOOL_ERASER     , KEY_E                         );
	set(MAP_TOOL_MOVE       , KEY_M                         );

	set(MAP_COLOR_1         , KEY_1                         );
	set(MAP_COLOR_2         , KEY_2                         );
	set(MAP_COLOR_3         , KEY_3                         );
	set(MAP_COLOR_4         , KEY_4                         );
	set(MAP_COLOR_5         , KEY_5                         );
	set(MAP_COLOR_6         , KEY_6                         );
	set(MAP_COLOR_7         , KEY_7                         );
	set(MAP_COLOR_8         , KEY_8                         );

	set(MAP_COLOR_PAGE_1    , KEY_1    , MODIFIER_BOTH_SHIFT);
	set(MAP_COLOR_PAGE_2    , KEY_2    , MODIFIER_BOTH_SHIFT);
	set(MAP_COLOR_PAGE_3    , KEY_3    , MODIFIER_BOTH_SHIFT);
	set(MAP_COLOR_PAGE_4    , KEY_4    , MODIFIER_BOTH_SHIFT);

	set(MAP_SELECT_ALL      , KEY_A    , MODIFIER_BOTH_CTRL );
	set(MAP_DESELECT_ALL    , KEY_A    , MODIFIER_BOTH_SHIFT,
	                                     MODIFIER_BOTH_CTRL );

	set(MAP_OPEN_FILE       , KEY_O    , MODIFIER_BOTH_CTRL );
	set(MAP_SAVE_FILE       , KEY_S    , MODIFIER_BOTH_CTRL );
	set(MAP_SAVE_FILE_AS    , KEY_S    , MODIFIER_BOTH_CTRL ,
	                                     MODIFIER_BOTH_SHIFT);

	set(MAP_NEW_PROJECT     , KEY_N    , MODIFIER_BOTH_CTRL ,
	                                     MODIFIER_BOTH_SHIFT);
	#ifndef __EMSCRIPTEN__
	set(MAP_NEW_PROJECT_1   , KEY_N    , MODIFIER_BOTH_CTRL );
	#endif

	set(MAP_RESIZE_CANVAS   , KEY_R    , MODIFIER_BOTH_CTRL );

	set(MAP_APP_MENU        , KEY_ESC                       );

	set(MAP_UNDO            , KEY_Z    , MODIFIER_BOTH_CTRL );
	set(MAP_REDO            , KEY_Z    , MODIFIER_BOTH_CTRL ,
	                                     MODIFIER_BOTH_SHIFT);
	set(MAP_REDO_1          , KEY_Y    , MODIFIER_BOTH_CTRL );

	set(MAP_DELETE          , KEY_DEL                       );
	set(MAP_CUT             , KEY_X    , MODIFIER_BOTH_CTRL );
	set(MAP_COPY            , KEY_C    , MODIFIER_BOTH_CTRL );
	set(MAP_PASTE           , KEY_V    , MODIFIER_BOTH_CTRL );
}

bool map_press(const Input &input, MappedKeyIndex index) {
	int key = input.input_map.key_list[index].key;
	int modifier = input.input_map.key_list[index].modifier;
	int sec_modifier = input.input_map.key_list[index].sec_modifier;

	if (key == -1) {
		return false;
	}

	if (!modifier_cond(input, modifier, sec_modifier)) {
		return false;
	}

	if (!input.key_list[key].press) {
		return false;
	}

	return true;
}

bool map_down(const Input &input, MappedKeyIndex index) {
	int key = input.input_map.key_list[index].key;
	int modifier = input.input_map.key_list[index].modifier;
	int sec_modifier = input.input_map.key_list[index].sec_modifier;

	if (key == -1) {
		return false;
	}

	if (!modifier_cond(input, modifier, sec_modifier)) {
		return false;
	}

	if (!input.key_list[key].down) {
		return false;
	}

	return true;
}

bool map_release(const Input &input, MappedKeyIndex index) {
	int key = input.input_map.key_list[index].key;
	int modifier = input.input_map.key_list[index].modifier;
	int sec_modifier = input.input_map.key_list[index].sec_modifier;

	if (key == -1) {
		return false;
	}

	if (!modifier_cond(input, modifier, sec_modifier)) {
		return false;
	}

	if (!input.key_list[key].release) {
		return false;
	}

	return true;
}
