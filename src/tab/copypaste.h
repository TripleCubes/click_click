#ifndef TAB__COPYPASTE_H
#define TAB__COPYPASTE_H

#include <GLFW/glfw3.h>

struct Tab;
struct GraphicStuff;
struct Input;
struct Vec2;

void to_clipboard(const Tab &tab, GLFWwindow *glfw_window);

void paste(Tab &tab, GraphicStuff &gs, const Input &input,
	Vec2 parent_pos, GLFWwindow *glfw_window);

#endif
