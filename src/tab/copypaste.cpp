#include "copypaste.h"

#include <string>
#include "tab.h"
#include "layer.h"
#include "../types/vec2.h"
#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../pos_convert.h"

#ifdef __EMSCRIPTEN__
extern std::string clipboard;
#endif

namespace {

const std::string TYPE_LABEL = "clicklayercopy";

std::vector<unsigned char> data;
Vec2i top_left;
Vec2i bottom_right;
std::string str;

char to_char(unsigned char pallete_index) {
	return pallete_index + 48;
}

unsigned char to_pallete_index(char c) {
	return c - 48;
}

void to_clipboard_selected(const Tab &tab
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	data.clear();
	top_left = vec2i_new(tab.sz.x - 1, tab.sz.y - 1);
	bottom_right = vec2i_new(0, 0);
	str.clear();

	data.resize(tab.sz.x * tab.sz.y, 0);

	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	const Layer &layer = tab.layer_list[layer_index];
	for (int y = 0; y < (int)layer.sz.y; y++) {
	for (int x = 0; x < (int)layer.sz.x; x++) {
		int i = y * tab.sz.x + x;

		if (tab.selection.map[i] == 0) {
			continue;
		}

		if (x < top_left.x) {
			top_left.x = x;
		}
		if (y < top_left.y) {
			top_left.y = y;
		}
		if (x > bottom_right.x) {
			bottom_right.x = x;
		}
		if (y > bottom_right.y) {
			bottom_right.y = y;
		}

		data[i] = layer.data[i];
	}
	}

	str += "type=" + TYPE_LABEL
		+ ";x=" + std::to_string(top_left.x)
		+ ";y=" + std::to_string(top_left.y)
		+ ";w=" + std::to_string(bottom_right.x - top_left.x + 1)
		+ ";h=" + std::to_string(bottom_right.y - top_left.y + 1)
		+ ";data=\n";
	for (int y = top_left.y; y <= bottom_right.y; y++) {
		for (int x = top_left.x; x <= bottom_right.x; x++) {
			int i = y * tab.sz.x + x;
			str += to_char(data[i]);
		}
		if (y == bottom_right.y) {
			str += ";";
		}
		else {
			str += "\n";
		}
	}

	#ifndef __EMSCRIPTEN__
	glfwSetClipboardString(glfw_window, str.c_str());
	#else
	clipboard = str;
	#endif
}

void to_clipboard_whole_layer(const Tab &tab
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	const Layer &layer = tab.layer_list[layer_index];

	str += "type=" + TYPE_LABEL
		+ ";x=0"
		+ ";y=0"
		+ ";w=" + std::to_string(tab.sz.x)
		+ ";h=" + std::to_string(tab.sz.y)
		+ ";data=\n";
	for (int y = 0; y < tab.sz.y; y++) {
		for (int x = 0; x < tab.sz.x; x++) {
			int i = y * tab.sz.x + x;
			str += to_char(layer.data[i]);
		}
		if (y == tab.sz.y - 1) {
			str += ";";
		}
		else {
			str += "\n";
		}
	}

	#ifndef __EMSCRIPTEN__
	glfwSetClipboardString(glfw_window, str.c_str());
	#else
	clipboard = str;
	#endif
}

bool label_check(const std::string &str) {
	int len = 6 + TYPE_LABEL.length();

	const std::string comp = "type=" + TYPE_LABEL + ";";

	for (int i = 0; i < len; i++) {
		if (str[i] != comp[i]) {
			return false;
		}
	}

	return true;
}

bool is_num(const std::string &str) {
	if (str.length() == 0) {
		return false;
	}

	for (int i = 0; i < (int)str.length(); i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}

	return true;
}

void rm_newline(std::string &str) {
	for (int i = (int)str.length() - 1; i >= 0; i--) {
		if (str[i] == '\n' || str[i] == '\r') {
			str.erase(str.begin() + i);
		}
	}
}

}

void to_clipboard(const Tab &tab
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	if (tab.selection.full_preview_list.size() != 0) {
		to_clipboard_selected(tab
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif	
		);
	}
	else {
		to_clipboard_whole_layer(tab
		#ifndef __EMSCRIPTEN__
			,glfw_window
		#endif	
		);
	}
}

bool get_paste_data(std::vector<unsigned char> &data, Vec2i &pos, Vec2i &sz
#ifndef __EMSCRIPTEN__
,GLFWwindow *glfw_window
#endif
) {
	#ifndef __EMSCRIPTEN__
	std::string paste_str = glfwGetClipboardString(glfw_window);
	#else
	std::string paste_str = clipboard;
	#endif
	rm_newline(paste_str);

	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;

	int min_len = 6 + TYPE_LABEL.length() + 13;

	if (paste_str.back() != ';') {
		return false;
	}
	if ((int)paste_str.length() < min_len) {
		return false;
	}
	if (!label_check(paste_str)) {
		return false;
	}

	std::string num_str;
	int w_num_start = 6 + TYPE_LABEL.length() + 2;
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		char c = paste_str[i];

		if (c < '0' || c > '9') {
			break;
		}

		num_str += c;
	}
	if (!is_num(num_str)) {
		return false;
	}
	x = std::stoi(num_str);

	w_num_start += num_str.length() + 3;
	num_str.clear();
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		char c = paste_str[i];

		if (c < '0' || c > '9') {
			break;
		}

		num_str += c;
	}
	if (!is_num(num_str)) {
		return false;
	}
	y = std::stoi(num_str);

	w_num_start += num_str.length() + 3;
	num_str.clear();
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		char c = paste_str[i];

		if (c < '0' || c > '9') {
			break;
		}

		num_str += c;
	}
	if (!is_num(num_str)) {
		return false;
	}
	w = std::stoi(num_str);

	w_num_start += num_str.length() + 3;
	num_str.clear();
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		char c = paste_str[i];

		if (c < '0' || c > '9') {
			break;
		}

		num_str += c;
	}
	if (!is_num(num_str)) {
		return false;
	}
	h = std::stoi(num_str);

	w_num_start += num_str.length() + 6;
	if ((int)paste_str.length() != w_num_start + w * h + 1) {
		return false;
	}
	data.clear();
	data.resize(w * h, 0);
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		if (i >= w_num_start + w*h) {
			break;
		}

		data[i - w_num_start] = to_pallete_index(paste_str[i]);
	}

	pos = vec2i_new(x, y);
	sz = vec2i_new(w, h);

	return true;
}
