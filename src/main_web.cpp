#ifdef __EMSCRIPTEN__

#include <iostream>
#include <vector>
#include <GLES2/gl2.h>
#include <GL/glfw.h>
#include <emscripten/emscripten.h>
#include <emscripten/html5.h>

#include "graphic_types/graphic_types.h"
#include "game_time.h"
#include "input.h"
#include "tab/tab.h"
#include "graphic/graphic.h"
#include "mainloop.h"

//TEST
#include "types/vec2.h"
#include "types/vec2i.h"

namespace {

GraphicStuff graphic_stuff;
GameTime game_time;
Input input;
std::vector<Tab> tab_list;
float game_start_time = 0;
float frame_start_time = 0;

bool init() {
	glfwInit();

	double window_w = 0;
	double window_h = 0;
	emscripten_get_element_css_size("#canvas", &window_w, &window_h);
	if (glfwOpenWindow(window_w, window_h, 8, 8, 8, 8, 0, 0, GLFW_WINDOW)
	                                                              != GL_TRUE) {
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

	double window_w = 0;
	double window_h = 0;
	emscripten_get_element_css_size("#canvas", &window_w, &window_h);
	graphic_stuff.just_resized = false;
	graphic_resize(graphic_stuff, vec2i_new(window_w, window_h));
	
	glfwPollEvents();

	input_update(input);
	update(graphic_stuff, tab_list, game_time, input);

	draw(graphic_stuff, tab_list, game_time, input);
	
	game_time.frame_time = glfwGetTime() - frame_start_time;
	game_time.frame_passed++;

	glfwSwapBuffers();
}

}

int main() {
	if (!init()) {
		std::cout << "program exited" << std::endl;
		return 0;
	}

	game_start_time = glfwGetTime();
	frame_start_time = game_start_time;

	double window_w = 0;
	double window_h = 0;
	emscripten_get_element_css_size("#canvas", &window_w, &window_h);

	graphic_stuff.current_window_sz.x = window_w;
	graphic_stuff.current_window_sz.y = window_h;
	graphic_stuff.px_scale = 2;
	if (!graphic_init(graphic_stuff)) {
		std::cout << "cant init graphic" << std::endl;
		std::cout << "program exited" << std::endl;
		return 0;
	}

	tab_new(tab_list, graphic_stuff, vec2_new(10, 200), vec2i_new(64, 64), 2);

	emscripten_set_main_loop(main_loop, 0, true);

	graphic_types_release_all(graphic_stuff);
	glfwTerminate();
	std::cout << "reached end of main" << std::endl;

	return 0;
}

#endif
