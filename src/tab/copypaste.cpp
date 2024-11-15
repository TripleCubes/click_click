#include "copypaste.h"

#include <string>
#include <vector>
#include "tab.h"
#include "layer.h"
#include "../types/vec2i.h"
#include "../types/vec2.h"
#include "../graphic_types/graphic_types.h"
#include "../input.h"
#include "../pos_convert.h"

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

void to_clipboard_selected(const Tab &tab, GLFWwindow *glfw_window) {
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

	glfwSetClipboardString(glfw_window, str.c_str());
}

void to_clipboard_whole_layer(const Tab &tab, GLFWwindow *glfw_window) {
	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	const Layer &layer = tab.layer_list[layer_index];

	str += "type=" + TYPE_LABEL
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

	glfwSetClipboardString(glfw_window, str.c_str());
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

Vec2i get_paste_data(const std::string &paste_str) {
	int w = 0;
	int h = 0;

	int min_len = 6 + TYPE_LABEL.length() + 13;

	if (paste_str.back() != ';') {
		return vec2i_new(-1, -1);
	}

	if ((int)paste_str.length() < min_len) {
		return vec2i_new(-1, -1);
	}

	if (!label_check(paste_str)) {
		return vec2i_new(-1, -1);
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
		return vec2i_new(-1, -1);
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
		return vec2i_new(-1, -1);
	}

	h = std::stoi(num_str);

	w_num_start += num_str.length() + 6;

	if ((int)paste_str.length() != w_num_start + w * h + 1) {
		return vec2i_new(-1, -1);
	}

	data.clear();
	data.resize(w * h, 0);
	for (int i = w_num_start; i < (int)paste_str.length(); i++) {
		if (i >= w_num_start + w*h) {
			break;
		}

		data[i - w_num_start] = to_pallete_index(paste_str[i]);
	}

	return vec2i_new(w, h);
}

}

void to_clipboard(const Tab &tab, GLFWwindow *glfw_window) {
	if (tab.selection.full_preview_list.size() != 0) {
		to_clipboard_selected(tab, glfw_window);
	}
	else {
		to_clipboard_whole_layer(tab, glfw_window);
	}
}

void paste(Tab &tab, GraphicStuff &gs, const Input &input,
Vec2 parent_pos, GLFWwindow *glfw_window) {
	str = glfwGetClipboardString(glfw_window);
	rm_newline(str);
	Vec2i sz = get_paste_data(str);

	Vec2 pos = vec2_add(parent_pos, tab.pos);
	Vec2 main_fb_mouse_pos = get_main_fb_mouse_pos(gs, input.mouse_pos);
	Vec2 tex_draw_mouse_pos
		= get_tex_draw_mouse_pos(tab, pos, main_fb_mouse_pos);

	int layer_index = tab.layer_order_list[tab.layer_order_list_index];
	Layer &layer = tab.layer_list[layer_index];
	
	for (int y = 0; y < sz.y; y++) {
	for (int x = 0; x < sz.x; x++) {
		int data_i = y * sz.x + x;
		int layer_x = tex_draw_mouse_pos.x + x;
		int layer_y = tex_draw_mouse_pos.y - sz.y + y;
		
		if (layer_x < 0 || layer_x >= tab.sz.x
		|| layer_y < 0 || layer_y >= tab.sz.y) {
			continue;
		}

		if (data[data_i] == 0) {
			continue;
		}

		int layer_data_i = layer_y * tab.sz.x + layer_x;

		layer.data[layer_data_i] = data[data_i];
	}
	}

	layer_set_texture_data(layer, gs);
}
