#include "input.h"

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#else
#include <GL/glfw.h>
#endif

#ifndef __EMSCRIPTEN__
void input_update(Input &input, GLFWwindow *glfw_window) {
	input.mouse_move = false;

	double mouse_x;
	double mouse_y;
	glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);
	if (mouse_x != input.mouse_pos.x || mouse_y != input.mouse_pos.y) {
		input.mouse_pos.x = mouse_x;
		input.mouse_pos.y = mouse_y;
		input.mouse_move = true;
	}


	input.left_click = false;
	input.left_release = false;
	if (glfwGetMouseButton(glfw_window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (!input.left_down) {
			input.left_click = true;
		}
		input.left_down = true;
	}
	else {
		if (input.left_down) {
			input.left_release = true;
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
	}
	else {
		if (input.right_down) {
			input.right_release = true;
		}
		input.right_down = false;
	}
}

#else
void input_update(Input &input) {
	input.mouse_move = false;

	int mouse_x = 0;
	int mouse_y = 0;
	glfwGetMousePos(&mouse_x, &mouse_y);
	if (mouse_x != input.mouse_pos.x || mouse_y != input.mouse_pos.y) {
		input.mouse_pos.x = mouse_x;
		input.mouse_pos.y = mouse_y;
		input.mouse_move = true;
	}


	input.left_click = false;
	input.left_release = false;
	if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS){
		if (!input.left_down) {
			input.left_click = true;
		}
		input.left_down = true;
	}
	else {
		if (input.left_down) {
			input.left_release = true;
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
	}
	else {
		if (input.right_down) {
			input.right_release = true;
		}
		input.right_down = false;
	}

}

#endif
