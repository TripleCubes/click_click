#ifndef INPUT_MAP_H
#define INPUT_MAP_H

#include <array>

struct Input;

const int MODIFIER_LEFT_CTRL = 0;
const int MODIFIER_RIGHT_CTRL = 1;
const int MODIFIER_BOTH_CTRL = 2;
const int MODIFIER_LEFT_SHIFT = 3;
const int MODIFIER_RIGHT_SHIFT = 4;
const int MODIFIER_BOTH_SHIFT = 5;
const int MODIFIER_LEFT_ALT = 6;
const int MODIFIER_RIGHT_ALT = 7;
const int MODIFIER_BOTH_ALT = 8;

enum MappedKeyIndex: int {
	MAP_ESC,
	MAP_ENTER,
	MAP_TAB,
	MAP_SHIFT_TAB,

	MAP_PAN,
	
	MAP_ZOOM_IN,
	MAP_ZOOM_OUT,
	MAP_ZOOM_0,
	
	MAP_TOOL_BRUSH,
	MAP_TOOL_CURVE,
	MAP_TOOL_FILL,
	MAP_TOOL_SELECT,
	MAP_TOOL_SELECT_MODE,
	MAP_TOOL_ERASER,

	MAP_COLOR_1,
	MAP_COLOR_2,
	MAP_COLOR_3,
	MAP_COLOR_4,
	MAP_COLOR_5,
	MAP_COLOR_6,
	MAP_COLOR_7,
	MAP_COLOR_8,

	MAP_COLOR_PAGE_1,
	MAP_COLOR_PAGE_2,
	MAP_COLOR_PAGE_3,
	MAP_COLOR_PAGE_4,

	MAP_SELECT_ALL,
	MAP_DESELECT_ALL,

	MAP_OPEN_FILE,
	MAP_SAVE_FILE,
	MAP_SAVE_FILE_AS,
	
	MAP_NEW_PROJECT,
	MAP_NEW_PROJECT_1,

	MAP_RESIZE_CANVAS,

	MAP_APP_MENU,

	MAP_UNDO,
	MAP_REDO,
	MAP_REDO_1,

	MAP_DELETE,
	MAP_CUT,
	MAP_COPY,
	MAP_PASTE,


	MAP_COUNT,
};

struct MappedKey {
	int key = -1;
	int modifier = -1;
	int sec_modifier = -1;
};

struct InputMap {
	std::array<MappedKey, MAP_COUNT> key_list;
};

void input_map_init(InputMap &input_map);
bool map_press(const Input &input, MappedKeyIndex index);
bool map_down(const Input &input, MappedKeyIndex index);
bool map_release(const Input &input, MappedKeyIndex index);

#endif
