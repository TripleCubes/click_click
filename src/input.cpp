#include "input.h"

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif

namespace {

#ifndef __EMSCRIPTEN__
const std::array<int, KEY_COUNT> KEY_GLFW_LIST = {
	GLFW_KEY_A,
	GLFW_KEY_B,
	GLFW_KEY_C,
	GLFW_KEY_D,
	GLFW_KEY_E,
	GLFW_KEY_F,
	GLFW_KEY_G,
	GLFW_KEY_H,
	GLFW_KEY_I,
	GLFW_KEY_J,
	GLFW_KEY_K,
	GLFW_KEY_L,
	GLFW_KEY_M,
	GLFW_KEY_N,
	GLFW_KEY_O,
	GLFW_KEY_P,
	GLFW_KEY_Q,
	GLFW_KEY_R,
	GLFW_KEY_S,
	GLFW_KEY_T,
	GLFW_KEY_U,
	GLFW_KEY_V,
	GLFW_KEY_W,
	GLFW_KEY_X,
	GLFW_KEY_Y,
	GLFW_KEY_Z,

	GLFW_KEY_0,
	GLFW_KEY_1,
	GLFW_KEY_2,
	GLFW_KEY_3,
	GLFW_KEY_4,
	GLFW_KEY_5,
	GLFW_KEY_6,
	GLFW_KEY_7,
	GLFW_KEY_8,
	GLFW_KEY_9,

	GLFW_KEY_LEFT_SHIFT,
	GLFW_KEY_RIGHT_SHIFT,
	GLFW_KEY_LEFT_CONTROL,
	GLFW_KEY_RIGHT_CONTROL,
	GLFW_KEY_LEFT_ALT,
	GLFW_KEY_RIGHT_ALT,

	GLFW_KEY_SPACE,
	GLFW_KEY_ENTER,
	GLFW_KEY_ESCAPE,
	GLFW_KEY_TAB,
	GLFW_KEY_BACKSPACE,
	GLFW_KEY_DELETE,
	GLFW_KEY_HOME,
	GLFW_KEY_END,

	GLFW_KEY_UP,
	GLFW_KEY_DOWN,
	GLFW_KEY_LEFT,
	GLFW_KEY_RIGHT,

	GLFW_KEY_MINUS,
	GLFW_KEY_EQUAL,
	GLFW_KEY_LEFT_BRACKET,
	GLFW_KEY_RIGHT_BRACKET,
	GLFW_KEY_SEMICOLON,
	GLFW_KEY_BACKSLASH,
	GLFW_KEY_APOSTROPHE,
	GLFW_KEY_COMMA,
	GLFW_KEY_PERIOD,
	GLFW_KEY_SLASH,
};
#else
const std::array<int, KEY_COUNT> KEY_GLFW_LIST = {
	'A',
	'B',
	'C',
	'D',
	'E',
	'F',
	'G',
	'H',
	'I',
	'J',
	'K',
	'L',
	'M',
	'N',
	'O',
	'P',
	'Q',
	'R',
	'S',
	'T',
	'U',
	'V',
	'W',
	'X',
	'Y',
	'Z',

	'0',
	'1',
	'2',
	'3',
	'4',
	'5',
	'6',
	'7',
	'8',
	'9',

	GLFW_KEY_LSHIFT,
	GLFW_KEY_RSHIFT,
	GLFW_KEY_LCTRL,
	GLFW_KEY_RCTRL,
	GLFW_KEY_LALT,
	GLFW_KEY_RALT,

	GLFW_KEY_SPACE,
	GLFW_KEY_ENTER,
	GLFW_KEY_ESC,
	GLFW_KEY_TAB,
	GLFW_KEY_BACKSPACE,
	GLFW_KEY_DEL,
	GLFW_KEY_HOME,
	GLFW_KEY_END,

	GLFW_KEY_UP,
	GLFW_KEY_DOWN,
	GLFW_KEY_LEFT,
	GLFW_KEY_RIGHT,

	'-',
	'=',
	'[',
	']',
	';',
	'\\',
	'\'',
	',',
	'.',
	'/',
};
#endif

#ifndef __EMSCRIPTEN__
void key_list_update(Input &input, GLFWwindow *glfw_window) {
#else
void key_list_update(Input &input) {
#endif
	input.key_event = false;

	for (int i = 0; i < KEY_COUNT; i++) {
		int key_glfw = KEY_GLFW_LIST[i];
		
		#ifndef __EMSCRIPTEN__
		int state = glfwGetKey(glfw_window, key_glfw);
		#else
		int state = glfwGetKey(key_glfw);
		#endif

		KeyState prev_state = input.key_list[i];

		input.key_list[i].press = false;
		input.key_list[i].release = false;

		if (state == GLFW_PRESS) {
			if (!prev_state.down) {
				input.key_list[i].press = true;
			}
			input.key_list[i].down = true;
			input.key_event = true;
		}
		else {
			if (prev_state.down) {
				input.key_list[i].release = true;
				input.key_event = true;
			}
			input.key_list[i].down = false;
		}
	}
}

char key_get_char_no_shift(int key) {
	if (key >= KEY_A && key <= KEY_Z) {
		return 'a' + key;
	}

	if (key >= KEY_0 && key <= KEY_9) {
		return '0' + key - KEY_0;
	}

	switch (key) {
		case KEY_MINUS:
			return '-';
			break;
		case KEY_EQUAL:
			return '=';
			break;
		case KEY_LEFT_SQUARE_BRACKET:
			return '[';
			break;
		case KEY_RIGHT_SQUARE_BRACKET:
			return ']';
			break;
		case KEY_SEMICOLON:
			return ';';
			break;
		case KEY_BACKSLASH:
			return '\\';
			break;
		case KEY_TICK:
			return '\'';
			break;
		case KEY_COMMA:
			return ',';
			break;
		case KEY_DOT:
			return '.';
			break;
		case KEY_FORWARD_SLASH:
			return '/';
			break;
		default:
			return ' ';
			break;
	}

	return ' ';
}

char key_get_char_shift(int key) {
	if (key >= KEY_A && key <= KEY_Z) {
		return 'A' + key;
	}

	switch (key) {
		case KEY_0:
			return ')';
			break;
		case KEY_1:
			return '!';
			break;
		case KEY_2:
			return '@';
			break;
		case KEY_3:
			return '#';
			break;
		case KEY_4:
			return '$';
			break;
		case KEY_5:
			return '%';
			break;
		case KEY_6:
			return '^';
			break;
		case KEY_7:
			return '&';
			break;
		case KEY_8:
			return '*';
			break;
		case KEY_9:
			return '(';
			break;

		case KEY_MINUS:
			return '_';
			break;
		case KEY_EQUAL:
			return '+';
			break;
		case KEY_LEFT_SQUARE_BRACKET:
			return '{';
			break;
		case KEY_RIGHT_SQUARE_BRACKET:
			return '}';
			break;
		case KEY_SEMICOLON:
			return ':';
			break;
		case KEY_BACKSLASH:
			return '|';
			break;
		case KEY_TICK:
			return '\'';
			break;
		case KEY_COMMA:
			return '<';
			break;
		case KEY_DOT:
			return '>';
			break;
		case KEY_FORWARD_SLASH:
			return '?';
			break;
		default:
			return ' ';
			break;
	}

	return ' ';
}

}

void input_init(Input &input) {
	input_map_init(input.input_map);
}

#ifndef __EMSCRIPTEN__
void input_update(Input &input, GLFWwindow *glfw_window) {
	input.mouse_event = false;
	input.mouse_move = false;

	double mouse_x;
	double mouse_y;
	glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);
	if (mouse_x != input.mouse_pos.x || mouse_y != input.mouse_pos.y) {
		input.prev_mouse_pos = input.mouse_pos;
		input.mouse_pos.x = mouse_x;
		input.mouse_pos.y = mouse_y;
		input.mouse_move = true;
		input.mouse_event = true;
	}


