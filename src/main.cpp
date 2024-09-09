#ifndef __EMSCRIPTEN__

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphic_types/graphic_types.h"
#include "graphic/graphic.h"
#include "game_time.h"
#include "input.h"
#include "tab/tab.h"
#include "mainloop.h"

#include "types/vec2i.h"

namespace {
const int INIT_W = 1000;
const int INIT_H = 600;

int window_w = INIT_W;
int window_h = INIT_H;

void on_resize(GLFWwindow *glfw_window, int w, int h) {
	window_w = w;
	window_h = h;
}

GLFWwindow * init() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *glfw_window
		= glfwCreateWindow(INIT_W, INIT_H, "click click", NULL, NULL);
	if (glfw_window == NULL) {
		std::cout << "failed to create window" << std::endl;
		glfwTerminate();
		return NULL;
	}

	glfwMakeContextCurrent(glfw_window);
	glfwSwapInterval(1);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize glad" << std::endl;
		return NULL;
	}

	glViewport(0, 0, INIT_W, INIT_H);

	glfwSetFramebufferSizeCallback(glfw_window, on_resize);


	glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return glfw_window;
}

}

int main () {
	GLFWwindow *glfw_window = init();
	if (glfw_window == NULL) {
		std::cout << "program exited" << std::endl;
		return 0;
	}

	float game_start_time = glfwGetTime();
	float frame_start_time = game_start_time;
	GameTime game_time;

	Input input;
	input_init(input);

	GraphicStuff graphic_stuff;
	graphic_stuff.current_window_sz.x = window_w;
	graphic_stuff.current_window_sz.y = window_h;
	graphic_stuff.px_scale = 2;
	if (!graphic_init(graphic_stuff)) {
		std::cout << "cant init graphic" << std::endl;
		std::cout << "program exited" << std::endl;
		return 0;
	}

	std::vector<Tab> tab_list;
	tab_new(tab_list, graphic_stuff, vec2_new(200, 100), vec2i_new(128, 128),2);

	const float REDRAW_REQUEST_WAIT = 0.5;
	int redraw_request_count = 0;

	while (!glfwWindowShouldClose(glfw_window)) {
		game_time.delta = glfwGetTime() - frame_start_time;
		frame_start_time = glfwGetTime();
		game_time.time_since_start = frame_start_time - game_start_time;

		graphic_stuff.just_resized = false;
		graphic_stuff.redraw_requested = false;
		graphic_stuff.cursor_icon = CURSOR_POINTER;
		graphic_resize(graphic_stuff, vec2i_new(window_w, window_h));
		
		//glfwWaitEventsTimeout(REDRAW_REQUEST_WAIT);
		glfwPollEvents();

		input_update(input, glfw_window);

		update(graphic_stuff, tab_list, game_time, input);

		if (redraw_request_count * REDRAW_REQUEST_WAIT
		                                        < game_time.time_since_start) {
			redraw_request_count++;
			graphic_stuff.redraw_requested = true;
		}

		draw(graphic_stuff, tab_list, game_time, input);

		game_time.frame_time = glfwGetTime() - frame_start_time;
		game_time.frame_passed++;

		glfwSwapBuffers(glfw_window);
	}

	graphic_types_release_all(graphic_stuff);
	glfwTerminate();
	std::cout << "reached end of main" << std::endl;

	return 0;
}

#endif
