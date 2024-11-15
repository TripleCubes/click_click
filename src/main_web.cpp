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
#include "settings.h"
#include "tab/tab.h"
#include "ui/app_ui.h"
#include "graphic/graphic.h"
#include "mainloop.h"
#include "consts.h"

#include "types/vec2i.h"
#include "types/vec2.h"

namespace {

States states;
Settings settings;
GraphicStuff graphic_stuff;
GameTime game_time;
Input input;
AppUI app_ui;
float game_start_time = 0;
float frame_start_time = 0;
int prev_cursor_icon = CURSOR_POINTER;

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
	graphic_stuff.draw_thirdlayer_ui = false;
	graphic_stuff.cursor_icon = CURSOR_POINTER;
	graphic_resize(graphic_stuff, vec2i_new(window_w, window_h));
	
	glfwPollEvents();

	states_update(states);
	input_update(input);
	update(
		states,
		settings,
		graphic_stuff,
		game_time,
		input,
		app_ui
	);
	
	if (settings.use_hardware_cursor
	&& prev_cursor_icon != graphic_stuff.cursor_icon) {
		if (graphic_stuff.cursor_icon == CURSOR_TEXT) {
			EM_ASM(
			document.body.style.cursor = 'text';
			);
		}
		else {
			EM_ASM(
			document.body.style.cursor = 'default';
			);
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

	glfwSwapBuffers();
}

}

int main() {
	EM_ASM({
		FS.mkdir(UTF8ToString($0));
		FS.mount(IDBFS, {}, UTF8ToString($0));

		FS.syncfs(true, function(err) {
			console.log('FS.syncfs(true) finished');
			if (err) {
				console.log(err);
			}
		});
	}, WEB_DATA_DIR.c_str());

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

	app_ui_init(app_ui, graphic_stuff);

	emscripten_set_main_loop(main_loop, 0, true);

	app_ui_release(app_ui, graphic_stuff);
	graphic_types_release_all(graphic_stuff);
	glfwTerminate();
	std::cout << "reached end of main()" << std::endl;

	return 0;
}

#endif
