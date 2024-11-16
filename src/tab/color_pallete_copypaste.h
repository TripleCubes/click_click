#ifndef TAB__COLOR_PALLETE_COPY_PASTE_H
#define TAB__COLOR_PALLETE_COPY_PASTE_H

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#endif

struct ColorPallete;
struct GraphicStuff;
struct Tab;

void color_pallete_to_clipboard(const ColorPallete &color_pallete
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
	);

void color_pallete_paste(ColorPallete &color_pallete, Tab &tab,
	GraphicStuff &gs
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
	);

#endif
