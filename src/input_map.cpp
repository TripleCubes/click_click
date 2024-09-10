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

bool any_modifier_down(const Input &input) {
	if (input.key_list[KEY_LEFT_SHIFT].down) {
		return true;
	}
	if (input.key_list[KEY_RIGHT_SHIFT].down) {
		return true;
	}

	if (input.key_list[KEY_LEFT_CTRL].down) {
		return true;
	}
	if (input.key_list[KEY_RIGHT_CTRL].down) {
		return true;
	}

	if (input.key_list[KEY_LEFT_ALT].down) {
		return true;
	}
	if (input.key_list[KEY_RIGHT_ALT].down) {
		return true;
	}

	return false;
}

bool modifier_cond(const Input &input, int modifier, int sec_modifier) {
	if (modifier == -1 && sec_modifier == -1) {
		return !any_modifier_down(input);
	}
	else {
		return modifier_down(input, modifier)
		&& modifier_down(input, sec_modifier);
	}
}

}

void input_map_init(InputMap &input_map) {
	auto set = [&input_map](MappedKeyIndex index, int key = -1,
	int modifier = -1, int sec_modifier = -1) {
		input_map.key_list[index].key = key;
		input_map.key_list[index].modifier = modifier;
		input_map.key_list[index].sec_modifier = sec_modifier;
	};

	set(MAP_PAN             , KEY_SPACE                     );
	
	set(MAP_ZOOM_IN         , KEY_EQUAL, MODIFIER_BOTH_CTRL );
	set(MAP_ZOOM_OUT        , KEY_MINUS, MODIFIER_BOTH_CTRL );
	set(MAP_ZOOM_0          , KEY_0    , MODIFIER_BOTH_CTRL );

	set(MAP_TOOL_BRUSH      , KEY_B                         );
	set(MAP_TOOL_CURVE      , KEY_C                         );
	set(MAP_TOOL_FILL       , KEY_F                         );
	set(MAP_TOOL_SELECT     , KEY_S                         );
	set(MAP_TOOL_SELECT_MODE, KEY_D                         );

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