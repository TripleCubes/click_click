#include "save_png.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <vector>
#include <iostream>
#include <stb_image_write.h>
#include "../tab/tab.h"
#include "../tab/layer.h"
#include "open_project.h"

namespace {

std::vector<unsigned char> data;

void get_data_from_tab(std::vector<unsigned char> &data, const Tab &tab) {
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
}

void get_data_from_open_project_data(std::vector<unsigned char> &data,
const OpenProjectData &open_project_data) {
	for (int y = 0; y < open_project_data.sz.y; y++) {
	for (int x = 0; x < open_project_data.sz.x; x++) {
		int data_i = (open_project_data.sz.x * y + x) * 4;
		int layer_data_i = open_project_data.sz.x * y + x;

		for (int i = 0; i < (int)open_project_data.layer_list.size(); i++) {
			const OpenProjectLayerData &layer =open_project_data.layer_list[i];
			
			unsigned char pallete_index = layer.data[layer_data_i];

			if (pallete_index == 0) {
				continue;
			}

			data[data_i    ]
				= open_project_data.pallete_data[pallete_index * 4    ];
			data[data_i + 1]
				= open_project_data.pallete_data[pallete_index * 4 + 1];
			data[data_i + 2]
				= open_project_data.pallete_data[pallete_index * 4 + 2];
			data[data_i + 3]
				= 255;

			break;
		}
	}
	}
}

}

bool save_png_from_tab(const std::string &path, const Tab &tab) {
	data.clear();
	data.resize(tab.sz.x * tab.sz.y * 4, 0);

	get_data_from_tab(data, tab);

	if (
		stbi_write_png(path.c_str(), tab.sz.x, tab.sz.y,
		               4, data.data(), 4 * tab.sz.x) == 0
	) {
		std::cout << "failed writing " << path << std::endl;
		return false;
	}

	#ifdef __EMSCRIPTEN__
	EM_ASM(
		FS.syncfs(function(err) {
			if (err) {
				console.log(err);
			}
		});
	);
	#endif

	return true;
}

#ifdef __EMSCRIPTEN__
bool web_save_png_from_open_project_data_no_syncfs(const std::string &path,
const OpenProjectData &open_project_data) {
	data.clear();
	data.resize(open_project_data.sz.x * open_project_data.sz.y * 4, 0);

	get_data_from_open_project_data(data, open_project_data);

	if (
		stbi_write_png(path.c_str(),
		               open_project_data.sz.x, open_project_data.sz.y,
		               4, data.data(), 4 * open_project_data.sz.x) == 0
	) {
		std::cout << "failed writing " << path << std::endl;
		return false;
	}

	return true;
}
#endif
