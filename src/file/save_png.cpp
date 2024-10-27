#include "save_png.h"

#include <vector>
#include <iostream>
#include <stb_image_write.h>
#include "../tab/tab.h"
#include "../tab/layer.h"

namespace {

std::vector<unsigned char> data;

}

bool save_png(const std::string &path, const Tab &tab) {
	data.clear();
	data.resize(tab.sz.x * tab.sz.y * 4, 0);

	for (int y = 0; y < tab.sz.y; y++) {
	for (int x = 0; x < tab.sz.x; x++) {
		int data_i = (tab.sz.x * y + x) * 4;
		int layer_data_i = tab.sz.x * y + x;

		for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
			int layer_i = tab.layer_order_list[i];
			const Layer &layer = tab.layer_list[layer_i];
			
			unsigned char pallete_index = layer.data[layer_data_i];

			if (pallete_index == 0) {
				continue;
			}

			data[data_i    ] = tab.pallete_data[pallete_index * 4    ];
			data[data_i + 1] = tab.pallete_data[pallete_index * 4 + 1];
			data[data_i + 2] = tab.pallete_data[pallete_index * 4 + 2];
			data[data_i + 3] = 255;

			break;
		}
	}
	}

	if (
		stbi_write_png(path.c_str(), tab.sz.x, tab.sz.y,
		               4, data.data(), 4 * tab.sz.x) == 0
	) {
		std::cout << "failed writing " << path << std::endl;
		return false;
	}

	return true;
}
