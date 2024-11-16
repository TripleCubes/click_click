#ifndef TAB__COLOR_PALLETE_COPY_PASTE_H
#define TAB__COLOR_PALLETE_COPY_PASTE_H

#include <GLFW/glfw3.h>

struct ColorPallete;
struct GraphicStuff;
struct Tab;

void color_pallete_to_clipboard(const ColorPallete &color_pallete,
	GLFWwindow *glfw_window);

void color_pallete_paste(ColorPallete &color_pallete, Tab &tab,
	GraphicStuff &gs, GLFWwindow *glfw_window);

#endif
