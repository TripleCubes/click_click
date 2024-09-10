#ifndef INPUT_H
#define INPUT_H

#include "types/vec2.h"

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif

#include <array>
#include "input_map.h"

const int KEY_A = 0;
const int KEY_B = 1;
const int KEY_C = 2;
const int KEY_D = 3;
const int KEY_E = 4;
const int KEY_F = 5;
const int KEY_G = 6;
const int KEY_H = 7;
const int KEY_I = 8;
const int KEY_J = 9;
const int KEY_K = 10;
const int KEY_L = 11;
const int KEY_M = 12;
const int KEY_N = 13;
const int KEY_O = 14;
const int KEY_P = 15;
const int KEY_Q = 16;
const int KEY_R = 17;
const int KEY_S = 18;
const int KEY_T = 19;
const int KEY_U = 20;
const int KEY_V = 21;
const int KEY_W = 22;
const int KEY_X = 23;
const int KEY_Y = 24;
const int KEY_Z = 25;

const int KEY_0 = 26;
const int KEY_1 = 27;
const int KEY_2 = 28;
const int KEY_3 = 29;
const int KEY_4 = 30;
const int KEY_5 = 31;
const int KEY_6 = 32;
const int KEY_7 = 33;
const int KEY_8 = 34;
const int KEY_9 = 35;

const int KEY_LEFT_SHIFT = 36;
const int KEY_RIGHT_SHIFT = 37;
const int KEY_LEFT_CTRL = 38;
const int KEY_RIGHT_CTRL = 39;
const int KEY_LEFT_ALT = 40;
const int KEY_RIGHT_ALT = 41;

const int KEY_SPACE = 42;
const int KEY_ENTER = 43;
const int KEY_ESC = 44;
const int KEY_TAB = 45;
const int KEY_BACKSPACE = 46;
const int KEY_DEL = 47;
const int KEY_HOME = 48;
const int KEY_END = 49;

const int KEY_UP = 50;
const int KEY_DOWN = 51;
const int KEY_LEFT = 52;
const int KEY_RIGHT = 53;

const int KEY_MINUS = 54;
const int KEY_EQUAL = 55;
const int KEY_LEFT_SQUARE_BRACKET = 56;
const int KEY_RIGHT_SQUARE_BRACKET = 57;
const int KEY_SEMICOLON = 58;
const int KEY_BACK_SLASH = 59;
const int KEY_TICK = 60;
const int KEY_COMMA = 61;
const int KEY_DOT = 62;
const int KEY_FORWARD_SLASH = 63;
const int KEY_BACK_TICK = 64;

const int KEY_COUNT = 65;

struct KeyState {
	bool press = false;
	bool down = false;
	bool release = false;
};

struct Input {
	Vec2 mouse_pos;
	Vec2 prev_mouse_pos;
	
	bool left_click = false;
	bool left_down = false;
	bool left_release = false;

	bool right_click = false;
	bool right_down = false;
	bool right_release = false;

	bool mouse_move = false;

	std::array<KeyState, KEY_COUNT> key_list;

	bool mouse_event = false;
	bool key_event = false;

	InputMap input_map;
};

#ifndef __EMSCRIPTEN__
void input_init(Input &input);
void input_update(Input &input, GLFWwindow *glfw_window);
#else
void input_update(Input &input);
#endif

bool ctrl_down(const Input &input);
bool shift_down(const Input &input);
char key_get_char(int key, bool shift);
char key_get_char_no_special(int key, bool shift);

#endif
