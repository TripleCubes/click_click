#include "save_project.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "file.h"
#include "../tab/tab.h"

namespace {

char to_ascii(unsigned char pallete_index) {
	return (char)pallete_index + 48;
}

int min(int a, int b) {
	if (a < b) {
		return a;
	}

	return b;
}

int div_1(int a, int b) {
	if (a % b == 0) {
		return a / b;
	}
	else {
		return a / b + 1;
	}
}

bool chunk(std::string &result, const Layer &layer, int chunk_x, int chunk_y) {
	bool has_data = false;

	for (int y = chunk_y * 8; y < min(chunk_y * 8 + 8, layer.sz.y); y++) {
	for (int x = chunk_x * 8; x < min(chunk_x * 8 + 8, layer.sz.x); x++) {
		int data_index = y * layer.sz.x + x;
		unsigned char pallete_index = layer.data[data_index];
		if (pallete_index != 0) {
			has_data = true;
		}
		result += to_ascii(pallete_index);
	}
	}

	return has_data;
}

std::string left_pad(const std::string &s, int n) {
	std::string pad = "";
	int diff = n - s.size();
	for (int i = 0; i < diff; i++) {
		pad += ' ';
	}
	return pad + s;
}

}

void save_project(const std::string &path, const Tab &tab) {
	std::string result;

	result += "SZ " + std::to_string(tab.sz.x)
	          + " " + std::to_string(tab.sz.y) + "\n\n";

	result += "POS " + std::to_string((int)tab.pos.x)
              + " " + std::to_string((int)tab.pos.y) + "\n\n";

	result += "PXSCALE " + std::to_string(tab.px_scale) + "\n\n";

	result += "PALLETE\n";
	for (int i = 0; i < (int)tab.pallete_data.size(); i++) {
		if (i % 4 == 3) {
			continue;
		}

		result += left_pad(std::to_string((int)tab.pallete_data[i]), 3);
		if (i % 16 == 14) {
			result += "\n";
		}
		else {
			if (i % 4 == 2) {
				result += "  ";
			}
			else {
				result += ' ';
			}
		}
	}
	result += '\n';

	for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
		int layer_index = tab.layer_order_list[i];
		const Layer &layer = tab.layer_list[layer_index];

		result += "LAYER \"" + layer.name + "\" "
			+ std::to_string(i) + "\n";

		for (int y = 0; y < div_1(tab.sz.y, 8); y++) {
		for (int x = 0; x < div_1(tab.sz.x, 8); x++) {
			result += std::to_string(x) + " " + std::to_string(y) + " ";

			std::string chunk_str;
			bool has_data = chunk(chunk_str, layer, x, y);

			if (has_data) {
				result += "y " + chunk_str + "\n";
			}
			else {
				result += "n\n";
			}
		}
		}

		result += "\n";
	}

	write_file(path, result);

	#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.syncfs(function(err) {
			if (err) {
				console.log(err);
			}
		});
	);
	#endif
}
