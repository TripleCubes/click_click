#include "color_pallete_copypaste.h"

#include <string>

#include "../ui/tab/color_pallete.h"
#include "../types/color.h"
#include "../graphic_types/graphic_types.h"
#include "../graphic_types/texture.h"
#include "tab.h"

namespace {

void rm_whitespace(std::string &str) {
	for (int i = (int)str.length() - 1; i >= 0; i--) {
		char c = str[i];
		if (c == '\n' || c == '\r' || c == ' ') {
			str.erase(str.begin() + i);
		}
	}
}

bool is_hex(const std::string &str) {
	for (int i = 0; i < (int)str.length(); i++) {
		char c = str[i];
		if ((c < '0' || c > '9') && (c < 'a' && c > 'f')) {
			return false;
		}
	}

	return true;
}

void pallete_data_color(Tab &tab, int pallete_index, Color color) {
	int index = pallete_index * 4;
	tab.pallete_data[index    ] = color.r * 255;
	tab.pallete_data[index + 1] = color.g * 255;
	tab.pallete_data[index + 2] = color.b * 255;
	tab.pallete_data[index + 3] = color.a * 255;
}

}

void color_pallete_to_clipboard(const ColorPallete &color_pallete,
GLFWwindow *glfw_window) {
	std::string str;
	for (int i = 0; i < (int)color_pallete.color_list.size(); i++) {
		Color color = color_pallete.color_list[i];
		str += color_to_hex(color);
		if (i % 8 == 7) {
			str += '\n';
		}
		else {
			str += ' ';
		}
	}
	glfwSetClipboardString(glfw_window, str.c_str());
}

void color_pallete_paste(ColorPallete &color_pallete, Tab &tab,
GraphicStuff &gs, GLFWwindow *glfw_window) {
	std::string str = glfwGetClipboardString(glfw_window);
	rm_whitespace(str);

	if (str.length() != 64*7) {
		return;
	}

	for (int i = 0; i < 64; i++) {
		std::string hex = str.substr(i*7 + 1, 6);
		if (!is_hex(hex)) {
			return;
		}

		Color color = hex_to_color('#' + hex);
		if (i == 0) {
			color.r = 0;
			color.g = 0;
			color.b = 0;
			color.a = 0;
		}

		color_pallete.color_list[i] = color;
		pallete_data_color(tab, i, color);
	}

	texture_data(gs, tab.pallete_texture_index,
		vec2i_new(16, 16), tab.pallete_data);
}
