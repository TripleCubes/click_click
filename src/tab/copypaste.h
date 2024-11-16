#ifndef TAB__COPYPASTE_H
#define TAB__COPYPASTE_H

#ifndef __EMSCRIPTEN__
#include <GLFW/glfw3.h>
#endif
#include <vector>

struct Tab;
struct GraphicStuff;
struct Input;
struct Vec2;
struct Vec2i;

void to_clipboard(const Tab &tab
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
	);

bool get_paste_data(std::vector<unsigned char> &data, Vec2i &pos, Vec2i &sz
	#ifndef __EMSCRIPTEN__
	,GLFWwindow *glfw_window
	#endif
	);

#endif
