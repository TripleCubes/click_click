#ifndef TAB__COPYPASTE_H
#define TAB__COPYPASTE_H

#include <GLFW/glfw3.h>
#include <vector>

struct Tab;
struct GraphicStuff;
struct Input;
struct Vec2;
struct Vec2i;

void to_clipboard(const Tab &tab, GLFWwindow *glfw_window);

bool get_paste_data(std::vector<unsigned char> &data, Vec2i &pos, Vec2i &sz,
	GLFWwindow *glfw_window);

#endif
