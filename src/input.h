#ifndef INPUT_H
#define INPUT_H

#include "types/vec2.h"

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif

struct Input {
	Vec2 mouse_pos;
	
	bool left_click = false;
	bool left_down = false;
	bool left_release = false;

	bool right_click = false;
	bool right_down = false;
	bool right_release = false;

	bool mouse_move = false;
};

#ifndef __EMSCRIPTEN__
void input_update(Input &input, GLFWwindow *glfw_window);
#else
void input_update(Input &input);
#endif

#endif
