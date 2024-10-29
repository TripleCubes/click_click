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
#include "../types/vec2i.h"

namespace {

std::vector<unsigned char> data;

void fill_block(std::vector<unsigned char> &data, Vec2i pos, Vec2i canvas_sz,
unsigned char r, unsigned char g, unsigned char b, unsigned char a,
int png_px_scale) {
	Vec2i data_pos = vec2i_mul(pos, png_px_scale);
	Vec2i canvas_sz_scaled = vec2i_mul(canvas_sz, png_px_scale);

	for (int y = data_pos.y; y < data_pos.y + png_px_scale; y++) {
	for (int x = data_pos.x; x < data_pos.x + png_px_scale; x++) {
		int data_i = (y * canvas_sz_scaled.x + x) * 4;

		data[data_i    ] = r;
		data[data_i + 1] = g;
		data[data_i + 2] = b;
		data[data_i + 3] = a;
	}
	}
}

void get_data_from_tab(std::vector<unsigned char> &data, const Tab &tab,
int png_px_scale) {
	for (int y = 0; y < tab.sz.y; y++) {
	for (int x = 0; x < tab.sz.x; x++) {
		int layer_data_i = tab.sz.x * y + x;

		for (int i = 0; i < (int)tab.layer_order_list.size(); i++) {
			int layer_i = tab.layer_order_list[i];
			const Layer &layer = tab.layer_list[layer_i];
			
			unsigned char pallete_index = layer.data[layer_data_i];

			if (pallete_index == 0) {
				continue;
			}

			unsigned char r = tab.pallete_data[pallete_index * 4    ];
			unsigned char g = tab.pallete_data[pallete_index * 4 + 1];
			unsigned char b = tab.pallete_data[pallete_index * 4 + 2];
			unsigned char a = 255;

			fill_block(
				data,
				vec2i_new(x, y),
				tab.sz,
				r, g, b, a,
				png_px_scale
			);

			break;
		}
	}
	}
}

#ifdef __EMSCRIPTEN__
void get_data_from_open_project_data(std::vector<unsigned char> &data,
const OpenProjectData &open_project_data, int png_px_scale) {
	for (int y = 0; y < open_project_data.sz.y; y++) {
	for (int x = 0; x < open_project_data.sz.x; x++) {
		int layer_data_i = open_project_data.sz.x * y + x;

		for (int i = 0; i < (int)open_project_data.layer_list.size(); i++) {
			const OpenProjectLayerData &layer =open_project_data.layer_list[i];
			
			unsigned char pallete_index = layer.data[layer_data_i];

			if (pallete_index == 0) {
				continue;
			}

			unsigned char r
				= open_project_data.pallete_data[pallete_index * 4    ];
			unsigned char g
				= open_project_data.pallete_data[pallete_index * 4 + 1];
			unsigned char b
				= open_project_data.pallete_data[pallete_index * 4 + 2];
			unsigned char a
				= 255;

			fill_block(
				data,
				vec2i_new(x, y),
				open_project_data.sz,
				r, g, b, a,
				png_px_scale
			);
			
			break;
		}
	}
	}
}
#endif

}

bool save_png_from_tab(const std::string &path, const Tab &tab,
int png_px_scale) {
	data.clear();
	data.resize(tab.sz.x * tab.sz.y * 4 * png_px_scale * png_px_scale, 0);

	get_data_from_tab(data, tab, png_px_scale);

	if (
		stbi_write_png(path.c_str(),
		               tab.sz.x * png_px_scale, tab.sz.y * png_px_scale,
		               4, data.data(), 4 * tab.sz.x * png_px_scale) == 0
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
const OpenProjectData &open_project_data, int png_px_scale) {
	data.clear();
	data.resize(open_project_data.sz.x * open_project_data.sz.y
	            * 4 * png_px_scale * png_px_scale, 0);

	get_data_from_open_project_data(data, open_project_data, png_px_scale);

	if (
		stbi_write_png(path.c_str(),
		               open_project_data.sz.x * png_px_scale,
		               open_project_data.sz.y * png_px_scale,
		               4, data.data(),
		               4 * open_project_data.sz.x * png_px_scale) == 0
	) {
		std::cout << "failed writing " << path << std::endl;
		return false;
	}

	return true;
}
#endif
