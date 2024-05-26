#ifdef __EMSCRIPTEN__

#include <iostream>
#include <GLES2/gl2.h>
#include <GL/glfw.h>
#include <emscripten/emscripten.h>

#include "graphic_types/graphic_types.h"
#include "game_time.h"
#include "graphic/graphic.h"
#include "mainloop.h"

namespace {

GraphicStuff graphic_stuff;
GameTime game_time;
float game_start_time = 0;
float frame_start_time = 0;

bool init() {
	const int INIT_W = 1000;
	const int INIT_H = 600;

	glfwInit();

	if (glfwOpenWindow(INIT_W,INIT_H,8, 8, 8, 8, 0, 0,GLFW_WINDOW) != GL_TRUE){
		std::cout << "cant create window" << std::endl;
		return false;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void main_loop() {
	game_time.delta = glfwGetTime() - frame_start_time;
	frame_start_time = glfwGetTime();
	game_time.time_since_start = frame_start_time - game_start_time;
	glfwPollEvents();
	update(game_time);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	draw(graphic_stuff);
	glfwSwapBuffers();
}

}

int main() {
	if (!init()) {
		return 0;
	}

	game_start_time = glfwGetTime();
	frame_start_time = game_start_time;

	graphic_stuff.current_window_sz.x = 1000;
	graphic_stuff.current_window_sz.y = 600;
	if (!graphic_init(graphic_stuff)) {
		std::cout << "cant init graphic" << std::endl;
		return 0;
	}

	emscripten_set_main_loop(main_loop, 0, true);

	glfwTerminate();
	return 0;
}

#endif
