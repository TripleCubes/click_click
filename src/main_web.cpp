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
#include "states.h"
#include "tab/tab.h"
#include "ui/tab_bar.h"
#include "ui/file_picker/file_picker.h"
#include "graphic/graphic.h"
#include "mainloop.h"
#include "consts.h"

#include "types/vec2i.h"
#include "types/vec2.h"

namespace {

States states;
GraphicStuff graphic_stuff;
GameTime game_time;
Input input;
TabBar tab_bar;
FilePicker file_picker;
float game_start_time = 0;
float frame_start_time = 0;

const float REDRAW_REQUEST_WAIT = 0.5;
int redraw_request_count = 0;

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
	graphic_stuff.redraw_requested = false;
	graphic_stuff.draw_secondlayer_ui = false;
	graphic_stuff.cursor_icon = CURSOR_POINTER;
	graphic_resize(graphic_stuff, vec2i_new(window_w, window_h));
	
	glfwPollEvents();

	input_update(input);
	update(
		states,
		graphic_stuff,
		tab_bar,
		file_picker,
		game_time,
		input
	);

	if (redraw_request_count * REDRAW_REQUEST_WAIT
											< game_time.time_since_start) {
		redraw_request_count++;
		graphic_stuff.redraw_requested = true;
	}

	draw(
		states,
		graphic_stuff,
		tab_bar,
		file_picker,
		game_time,
		input
	);
	
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

	input_init(input);

	graphic_stuff.current_window_sz.x = window_w;
	graphic_stuff.current_window_sz.y = window_h;
	graphic_stuff.px_scale = 2;
	if (!graphic_init(graphic_stuff)) {
		std::cout << "cant init graphic" << std::endl;
		std::cout << "program exited" << std::endl;
		return 0;
	}

	tab_bar_init(tab_bar, graphic_stuff, vec2_new(SIDE_BAR_W + 4 + 3, 4 + 3));
	file_picker_init(file_picker);

	emscripten_set_main_loop(main_loop, 0, true);

	tab_bar_release(tab_bar, graphic_stuff);
	graphic_types_release_all(graphic_stuff);
	glfwTerminate();
	std::cout << "reached end of main" << std::endl;

	return 0;
}

#endif
