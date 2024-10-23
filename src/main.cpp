#ifndef __EMSCRIPTEN__

#include <iostream>
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphic_types/graphic_types.h"
#include "graphic/graphic.h"
#include "game_time.h"
#include "input.h"
#include "consts.h"
#include "states.h"
#include "settings.h"
#include "mainloop.h"
#include "ui/app_ui.h"

#include "types/vec2i.h"
#include "types/vec2.h"

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


//	glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

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

	GLFWcursor *text_cursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);
	int prev_cursor_icon = CURSOR_POINTER;

	States states;
	Settings settings;

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

	AppUI app_ui;
	app_ui_init(app_ui, graphic_stuff);

	const float REDRAW_REQUEST_WAIT = 0.5;
	int redraw_request_count = 0;

	while (!glfwWindowShouldClose(glfw_window)) {
		game_time.delta = glfwGetTime() - frame_start_time;
		frame_start_time = glfwGetTime();
		game_time.time_since_start = frame_start_time - game_start_time;

		graphic_stuff.just_resized = false;
		graphic_stuff.redraw_requested = false;
		graphic_stuff.draw_secondlayer_ui = false;
		graphic_stuff.cursor_icon = CURSOR_POINTER;
		graphic_resize(graphic_stuff, vec2i_new(window_w, window_h));
	
		//glfwWaitEventsTimeout(REDRAW_REQUEST_WAIT);
		glfwPollEvents();

		states_update(states);
		input_update(input, glfw_window);

		update(
			states,
			settings,
			graphic_stuff,
			game_time,
			input,
			app_ui,
			glfw_window
		);

		if (prev_cursor_icon != graphic_stuff.cursor_icon) {
			if (graphic_stuff.cursor_icon == CURSOR_TEXT) {
				glfwSetCursor(glfw_window, text_cursor);
			}
			else {
				glfwSetCursor(glfw_window, NULL);
			}
		}
		prev_cursor_icon = graphic_stuff.cursor_icon;

		if (redraw_request_count * REDRAW_REQUEST_WAIT
		                                        < game_time.time_since_start) {
			redraw_request_count++;
			graphic_stuff.redraw_requested = true;
		}

		draw(
			states,
			settings,
			graphic_stuff,
			game_time,
			input,
			app_ui
		);

		game_time.frame_time = glfwGetTime() - frame_start_time;
		game_time.frame_passed++;

		glfwSwapBuffers(glfw_window);
	}

	app_ui_release(app_ui, graphic_stuff);
	graphic_types_release_all(graphic_stuff);
	glfwDestroyCursor(text_cursor);
	glfwTerminate();
	std::cout << "reached end of main" << std::endl;

	return 0;
}

#endif
