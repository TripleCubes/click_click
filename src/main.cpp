#ifndef __EMSCRIPTEN__

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "graphic_types/graphic_types.h"
#include "graphic/graphic.h"
#include "game_time.h"
#include "mainloop.h"

#include "types/vec2i.h"

#include <cmath>
#include <chrono>
#include <thread>

namespace {

GLFWwindow * init() {
	const int INIT_W = 1000;
	const int INIT_H = 600;

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

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize glad" << std::endl;
		return NULL;
	}

	glViewport(0, 0, INIT_W, INIT_H);

	// CALLBACKS

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return glfw_window;
}

void sleep_for_sec(float time_sec) {
	using namespace std::chrono;
	using namespace std::this_thread;

	sleep_for(milliseconds((int)std::round(time_sec * 1000)));
}

}

int main () {
	const int FPS_CAP = 60;

	GLFWwindow *glfw_window = init();
	if (glfw_window == NULL) {
		return 0;
	}

	float game_start_time = glfwGetTime();
	float frame_start_time = game_start_time;
	GameTime game_time;

	GraphicStuff graphic_stuff;
	graphic_stuff.current_window_sz.x = 1000;
	graphic_stuff.current_window_sz.y = 600;
	if (!graphic_init(graphic_stuff)) {
		std::cout << "cant init graphic" << std::endl;
		return 0;
	}

	while (!glfwWindowShouldClose(glfw_window)) {
		game_time.delta = glfwGetTime() - frame_start_time;
		frame_start_time = glfwGetTime();
		game_time.time_since_start = frame_start_time - game_start_time;
		glfwPollEvents();
		update(game_time);

		draw(graphic_stuff);
		glfwSwapBuffers(glfw_window);

		float frame_time = glfwGetTime() - frame_start_time;
		sleep_for_sec(1/(float)FPS_CAP - frame_time);
	}

	glfwTerminate();
	std::cout << "reached end of main" << std::endl;

	return 0;
}

#endif