	input.left_click = false;
	input.left_release = false;
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (!input.left_down) {
			input.left_click = true;
		}
		input.left_down = true;
		input.mouse_event = true;
	}
	else {
		if (input.left_down) {
			input.left_release = true;
			input.mouse_event = true;
		}
		input.left_down = false;
	}

	input.right_click = false;
	input.right_release = false;
	if (glfwGetMouseButton(glfw_window,GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
		if (!input.right_down) {
			input.right_click = true;
		}
		input.right_down = true;
		input.mouse_event = true;
	}
	else {
		if (input.right_down) {
			input.right_release = true;
			input.mouse_event = true;
		}
		input.right_down = false;
	}


	key_list_update(input, glfw_window);
}

#else
void input_update(Input &input) {
	input.mouse_event = false;
	input.mouse_move = false;

	int mouse_x = 0;
	int mouse_y = 0;
	glfwGetMousePos(&mouse_x, &mouse_y);
	if (mouse_x != input.mouse_pos.x || mouse_y != input.mouse_pos.y) {
		input.prev_mouse_pos = input.mouse_pos;
		input.mouse_pos.x = mouse_x;
		input.mouse_pos.y = mouse_y;
		input.mouse_move = true;
		input.mouse_event = true;
	}


	input.left_click = false;
	input.left_release = false;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (!input.left_down) {
			input.left_click = true;
		}
		input.left_down = true;
		input.mouse_event = true;
	}
	else {
		if (input.left_down) {
			input.left_release = true;
			input.mouse_event = true;
		}
		input.left_down = false;
	}

	input.right_click = false;
	input.right_release = false;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS){
		if (!input.right_down) {
			input.right_click = true;
		}
		input.right_down = true;
		input.mouse_event = true;
	}
	else {
		if (input.right_down) {
			input.right_release = true;
			input.mouse_event = true;
		}
		input.right_down = false;
	}


	key_list_update(input);
}

#endif

bool ctrl_down(const Input &input) {
	return input.key_list[KEY_LEFT_CTRL].down
		|| input.key_list[KEY_RIGHT_CTRL].down;
}

bool shift_down(const Input &input) {
	return input.key_list[KEY_LEFT_SHIFT].down
		|| input.key_list[KEY_RIGHT_SHIFT].down;
}

char key_get_char(int key, bool shift) {
	if (!shift) {
		return key_get_char_no_shift(key);
	}
	else {
		return key_get_char_shift(key);
	}
}

char key_get_char_no_special(int key, bool shift) {
	if (key >= KEY_A && key <= KEY_Z) {
		return key_get_char(key, shift);
	}

	if (!shift && key >= KEY_0 && key <= KEY_9) {
		return key_get_char(key, false);
	}

	if (shift && key == KEY_MINUS) {
		return '_';
	}

	return ' ';
}
